# Client–Server System (C)

A client–server system programming project implemented in **C** as part of
the **Operating Systems** course at the **Technion – Israel Institute of Technology**.

The project demonstrates server design, request handling, and internal
queue-based scheduling.

---

## Project Overview

This project implements a server that accepts requests from multiple clients,
manages them using an internal queue, and processes them according to the
defined service logic.

The design emphasizes modularity, correctness, and low-level systems
programming principles.

---

## Architecture

- **Server (`server.c`)**
  - Accepts and manages client connections
  - Dispatches requests for processing

- **Client (`client.c`)**
  - Sends requests to the server
  - Receives and handles responses

- **Request Handling**
  - `request.c / request.h` – request parsing and handling logic

- **Queue Management**
  - `queue.c / queue.h` – internal request queue abstraction

- **Service Logic**
  - `segel.c / segel.h` – core server-side functionality

- **Output & Logging**
  - `output.c` – formatted output and diagnostics

---

## Technologies & Concepts

- C (system-level programming)
- Client–server architecture
- Request queuing
- Modular design with headers
- Makefile-based build
- Debugging with GDB

---

## My Role

- Implemented server-side request handling logic
- Designed and implemented queue data structure
- Integrated client–server communication
- Debugged concurrency and request flow issues
- Ensured correct resource management and cleanup

---

## Build & Run

```bash
make
