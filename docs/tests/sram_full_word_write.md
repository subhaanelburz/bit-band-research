# SRAM Full Word Write Tests

**Quick Navigation**  
[Home](../../README.md) | [Docs Home](../README.md) | [Background](../background/README.md) | [Methodology](../methodology/README.md) | [Experiments](README.md) | [Results](../results/README.md) | [Glossary](../background/glossary.md) | [References](../references/README.md)

This page compares two ways to write a full 32-bit value to SRAM on the TM4C123GXL:

- A normal word store (baseline)
- A “full word write” built out of 32 bit-band alias writes (worst case for bit-banding)

Both tests use SysTick as a down-counter to measure raw clock cycles, then compute cycles per iteration. Code size is taken from the linker `.map` file.

Related: [SRAM Single Bit Update Tests](sram_single_bit_update.md) | [Atomicity Tests](atomicity.md)

---

## Test A: `sram_rmw_word_write` (normal 32-bit store)

### What it does

This test writes a full 32-bit constant (`0x89ABCDEF`) into a global SRAM variable using a normal C assignment. Each loop iteration is basically one store to SRAM, so this is the baseline for “how fast can we write a word”.

### Results

| Iterations | Ticks (Clock Cycles) | Cycles/Iteration |
| --- | --- | --- |
| 10,000 | 150,012 | 15.0012 |
| 50,000 | 750,012 | 15.00024 |
| 100,000 | 1,600,013 | 16.00013 |

**Code size (.text from `.map`)**  
Size (Bytes): 324

### Notes

The cycles/iteration stabilizes around ~15–16 cycles per word write. Small variation is normal at these loop sizes because you’re still paying some fixed setup cost (loop overhead, timer snapshots, etc.), but the important point is that the baseline is “tens of cycles”, not hundreds.

---

## Test B: `sram_bb_word_write` (32 alias writes per word)

### What it does

This test writes the same 32-bit constant (`0x89ABCDEF`) into SRAM using bit-banding. Since bit-banding can only set/clear a single bit at a time, the code rebuilds a word by:

1) Pointing at the alias address for bit 0 of the SRAM variable  
2) Writing 32 alias words in a row (bit 0 through bit 31)

So one “word write” here is intentionally implemented as 32 single-bit alias writes.

### Results

| Iterations | Ticks (Clock Cycles) | Cycles/Iteration | Cycles per bit |
| --- | --- | --- | --- |
| 1,000 | 850,011 | 850.011 | 26.56284375 |
| 2,500 | 2,127,512 | 851.0048 | 26.5939 |
| 5,000 | 4,255,012 | 851.0024 | 26.593825 |
| 7,500 | 6,382,512 | 851.0016 | 26.5938 |
| 10,000 | 8,510,012 | 851.0012 | 26.5937875 |

**Code size (.text from `.map`)**  
Size (Bytes): 376

### Notes

The results stabilize very cleanly around ~851 cycles per 32-bit word write. The “cycles per bit” value (~26.6) lines up with the structure of the test: a full word write is 32 separate alias writes, so the total cost is basically 32 × (cost per bit) plus small loop overhead.

---

## Comparison (what this means)

This is the biggest performance gap in the whole project, and it makes sense mechanically.

A normal SRAM store updates all 32 bits in one instruction, landing around **15–16 cycles per word** in this measurement setup. The bit-band “full word write” has to do **32 separate single-bit updates**, landing around **851 cycles per word**.

That’s roughly **~50–60× slower** for the bit-band approach in this specific “write a full 32-bit value” scenario.

Code size also moved up: **324 bytes** (normal store) vs **376 bytes** (bit-band word write). The extra code comes from address setup and the inner loop that walks across 32 alias words.

The takeaway is simple: bit-banding is great when you truly need a single-bit set/clear, but it is a bad fit for multi-bit or full-word updates.

---

## Conclusion

For writing a full 32-bit value in SRAM, the normal word store is dramatically faster and slightly smaller in code size. The bit-band version is intentionally a worst-case example: it turns one word write into 32 separate operations, which is why it costs hundreds of cycles. If your goal is performance (or just a clean implementation) for multi-bit updates, bit-banding is not the right tool here.

---

## Next

- Back to experiments index: [docs/tests/README.md](README.md)  
- Go to overall conclusions: [docs/results/README.md](../results/README.md)
