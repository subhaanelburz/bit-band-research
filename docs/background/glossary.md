# Glossary

**Quick Navigation**  
[Home](../../README.md) | [Docs Home](../README.md) | [Background](README.md) | [Methodology](../methodology/README.md) | [Experiments](../tests/README.md) | [Results](../results/README.md) | [References](../references/README.md)

This glossary explains the main terms used in this project as simple as possible. It is written for readers who may be new to embedded systems, memory-mapped I/O, or microcontroller performance measurement.

## Contents

- [Glossary](#glossary)
  - [Contents](#contents)
  - [Bit-banding](#bit-banding)
  - [Bit-band region](#bit-band-region)
  - [Alias region](#alias-region)
  - [Alias word](#alias-word)
  - [SRAM](#sram)
  - [Peripheral memory and memory-mapped I-O](#peripheral-memory-and-memory-mapped-i-o)
  - [Word, byte, and halfword](#word-byte-and-halfword)
  - [Bit-band address formula](#bit-band-address-formula)
  - [Bit-band mapping examples](#bit-band-mapping-examples)
    - [Example 1](#example-1)
    - [Example 2](#example-2)
    - [Example 3](#example-3)
    - [Example 4](#example-4)
  - [Read-Modify-Write (RMW)](#read-modify-write-rmw)
  - [Set, clear, and toggle](#set-clear-and-toggle)
  - [Atomicity](#atomicity)
  - [Race condition](#race-condition)
  - [Interrupt ISR and SysTick](#interrupt-isr-and-systick)
  - [Ticks / clock cycles](#ticks--clock-cycles)
  - [SysTick rollover](#systick-rollover)
  - [Loop overhead](#loop-overhead)
  - [Code size .text and .map file](#code-size-text-and-map-file)
  - [GPIO](#gpio)
  - [Volatile](#volatile)

---

## Bit-banding

Bit-banding is a hardware feature on some ARM Cortex-M chips that lets you change one **single bit** by writing to a special address called an **alias address**.

Normally, to change one bit inside a 32-bit value, software usually does:

1. Read the whole 32-bit value
2. Change one bit in code
3. Write the whole 32-bit value back

Bit-banding gives you a shortcut. You write to an alias address and the hardware updates just that one bit for you.

So, bit-banding uses a special set of addresses, called alias addresses, that act like controls for individual bits inside normal memory such as SRAM or peripheral registers.

The real data still lives in normal memory, and bits do not get their own storage location. Instead, when software writes to an alias address, the hardware detects that the address belongs to the bit-band alias region, decodes it using fixed address math, and then sets or clears the corresponding bit in the original memory location.

Nothing is actually stored at the alias address itself. Writing a value of 0 clears the target bit, writing a value of 1 sets the target bit, and reading from the alias address returns either 0 or 1 depending on the current state of that bit.

Bit-banding only works for specific memory regions, not all of SRAM or all peripherals.

On the TM4C, only a limited SRAM region and a limited peripheral region support bit-banding, even though the alias address space is much larger.

The alias region exists only to trigger hardware behavior and is reserved by the ARM memory map, meaning it should never be treated as normal memory.

Because a bit-band operation is performed as a single hardware action with no read-modify-write step, it provides atomic single-bit set and clear operations.

This is the main advantage of bit-banding when code and interrupts may access the same word at the same time.

---

## Bit-band region

A bit-band region is the real memory range where the bit you care about actually lives.

On the TM4C, there are two main bit-band regions:

- SRAM bit-band region (RAM area)
- Peripheral bit-band region (hardware register area)

Bit-banding does not mean the bit has its own real memory location. The bit still lives inside a normal byte or word in the bit-band region.

---

## Alias region

An alias region is a special mapped address range. Each word address in the alias region points to one specific bit in the real bit-band region.

Think of it like a remote control:

- The alias address is what you write to
- The real bit inside SRAM or a peripheral register is what actually changes

On the TM4C:

- SRAM alias base is `0x2200.0000`
- Peripheral alias base is `0x4200.0000`

---

## Alias word

An alias word is a 32-bit word inside the alias region. Even though it is a word, it controls just one bit in the real memory.

Basic rule:

- Write `0` to the alias word to clear the bit
- Write `1` to the alias word to set the bit
- Reading the alias word gives back `0` or `1`

---

## SRAM

SRAM is the normal RAM on the microcontroller. It is where your data lives while the program runs.

Examples:

- global variables
- local variables on the stack
- arrays and buffers

In this project we test SRAM because it is normal memory, so it helps compare bit-banding vs RMW on variables, not just on hardware registers.

---

## Peripheral memory and memory-mapped I-O

Peripherals are hardware parts like GPIO, timers, UART, etc. On a microcontroller, these are controlled using addresses like they are memory. This is called memory-mapped I-O.

Example idea:

- a GPIO register lives at an address in the `0x4000.0000` range
- writing to that address changes the pin state

So peripheral memory is not RAM. It is a set of addresses that control hardware.

---

## Word, byte, and halfword

These are common data sizes:

- Byte: 8 bits
- Halfword: 16 bits
- Word: 32 bits

The TM4C is a 32-bit microcontroller, so many registers and alias accesses are done as 32-bit words.

---

## Bit-band address formula

This is the format used in our code comments and labs:

Bit Band Address Formula  
Bit_Band_Address = Bit_Band_Base + ( (Byte_Offset) * 32 ) + ( (Bit_Number) * 4 )  
Bit_Band_Address = Bit_Band_Base + ( (OG_Address - OG_Address_Base) * 32 ) + ( (Bit_Number) * 4 )

Extra notes:

- 32 in hex is 0x20
- 4 in hex is 0x4

Example calculation  
0x2200.0000 + ( (0x2000.1000 - 0x2000.0000) * 32 ) + (0 * 4)  
= 0x2200.0000 + 0x1000 * 0x20  
= 0x2200.0000 + 0x2.0000  
= 0x2202.0000

What each name means:

- Bit_Band_Base: start of the alias region (`0x2200.0000` for SRAM alias, `0x4200.0000` for peripheral alias)
- OG_Address: the real address where the bit lives
- OG_Address_Base: start of the real bit-band region (`0x2000.0000` for SRAM bit-band, `0x4000.0000` for peripheral bit-band)
- Bit_Number: which bit you want

---

## Bit-band mapping examples

These examples are based on the TM4C datasheet mapping section, rewritten into the same format used above.

<p align="center">
  <img src="images/bit-band.png" width="600">
</p>

### Example 1

Alias word at `0x23FF.FFE0` maps to bit 0 of the bit-band byte at `0x200F.FFFF`

0x23FF.FFE0  
= 0x2200.0000 + ( (0x200F.FFFF - 0x2000.0000) * 32 ) + (0 * 4)  
= 0x2200.0000 + ( 0x000F.FFFF * 32 ) + 0  
= 0x2200.0000 + ( 0x000F.FFFF * 0x20 ) + 0  
= 0x23FF.FFE0

### Example 2

Alias word at `0x23FF.FFFC` maps to bit 7 of the bit-band byte at `0x200F.FFFF`

0x23FF.FFFC  
= 0x2200.0000 + ( (0x200F.FFFF - 0x2000.0000) * 32 ) + (7 * 4)  
= 0x2200.0000 + ( 0x000F.FFFF * 0x20 ) + (7 * 0x4)  
= 0x23FF.FFFC

### Example 3

Alias word at `0x2200.0000` maps to bit 0 of the bit-band byte at `0x2000.0000`

0x2200.0000  
= 0x2200.0000 + ( (0x2000.0000 - 0x2000.0000) * 32 ) + (0 * 4)  
= 0x2200.0000 + ( 0 * 0x20 ) + 0  
= 0x2200.0000

### Example 4

Alias word at `0x2200.001C` maps to bit 7 of the bit-band byte at `0x2000.0000`

0x2200.001C  
= 0x2200.0000 + ( (0x2000.0000 - 0x2000.0000) * 32 ) + (7 * 4)  
= 0x2200.0000 + 0 + (7 * 0x4)  
= 0x2200.001C

---

## Read-Modify-Write (RMW)

RMW is the common software method to change a bit inside a 32-bit value.

It is basically:

1. Read the 32-bit word
2. Change the bit using OR, AND, or XOR
3. Write the 32-bit word back

Examples:

- Set bit n: `x |= (1u << n)`
- Clear bit n: `x &= ~(1u << n)`
- Toggle bit n: `x ^= (1u << n)`

RMW is simple and flexible, but it can have problems if an interrupt happens between the read and the write.

---

## Set, clear, and toggle

These are the basic single-bit actions:

- Set: force a bit to 1
- Clear: force a bit to 0
- Toggle: flip a bit (0 becomes 1, 1 becomes 0)

RMW supports toggle directly with XOR.  
Bit-banding supports set and clear directly by writing 1 or 0 to the alias word.

---

## Atomicity

Atomicity means something happens as one clean action, so it cannot be half done if an interrupt happens.

In this project:

- A bit-band set or clear is treated like one hardware action on that bit
- A software RMW update can be interrupted between the read and write, which can cause problems

---

## Race condition

A race condition happens when two pieces of code touch the same data and the result depends on timing.

Common example with RMW:

- Main code reads a register
- Interrupt runs and changes that same register
- Main code writes back an older value and accidentally removes what the interrupt changed

Our atomicity tests are designed to show this.

---

## Interrupt ISR and SysTick

Interrupt: a hardware event that pauses normal code to run a special function.

ISR: Interrupt Service Routine, the function that runs during an interrupt.

SysTick: a timer inside the Cortex-M core.  
We use SysTick for:

- Timing tests (as a down-counter)
- Atomicity tests (as a periodic interrupt)

---

## Ticks / clock cycles

A tick is one step of the SysTick counter.

If SysTick is running from the CPU clock:

- 1 tick equals 1 CPU clock cycle

So:

- cycles per operation = ticks divided by the number of loop runs

---

## SysTick rollover

SysTick is 24-bit on Cortex-M. That means it counts down and wraps around after about 16.7 million ticks.

If a test runs too long, the counter can wrap and your tick math will be wrong. That is why our tests use safe loop sizes.

---

## Loop overhead

Every loop has extra work that is not the operation you care about, like:

- adding 1 to the loop counter
- checking the loop condition
- branching back to the top

If the loop is short, this extra work can make results look slower than they really are.  
If the loop is very long, the average settles and becomes more trustworthy.

---

## Code size .text and .map file

.text: the part of the program that contains the machine code instructions.

.map file: a linker output file that shows section sizes and memory layout.

We use the .map file to record .text size in bytes for each test, so we can compare code size between bit-banding and RMW.

---

## GPIO

GPIO means General Purpose Input Output.

GPIO pins are simple digital pins that can be set high or low. On TM4C, the GPIO control registers live in peripheral memory, so GPIO tests are a realistic place to compare bit-banding vs RMW.

---

## Volatile

volatile tells the compiler:
do not remove or “simplify” reads and writes, because the value may change in a way the compiler cannot see.

We use volatile for:

- hardware registers
- timing variables we inspect in the debugger
- shared variables that interrupts can touch

This helps keep our timing measurements and bit-banding logic accurate.
