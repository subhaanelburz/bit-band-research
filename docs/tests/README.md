# Experiments

**Quick Navigation**  
[Home](../../README.md) | [Docs Home](../README.md) | [Background](../background/README.md) | [Methodology](../methodology/README.md) | [Glossary](../background/glossary.md) | [References](../references/README.md) | [Results](../results/README.md)

This README contains the benchmark programs run on the TM4C123GXL. The experiments compare bit-banding vs Read-Modify-Write (RMW) across a few common situations, then summarizes the results into clear takeaways.

For the main experiments, each test links to its own page with the raw measurements, setup notes, and a short conclusion for that specific test.

---

## How this section is organized

- **Core experiments** directly answer the main question of this project and have dedicated pages.
- **Support experiments** are duplicates, learning examples, or teammate variants that are kept for completeness.

---

## Core experiments

### 1) GPIO set and clear: bit-banding vs RMW

These tests use Port F PF1 (red LED), so the comparison happens on a real peripheral register.

- **`gpio_bitband`**  
  Bit-banding alias writes to set and clear PF1 (set then clear, set-only, clear-only).  
  Details: [GPIO Set and Clear](gpio_set_clear.md)

- **`gpio_rmw`**  
  RMW using OR and AND on `GPIO_PORTF_DATA_R` (set then clear, set-only, clear-only).  
  Details: [GPIO Set and Clear](gpio_set_clear.md)

Why it matters: this is the cleanest head-to-head for set and clear on real hardware.

---

### 2) GPIO toggle: what bit-banding cannot do in one step

- **`gpio_rmw_toggle`**  
  RMW toggle using XOR on PF1.  
  Details: [GPIO Toggle](gpio_toggle.md)

Why it matters: it shows a real advantage of RMW (native toggle).

---

### 3) SRAM full word write: why bit-banding can be a bad fit

These tests use a global SRAM variable and compare one normal store vs many alias writes.

- **`sram_bb_word_write`**  
  Writes a full 32-bit value by writing 32 separate bits through alias addresses.  
  Details: [SRAM Full Word Write](sram_full_word_write.md)

- **`sram_rmw_word_write`**  
  Writes a full 32-bit value using a normal store (`sram_variable = 0x89ABCDEF`).  
  Details: [SRAM Full Word Write](sram_full_word_write.md)

Why it matters: this is the clearest “bit-banding worst case” in the project.

---

### 4) SRAM single bit updates: bit-banding vs true bit-level RMW

These tests focus on a single bit update, which is the situation bit-banding is meant for.

- **`sram_bb_bit_write_[Ali]`**  
  Repeated writes to one SRAM bit alias address (single-bit test).  
  Included as the single-bit bit-band baseline.

- **`sram_rmw_word_write_[Jose_Skand]`**  
  Toggles one bit using an XOR mask (`sram_variable ^= mask`).  
  Included as the single-bit bit-level RMW comparison.

Details: [SRAM Single Bit Update](sram_single_bit_update.md)

Why it matters: it is the most fair speed comparison for single-bit work.

---

### 5) Atomicity: interrupts and race conditions

These tests check correctness when the main code and an ISR touch the same data.

- **`atomicity_rmw`**  
  Main and SysTick ISR both increment the same shared counter.  
  Metric: lost increments (`lost_inc`).  
  Details: [Atomicity Tests](atomicity.md)

- **`atomicity_bitband`**  
  Main and SysTick ISR update different bits in the same word using alias writes, with a fail counter check.  
  Metric: `fail_count` and final state checks.  
  Details: [Atomicity Tests](atomicity.md)

Why it matters: this is where bit-banding shows its strongest correctness advantage.

---

## Support experiments

These are kept in the repo for completeness, but they do not need full write-ups.

- **`sample_systick`**  
  Simple SysTick interrupt demo for learning. Not a benchmark.

- **`sram_bb_word_write_[Jose_skand]`**  
  Despite the name, this is another single-bit bit-band write variant. It overlaps with Ali’s single-bit test.

- **`sram_rmw_bit_write_Ali`**  
  A plain word store baseline (`sram_variable = 0x1`). Useful as a quick sanity check, but it overlaps with `sram_rmw_word_write` as “normal store cost”.

---

## Results

For the overall conclusions and combined results across all experiments, see:  
[Results and Conclusions](../results/README.md)
