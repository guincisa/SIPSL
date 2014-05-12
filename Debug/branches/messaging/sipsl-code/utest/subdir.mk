################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../branches/messaging/sipsl-code/utest/HEADERS_TEST.cpp \
../branches/messaging/sipsl-code/utest/UTESTTOOL.cpp 

OBJS += \
./branches/messaging/sipsl-code/utest/HEADERS_TEST.o \
./branches/messaging/sipsl-code/utest/UTESTTOOL.o 

CPP_DEPS += \
./branches/messaging/sipsl-code/utest/HEADERS_TEST.d \
./branches/messaging/sipsl-code/utest/UTESTTOOL.d 


# Each subdirectory must supply rules for building sources it contributes
branches/messaging/sipsl-code/utest/%.o: ../branches/messaging/sipsl-code/utest/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DCOMPILEFORSUSE -I"/home/guic/DEV/workspace/SIPSL_SVN/sipsl-code/hdr" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


