# Atomicity Tests

**Quick Navigation**  
[Home](../../README.md) | [Docs Home](../README.md) | [Background](../background/README.md) | [Methods](../methodology/README.md) | [Experiments](README.md) | [Results](../results/README.md) | [Glossary](../background/glossary.md) | [References](../references/README.md)

This page covers the atomicity part of the project. Both tests use a SysTick interrupt to create contention between an ISR and main code, then check whether the shared state stays correct.

Note about timing: these tests set SysTick `CLK_SRC = 0`, which selects `PIOSC / 4 = 4 MHz`, so the SysTick period math is based on a 4 MHz SysTick clock.

---

## Atomicity RMW (expected to fail)

This test checks what happens when main code and the SysTick ISR both do `shared_count++` on the same variable. An increment is not one single hardware action. It is a read, add, and write. If the interrupt fires at the wrong time, one increment can overwrite the other, and the final count ends up smaller than it should be.

At the end of the run:

- `expected_total = loop_iterations + ISR_count`
- `actual_total = shared_count`
- `lost_inc = expected_total - actual_total`

If `lost_inc > 0`, then at least one increment was lost.

### Results

| Loop iterations | Trial 1 lost_inc | Trial 2 lost_inc | Trial 3 lost_inc | Average lost_inc |
| --- | --- | --- | --- | --- |
| 100,000 | 23 | 24 | 32 | 26.33 |
| 500,000 | 160 | 150 | 207 | 172.33 |
| 1,000,000 | 318 | 296 | 314 | 309.33 |

### Conclusion

The nonzero `lost_inc` values show that `shared_count++` is not atomic when both main code and an ISR update the same variable. The exact failure count changes slightly each run because the interrupt hits at different times, but the outcome is consistent. Without protection (disabling interrupts around the update, using an atomic method, or changing the design), a shared counter like this can be wrong even if the code looks simple.

---

## Atomicity bit-banding (expected to pass)

This test uses one shared SRAM word (`sram_variable`) and updates it using bit-band alias addresses. Main code repeatedly sets and clears bit 0 through its alias address, while the SysTick ISR repeatedly sets and clears bit 1 through its alias address. Both contexts also read back through the alias address and count any mismatch in `fail_count`.

At the end of the run:

- `pass = (ISR_count > 0) && (final_result == 0)`
- `fail_count` should stay at 0 if the alias reads always match what was written

### Results

Across all tested settings, the outcome was the same:

| SysTick period | Loop iterations | pass | fail_count |
| --- | --- | --- | --- |
| 250 us | 100,000 | 1 | 0 |
| 250 us | 500,000 | 1 | 0 |
| 250 us | 1,000,000 | 1 | 0 |
| 50 us | 100,000 | 1 | 0 |
| 50 us | 500,000 | 1 | 0 |
| 50 us | 1,000,000 | 1 | 0 |

### Conclusion

This test supports that bit-banding provides reliable single-bit updates under interrupts. Every run had `fail_count = 0`, `pass = 1`, and ended with `final_result = 0`, meaning the ISR ran and the shared word stayed consistent. Compared to the RMW counter test, this shows why bit-banding is useful when main code and interrupts may touch the same word.

---

## Next

- Back to experiments index: [docs/tests/README.md](README.md)  
- Go to overall conclusions: [docs/results/README.md](../results/README.md)
