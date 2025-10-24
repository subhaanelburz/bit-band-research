#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "clock.h"
#include "wait.h"


static inline void __enable_irq(void) { __asm(" cpsie i"); }
static inline void __disable_irq(void) { __asm(" cpsid i"); }



volatile uint32_t sram_variable;
volatile uint32_t interrupt_counter = 0;
#define SYSTICK_RELOAD_VALUE (10000 - 1)


void SysTick_Handler(void)
{
    interrupt_counter++;
}


int main(void)
{
    initSystemClockTo40Mhz(); // Enable system clock @ 40MHz

    // ----- SysTick Initialization -----
    NVIC_ST_RELOAD_R = SYSTICK_RELOAD_VALUE;
    NVIC_ST_CURRENT_R = 0x0;
    NVIC_ST_CTRL_R = NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_INTEN | NVIC_ST_CTRL_ENABLE;
    __enable_irq();

    volatile uint32_t start_ticks = 0;
    volatile uint32_t end_ticks = 0;
    volatile uint32_t total_ticks = 0;
    volatile uint32_t loop_count = 0;
    volatile uint32_t start_interrupts = 0;
    volatile uint32_t end_interrupts = 0;

    sram_variable = 0; // clear the variable initially


    uint32_t base_bb_address = 0x22000000 + (((uint32_t) &sram_variable - 0x20000000) * 32);


    volatile uint32_t* bit5_bb_address = (volatile uint32_t*) (base_bb_address + (5 * 4));


    // ----- The Timed Test -----
    __disable_irq();
    start_ticks = NVIC_ST_CURRENT_R;
    start_interrupts = interrupt_counter;
    __enable_irq();


    for(loop_count = 0; loop_count < 10000; loop_count++)
    {
        *bit5_bb_address = 1; // Set bit 5
    }

    // ----- Get End Time -----
    __disable_irq();
    end_ticks = NVIC_ST_CURRENT_R;
    end_interrupts = interrupt_counter;
    __enable_irq();


    // ----- Calculate Elapsed Time -----
    uint32_t reloads = end_interrupts - start_interrupts;
    total_ticks = (reloads * (SYSTICK_RELOAD_VALUE + 1)) + (start_ticks - end_ticks);


    while(1) // Breakpoint here
    {

    }

    return 0;
}
