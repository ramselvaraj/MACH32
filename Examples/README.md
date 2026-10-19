# MACH32 hardware examples

Build all examples:

```bash
make examples
```

Build and flash one example:

```bash
make EXAMPLE=02_rcc_systick_blink
make EXAMPLE=02_rcc_systick_blink flash
```

Only one example can run on the board at one time.

## Test list

| Example | Driver test | Required connection | Expected result |
|---|---|---|---|
| `01_led_blink` | RCC clock gate and GPIO output | None | LD2 blinks. |
| `02_rcc_systick_blink` | 180 MHz RCC setup and SysTick | None | LD2 changes state every 500 ms. |
| `03_button_input` | GPIO input | None | LD2 stays on while you press the blue button. |
| `04_usart_echo` | Polling USART2 | USB only | The serial terminal receives and echoes text. |
| `05_exti_button` | EXTI and NVIC | None | Each blue-button press changes the LD2 state. |
| `06_nvic_software_irq` | Software-generated NVIC interrupt | None | LD2 changes state every 500 ms. |
| `07_timer_interrupt` | TIM2 update interrupt | None | LD2 changes state every 500 ms. |
| `08_pwm_fade` | TIM2 PWM | None | LD2 fades between dark and bright. |
| `09_spi_loopback` | SPI1 polling transfer | Connect PA7 to PA6 | LD2 stays on when all returned bytes match. |
| `10_i2c_scan` | I2C1 address probe | Connect an I2C device to PB8 and PB9 | The terminal lists each detected address. |
| `11_adc_uart` | ADC1 channel 0 | Connect PA0 to GND or 3V3 | The terminal shows values near 0 or 4095. |
| `12_dma_memory` | DMA2 memory transfer | None | LD2 stays on when copied words match. |
| `13_flash_test` | Flash erase and word programming | None | LD2 stays on when Flash verification passes. |
| `14_crc32_test` | Software CRC32 | None | LD2 stays on when the standard test passes. |
| `15_iwdg_reset` | Independent watchdog | None | LD2 blinks for three seconds, pauses, and repeats after reset. |
| `16_can_loopback` | Internal CAN loopback | None | LD2 stays on when the received frame matches. |

## USART terminal

USART2 uses the ST-LINK virtual serial port at `/dev/ttyACM0`.

Open it at 115200 baud:

```bash
screen /dev/ttyACM0 115200
```

Press `Ctrl-A`, and then press `K` to close `screen`.

## SPI wiring

Disconnect board power before you change wires.

Connect PA7 to PA6.

The Arduino header names are usually D11 for PA7 and D12 for PA6.

The example sends bytes on PA7 and receives the same bytes on PA6.

## I2C wiring

Connect the I2C device as follows:

| Nucleo pin | I2C signal |
|---|---|
| PB8 or D15 | SCL |
| PB9 or D14 | SDA |
| 3V3 | Device power |
| GND | Device ground |

Use external pull-up resistors when the I2C module does not contain them.

The example also enables the weak internal pull-up resistors.

## ADC wiring

PA0 is the Arduino A0 pin.

Connect A0 to GND to test a low conversion value.

Connect A0 to 3V3 to test a high conversion value.

Do not connect A0 to a voltage higher than 3.3 V.

## Flash warning

The Flash example erases sector 7.

Sector 7 covers `0x08060000` through `0x0807FFFF`.

Do not run this example after the bootloader uses that sector for an application image.
