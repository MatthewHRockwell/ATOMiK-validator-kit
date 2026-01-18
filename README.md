# ATOMiK Validator Kit (v1.0-Lite)

**Verify the "Stateless" Architecture of the ATOMiK Protocol**

> *"The Memory Wall is not a law of physics. It is a design choice."*

This repository contains the core software tools required to verify the claims of the **ATOMiK Protocol (Adaptive Temporal Operational Micro-Kernel)**. It enables security researchers, developers, and hardware architects to audit the **Genome compilation process** and test **Burn-on-Read security mechanics** in a local simulation environment.

---

## The Core Claims

### Stateless Design
The "Operating System" is defined purely by a **transient JSON schema**. There is no persistent OS image or assembled state stored in RAM.

### Burn-on-Read Security
Encryption keys (hardware maps) are destroyed at the **register level immediately upon use**, providing hardware-native perfect forward secrecy.

### Polymorphic Noise
Without the active, synchronized hardware map, data on the wire is **mathematically indistinguishable from random noise**.

---

## Technical Primer: How It Works

### The Death of the Memory Wall
Traditional kernels rely on persistent state—storing data in RAM and waiting for CPU fetch cycles. ATOMiK replaces this with a **Transient Blueprint**. The `test_schema.json` defines hardware behavior for a specific transaction window, eliminating the need for a heavy, vulnerable OS image.

### Burn-on-Read (OTP Mode)
Our V3 Kernel treats the hardware map as a transactional requirement. In `OTP_MODE`, the system forces a map rotation the moment a transaction is completed. This creates an **Asynchronous Stateless Network** where nodes stay in sync via "Proof of Origin" handshakes rather than vulnerable global clocks.

---

## Repository Contents

- **`agc.py`**  
  ATOMiK Genome Compiler. Translates JSON schemas into binary Genome files (`.gnm`).  
  Inspect this to verify that the output contains only logic tags with no hidden payload.

- **`test_schema.json`**  
  The Universal Schema. Sample definition for a DoD-grade secure uplink.

- **`aos.c` / `atomik_core_sim.c`**  
  Kernel Simulator. C-based simulation of the ATOMiK FPGA Core (37 ns latency profile).

- **`atomik_core.h`**  
  Header file containing core definitions and error codes.

- **`build.bat` / `Makefile`**  
  Build scripts for one-click compilation on Windows, Linux, and macOS.

---

## The "Burn" Challenge

You are invited to verify the **Burn-on-Read** protocol yourself.

### 1. Compile the Genome

```bash
python agc.py test_schema.json
```
Observe that the output file `DOD_V1.gnm` is a minimal configuration artifact, demonstrating that no persistent state is transmitted.

### 2. Build the Simulator

**Windows:** Double-click `build.bat` (or run it in your terminal).

**Linux/Mac:** Run `make`.

**Manual:** `gcc aos.c atomik_core_sim.c -o aos -lws2_32`

### 3. Run the Kernel & Verify

Launch the simulator:

```
./aos
```

Inside the AOS terminal:

- **Boot the core:** `boot`

- **Load the Genome:** `load DOD_V1`

  - *Observe:* The log confirms `OTP Mode: ON (Burn-After-Reading)`.

- **Establish a Link:** `connect 8888` (Requires a second AOS instance running `listen 8888` or a dummy socket).

- **Send a Packet:** `secure TEST_MESSAGE`

  - *Observe:* The kernel triggers `[KEY BURNED]` instantly.

**Result:** The hardware map used for `TEST_MESSAGE` no longer exists. A replay attack is now mathematically impossible.

---

## Roadmap

- **V1 (Current):** Software Simulation & Verification Kit.

- **V2 (Hardware):** Physical FPGA Implementation (Tang Nano 9K) with Fiber-Optic P2P Mesh.

- **V3 (Future):** Photonic Interference Logic using resin-cast optical waveguides.

---

## Feature Request

All feature requests are welcome, however we are unable to accomodate all requests. Requests can be submitted through **[Jira](https://matthewhrockwell.atlassian.net/jira/software/projects/SCRUM/form/1?atlOrigin=eyJpIjoiMWFjYzAyYjc2MWZiNDgxNThkYjFkNDg3NDk3MDFiY2YiLCJwIjoiaiJ9)**

---

## License

This project is licensed under the **Apache License 2.0**. This ensures that the ATOMiK Validator Kit remains an open, auditable standard for the security community while protecting the underlying intellectual property of the ATOMiK Protocol.

***Copyright © 2026 ATOMiK Protocol.** Released for educational and verification purposes.*
