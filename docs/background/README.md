# Background

**Quick Navigation**  
[Home](../../README.md) | [Docs Home](../README.md) | [Methodology](../methodology/README.md) | [Experiments](../tests/README.md) | [Results](../results/README.md) | [Glossary](glossary.md) | [References](../references/README.md) | [Setup](../setup/README.md)

This page is the "why" behind the project. If you are new here, read this first. It explains what we were trying to learn, what we expected to happen, and what questions we wanted to answer.

---

## Why we did this project

I joined ACM Research and wanted a project that was small enough to finish, but still real and useful. At the same time, I was learning the TM4C123GXL board, so I decided to do a research-style performance study on it. (Subhaan)

The topic we picked is [bit-banding](glossary.md#bit-banding) vs [Read-Modify-Write (RMW)](glossary.md#read-modify-write-rmw). People talk about bit-banding like it is a "fast trick" and also like it is the "safe" way to change one bit. But there is not much clear data (on this exact board) that answers the simple question:

When is bit-banding actually worth using?

---

## What we expected (our hypothesis)

Before measuring anything, our guess was:

- Bit-banding would **not** be a big speed upgrade over RMW on the TM4C123GXL.
- Bit-banding might be **slightly smaller** in code size in some cases.
- Bit-banding should be better for [atomicity](glossary.md#atomicity), because it does a single-bit update without the read step.

So basically: we thought that bit-banding wouldn't be faster, but it might still be worth it in the right situations.

---

## The questions we wanted to answer

This project is trying to answer these questions with real measurements:

1. **Speed:** Is bit-banding faster, slower, or basically the same as RMW for single-bit work?
2. **Code size:** Does bit-banding make the compiled program smaller?
3. **Atomicity:** Can we show a real race problem with RMW under interrupts, and show bit-banding avoids it?
4. **Where does it break down:** What happens when you try to use bit-banding for multi-bit writes (like writing a full 32-bit word)?

---

## What makes this important

Embedded code is full of single-bit updates:

- turning an LED on or off using [GPIO](glossary.md#gpio)
- setting or clearing a status flag in [SRAM](glossary.md#sram)
- flipping a control bit in a peripheral register

RMW is the normal way to do it, but it can be risky if an interrupt touches the same word at the wrong time. Bit-banding is designed to help with that, but it has tradeoffs:

- only works in certain memory regions (not all memory)
- set and clear is easy, but toggle is not direct
- it is not portable to every microcontroller family

This repo is basically a physical check of those tradeoffs.

---

## What hardware you need (important)

To run these exact tests as-is, you need the **TM4C123GXL** (Texas Instruments Tiva C Series).

- I personally bought my own board.
- For my research team members, Professor **Jimmie Bud Davis** generously provided boards for free so we could all run the same experiments.

If you do not have this board, you can still read the results in the repo, but you cannot reproduce the exact numbers in this repo without similar hardware.

---

## What is included in this repo

- Multiple small Code Composer Studio projects in `ccs_projects/` (one per test)
- Documentation in `docs/` written like a simple research paper
- A copy of the TM4C datasheet in `docs/references/` (used for the bit-banding memory map and examples)

---

## FAQ

### What is the purpose of this research project?

To experimentally evaluate whether bit-banding is worth using over RMW on the TM4C123GXL, focusing on speed, code size, and atomicity.

### What new info are we creating?

Real measurement data on a specific hardware platform. This gives practical guidance to embedded programmers choosing between the two methods.

### Why not just trust the datasheet?

We do use the datasheet as the "source of truth" for the memory map and how bit-banding works. But the whole point of this project is to measure what it looks like in practice:

- how fast it is
- how much code it generates
- what happens under interrupts

### Is bit-banding always better?

No. It can be similar in speed to RMW for single-bit set/clear, but it has limits:

- no direct toggle
- not good for multi-bit writes
- only exists in certain memory regions

### Why the TM4C123GXL board?

It is the current board used in CSE 3442 Embedded Systems 1, easy to access, and our group already had boards available. It also supports Cortex-M4 bit-banding, which makes it a good target for this study.

### Can I run this on a STM32 or another board?

Not directly. Some boards do not support bit-banding at all, and even if they do, the memory map and results will be different. You would need to rewrite the alias address macros and re-run the tests.

---

## Where to go next

- [Methodology](../methodology/README.md)
- [Experiments](../tests/README.md)
- [Results](../results/README.md)
- [Glossary](glossary.md)
