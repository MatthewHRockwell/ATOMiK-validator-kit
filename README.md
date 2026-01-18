# ATOMiK Validator Kit (v1.0-Lite)

**Verify the "Stateless" Architecture of the ATOMiK Protocol.**

> "The Memory Wall is not a law of physics. It is a design choice."

This repository contains the core software tools required to verify the claims of the ATOMiK Protocol (Adaptive Temporal Operational Micro-Kernel). It allows security researchers to audit the "Genome" compilation process and test the "Burn-on-Read" security mechanics in a local simulation environment.

## The Core Claims
1.  **Stateless Design:** The "Operating System" is defined purely by a transient JSON schema. There is no persistent OS image.
2.  **Burn-on-Read Security:** Encryption keys are destroyed at the hardware register level immediately upon use (Perfect Forward Secrecy).
3.  **Polymorphic Noise:** Without the active hardware map, data on the wire is mathematically indistinguishable from random noise.

## Contents
* `agc.py`: **ATOMiK Genome Compiler.** Translates human-readable JSON schemas into binary "Genome" files (.gnm). Inspect this to prove that the output contains *only* logic tags, with no hidden OS payload.
* `test_schema.json`: **The Universal Schema.** A sample definition file for a DoD-grade secure uplink.
* `aos.c` / `atomik_core_sim.c`: **The Kernel Simulator.** A C-based simulation of the ATOMiK FPGA Core (37ns latency profile) to test key destruction logic.

## The "Burn" Challenge
We invite you to verify the "Burn-on-Read" protocol yourself:

1.  **Compile the Genome:**
    ```bash
    python agc.py test_schema.json
    ```
    *Observe that the output `.gnm` file is just a few bytes of configuration.*

2.  **Build the Simulator:**
    ```bash
    gcc aos.c atomik_core_sim.c -o aos
    ```

3.  **Run the Kernel:**
    ```bash
    ./aos
    ```

4.  **Execute the Test:**
    * Boot the core: `boot`
    * Load the Genome: `load DOD_V1`
    * **Observe:** The log will confirm `OTP Mode: ON (Burn-After-Reading)`.
    * Send a packet: `secure TEST_PACKET`
    * **Observe:** The kernel immediately triggers `[KEY BURNED]`.

## Roadmap
* **V1 (Current):** Simulation & Verification Kit.
* **V2 (Hardware):** FPGA Implementation (Tang Nano 9K) with Fiber-Optic P2P Mesh.
* **V3 (Future):** Photonic Interference Logic.

---
*Copyright © 2026 ATOMiK Protocol. Released for educational and verification purposes.*
