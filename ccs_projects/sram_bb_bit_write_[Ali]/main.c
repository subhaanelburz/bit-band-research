#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "clock.h"
#include "wait.h"

/*
 * This is the SRAM bit-banding test, where we are testing writing an entire 32-bit word
 * to a variable in SRAM using bit-banding
 */

// declaring a global variable so memory address is fixed/permanent
// if declared in main, its declared in the stack in which variables are temporarily stored
// basically if in main, not guaranteed permanent address, so declared globally instead

volatile uint32_t sram_variable;

int main(void)
{
    initSystemClockTo40Mhz(); // Enable system clock

    NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M; // set reload value to the max, 0x00FFFFFF

    NVIC_ST_CURRENT_R = 0x0; // set current to any value

    // enable clock to 40 MHz, and enable the counter
    NVIC_ST_CTRL_R |= NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_ENABLE;

    volatile uint32_t start_ticks = 0;
    volatile uint32_t end_ticks = 0;
    volatile uint32_t ticks = 0;

    // the counter counts down every clock cycle, and the final value of ticks
    // will be the # of clock cycles it takes to complete the code between the
    // start ticks and end ticks

    volatile uint32_t i = 0;
    volatile uint32_t j = 0;

    sram_variable = 0; // clear the variable initially

    // calculate the base bit banded address (bit 0, thats why there is no 0*4)
    volatile uint32_t base_bb_address = 0x22000000 + (((uint32_t) &sram_variable - 0x20000000) * 32);

    start_ticks = NVIC_ST_CURRENT_R; // start the timer

    volatile uint32_t* bb_address = (volatile uint32_t*) base_bb_address;


    for(i = 0; i < 100000; i++)
    {

        *bb_address = i & 1;
    }

    end_ticks = NVIC_ST_CURRENT_R; // end the timer

    ticks = (start_ticks - end_ticks) & 0x00FFFFFF; // will be the raw number of clock cycles / ticks

    while(1) // set a breakpoint here to view values in memory
    {
        // loop infinitely
    }

    return 0;
}
