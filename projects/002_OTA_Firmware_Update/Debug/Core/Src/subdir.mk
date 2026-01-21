################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/custom_job_parser.c \
../Core/Src/main.c \
../Core/Src/ota_application.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_hal_timebase_tim.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/custom_job_parser.o \
./Core/Src/main.o \
./Core/Src/ota_application.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_hal_timebase_tim.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/custom_job_parser.d \
./Core/Src/main.d \
./Core/Src/ota_application.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_hal_timebase_tim.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx '-DMBEDTLS_CONFIG_FILE="mbedtls_config.h"' -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/Core/Src" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/BSP" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/BSP/utils" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/config" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/mqtt_helper" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/FreeRTOS-Kernel/include" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/FreeRTOS-Kernel/portable/GCC/ARM_CM4F" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/standard/coreJSON/source/include" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/standard/logging-stack" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/aws/aws-iot-core-mqtt-file-streams/source/include" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/3rdparty/tinycbor/src" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/aws/Jobs-for-AWS-IoT-embedded-sdk/source/include" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/aws/Jobs-for-AWS-IoT-embedded-sdk/source/otaJobParser/include" -I../Middlewares/Third_Party/Infineon_Wireless_Connectivity/aws-iot-device-sdk-embedded-C/platform/include/ -I../Middlewares/Third_Party/Infineon_Wireless_Connectivity/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/ -I../Middlewares/Third_Party/Infineon_Wireless_Connectivity/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/source/include/ -I../Middlewares/Third_Party/Infineon_Wireless_Connectivity/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/source/interface/ -I../Middlewares/Third_Party/Infineon_Wireless_Connectivity/aws-iot-device-sdk-embedded-C/libraries/standard/backoffAlgorithm/source/include/ -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/custom_job_parser.cyclo ./Core/Src/custom_job_parser.d ./Core/Src/custom_job_parser.o ./Core/Src/custom_job_parser.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/ota_application.cyclo ./Core/Src/ota_application.d ./Core/Src/ota_application.o ./Core/Src/ota_application.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_hal_timebase_tim.cyclo ./Core/Src/stm32f4xx_hal_timebase_tim.d ./Core/Src/stm32f4xx_hal_timebase_tim.o ./Core/Src/stm32f4xx_hal_timebase_tim.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

