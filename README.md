# STM32 PWM LED Blink via USART

This project allows controlling the blink rate of an LED connected to STM32 via USART. The user can enter a blink rate (1-1000) through a serial terminal, and the LED will blink accordingly using PWM.

## Hardware Requirements
- STM32 microcontroller (e.g., STM32F103C8T6)
- LED connected to **PA0** (or modify for PB2)
- USB-UART adapter (or built-in ST-Link UART)
- Serial terminal (Putty, Tera Term, etc.)

## Features
- Uses **USART1** for communication.
- Uses **TIM2** for PWM generation.
- Accepts user input for setting the LED blink rate.
- Non-blocking USART input handling.

## Pin Configuration
| Function | STM32 Pin |
|----------|----------|
| LED PWM  | PA0 (TIM2_CH1) |
| UART TX  | PA9 (USART1_TX) |
| UART RX  | PA10 (USART1_RX) |

## Code Explanation
### Initialization
- `uart_init()`: Configures USART1 for serial communication.
- `gpio_init()`: Sets PA0 as an alternate function push-pull (PWM output).
- `timer_init()`: Configures TIM2 for PWM with adjustable frequency.

### Functionality
1. The user enters a blink rate (1-1000).
2. The system calculates the PWM period and updates **TIM2_ARR**.
3. The LED blinks according to the user-defined frequency.

## How to Use
1. Flash the code onto your STM32.
2. Open a serial terminal (Baud rate: **9600**).
3. Enter a number between 1-1000.
4. The LED starts blinking at the specified rate.
5. Enter a new value to change the blink rate.

## Modifying for PB2
- Change the GPIO configuration in `gpio_init()`:
  ```c
  GPIOB_CRL &= ~(0xF << (2 * 4));
  GPIOB_CRL |= (0xB << (2 * 4)); // Set PB2 as alternate function push-pull

