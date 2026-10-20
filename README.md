# MACH32 - STM32 Baremetal Drivers

![MACH32](assets/mach32.jpeg)

A learning-focused bare-metal driver pack for the STM32F446RE.

Included drivers:

- RCC, GPIO, SysTick, NVIC, and EXTI.
- Polling USART, SPI, and I2C.
- Timers, PWM, ADC, and DMA.
- Internal Flash, CRC32, and the independent watchdog.
- Polling bxCAN.

The code uses ST and Arm CMSIS register definitions. The `Examples` directory
contains focused hardware tests for the driver pack.

## Learning bootloader

MACH32 includes a small UART-controlled A/B bootloader.

```bash
make boot-demo
make flash-demo
screen /dev/ttyACM0 115200
```

The bootloader accepts `status`, `boot A`, and `boot B`. It can also erase and
upload either application slot through USART2:

```bash
python3 tools/uart_upload.py /dev/ttyACM0 build/app_b/firmware.bin B
```

It starts application A after a ten-second inactivity timeout. Application A
blinks LD2 slowly. Application B blinks LD2 quickly.

See `Bootloader/README.md` for the Flash layout and test instructions.

## References

- [STM32F446 datasheet](https://www.st.com/resource/en/datasheet/stm32f446mc.pdf)
- [RM0390 reference manual](https://www.st.com/resource/en/reference_manual/rm0390-stm32f446xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [UM1724 Nucleo-64 user manual](https://www.st.com/resource/en/user_manual/um1724-stm32-nucleo64-boards-mb1136-stmicroelectronics.pdf)
- [PM0214 Cortex-M4 programming manual](https://www.st.com/resource/en/programming_manual/pm0214-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf)
