################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/Users/robertdekok/Library/Mobile\ Documents/com~apple~CloudDocs/ARDUINO/libraries/TFT_ST7735-master/TFT_ST7735.cpp 

LINK_OBJ += \
./libraries/TFT_ST7735-master/TFT_ST7735.cpp.o 

CPP_DEPS += \
./libraries/TFT_ST7735-master/TFT_ST7735.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
libraries/TFT_ST7735-master/TFT_ST7735.cpp.o: /Users/robertdekok/Library/Mobile\ Documents/com~apple~CloudDocs/ARDUINO/libraries/TFT_ST7735-master/TFT_ST7735.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/Applications/Eclipse/arduino-oxygen/Eclipse.app/Contents/MacOS//arduinoPlugin/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.4-arduino2/bin/avr-g++" -c -g -Os -Wall -Wextra -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -flto -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10802 -DARDUINO_AVR_NANO -DARDUINO_ARCH_AVR   -I"/Applications/Arduino-1.8.3.app/Contents/Java/hardware/arduino/avr/cores/arduino" -I"/Applications/Arduino-1.8.3.app/Contents/Java/hardware/arduino/avr/variants/eightanaloginputs" -I"/Applications/Arduino-1.8.3.app/Contents/Java/hardware/arduino/avr/libraries/SPI" -I"/Applications/Arduino-1.8.3.app/Contents/Java/hardware/arduino/avr/libraries/SPI/src" -I"/Users/robertdekok/Library/Mobile Documents/com~apple~CloudDocs/ARDUINO/libraries/TFT_ST7735-master" -I"/Applications/Arduino-1.8.3.app/Contents/Java/hardware/arduino/avr/libraries/Wire" -I"/Applications/Arduino-1.8.3.app/Contents/Java/hardware/arduino/avr/libraries/Wire/src" -I"/Applications/Arduino-1.8.3.app/Contents/Java/hardware/arduino/avr/libraries/EEPROM" -I"/Applications/Arduino-1.8.3.app/Contents/Java/hardware/arduino/avr/libraries/EEPROM/src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '


