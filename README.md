# Advance Virtualization and Containerization

## Overview
This repository contains a collection of projects exploring advanced topics in virtualization, containerization, and Linux internals.  
It is divided into three major modules:
1. **Docker-like Lightweight Container Platform** — building containerization primitives from scratch using Linux namespaces, control groups, and orchestration scripts.  
2. **Lightweight Hypervisor Using KVM** — working directly with the Kernel-based Virtual Machine (KVM) to create a minimal hypervisor capable of running guest code and emulating devices.  
3. **Linux Internals Exploration** — delving into kernel modules, character device drivers, memory management, and process statistics.
## Directory Structure
├── Docker-like Lightweight Container Platform
│ ├── task1/ # Namespace isolation
│ ├── task2/ # Simple container script
│ ├── task3/ # Multi-container orchestration
│ └── task4/ # Service orchestration
├── Lightweight Hypervisor Using KVM
│ ├── part1a/ # KVM overview
│ ├── part1b/ # Simple KVM guest
│ └── part2/ # Device emulation
└── Linux Internals Exploration
├── 1/ # Kernel modules
├── 2.1/ # Character device driver
├── 2.2/ # Multi-module interaction
├── 3.1/ # Page fault stats
└── 3.2/ # Memory stats
