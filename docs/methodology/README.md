# Methodology

**Quick Navigation**  
[Home](../../README.md) | [Docs Home](../README.md) | [Background](../background/README.md) | [Experiments](../tests/README.md) | [Results](../results/README.md) | [Setup](../setup/README.md) | [Glossary](../background/glossary.md) | [References](../references/README.md)

This page explains the methodology used to evaluate speed, code size, and atomicity for bit-banding vs Read-Modify-Write (RMW) on the TM4C123GXL.

---

## What we measured

### 1) Speed (cycle cost)

We measure how many CPU cycles an operation takes using the SysTick timer as a cycle counter, then convert that into cycles per loop iteration.

### 2) Code size

We measure code size as the number of bytes of machine code stored in flash for each test program. In practice, we record the `.text` section size in bytes from the linker `.map` file. Smaller `.text` usually means less flash used and sometimes simpler code generation.

### 3) Atomicity

We test whether an operation can lose updates when an interrupt happens at the wrong time. This is the main correctness risk with multi-step RMW on a shared word.

All raw results are recorded into a shared spreadsheet, then summarized in the docs pages under Experiments and Results.

---

## Common benchmark setup (used in most tests)

### Clock

All timing benchmarks run with the system clock set to **40 MHz** using `initSystemClockTo40Mhz();` so every project is comparable.

### SysTick timing

SysTick is configured as a 24-bit down-counter:

- `NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M;` (0x00FFFFFF)
- `NVIC_ST_CURRENT_R = 0x0;` (clear)
- enable SysTick with CPU clock

We time only the loop we care about:

- `start_ticks = NVIC_ST_CURRENT_R;`
- run the loop
- `end_ticks = NVIC_ST_CURRENT_R;`

Because SysTick counts down:

`ticks = (start_ticks - end_ticks) & 0x00FFFFFF;`

We choose loop sizes that keep the test safely under the rollover limit so the math stays valid.

### Loop sizes

Most GPIO tests use **1,000,000 iterations** so the measured time is large enough that the result is stable and not dominated by the loop itself.

Some SRAM tests use **10,000** or **100,000** iterations depending on how much work happens per loop. For example, the SRAM bit-band word write does 32 alias writes per outer loop, so it gets big fast.

---

## Keeping comparisons fair

To make the comparisons fair across projects:

- Same clock and SysTick setup
- Same loop structure for both approaches (as close as possible)
- `volatile` used on timing variables and test targets when needed so the compiler does not remove the work
- GPIO tests force Port F onto APB so the addresses match the bit-band alias math used in the bit-band GPIO project

---

## GPIO methodology (peripheral register tests)

Target: PF1 (red LED) using `GPIO_PORTF_DATA_R`.

We compare:

- **RMW set and clear:** `GPIO_PORTF_DATA_R |= mask;` and `GPIO_PORTF_DATA_R &= ~mask;`
- **Bit-banding set and clear:** write `1` or `0` to the PF1 alias word

We record three timing values per method:

1. set then clear in the same loop (two writes per loop)
2. set only
3. clear only

We also include a separate test for:

- **RMW toggle:** `GPIO_PORTF_DATA_R ^= mask;`

This is included because toggle is a common real-world operation and bit-banding does not have a direct one-step toggle.

---

## SRAM methodology (normal memory tests)

Target: a global `volatile uint32_t sram_variable;` so its address is stable and the alias address calculation stays consistent.

We test two categories:

### 1) Single-bit updates in SRAM

- **RMW bit toggle:** uses a mask and XOR on the SRAM word
- **Bit-banding bit write:** writes to one alias address for a chosen bit

These tests show the cost of bit-level updates in normal memory, not just on a peripheral register.

### 2) Whole word writes in SRAM

- **Direct word write:** `sram_variable = 0x89ABCDEF;` inside a loop
- **Bit-banding emulated word write:** writes 32 bits by doing 32 alias writes (one per bit)

This test is important because it shows what happens when bit-banding is used for the wrong job. Bit-banding is built for single-bit updates, not full 32-bit writes.

---

## Code size measurement details

For each project we record the `.text` size in bytes from the linker `.map` file.

What this means:

- `.text` is the program instructions (the actual compiled code)
- It lives in flash memory
- It is reported in bytes
- Smaller `.text` is usually better for embedded projects with limited flash

To keep code size comparisons fair:

- build all projects with the same compiler optimization settings in CCS
- record the `.text` size the same way every time

---

## Atomicity test methodology (interrupt contention)

Goal: create a realistic race condition where RMW can lose updates if an ISR interrupts at the wrong time.

One clean way to test this is a shared counter word in SRAM:

- `volatile uint32_t counter;`
- Main code repeatedly increments the counter using an RMW style operation
- SysTick ISR also increments the same counter at a steady rate

If both are incrementing correctly, the final counter value should equal:

`main_increments + isr_increments`

With RMW on the same shared word, it is possible for an interrupt to happen between the read and write steps, which can cause one increment to overwrite the other. That shows up as a final counter value that is smaller than expected.

Then we repeat the same idea using bit-banding:

- Instead of incrementing the whole counter, we design the test so the main and ISR update separate bits (or use a single-bit flag style counter), and those updates are done through alias writes.
- If the updates never get lost, that supports the claim that bit-banding avoids the read-then-write window for single-bit changes.

For this test we record:

- expected final value (based on how many times each loop ran)
- actual final value
- how many updates were lost (difference)

Timing is optional here because atomicity is primarily about correctness.
