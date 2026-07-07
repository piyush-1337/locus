# Locus

A zero-allocation, fully iterative DNS resolver built from scratch. 

## Features

* **Zero-Allocation Parsing:** Avoids heap allocations and object slicing. Protocol parsing relies entirely on strict byte offset tracking, pointer math, and borrowing.
* **Custom Binary Protocol:** No external DNS libraries. Implements the raw RFC 1035 UDP packet specifications.

## Build

### Requirements:

- CMake
- Ninja (or gnumake)

#### Nix users:
```bash
$ nix develop
```

#### Build

```bash
mkdir build
cd build

cmake -G Ninja ..
ninja
```

## Usage

Locus takes a single domain target and prints the every hops across servers
```bash
$ ./locus --domain wikipedia.org
```
