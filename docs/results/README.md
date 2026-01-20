# Results and Conclusions

**Quick Navigation**  
[Home](../../README.md) | [Docs Home](../README.md) | [Background](../background/README.md) | [Methodology](../methodology/README.md) | [Experiments](../tests/README.md) | [Glossary](../background/glossary.md) | [References](../references/README.md)

This page ties together the entire project and answers the main question:

**Is bit-banding worth it on the TM4C123GXL, compared to normal read-modify-write (RMW)?**

Short version: it depends. Bit-banding is not a cheat code, but it is still worth using when you need safe single-bit updates.

---

## Hypothesis check

Before measuring anything, the guess was:

- Bit-banding would not be a big speed upgrade over RMW on the TM4C123GXL.
- Bit-banding might be slightly smaller in code size in some cases.
- Bit-banding should be better for atomicity, because it does a single-bit update without the read step.
- Overall: bit-banding would not be faster, but could still be worth it in the right situations.

**Verdict:** this held up.

---

## Summary of experiments and outcomes

The experiments were split into five main parts. Each page below contains the raw tables, code size, and per-test conclusions.

### 1) GPIO toggle (RMW only)

Page: [GPIO Toggle](../tests/gpio_toggle.md)

- RMW toggle using XOR reached a stable cost of about **2.22 cycles per toggle** at high iteration counts.
- This is a key advantage of RMW: it can toggle in one operation, while bit-banding cannot toggle directly.

### 2) GPIO set and clear (RMW vs bit-banding)

Page: [GPIO Set and Clear](../tests/gpio_set_clear.md)

- Single set or single clear stabilized around **19 cycles per iteration**.
- Set then clear in one loop stabilized around **27 cycles per iteration** (two operations per loop).
- In these measurements, **bit-banding and RMW were effectively the same speed** for GPIO set and clear.
- Code size was close, with bit-banding slightly smaller in this specific pair.

### 3) SRAM single-bit update (RMW bit toggle vs bit-banding bit write)

Page: [SRAM Single Bit Update](../tests/sram_single_bit_update.md)

- The stable part of the data showed:
  - RMW XOR toggle was about **18 cycles per iteration**
  - Bit-band alias write was about **19 cycles per iteration**
- So for SRAM single-bit updates, **RMW was slightly faster** in this setup.
- Code size was also extremely close between the two.

### 4) SRAM full 32-bit write (RMW word write vs bit-banding emulated word write)

Page: [SRAM Full Word Write](../tests/sram_full_word_write.md)

- RMW baseline word write came out around **15 to 16 cycles per iteration**.
- Bit-banding "word write" (32 alias writes per word) came out around **851 cycles per iteration**, or about **26.6 cycles per bit**.
- This is the clearest result in the project:
  - **Bit-banding is not meant for multi-bit writes.**
  - For full words, it is massively slower and not worth it.

### 5) Atomicity (bit-banding vs RMW under interrupts)

Page: [Atomicity Tests](../tests/atomicity.md)

- Bit-banding test:
  - Pass was always true
  - Fail count stayed at 0 across multiple timings and iteration counts
- RMW atomicity test:
  - Lost increments happened every run
  - Failures scaled with loop size (example: tens of lost increments at 100k, hundreds at 1M)
- This supports the biggest reason bit-banding exists:
  - **it provides a clean single-bit update path that avoids RMW race conditions**

---

## Final conclusions: when bit-banding is worth it

### Bit-banding is worth it when:

- You need a **safe single-bit update** to a shared word that might be touched by an ISR.
- You are dealing with **memory-mapped peripheral registers** where you want to change one bit without risking other bits.
- Correctness matters more than micro-optimizing 1 or 2 cycles.
- You want code that is easier to reason about in interrupt-heavy firmware.

### RMW is better when:

- You need a **true toggle** (XOR wins here, bit-banding does not have a direct toggle).
- You are updating multiple bits, or writing full words.
- You are in a simple context where interrupts do not touch the same word, or you already protect the operation (disable interrupts, critical sections, etc.).
- You care more about keeping code portable across chips (bit-banding is not available on all Cortex-M parts).

---

## What this project confirmed

- Bit-banding was not a big speed upgrade over RMW in realistic usage.
- Code size differences were small and depended on the exact test.
- The real win for bit-banding was atomicity, not speed.
- Using bit-banding as a replacement for normal word writes is a bad idea.

---

## Limitations and notes

- Measurements use SysTick as a cycle counter, so loop overhead matters at small iteration counts.
- Some tests show SysTick wrap issues if the run is too long for the 24-bit counter range.
- These results are for the TM4C123GXL at the chosen configuration and toolchain. Other chips and compilers can shift exact cycle counts.

---

## Practical takeaway

If you are writing embedded code on the TM4C123GXL:

- Use **RMW** for normal work, especially toggling and multi-bit changes.
- Use **bit-banding** when you truly need safe single-bit updates without race conditions.
- Do not use bit-banding to emulate full word writes unless you want to burn cycles for no benefit.

---

## Backlinks

- Back to experiments index: [docs/tests/README.md](../tests/README.md)  
- Back to docs home: [docs/README.md](../README.md)
