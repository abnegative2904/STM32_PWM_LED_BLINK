#include <stdint.h>

#define RCC_APB2ENR  (*(volatile unsigned int*)0x40021018)
#define RCC_APB1ENR  (*(volatile unsigned int*)0x4002101C)
#define GPIOA_CRL    (*(volatile unsigned int*)0x40010800)
#define GPIOA_CRH    (*(volatile unsigned int*)0x40010804)
#define USART1_SR    (*(volatile unsigned int*)0x40013800)
#define USART1_DR    (*(volatile unsigned int*)0x40013804)
#define USART1_BRR   (*(volatile unsigned int*)0x40013808)
#define USART1_CR1   (*(volatile unsigned int*)0x4001380C)

#define TIM2_CR1  (*(volatile unsigned int*)0x40000000)
#define TIM2_CNT  (*(volatile unsigned int*)0x40000024)
#define TIM2_PSC  (*(volatile unsigned int*)0x40000028)
#define TIM2_ARR  (*(volatile unsigned int*)0x4000002C)
#define TIM2_CCR1 (*(volatile unsigned int*)0x40000034)
#define TIM2_CCMR1 (*(volatile unsigned int*)0x40000018)
#define TIM2_CCER  (*(volatile unsigned int*)0x40000020)

volatile int blink_rate = 0;
volatile int busy = 0;
char inputBuffer[10];
volatile int inputIndex = 0;
volatile int newData = 0;

// ---- UART Initialization ----
void uart_init(void) {
    RCC_APB2ENR |= (1 << 2);   // Enable GPIOA clock
    RCC_APB2ENR |= (1 << 14);  // Enable USART1 clock

    // PA9 (TX)
    GPIOA_CRH &= ~(0xF << (1 * 4));
    GPIOA_CRH |= (0xB << (1 * 4));

    // PA10 (RX)
    GPIOA_CRH &= ~(0xF << (2 * 4));
    GPIOA_CRH |= (0x4 << (2 * 4));

    USART1_BRR = 0x341; // Baud rate
    USART1_CR1 |= (1 << 13) | (1 << 3) | (1 << 2); // Enable USART, TX, RX
}

void uart_send_char(char c) {
    while (!(USART1_SR & (1 << 7)));
    USART1_DR = c;
}

void uart_send_string(char *str) {
    while (*str) {
        uart_send_char(*str++);
    }
}


void gpio_init(void) {
    RCC_APB2ENR |= (1 << 2);  // Enable GPIOA clock
    GPIOA_CRL &= ~(0xF << (0 * 4));
    GPIOA_CRL |= (0xB << (0 * 4)); // PA0 as alternate function push-pull
}


void timer_init(void) {
    RCC_APB1ENR |= (1 << 0); // Enable TIM2 clock
    TIM2_PSC = 800 - 1;  
    TIM2_ARR = 1000;      

    TIM2_CCMR1 |= (0x6 << 4); // PWM mode 1 on channel 1 (PA0)
    TIM2_CCER |= (1 << 0);    //PA0
    TIM2_CR1 |= (1 << 0);     // Enable timer
}

void set_blink_rate(int rate) {
    if (rate > 0 && rate <= 1000) {
        TIM2_ARR = 10000 / rate;  // Set period according to blink rate
        TIM2_CCR1 = TIM2_ARR / 2;  // 50% duty cycle
        TIM2_CNT = 0;  // Reset counter
        uart_send_string("\r\nBlink Rate Set, Enter New To Change :");
    } else {
        uart_send_string("\r\nInvalid Rate! Enter 1-1000\r\n");
    }
}

int string_to_int(char *str) {
    int result = 0;
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    return result;
}

void uart_check_input(void) {
    if (USART1_SR & (1 << 5)) {
        char ch = USART1_DR;
        if (ch >= '0' && ch <= '9') {  // Accept only no.
            if (inputIndex < sizeof(inputBuffer) - 1) {
                inputBuffer[inputIndex++] = ch;
                inputBuffer[inputIndex] = '\0';
                uart_send_char(ch);
            }
        } else if (ch == '\r' || ch == '\n') {
            if (inputIndex > 0) {
                newData = 1;
                inputIndex = 0;
            }
        }
    }
}

int main(void) {
    uart_init();
    gpio_init();
    timer_init();
    uart_send_string("\r\nEnter blink rate (1-1000):");

    while (1) {
        uart_check_input();
        if (newData) {
            newData = 0;
            int rate = string_to_int(inputBuffer);
            set_blink_rate(rate);
        }
    }
}
