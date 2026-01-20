# GPIO Set and Clear Tests

**Quick Navigation**  
[Home](../../README.md) | [Docs Home](../README.md) | [Background](../background/README.md) | [Methodology](../methodology/README.md) | [Experiments](README.md) | [Results](../results/README.md) | [Glossary](../background/glossary.md) | [References](../references/README.md)

This page compares two ways to drive the on-board red LED (PF1) on the TM4C123GXL:

- `gpio_rmw`: classic read-modify-write (OR to set, AND to clear) on `GPIO_PORTF_DATA_R`
- `gpio_bitband`: bit-banding alias writes to the PF1 bit

Each program measures three cases:

1) set then clear in the same loop (two operations per loop)
2) set only
3) clear only

SysTick runs as a down-counter to measure raw ticks (clock cycles), and cycles/iteration is computed as `ticks / iterations`. Code size is taken from the linker `.map` file.

Related: [GPIO Toggle Test](gpio_toggle.md) | [Atomicity Tests](atomicity.md)

---

## Test A: `gpio_rmw` (OR/AND set and clear)

**Code size (.text from `.map`)**  
Size (Bytes): 436

### Results

**Set then clear (two operations per loop)**

| Iterations | Ticks (Clock Cycles) | Cycles/Iteration |
| --- | --- | --- |
| 10,000 | 260,012 | 26.0012 |
| 50,000 | 1,300,012 | 26.00024 |
| 100,000 | 2,700,013 | 27.00013 |
| 250,000 | 6,750,013 | 27.000052 |
| 500,000 | 13,500,013 | 27.000026 |

**Set only / clear only (one operation per loop)**  
(Set-only and clear-only measured the same)

| Iterations | Ticks (Clock Cycles) | Cycles/Iteration |
| --- | --- | --- |
| 10,000 | 180,012 | 18.0012 |
| 50,000 | 900,012 | 18.00024 |
| 100,000 | 1,900,013 | 19.00013 |
| 250,000 | 4,750,013 | 19.000052 |
| 500,000 | 9,500,013 | 19.000026 |

---

## Test B: `gpio_bitband` (bit-band alias set and clear)

**Code size (.text from `.map`)**  
Size (Bytes): 416

### Results

The cycle results matched the RMW test at every iteration count in this run.

**Set then clear (two operations per loop)**

| Iterations | Ticks (Clock Cycles) | Cycles/Iteration |
| --- | --- | --- |
| 10,000 | 260,012 | 26.0012 |
| 50,000 | 1,300,012 | 26.00024 |
| 100,000 | 2,700,013 | 27.00013 |
| 250,000 | 6,750,013 | 27.000052 |
| 500,000 | 13,500,013 | 27.000026 |

**Set only / clear only (one operation per loop)**

| Iterations | Ticks (Clock Cycles) | Cycles/Iteration |
| --- | --- | --- |
| 10,000 | 180,012 | 18.0012 |
| 50,000 | 900,012 | 18.00024 |
| 100,000 | 1,900,013 | 19.00013 |
| 250,000 | 4,750,013 | 19.000052 |
| 500,000 | 9,500,013 | 19.000026 |

---

## Explanation of results

Two patterns show up clearly. First, the set-only and clear-only loops settle around 19 cycles per iteration once the iteration count is high enough, and set and clear come out the same because both are just one write to the pin. Second, the set-then-clear loop settles around 27 cycles per iteration, but that does not mean each operation is 27 cycles. It is two operations in one loop body, so the loop overhead is paid once while doing two writes, which makes the per-iteration number look better than simply doubling the set-only result. In these measurements, plain GPIO set/clear ended up essentially identical between RMW and bit-banding, so performance alone does not separate them here.

---

## Conclusion

For simple GPIO set and clear on PF1, both approaches landed at the same measured cycle costs in this setup: about 19 cycles for a single set (or clear), and about 27 cycles when doing set and clear back-to-back inside the same loop. The main difference in this pair is code size, where bit-banding was slightly smaller (416 bytes vs 436 bytes). In practice, the bigger reason to choose bit-banding is not speed here, it is the single-bit update behavior under interrupts (bit-banding is meant to be a clean single-bit operation, while RMW can be vulnerable if other code touches the same word at the wrong time).

---

## Next

- Back to experiments index: [docs/tests/README.md](README.md)  
- Go to overall conclusions: [docs/results/README.md](../results/README.md)
