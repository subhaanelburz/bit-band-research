# References

**Quick Navigation**  
[Home](../../README.md) | [Docs Home](../README.md) | [Background](../background/README.md) | [Methodology](../methodology/README.md) | [Experiments](../tests/README.md) | [Results](../results/README.md) | [Glossary](../background/glossary.md)

This page contains the sources used for the literature review for:

**Is Bit-Banding Worth It? An Experimental Comparison of Bit-Banding vs Read-Modify-Write (RMW) on the TM4C123GXL** (2025)

---

## Literature Review Sources

### Texas Instruments. TM4C123GH6PM Microcontroller Datasheet (Tiva C Series)

Local copy: `docs/references/tm4c_datasheet.pdf`  
**Summary:** Primary board documentation for the TM4C123GH6PM. Includes the bit-banding memory map, supported address ranges for SRAM and peripheral bit-banding, and the alias address mapping examples used in this project.  
**Relevance:** This is the main source used to confirm the TM4C bit-band region ranges, verify alias address calculations, and ensure our tests target valid SRAM and peripheral address space on the TM4C123GXL.

### Intel. Atomic Read Modify Write Primitives for I/O Devices. Intel Corporation, 2008

Link: <https://www.intel.com/content/dam/doc/white-paper/atomic-read-modify-write-primitives-i-o-devices-paper.pdf>  
**Summary:** Discusses the challenges of using standard RMW operations for device registers, especially when reads and writes do not map cleanly to the true hardware state. The paper proposes hardware primitives that ensure atomicity for I/O operations.  
**Relevance:** Even though RMW is simple, it is not always reliable in hardware contexts, which matches what we investigate on the TM4C board. This connects directly to our atomicity metric, since RMW sequences can be non-atomic while bit-banding provides atomic single-bit updates.

### Ricardo Jesus and Michèle Weiland. 2023. A Study on the Performance Implications of AArch64 Atomics

In *High Performance Computing: 38th International Conference, ISC High Performance 2023, Hamburg, Germany, May 21-25, 2023, Proceedings.* Springer-Verlag, Berlin, Heidelberg, 279-296.  
Link: <https://doi.org/10.1007/978-3-031-32041-5_15>  
**Summary:** An experimental study of how different atomic instructions on ARM AArch64 affect performance in high-performance computing workloads. It compares the costs of atomic operations and discusses scaling behavior.  
**Relevance:** This is not Cortex-M4, but it shows why atomic operations should be measured rather than assumed to be “better” by default. It supports our approach of timing atomic mechanisms (bit-banding) and comparing them against standard RMW in real benchmarks.

### Yangtao, Cheng. How to Use Bit-band and BME on the KE04 and KE06 Subfamilies. NXP Application Note AN4838, 2013

Link: <https://www.nxp.com/docs/en/application-note/AN4838.pdf>  
**Summary:** Explains how NXP microcontrollers implement bit-banding and the Bit Manipulation Engine, with examples of atomic bit operations and a discussion of efficiency compared to plain C RMW sequences.  
**Relevance:** Provides concrete examples of hardware-assisted bit manipulation reducing instruction count and avoiding race conditions. This supports the idea that bit-level hardware mechanisms are especially valuable for atomicity compared to multi-instruction RMW.

### Khan Shaikhul Hadi, Naveed Ul Mustafa, Mark Heinrich, and Yan Solihin. 2025. Hardware Support for Durable Atomic Instructions for Persistent Parallel Programming

In *Proceedings of the 60th Annual ACM/IEEE Design Automation Conference (DAC '23).* IEEE Press, 1-6.  
Link: <https://doi-org.ezproxy.uta.edu/10.1109/DAC56929.2023.10247729>  
**Summary:** Proposes durable atomic instructions (DAIs) that make memory updates atomic and persistent by extending the MESI protocol, with the goal of improving reliability and performance compared to software-managed atomics.  
**Relevance:** Shows a broader trend: hardware-backed atomic operations can be more efficient and reliable than software-based RMW sequences. This aligns with our focus on bit-banding as a hardware mechanism for atomic single-bit updates.

### ARM. About Bit-Banding. ARM Developer Documentation, 2025

Link: <https://developer.arm.com/documentation/100165/0201/Programmers-Model/Bit-banding/About-bit-banding>  
**Summary:** Official ARM documentation describing bit-banding and how alias addresses are formed and used.  
**Relevance:** Primary source used to verify alias address calculations and ensure our implementation matches the intended ARM memory model behavior.

---

## How These Sources Connect to the Project

- **Atomicity motivation:** Intel and DAC literature emphasize that multi-instruction RMW sequences can cause correctness issues in low-level or concurrent contexts, which supports testing atomicity explicitly.
- **Measurement mindset:** The AArch64 atomics study reinforces the need to quantify the cost of atomic operations rather than relying on assumptions.
- **Correct implementation:** ARM and NXP documentation help ensure alias addressing and bit-level operations are implemented correctly before comparing performance.

---

## Citation for this Repository

If you cite this project in a report, presentation, or academic submission:

**Elburz, S. (2025).** *Is Bit-Banding Worth It? An Experimental Comparison of Bit-Banding vs Read-Modify-Write (RMW) on the TM4C123GXL* [GitHub repository].
