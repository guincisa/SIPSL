################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../test-tools/ptestcslee.cpp \
../test-tools/sendU.cpp 

OBJS += \
./test-tools/ptestcslee.o \
./test-tools/sendU.o 

CPP_DEPS += \
./test-tools/ptestcslee.d \
./test-tools/sendU.d 


# Each subdirectory must supply rules for building sources it contributes
test-tools/%.o: ../test-tools/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


