# Docs Home

**Quick Navigation**  
[Home](../README.md) | [Background](background/README.md) | [Methodology](methodology/README.md) | [Experiments](tests/README.md) | [Results](results/README.md) | [Setup](setup/README.md) | [Glossary](background/glossary.md) | [References](references/README.md)

This is the homepage of the documentation for:

**Is Bit-Banding Worth It? An Experimental Comparison of Bit-Banding vs Read-Modify-Write (RMW) on the TM4C123GXL** (2025)

The docs are written to feel like a short research paper, but with simple wording and practical steps. If you are new to bit-banding, you will probably want to check the glossary while you read.

---

## How the docs are organized

If you want the quick version, read in this order:

1. **Background**  
   Why we did this project, what we expected going in, and what questions we wanted to answer.  
   [Go to Background](background/README.md)

2. **Methodology**  
   How we measured cycle cost, code size, and atomicity on the TM4C123GXL.  
   [Go to Methodology](methodology/README.md)

3. **Experiments**  
   A list of all benchmark programs, each with explanation, data, and takeaways.  
   [Go to Experiments](tests/README.md)

4. **Results**  
   The conclusion: what mattered most across all tests.  
   [Go to Results](results/README.md)

---

## Reproducing the benchmarks

If you want to run the tests yourself, the shortest path is:

1. Follow the CCS import and setup guide: [Setup](setup/README.md)
2. Build and flash any test project in `ccs_projects/`
3. Run it, stop at the breakpoint, and record:
   - `start_ticks`, `end_ticks`, and `ticks`
   - `.text` size from the linker `.map` file
4. Compare your data to the tables in the experiment pages and the summary in results.

---

## Repo Map

- Background: [docs/background/README.md](background/README.md)
- Glossary: [docs/background/glossary.md](background/glossary.md)
- Methodology: [docs/methodology/README.md](methodology/README.md)
- Experiments (benchmarks): [docs/tests/README.md](tests/README.md)
- Results summary: [docs/results/README.md](results/README.md)
- Setup guide: [docs/setup/README.md](setup/README.md)
- References and datasheet: [docs/references/README.md](references/README.md)
