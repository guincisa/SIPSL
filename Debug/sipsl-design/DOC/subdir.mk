################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../sipsl-design/DOC/DBLOGIC.cpp 

OBJS += \
./sipsl-design/DOC/DBLOGIC.o 

CPP_DEPS += \
./sipsl-design/DOC/DBLOGIC.d 


# Each subdirectory must supply rules for building sources it contributes
sipsl-design/DOC/%.o: ../sipsl-design/DOC/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


