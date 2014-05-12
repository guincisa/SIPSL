################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../sipsl-code/src/ACTION.cpp \
../sipsl-code/src/ALARM.cpp \
../sipsl-code/src/ALO.cpp \
../sipsl-code/src/CALL_OSET.cpp \
../sipsl-code/src/COMAP.cpp \
../sipsl-code/src/CS_HEADERS.cpp \
../sipsl-code/src/DAO.cpp \
../sipsl-code/src/DOA.cpp \
../sipsl-code/src/ENGINE.cpp \
../sipsl-code/src/MESSAGE.cpp \
../sipsl-code/src/SIPENGINE.cpp \
../sipsl-code/src/SIPUTIL.cpp \
../sipsl-code/src/SL_CC.cpp \
../sipsl-code/src/SPIN.cpp \
../sipsl-code/src/SUDP.cpp \
../sipsl-code/src/TRNSCT_SM.cpp \
../sipsl-code/src/TRNSPRT.cpp \
../sipsl-code/src/VALO.cpp \
../sipsl-code/src/test.cpp 

OBJS += \
./sipsl-code/src/ACTION.o \
./sipsl-code/src/ALARM.o \
./sipsl-code/src/ALO.o \
./sipsl-code/src/CALL_OSET.o \
./sipsl-code/src/COMAP.o \
./sipsl-code/src/CS_HEADERS.o \
./sipsl-code/src/DAO.o \
./sipsl-code/src/DOA.o \
./sipsl-code/src/ENGINE.o \
./sipsl-code/src/MESSAGE.o \
./sipsl-code/src/SIPENGINE.o \
./sipsl-code/src/SIPUTIL.o \
./sipsl-code/src/SL_CC.o \
./sipsl-code/src/SPIN.o \
./sipsl-code/src/SUDP.o \
./sipsl-code/src/TRNSCT_SM.o \
./sipsl-code/src/TRNSPRT.o \
./sipsl-code/src/VALO.o \
./sipsl-code/src/test.o 

CPP_DEPS += \
./sipsl-code/src/ACTION.d \
./sipsl-code/src/ALARM.d \
./sipsl-code/src/ALO.d \
./sipsl-code/src/CALL_OSET.d \
./sipsl-code/src/COMAP.d \
./sipsl-code/src/CS_HEADERS.d \
./sipsl-code/src/DAO.d \
./sipsl-code/src/DOA.d \
./sipsl-code/src/ENGINE.d \
./sipsl-code/src/MESSAGE.d \
./sipsl-code/src/SIPENGINE.d \
./sipsl-code/src/SIPUTIL.d \
./sipsl-code/src/SL_CC.d \
./sipsl-code/src/SPIN.d \
./sipsl-code/src/SUDP.d \
./sipsl-code/src/TRNSCT_SM.d \
./sipsl-code/src/TRNSPRT.d \
./sipsl-code/src/VALO.d \
./sipsl-code/src/test.d 


# Each subdirectory must supply rules for building sources it contributes
sipsl-code/src/%.o: ../sipsl-code/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


