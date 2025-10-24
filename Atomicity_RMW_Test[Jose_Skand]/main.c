#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "clock.h"
#include "wait.h"

//These are the global interrupt variables
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
    initSystemClockTo40Mhz();

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


    __disable_irq();
    start_ticks = NVIC_ST_CURRENT_R;
    start_interrupts = interrupt_counter;
    __enable_irq();


    for(loop_count = 0; loop_count < 10000; loop_count++)
    {
        sram_variable = 0x89ABCDEF;
    }


    __disable_irq();
    end_ticks = NVIC_ST_CURRENT_R;
    end_interrupts = interrupt_counter;
    __enable_irq();



    uint32_t reloads = end_interrupts - start_interrupts;

    // Total ticks = (Number of reloads * Ticks per reload) + (Start value - End value)
    // Note: (SYSTICK_RELOAD_VALUE + 1) is the number of ticks per period (e.g., 10,000)
    total_ticks = (reloads * (SYSTICK_RELOAD_VALUE + 1)) + (start_ticks - end_ticks);


    while(1) // set a breakpoint here to view values
    {

    }

    return 0;
}
