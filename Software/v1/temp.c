/*

// function prototypes
void write_decimals(u08_t dec_num);
void execute_step(void);
void next_step(void);
void update_BPM(void);
void GPIO_ISR(void);
void TIMER_ISR(void);

// Seven-Segment registers
#define SegEn_ADDR      0x80001038      // digit enable register
#define SegDec_ADDR     0x80001039      // decimal register
#define SegDigLo_ADDR   0x8000103C      // lower 4 digits
#define SegDigHi_ADDR   0x80001040      // higher 4 digits

// Timer addresses
#define RPTC_CNTR       0x80001200      // Counter
#define RPTC_HRC        0x80001204      // Hi reference/capture register
#define RPTC_LRC        0x80001208      // Lo reference/capture register
#define RPTC_CTRL       0x8000120C      // Control register

// GPIO1 registers
#define GPIO_READ       0x80001400      // Read register
#define GPIO_LEDs       0x80001404      // Write register
#define GPIO_INOUT      0x80001408      // Write enable register
#define GPIO_INTE       0x8000140C      // Interrupt enable register
#define GPIO_PTRIG      0x80001410      // Interrupt trigger polarity
#define GPIO_CTRL       0x80001418      // Control register
#define GPIO_INTS       0x8000141C      // Interrupts register

// GPIO2 registers
#define GPIO2_READ      0x80001500      // Read register
#define GPIO2_OUT       0x80001504      // Write register
#define GPIO2_INOUT     0x80001508      // Write enable register
#define GPIO2_INTE      0x8000150C      // Interrupt enable register
#define GPIO2_PTRIG     0x80001510      // Interrupt trigger polarity
#define GPIO2_CTRL      0x80001518      // Control register
#define GPIO2_INTS      0x8000151C      // Interrupts register

// Masks
#define BTN_CNTR_MASK   0x100000        // center pushbutton
#define BTN_LEFT_MASK   0x80000         // left pushbutton
#define BTN_UP_MASK     0x40000         // top pushbutton
#define BTN_RGHT_MASK   0x20000         // right pushbutton
#define BTN_DOWN_MASK   0x10000         // bottom pushbutton
#define TMR_INT_MASK    0x40            // timer interrupt
#define ROT_BTN1_MASK   0x200000        // right rotary encoder button
#define ROT_BTN2_MASK   0x100           // left rotary encoder button

// Initialization values
#define SWITCH_INIT     0xFFFF        // write to GPIO to set LEDs as outputs, switches as inputs
#define TIMER_INIT      0x021         // value to write to control register to initialize counter
#define TICK_80_BPM     0x023C3460    // 100 MHz clock ticks per 80 BPM beat

static system_states_t states;    // Declare static struct to contain system states

int main(void)
{
  u08_t reg = 0;
  while (1){
    execute_step();   // go to sequence routine
    update_BPM();     // update the sequence rate
    reg =  READ_REGISTER_8(MIDI_NOTE_ON);
    WRITE_REGISTER_8(SegDigLo_ADDR, reg);
  }

    return (0);
}


// Interrupt service routine for the GPIO inputs - IRQ line 4
void GPIO_ISR(void)
{
  // Read GPIO interrupt register to determine source of interrupt
  u32_t gpio1_intr = M_PSP_READ_REGISTER_32(GPIO_INTS);
  u32_t gpio2_intr = M_PSP_READ_REGISTER_32(GPIO2_INTS);
  u08_t midi_intr = M_PSP_READ_REGISTER_32(MIDI_INTS);
  u32_t timer_count;
  u08_t val;
  if ((gpio1_intr > 0) || (gpio2_intr > 0) || (midi_intr > 0))
  {
    M_PSP_WRITE_REGISTER_32(GPIO_CTRL, CLEAR);    // clear and disable pending GPIO interrupts
    M_PSP_WRITE_REGISTER_32(GPIO_INTS, CLEAR);    // clear pending GPIO interrupts
    M_PSP_WRITE_REGISTER_32(GPIO2_CTRL, CLEAR);    // clear and disable pending GPIO2 interrupts
    M_PSP_WRITE_REGISTER_32(GPIO2_INTS, CLEAR);    // clear pending GPIO2 interrupts


    if (states.mode == RECORD)     // interrupt functionality for RECORD mode
    {

      if ((gpio1_intr & BTN_CNTR_MASK) > 0)   // if center button pressed
      {
        states.mode = PLAY;                                       // switch to PLAY mode
        states.timer_count = states.seq_timer;                    // timer countdown will be the sequence step time
        timer_count = (states.timer_count << 2) >> states.subdiv; // set timer countdown based on note subdivision
        M_PSP_WRITE_REGISTER_32(RPTC_LRC, timer_count);           // write value to timer
        M_PSP_WRITE_REGISTER_32(GPIO_INTE, 0x350000);             // enable interrupts for PLAY mode
      }

      else if((gpio1_intr & BTN_RGHT_MASK) > 0)   // otherwise, if right button pressed
      {
        states.step++;            // increment selected step
        if (states.step > LAST)
        {
          states.step = FIRST;    // wrap around from last step to first step
        }
      }

      else if((gpio1_intr & BTN_LEFT_MASK) > 0)   // otherwise, if left button pressed
      {
        if (states.step == FIRST)
        {
          states.step = LAST;     // wrap around from first step to last step
        }

        else
        {
          states.step--;          // decrement selected step
        }
      }

      else if ((gpio2_intr & ROT_BTN2_MASK) > 0)                 // if right rotary encoder button pressed
      {
        val = M_PSP_READ_REGISTER_32(GPIO2_READ);           // read value from GPIO2 register
        states.note[states.step].note_select = val & 0xFF;  // mask off lower byte and store to note value
        states.note[states.step].volume = 0x70;             // set volume of note to 0x70
        states.step++;                                      // increment step
        if (states.step > LAST)
        {
          states.step = FIRST;                              // wrap around from last step to first step
        }

      }
    }
    // sequencer in PLAY mode
    else if (states.mode == PLAY)
    {

      if ((gpio1_intr & BTN_CNTR_MASK) > 0)   // if center button pressed
      {
        M_PSP_WRITE_REGISTER_32(RPTC_CTRL, 0xC0);                 // clear and disable interrupts, reset timer
        stop_note();                                              // stop currently playing note
        states.mode = RECORD;                                     // switch to RECORD mode
        states.timer_count = BLINK_TIMER;                         // set timer count to LED blink rate
        M_PSP_WRITE_REGISTER_32(GPIO_INTE, 0x3A0000);             // enable interrupts for RECORD mode
        timer_count = (states.timer_count << 2) >> states.subdiv; // set timer countdown based on note subdivision
        M_PSP_WRITE_REGISTER_32(RPTC_LRC, timer_count);           // write timer counter to register
        M_PSP_WRITE_REGISTER_32(RPTC_CTRL, 0x21);                 // re-enable interrupts and start timer
      }

      else if ((gpio1_intr & BTN_UP_MASK) > 0)  // if top button pressed
      {
        states.subdiv++;          // increment note subdivision
         if (states.subdiv > 6)
         {
           states.subdiv = 0;     // wrap around if value out of range
         }
      }

      else if ((gpio1_intr & BTN_DOWN_MASK) > 0)  // if bottom button pressed
      {
        if (states.subdiv <= 0)
        {
          states.subdiv = 6;    // wrap around if value out of range
        }
        else
        {
          states.subdiv--;      // decrement note subdivision
        }
      }

    }


}

// Interrupt service routine for the timer - IRQ line 3
void TIMER_ISR(void)
{
    u32_t intr = M_PSP_READ_REGISTER_32(RPTC_CTRL);   // read timer control register
    u32_t timer_count;
    if ((intr & TMR_INT_MASK) > 0)                // if timer has active interrupt
    {

      M_PSP_WRITE_REGISTER_32(RPTC_CTRL, 0x41);   // disable interrupts and clear pending interrupt
      if (states.mode == PLAY)          // if sequencer is in PLAY mode
      {
        next_step();                            // move to next sequece step
        states.timer_count = states.seq_timer;  // set timer value to sequence rate timer
        stop_note();                            // stop currently playing note
      }

      else if (states.mode == RECORD)   // otherwise, if sequencer is in RECORD mode
      {
        if (states.led_toggle == ON)            // if LED is on
        {
          states.led_toggle = OFF;              // toggle LED off
        }

        else
        {
          states.led_toggle = ON;               // otherwise, toggle LED on
        }

        states.timer_count = BLINK_TIMER;       // set timer count rate to LED blink rate
      }

      timer_count = (states.timer_count << 2) >> states.subdiv; // set timer countdown based on note subdivision
      M_PSP_WRITE_REGISTER_32(RPTC_LRC, timer_count);           // write timer counter value to register
      M_PSP_WRITE_REGISTER_32(RPTC_CTRL, 0x21);                 // enable timer interrupts in register

    }

    bspClearExtInterrupt(3);  // clear active interrupts on IRQ line 3
}

// Perform actions corresponding to the current sequencer step
void execute_step(void)
{
  u32_t step_active = (0x1 << 15) >> states.step;   // active step--shifts reorder the steps as left to right
  u32_t reg = M_PSP_READ_REGISTER_32(GPIO_READ);    // read GPIO1 register
  u32_t reg2 = M_PSP_READ_REGISTER_32(GPIO2_READ);  // read GPIO2 register
  u08_t rot_val;
  u32_t note_val;
  if (states.mode == RECORD)      // if in RECORD mode
  {

    if (states.led_toggle == ON)  // if LED toggle on
    {
      M_PSP_WRITE_REGISTER_32(GPIO_LEDs, step_active);  // turn on LED corresponding to active step
    }

    else
    {
      M_PSP_WRITE_REGISTER_32(GPIO_LEDs, CLEAR);    // otherwise, turn that LED off
    }

    rot_val = reg2 & 0xFF;                              // mask off left rotary encoder value
    note_val = rotary_to_note(rot_val);                 // get note value to write to display
    M_PSP_WRITE_REGISTER_32(SegDigHi_ADDR, note_val);   // write note value to seven-segment display
  }
  else if (states.mode == PLAY)   // if in PLAY mode
  {

    if ((reg & step_active) > 0)    // if the switch corresponding to the current step is high
    {
      play_note(states.note[states.step]);              // play the stored note
      M_PSP_WRITE_REGISTER_32(GPIO_LEDs, step_active);  // light LED corresponding to active step
      rot_val = states.note[states.step].note_select;   // get integer note value corresponding to current step
      note_val = rotary_to_note(rot_val);               // get display note value for currently active step
      M_PSP_WRITE_REGISTER_32(SegDigHi_ADDR, note_val); // display that note value on the high digits
    }

    else                            // otherwise, if the switch is low
    {
      stop_note();                                          // stop currently playing note
      M_PSP_WRITE_REGISTER_32(GPIO_LEDs, CLEAR);            // turn off LEDs
      M_PSP_WRITE_REGISTER_32(SegDigHi_ADDR, 0x16161616);   // write dashes to high digits
    }

  }
  write_decimals(states.subdiv);   // write decimal values corresponding to the current note subdivision

  return;
}


// Update sequencer rate based on right rotary encoder's value
void update_BPM(void)
{
    u32_t reg = M_PSP_READ_REGISTER_32(GPIO_READ);      // read GPIO1 register
    u08_t rot_val = reg >> 24;                          // shift to mask off right rotary encoder value
    u32_t BCD_val = rotary_to_BCD(rot_val);             // get BCD value of BPM rate to write to seven-segment display
    M_PSP_WRITE_REGISTER_32(SegDigLo_ADDR, BCD_val);    // write to seven-segment display
    states.seq_timer = rotary_to_count(rot_val);        // get timer value for BPM rate to store as system state
    return;
}

*/
