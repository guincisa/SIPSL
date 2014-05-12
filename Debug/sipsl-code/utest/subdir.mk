################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../sipsl-code/utest/HEADERS_TEST.cpp \
../sipsl-code/utest/UTESTTOOL.cpp 

OBJS += \
./sipsl-code/utest/HEADERS_TEST.o \
./sipsl-code/utest/UTESTTOOL.o 

CPP_DEPS += \
./sipsl-code/utest/HEADERS_TEST.d \
./sipsl-code/utest/UTESTTOOL.d 


# Each subdirectory must supply rules for building sources it contributes
sipsl-code/utest/%.o: ../sipsl-code/utest/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


