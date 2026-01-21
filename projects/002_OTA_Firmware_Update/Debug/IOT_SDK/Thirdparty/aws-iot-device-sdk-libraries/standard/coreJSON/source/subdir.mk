################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/standard/coreJSON/source/core_json.c 

OBJS += \
./IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/standard/coreJSON/source/core_json.o 

C_DEPS += \
./IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/standard/coreJSON/source/core_json.d 


# Each subdirectory must supply rules for building sources it contributes
IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/standard/coreJSON/source/%.o IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/standard/coreJSON/source/%.su IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/standard/coreJSON/source/%.cyclo: ../IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/standard/coreJSON/source/%.c IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/standard/coreJSON/source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx '-DMBEDTLS_CONFIG_FILE="mbedtls_config.h"' -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/Core/Src" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/BSP" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/BSP/utils" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/config" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/mqtt_helper" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/FreeRTOS-Kernel/include" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/FreeRTOS-Kernel/portable/GCC/ARM_CM4F" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/standard/coreJSON/source/include" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/standard/logging-stack" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/aws/aws-iot-core-mqtt-file-streams/source/include" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/3rdparty/tinycbor/src" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/aws/Jobs-for-AWS-IoT-embedded-sdk/source/include" -I"C:/Users/bhara/STM32CubeIDE/fastbit-stm32WB-IOT-board/Projects-and-examples/002_OTA_Firmware_Update/IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/aws/Jobs-for-AWS-IoT-embedded-sdk/source/otaJobParser/include" -I../Middlewares/Third_Party/Infineon_Wireless_Connectivity/aws-iot-device-sdk-embedded-C/platform/include/ -I../Middlewares/Third_Party/Infineon_Wireless_Connectivity/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/ -I../Middlewares/Third_Party/Infineon_Wireless_Connectivity/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/source/include/ -I../Middlewares/Third_Party/Infineon_Wireless_Connectivity/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/source/interface/ -I../Middlewares/Third_Party/Infineon_Wireless_Connectivity/aws-iot-device-sdk-embedded-C/libraries/standard/backoffAlgorithm/source/include/ -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-IOT_SDK-2f-Thirdparty-2f-aws-2d-iot-2d-device-2d-sdk-2d-libraries-2f-standard-2f-coreJSON-2f-source

clean-IOT_SDK-2f-Thirdparty-2f-aws-2d-iot-2d-device-2d-sdk-2d-libraries-2f-standard-2f-coreJSON-2f-source:
	-$(RM) ./IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/standard/coreJSON/source/core_json.cyclo ./IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/standard/coreJSON/source/core_json.d ./IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/standard/coreJSON/source/core_json.o ./IOT_SDK/Thirdparty/aws-iot-device-sdk-libraries/standard/coreJSON/source/core_json.su

.PHONY: clean-IOT_SDK-2f-Thirdparty-2f-aws-2d-iot-2d-device-2d-sdk-2d-libraries-2f-standard-2f-coreJSON-2f-source

