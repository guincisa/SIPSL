################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../branches/messaging/sipsl-code/src/ACTION.cpp \
../branches/messaging/sipsl-code/src/ALARM.cpp \
../branches/messaging/sipsl-code/src/ALO.cpp \
../branches/messaging/sipsl-code/src/CALLMNT.cpp \
../branches/messaging/sipsl-code/src/CALL_OSET.cpp \
../branches/messaging/sipsl-code/src/COMAP.cpp \
../branches/messaging/sipsl-code/src/COMM.cpp \
../branches/messaging/sipsl-code/src/COMM_OLD.cpp \
../branches/messaging/sipsl-code/src/CS_HEADERS.cpp \
../branches/messaging/sipsl-code/src/DAO.cpp \
../branches/messaging/sipsl-code/src/DOA.cpp \
../branches/messaging/sipsl-code/src/ENGINE.cpp \
../branches/messaging/sipsl-code/src/MESSAGE.cpp \
../branches/messaging/sipsl-code/src/P_HEADERS.cpp \
../branches/messaging/sipsl-code/src/SIPENGINE.cpp \
../branches/messaging/sipsl-code/src/SIPUTIL.cpp \
../branches/messaging/sipsl-code/src/SL_CC.cpp \
../branches/messaging/sipsl-code/src/SPIN.cpp \
../branches/messaging/sipsl-code/src/SUDP.cpp \
../branches/messaging/sipsl-code/src/TRNSCT_SM.cpp \
../branches/messaging/sipsl-code/src/TRNSPRT.cpp \
../branches/messaging/sipsl-code/src/VALO.cpp \
../branches/messaging/sipsl-code/src/test.cpp 

OBJS += \
./branches/messaging/sipsl-code/src/ACTION.o \
./branches/messaging/sipsl-code/src/ALARM.o \
./branches/messaging/sipsl-code/src/ALO.o \
./branches/messaging/sipsl-code/src/CALLMNT.o \
./branches/messaging/sipsl-code/src/CALL_OSET.o \
./branches/messaging/sipsl-code/src/COMAP.o \
./branches/messaging/sipsl-code/src/COMM.o \
./branches/messaging/sipsl-code/src/COMM_OLD.o \
./branches/messaging/sipsl-code/src/CS_HEADERS.o \
./branches/messaging/sipsl-code/src/DAO.o \
./branches/messaging/sipsl-code/src/DOA.o \
./branches/messaging/sipsl-code/src/ENGINE.o \
./branches/messaging/sipsl-code/src/MESSAGE.o \
./branches/messaging/sipsl-code/src/P_HEADERS.o \
./branches/messaging/sipsl-code/src/SIPENGINE.o \
./branches/messaging/sipsl-code/src/SIPUTIL.o \
./branches/messaging/sipsl-code/src/SL_CC.o \
./branches/messaging/sipsl-code/src/SPIN.o \
./branches/messaging/sipsl-code/src/SUDP.o \
./branches/messaging/sipsl-code/src/TRNSCT_SM.o \
./branches/messaging/sipsl-code/src/TRNSPRT.o \
./branches/messaging/sipsl-code/src/VALO.o \
./branches/messaging/sipsl-code/src/test.o 

CPP_DEPS += \
./branches/messaging/sipsl-code/src/ACTION.d \
./branches/messaging/sipsl-code/src/ALARM.d \
./branches/messaging/sipsl-code/src/ALO.d \
./branches/messaging/sipsl-code/src/CALLMNT.d \
./branches/messaging/sipsl-code/src/CALL_OSET.d \
./branches/messaging/sipsl-code/src/COMAP.d \
./branches/messaging/sipsl-code/src/COMM.d \
./branches/messaging/sipsl-code/src/COMM_OLD.d \
./branches/messaging/sipsl-code/src/CS_HEADERS.d \
./branches/messaging/sipsl-code/src/DAO.d \
./branches/messaging/sipsl-code/src/DOA.d \
./branches/messaging/sipsl-code/src/ENGINE.d \
./branches/messaging/sipsl-code/src/MESSAGE.d \
./branches/messaging/sipsl-code/src/P_HEADERS.d \
./branches/messaging/sipsl-code/src/SIPENGINE.d \
./branches/messaging/sipsl-code/src/SIPUTIL.d \
./branches/messaging/sipsl-code/src/SL_CC.d \
./branches/messaging/sipsl-code/src/SPIN.d \
./branches/messaging/sipsl-code/src/SUDP.d \
./branches/messaging/sipsl-code/src/TRNSCT_SM.d \
./branches/messaging/sipsl-code/src/TRNSPRT.d \
./branches/messaging/sipsl-code/src/VALO.d \
./branches/messaging/sipsl-code/src/test.d 


# Each subdirectory must supply rules for building sources it contributes
branches/messaging/sipsl-code/src/%.o: ../branches/messaging/sipsl-code/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DCOMPILEFORSUSE -I"/home/guic/DEV/workspace/SIPSL_SVN/sipsl-code/hdr" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


