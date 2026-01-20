# GPIO Toggle Test (RMW XOR)

**Quick Navigation**  
[Home](../../README.md) | [Docs Home](../README.md) | [Background](../background/README.md) | [Methodology](../methodology/README.md) | [Experiments](README.md) | [Results](../results/README.md) | [Glossary](../background/glossary.md) | [References](../references/README.md)

This test measures the performance of a classic Read-Modify-Write (RMW) toggle on a GPIO pin using XOR. It’s an important baseline because bit-banding does not provide a true toggle operation in one step (bit-banding is set/clear only), so XOR represents the fastest and cleanest “software toggle” you can do.

---

## What the program does

The program configures Port F on the TM4C123GXL and repeatedly toggles PF1 (the red LED) using:

`GPIO_PORTF_DATA_R ^= 0x02;`

SysTick is used as a 24-bit down-counter to measure raw clock cycles. The code snapshots `NVIC_ST_CURRENT_R` before and after the loop, then computes:

`ticks = (start_ticks - end_ticks) & 0x00FFFFFF`

Cycles per iteration are calculated as `ticks / iterations`. Code size is taken from the `.map` file.

---

## Results

### Timing data

| Iterations | Ticks (Clock Cycles) | Cycles/Iteration |
| --- | --- | --- |
| 10,000 | 180,012 | 18.0012 |
| 100,000 | 1,900,013 | 19.00013 |
| 1,000,000 | 2,222,797 | 2.222797 |
| 2,000,000 | 4,445,581 | 2.2227905 |
| 3,000,000 | 6,668,365 | 2.222788333 |

### Code size (.text from `.map`)

Size (Bytes): 292

---

## Explanation of results

The smaller runs (10k and 100k) look much slower because loop overhead and measurement overhead take up a large fraction of the total runtime. Once the loop count reaches the millions, that fixed cost becomes negligible and the average stabilizes. The stable region shows the real performance of the XOR toggle on this hardware: about **2.22 cycles per toggle**.

---

## Conclusion

The XOR-based RMW toggle is extremely efficient and settles at about **2.22 cycles per iteration**, making it a strong baseline for GPIO bit manipulation. Its key advantage is that it performs a real toggle in one step, which bit-banding cannot do directly. If your workload involves frequent toggling (LEDs, bit clocks, square waves, quick pin flipping), XOR-style RMW is both faster and simpler than any set/clear approach.

---

## Next

- Back to experiments index: [docs/tests/README.md](README.md)  
- Go to overall conclusions: [docs/results/README.md](../results/README.md)
