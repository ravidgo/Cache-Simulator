# Cache-Simulator
An implementation of a cache simulator that models a two-level cache system.

## Description

The Cache Simulator project implements a flexible cache memory simulator designed to model data accesses with configurable parameters. This simulator supports both single-level (L1) and dual-level (L2) cache systems, and it is capable of calculating miss rates and average access times based on the provided configuration and trace file. This project demonstrates my ability to create a versatile tool for cache performance analysis, showcasing skills in system-level programming and memory management.

## Technologies Used

- **Programming Languages:** C++
- **Tools:** Make for build automation, Git for version control

## Features

- **Flexible Cache Configuration:** Allows configuration of cache size, block size, associativity, and access times for both L1 and L2 caches.
- **Data Access Simulation:** Simulates read and write operations, calculating cache miss rates and average access times based on the provided trace file.
- **Support for Cache Policies:** Includes support for No Write Allocate or Write Allocate policies and Write Back caching.
- **Inclusive Caching:** Follows the inclusive cache principle, where the L1 cache is a subset of the L2 cache.
- **LRU Replacement Policy:** Implements Least Recently Used (LRU) policy for cache eviction.

## Getting Started

### Prerequisites

- C++ Compiler (e.g., g++)
- Make utility for build automation

### Installation

Clone the repository to your local machine:

```sh
git clone https://github.com/[YourGitHubUsername]/Cache-Simulator.git
cd Cache-Simulator
```

Compile the project using the provided `Makefile`:

```sh
make
```

### Usage

Run the simulator with the following command:

```sh
./cacheSim <input file> --mem-cyc <# of cycles> --bsize <block log2(size)> --wr-alloc <0: No Write Allocate; 1: Write Allocate> --l1-size <log2(size)> --l1-assoc <log2(# of ways)> --l1-cyc <# of cycles> --l2-size <log2(size)> --l2-assoc <log2(# of ways)> --l2-cyc <# of cycles>
```

**Example:**

```sh
./cacheSim example.in --mem-cyc 100 --bsize 5 --wr-alloc 1 --l1-size 16 --l1-assoc 3 --l1-cyc 1 --l2-size 20 --l2-assoc 4 --l2-cyc 5
```

### Output

The output will include:

- `L1miss=<L1 miss rate>`
- `L2miss=<L2 miss rate>`
- `AccTimeAvg=<avg. acc. time>`

Miss rates are provided as decimal fractions with three decimal places. The average access time is given in clock cycles with three decimal places.

## Contributing

Contributions to the Cache Simulator project are welcome! Please fork the repository, make your changes, and submit a pull request with your improvements.
