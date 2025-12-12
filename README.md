# ProcessThreadManager

A modern C++20 library for process and thread management on Unix systems, demonstrating advanced operating system concepts including process lifecycle management, thread pooling, inter-process communication (IPC), and synchronization primitives.

Full API documentation is available at: [https://swilltec.github.io/ProcessThreadManager/](https://swilltec.github.io/ProcessThreadManager/)

The documentation includes detailed class references, method descriptions, usage examples, and design rationale for all library components. Documentation is generated using Doxygen from inline source code comments.


[![Documentation](https://img.shields.io/badge/docs-GitHub%20Pages-blue)](https://swilltec.github.io/ProcessThreadManager/)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![POSIX](https://img.shields.io/badge/platform-POSIX-orange.svg)](https://pubs.opengroup.org/onlinepubs/9699919799/)

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Building](#building)
- [Running Tests](#running-tests)
- [Library Components](#library-components)
- [Usage Examples](#usage-examples)
- [API Reference](#api-reference)
- [Thread Safety](#thread-safety)
- [Troubleshooting](#troubleshooting)
- [Resources](#resources)

## 🎯 Overview

ProcessThreadManager is a comprehensive C++20 library providing a robust framework for managing processes, threads, and inter-process communication on POSIX-compliant Unix systems. This library demonstrates core operating system concepts while providing production-ready tools.

**Key Capabilities:**
- Process lifecycle management using POSIX `fork()`, `exec()`, and signal handling
- Thread pooling with fixed-size worker pools and task queues
- Multiple IPC mechanisms: pipes, FIFOs, shared memory, message queues
- Synchronization primitives with deadlock detection and timeout support
- Concurrency control with race condition prevention and resource management

## ✨ Features

### Process Management
- ✅ Fork-based process creation with automatic error handling
- ✅ Complete lifecycle tracking (CREATED, RUNNING, BLOCKED, READY, TERMINATED)
- ✅ Exit status retrieval using `waitpid()`
- ✅ Signal handling (SIGTERM/SIGKILL)
- ✅ Zombie prevention through automatic reaping
- ✅ Resource limit management via `setrlimit()`

### Thread Pool
- ✅ Fixed-size worker pool with configurable thread count
- ✅ Thread-safe task queue with automatic distribution
- ✅ Future-based asynchronous result retrieval
- ✅ Thread state monitoring (IDLE, RUNNING, BLOCKED, TERMINATED)
- ✅ Graceful shutdown with task completion guarantee
- ✅ Exception propagation through futures

### Inter-Process Communication
- ✅ **Unnamed Pipes** - Fast parent-child communication
- ✅ **Named Pipes (FIFO)** - Filesystem-based IPC for unrelated processes
- ✅ **Shared Memory** - High-performance memory sharing via POSIX shm
- ✅ **Message Queues** - Structured message passing with System V IPC

### Synchronization Primitives
- ✅ **SafeMutex** - Deadlock detection and timeout support
- ✅ **SafeLockGuard** - RAII-based automatic locking
- ✅ **Semaphore** - POSIX semaphores with timed operations
- ✅ **Reader-Writer Lock** - Concurrent reads, exclusive writes
- ✅ **Barrier** - Multi-thread synchronization points
- ✅ **Condition Variable** - Thread signaling with predicate support
- ✅ **SpinLock** - Low-latency busy-wait locks

## 🔧 Prerequisites

### Compiler & Build Tools
- **GCC 10+** or **Clang 12+** with full C++20 support
- **CMake 3.20+** for build configuration
- **Make** or **Ninja** build system

### System Requirements
- **Operating System**: Linux (3.10+), macOS (11.0+), FreeBSD (12.0+), or POSIX-compliant Unix
- **Libraries**: pthread, librt (POSIX realtime extensions)

### Installation Commands

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential cmake git
```

**Fedora/RHEL:**
```bash
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake git
```

**macOS (Homebrew):**
```bash
brew install cmake git
```

**FreeBSD:**
```bash
pkg install cmake gcc git
```

## 📦 Installation

```bash
# Clone repository
git clone https://github.com/swilltec/ProcessThreadManager.git
cd ProcessThreadManager

# Initialize submodules (if any)
git submodule update --init --recursive
```

## 🔨 Building

### Using Make

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build library and tests
make

# Build with parallel jobs (faster)
make -j$(nproc)  # Linux
make -j$(sysctl -n hw.ncpu)  # macOS/FreeBSD
```

### Build Targets

```bash
# Build everything (library + tests)
make

# Build only the library
make ProcessThreadManager

# Build only tests
make test_manager

# Clean build artifacts
make clean

# Install to system (requires sudo)
sudo make install
```

### Build Types

```bash
# Debug build (default)
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Release build (optimized)
cmake -DCMAKE_BUILD_TYPE=Release ..
make

# Release with debug symbols
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
make
```

### Build Options

```bash
# Enable sanitizers
cmake -DENABLE_ASAN=ON ..          # AddressSanitizer
cmake -DENABLE_TSAN=ON ..          # ThreadSanitizer
cmake -DENABLE_UBSAN=ON ..         # UndefinedBehaviorSanitizer

# Disable tests
cmake -DBUILD_TESTS=OFF ..

# Specify compiler
cmake -DCMAKE_CXX_COMPILER=g++-11 ..
cmake -DCMAKE_CXX_COMPILER=clang++ ..
```

### Makefile Reference

The project includes both CMake and traditional Make support:

**Using the Makefile directly:**
```bash
# Build everything
make

# Build only library
make library

# Run tests
make test

# Install to system
sudo make install

# Clean up
make clean

make docs  # Generate documentation

make docs-open  # Open documentation in browser
```

## 🧪 Running Tests

### Using CTest (CMake)


### Using Make

```bash
# Run tests directly
make test-all

# Or Run 
make run

# Or run executable
./test_manager

# Run with specific test
./test_manager 1    # Process management
./test_manager 2    # Thread pool
./test_manager 3    # IPC mechanisms
./test_manager 4    # Synchronization
```

### Memory Leak Detection


## 📚 Library Components

### ProcessManager
Manages process creation, monitoring, and termination using POSIX APIs.

**Process States:**
```
CREATED → RUNNING → BLOCKED → READY → TERMINATED
```

### ThreadPool
Fixed-size worker thread pool with FIFO task scheduling and future-based results.

### IPC Mechanisms

| Mechanism | Performance | Use Case | Complexity |
|-----------|-------------|----------|------------|
| Unnamed Pipe | Fast | Parent-child only | Simple |
| Named Pipe (FIFO) | Medium | Any processes | Simple |
| Shared Memory | Very Fast | High-throughput | Medium |
| Message Queue | Medium | Structured data | Medium |

### Synchronization Primitives

| Primitive | Purpose | Best For |
|-----------|---------|----------|
| SafeMutex | Mutual exclusion | Critical sections |
| Semaphore | Resource counting | Producer-consumer |
| RWLock | Reader-writer | Read-heavy workloads |
| Barrier | Sync point | Parallel algorithms |
| ConditionVariable | Thread signaling | Event notification |
| SpinLock | Low-latency | Short critical sections |

## 💡 Usage Examples

### Process Management

```cpp
#include "ProcessManager.h"

ProcessManager pm;

// Create child process
pid_t pid = pm.createProcess([]() {
    std::cout << "Child process running\n";
    return 0;
});

// Wait for completion
int status = pm.waitForProcess(pid);
```

### Thread Pool

```cpp
#include "ThreadPool.h"

ThreadPool pool(4);  // 4 worker threads

// Submit task
auto future = pool.submit([]() {
    return 42;
});

// Get result
int result = future.get();
```

### Shared Memory IPC

```cpp
#include "IPC.h"

// Process A: Create and write
SharedMemory shm("/myshm", 1024);
shm.write("Hello from Process A");

// Process B: Open and read
SharedMemory shm("/myshm", 1024);
std::string data = shm.read();
```

### Synchronization

```cpp
#include "Synchronization.h"

SafeMutex mutex;

{
    SafeLockGuard lock(mutex);
    // Critical section - automatically unlocked
}
```

## 📖 API Reference

### ProcessManager

```cpp
class ProcessManager {
    pid_t createProcess(const std::string& name, std::function<int()> task);
    bool waitForProcess(pid_t pid, int* status = nullptr);
    bool terminateProcess(pid_t pid, int signal = SIGTERM);
    ProcessState getProcessState(pid_t pid);
    void waitForAll();
    void terminateAll();
};
```

### ThreadPool

```cpp
class ThreadPool {
    explicit ThreadPool(size_t numThreads);
    
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>;
    
    size_t getPoolSize() const;
    void shutdown();
};
```

### Synchronization

```cpp
class SafeMutex {
    bool lock(std::chrono::milliseconds timeout = std::chrono::milliseconds(5000));
    void unlock();
};

class Semaphore {
    explicit Semaphore(unsigned int value = 0);
    bool wait();
    bool post();
};

class RWLock {
    void readLock();
    void writeLock();
    void readUnlock();
    void writeUnlock();
};
```

## 🔒 Thread Safety

**Thread-Safe:**
- ✅ ThreadPool - All public methods
- ✅ SafeMutex, Semaphore, RWLock
- ✅ Barrier, ConditionVariable

**Not Thread-Safe (use external synchronization):**
- ❌ ProcessManager
- ❌ Pipe/NamedPipe (if shared)
- ❌ SharedMemory data access

## 🔧 Troubleshooting

### Compilation Errors

```bash
# Missing pthread
g++ -std=c++20 -pthread main.cpp -o program

# Undefined reference to pthread_create
g++ main.cpp -pthread -o program
```

### Runtime Issues

```bash
# Shared memory permission denied
rm /dev/shm/test_shm

# Too many open files
ulimit -n 4096
```

### Deadlock Detection

```bash
# Get process threads
ps -T -p <PID>

# Check thread states (Linux)
pstack <PID>

# FreeBSD
procstat -k <PID>
```

## 📚 Resources

### Documentation
- [POSIX Threads Programming - LLNL](https://hpc-tutorials.llnl.gov/posix/)
- [Linux man pages](https://man7.org/linux/man-pages/)
- [FreeBSD Handbook](https://docs.freebsd.org/en/books/handbook/)

### Recommended Reading
- **"Operating System Concepts"** by Silberschatz, Galvin, Gagne
- **"UNIX Network Programming"** by W. Richard Stevens
- **"C++ Concurrency in Action"** by Anthony Williams

---

**License:** MIT  
**Version:** 1.0.0  
**Last Updated:** December 2025
