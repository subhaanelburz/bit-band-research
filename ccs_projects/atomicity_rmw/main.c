#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "clock.h"
#include "wait.h"

// in this test we check and see if rmw will mess up due to race conditions
// basically checking to see if it is atomic or not

// declaring a global variable so memory address is fixed/permanent
// if declared in main, its declared in the stack in which variables are temporarily stored
// basically if in main, not guaranteed permanent address, so declared globally instead

volatile uint32_t ISR_count = 0;
volatile uint32_t shared_count = 0;
volatile uint32_t expected_total = 0;
volatile uint32_t actual_total = 0;
volatile uint32_t lost_inc = 0;

void SysTick_Handler(void)
{
    ISR_count++;
    shared_count++;
}

int main(void)
{
    initSystemClockTo40Mhz(); // Enable system clock

    // Set RELOAD for 250 us
    // 4,000,000 * (250*10^-6) = 1000
    NVIC_ST_RELOAD_R = (1000 - 1);

    // Clear Current
    NVIC_ST_CURRENT_R = 0;

    // Set Control to a 4 MHZ Clock
    NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_CLK_SRC;
    NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE | NVIC_ST_CTRL_INTEN; // enable counter and interrupt

    uint32_t i;
    for (i = 0; i < 1000000; i++)
    {
        shared_count++;
    }

    NVIC_ST_CTRL_R = 0; // disable SysTick

    // this is the value we should get for the counter
    expected_total = 1000000 + ISR_count;

    // this is the actual final count we got
    actual_total = shared_count;

    // how many increments did we lose?
    lost_inc = expected_total - actual_total;

    while(1) // set a breakpoint here to view values in memory
    {
        // loop infinitely
    }

    // return 0;
}
