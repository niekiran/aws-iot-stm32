################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../IOT_SDK/Thirdparty/FreeRTOS-Kernel/croutine.c \
../IOT_SDK/Thirdparty/FreeRTOS-Kernel/event_groups.c \
../IOT_SDK/Thirdparty/FreeRTOS-Kernel/freertos_hooks.c \
../IOT_SDK/Thirdparty/FreeRTOS-Kernel/list.c \
../IOT_SDK/Thirdparty/FreeRTOS-Kernel/queue.c \
../IOT_SDK/Thirdparty/FreeRTOS-Kernel/stream_buffer.c \
../IOT_SDK/Thirdparty/FreeRTOS-Kernel/tasks.c \
../IOT_SDK/Thirdparty/FreeRTOS-Kernel/timers.c 

OBJS += \
./IOT_SDK/Thirdparty/FreeRTOS-Kernel/croutine.o \
./IOT_SDK/Thirdparty/FreeRTOS-Kernel/event_groups.o \
./IOT_SDK/Thirdparty/FreeRTOS-Kernel/freertos_hooks.o \
./IOT_SDK/Thirdparty/FreeRTOS-Kernel/list.o \
./IOT_SDK/Thirdparty/FreeRTOS-Kernel/queue.o \
./IOT_SDK/Thirdparty/FreeRTOS-Kernel/stream_buffer.o \
./IOT_SDK/Thirdparty/FreeRTOS-Kernel/tasks.o \
./IOT_SDK/Thirdparty/FreeRTOS-Kernel/timers.o 

C_DEPS += \
./IOT_SDK/Thirdparty/FreeRTOS-Kernel/croutine.d \
./IOT_SDK/Thirdparty/FreeRTOS-Kernel/event_groups.d \
./IOT_SDK/Thirdparty/FreeRTOS-Kernel/freertos_hooks.d \
./IOT_SDK/Thirdparty/FreeRTOS-Kernel/list.d \
./IOT_SDK/Thirdparty/FreeRTOS-Kernel/queue.d \
./IOT_SDK/Thirdparty/FreeRTOS-Kernel/stream_buffer.d \
./IOT_SDK/Thirdparty/FreeRTOS-Kernel/tasks.d \
./IOT_SDK/Thirdparty/FreeRTOS-Kernel/timers.d 


# Each subdirectory must supply rules for building sources it contributes
IOT_SDK/Thirdparty/FreeRTOS-Kernel/%.o IOT_SDK/Thirdparty/FreeRTOS-Kernel/%.su IOT_SDK/Thirdparty/FreeRTOS-Kernel/%.cyclo: ../IOT_SDK/Thirdparty/FreeRTOS-Kernel/%.c IOT_SDK/Thirdparty/FreeRTOS-Kernel/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx '-DMBEDTLS_CONFIG_FILE="mbedtls_config.h"' -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/Core/Src" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/BSP" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/BSP/utils" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/config" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/mqtt_helper" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/FreeRTOS-Kernel/include" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/FreeRTOS-Kernel/portable/GCC/ARM_CM4F" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/standard/coreJSON/source/include" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/standard/logging-stack" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/aws/aws-iot-core-mqtt-file-streams/source/include" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/3rdparty/tinycbor/src" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/aws/Jobs-for-AWS-IoT-embedded-sdk/source/include" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/aws/Jobs-for-AWS-IoT-embedded-sdk/source/otaJobParser/include" -I../Middlewares/Third_Party/Infineon_Wireless_Connectivity/aws-iot-device-sdk-embedded-C/platform/include/ -I../Middlewares/Third_Party/Infineon_Wireless_Connectivity/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/ -I../Middlewares/Third_Party/Infineon_Wireless_Connectivity/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/source/include/ -I../Middlewares/Third_Party/Infineon_Wireless_Connectivity/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/source/interface/ -I../Middlewares/Third_Party/Infineon_Wireless_Connectivity/aws-iot-device-sdk-embedded-C/libraries/standard/backoffAlgorithm/source/include/ -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-IOT_SDK-2f-Thirdparty-2f-FreeRTOS-2d-Kernel

clean-IOT_SDK-2f-Thirdparty-2f-FreeRTOS-2d-Kernel:
	-$(RM) ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/croutine.cyclo ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/croutine.d ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/croutine.o ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/croutine.su ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/event_groups.cyclo ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/event_groups.d ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/event_groups.o ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/event_groups.su ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/freertos_hooks.cyclo ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/freertos_hooks.d ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/freertos_hooks.o ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/freertos_hooks.su ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/list.cyclo ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/list.d ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/list.o ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/list.su ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/queue.cyclo ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/queue.d ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/queue.o ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/queue.su ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/stream_buffer.cyclo ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/stream_buffer.d ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/stream_buffer.o ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/stream_buffer.su ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/tasks.cyclo ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/tasks.d ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/tasks.o ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/tasks.su ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/timers.cyclo ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/timers.d ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/timers.o ./IOT_SDK/Thirdparty/FreeRTOS-Kernel/timers.su

.PHONY: clean-IOT_SDK-2f-Thirdparty-2f-FreeRTOS-2d-Kernel

