# SRAM Single Bit Update Tests

**Quick Navigation**  
[Home](../../README.md) | [Docs Home](../README.md) | [Background](../background/README.md) | [Methods](../methodology/README.md) | [Experiments](README.md) | [Results](../results/README.md) | [Glossary](../background/glossary.md) | [References](../references/README.md)

This page compares two ways to update a single bit inside an SRAM word on the TM4C123GXL:

- RMW bit toggle using XOR (Jose and Skand)
- Bit-banding single-bit write (Ali)

Both tests use SysTick as a down-counter to measure raw clock cycles, then compute cycles per iteration. Code size is taken from the linker `.map` file.

Related: [Atomicity Tests](atomicity.md)

---

## Test A: `sram_rmw_word_write_[Jose Skand]` (single-bit XOR toggle)

### What it does

This test toggles one bit in a normal SRAM variable using a classic read-modify-write pattern (`sram_variable ^= mask`). Each loop iteration performs a read, an XOR, and a write, which makes it a good baseline for “software only” bit updates in SRAM.

### Results

**Timing data**

| Iterations | Ticks (Clock Cycles) | Cycles/Iteration |
| --- | --- | --- |
| 10,000 | 170012 | 17.0012 |
| 50,000 | 850012 | 17.00024 |
| 100,000 | 1800013 | 18.00013 |
| 500,000 | 9000013 | 18.000026 |
| 750,000 | 13500013 | 18.00001733 |
| 1,000,000 | 1222797 | 1.222797 |

**Code size (.text from `.map`)**  
Size (Bytes): 332

### Notes on the 1,000,000 row

The sudden drop at 1,000,000 iterations is not a real speedup. SysTick is a 24-bit counter, so it wraps after about 16.7 million ticks. At around 18 cycles per iteration, 1,000,000 iterations can exceed that limit and produce a wrapped tick difference. The 10k to 750k rows are the reliable range for this test.

---

## Test B: `sram_bb_bit_write_[Ali]` (bit-banding single-bit write)

### What it does

This test uses the SRAM bit-band alias region to write one specific bit (bit 0) of an SRAM variable through its alias address. The loop writes alternating 0 and 1 (`i & 1`) to the alias word, which makes the hardware set or clear that one target bit.

### Results

**Timing data**

| Iterations | Ticks (Clock Cycles) | Cycles/Iteration |
| --- | --- | --- |
| 10,000 | 180014 | 18.0014 |
| 50,000 | 900015 | 18.0003 |
| 100,000 | 1900015 | 19.00015 |
| 500,000 | 9500016 | 19.000032 |
| 750,000 | 14250016 | 19.00002133 |
| 1,000,000 | 2222800 | 2.2228 |

**Code size (.text from `.map`)**  
Size (Bytes): 340

### Notes on the 1,000,000 row

Same issue as the XOR test: SysTick wraps after about 16.7 million ticks. At about 19 cycles per iteration, 1,000,000 iterations can exceed the counter range, so the tick result wraps and the cycles/iteration value becomes misleading. The 10k to 750k rows are the reliable range for this test.

---

## Comparison (what this means)

Using the stable part of the data (500k to 750k iterations), the XOR toggle lands at about **18 cycles per iteration**, while the bit-band alias write lands at about **19 cycles per iteration**. In other words, for a single-bit update in SRAM, bit-banding was roughly 1 cycle slower per update in this setup.

Code size was very close: **332 bytes** for the XOR test and **340 bytes** for the bit-band test. That difference is small, but it still shows bit-banding can cost a little more code for address setup and alias handling.

The bigger tradeoff is not speed, it is behavior under interrupts. The XOR method is a read-modify-write update, so it can be non-atomic if an ISR touches the same word. Bit-banding is designed for clean single-bit updates and is much easier to reason about when interrupts are involved (see the atomicity page).

---

## Conclusion

For SRAM single-bit updates, the RMW XOR toggle was slightly faster and also gives a true toggle in one step. Bit-banding was slightly slower in these timing runs, but it provides a cleaner single-bit update path that is much safer when interrupts or other code may touch the same word.

---

## Next

- Back to experiments index: [docs/tests/README.md](README.md)  
- Go to overall conclusions: [docs/results/README.md](../results/README.md)
