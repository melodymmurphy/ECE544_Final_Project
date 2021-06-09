################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/control.c \
../src/display.c \
../src/initialization.c \
../src/interrupts.c \
../src/luts.c \
../src/main.c \
../src/menu.c \
../src/midi.c \
../src/sequencer.c \
../src/signal_generator.c \
../src/system.c 

OBJS += \
./src/control.o \
./src/display.o \
./src/initialization.o \
./src/interrupts.o \
./src/luts.o \
./src/main.o \
./src/menu.o \
./src/midi.o \
./src/sequencer.o \
./src/signal_generator.o \
./src/system.o 

C_DEPS += \
./src/control.d \
./src/display.d \
./src/initialization.d \
./src/interrupts.d \
./src/luts.d \
./src/main.d \
./src/menu.d \
./src/midi.d \
./src/sequencer.d \
./src/signal_generator.d \
./src/system.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MicroBlaze gcc compiler'
	mb-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -IC:/Work/Vitis_Workspace/ECE544_Final_Project_v4/export/ECE544_Final_Project_v4/sw/ECE544_Final_Project_v4/freertos10_xilinx_domain/bspinclude/include -mlittle-endian -mxl-barrel-shift -mxl-pattern-compare -mno-xl-soft-div -mcpu=v11.0 -mno-xl-soft-mul -mxl-multiply-high -mhard-float -mxl-float-convert -mxl-float-sqrt -Wl,--no-relax -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


