#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "clock.h"
#include "wait.h"

volatile uint32_t ISR_count = 0;
volatile uint32_t sram_variable = 0;
volatile uint32_t fail_count = 0;
volatile uint32_t *isr_bit_alias_ptr;

void SysTick_Handler(void)
{
    ISR_count++;
    *isr_bit_alias_ptr = 1;

    if (*isr_bit_alias_ptr != 1)
    {
        fail_count++;
    }

    *isr_bit_alias_ptr = 0;
}

int main(void)
{
    initSystemClockTo40Mhz(); // Enable system clock

    // alias address for bit 0 of sram_variable
    volatile uint32_t alias_base_addr = 0x22000000 + (((uint32_t)&sram_variable - 0x20000000) * 32);

    // create pointer to the alias address for easy control
    volatile uint32_t *main_bit0_ptr = (volatile uint32_t *)alias_base_addr;

    // set the ISR pointer variable to alias address for bit 1 (adds 4 since its pointer arithmetic)
    isr_bit_alias_ptr = main_bit0_ptr + 1;

    // Set RELOAD for 250 us
    // 4,000,000 * (250*10^-6) = 1000
    NVIC_ST_RELOAD_R = (1000 - 1);

    // Clear Current
    NVIC_ST_CURRENT_R = 0;

    // Set Control to a 4 MHZ Clock
    NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_CLK_SRC;
    NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE | NVIC_ST_CTRL_INTEN; // enable counter and interrupt

    while (ISR_count == 0) {}

    uint32_t i;
    for (i = 0; i < 100000; i++)
    {
        *main_bit0_ptr = 1;

        if (*main_bit0_ptr != 1)
        {
            fail_count++;
        }

        *main_bit0_ptr = 0;
    }

    NVIC_ST_CTRL_R = 0; // disable SysTick

    volatile uint32_t final_result = sram_variable;
    volatile uint32_t pass = (ISR_count > 0) && (final_result == 0);

    while (1) // set a breakpoint here to view values in memory
    {
        // loop infinitely
    }

	// return 0;
}
