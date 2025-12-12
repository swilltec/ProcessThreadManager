# ProcessThreadManager

A modern C++20 library for process and thread management on Unix systems, demonstrating advanced operating system concepts including process lifecycle management, thread pooling, inter-process communication (IPC), and synchronization primitives.

[![Documentation](https://img.shields.io/badge/docs-GitHub%20Pages-blue)](https://swilltec.github.io/ProcessThreadManager/)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![POSIX](https://img.shields.io/badge/platform-POSIX-orange.svg)](https://pubs.opengroup.org/onlinepubs/9699919799/)

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Documentation](#documentation)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Building](#building)
- [Running Tests](#running-tests)
- [Library Components](#library-components)
- [Usage Examples](#usage-examples)
- [Learning Objectives](#learning-objectives)
- [Core Concepts Demonstrated](#core-concepts-demonstrated)
- [API Reference](#api-reference)
- [Thread Safety](#thread-safety)
- [Performance Considerations](#performance-considerations)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)
- [Additional Resources](#additional-resources)

## 🎯 Overview

ProcessThreadManager is a comprehensive C++20 library that provides a robust framework for:

- **Process lifecycle management** - Create, monitor, and control child processes with full state tracking
- **Thread pooling** - Efficient task execution using a fixed pool of worker threads
- **Inter-process communication** - Multiple IPC mechanisms for process coordination
- **Synchronization primitives** - Thread-safe operations with deadlock detection and prevention
- **Concurrency control** - Race condition prevention and resource management

The library follows modern C++ best practices and leverages C++20 features for type safety and performance.

## ✨ Features

### Process Management
- ✅ Create and manage multiple child processes using `fork()`
- ✅ Track process states (CREATED, RUNNING, BLOCKED, READY, TERMINATED)
- ✅ Wait for process completion with exit status retrieval
- ✅ Graceful shutdown (SIGTERM) with forceful fallback (SIGKILL)
- ✅ Comprehensive process information and state monitoring
- ✅ Automatic zombie process prevention

### Thread Pool
- ✅ Fixed-size worker thread pool for optimal resource utilization
- ✅ Task queue with automatic work distribution
- ✅ Future-based asynchronous result retrieval with type safety
- ✅ Thread state monitoring (IDLE, RUNNING, BLOCKED, TERMINATED)
- ✅ Graceful shutdown ensuring task completion
- ✅ Exception handling within worker threads

### Inter-Process Communication (IPC)
- ✅ **Unnamed Pipes** - Fast unidirectional parent-child communication
- ✅ **Named Pipes (FIFO)** - Filesystem-based bidirectional process communication
- ✅ **Shared Memory** - High-performance memory sharing via POSIX shm
- ✅ **Message Queues** - Structured message passing using System V IPC

### Synchronization Primitives
- ✅ **SafeMutex** - Mutex wrapper with deadlock detection and configurable timeout
- ✅ **SafeLockGuard** - RAII-based lock management
- ✅ **Semaphore** - POSIX semaphore wrapper for resource counting
- ✅ **Reader-Writer Lock** - Multiple concurrent readers, exclusive writer access
- ✅ **Barrier** - Synchronization point for coordinating multiple threads
- ✅ **Condition Variables** - Thread signaling with predicate support
- ✅ **SpinLock** - Low-latency busy-wait locking for short critical sections

## 📚 Documentation

**Full API documentation is available at:** [https://swilltec.github.io/ProcessThreadManager/](https://swilltec.github.io/ProcessThreadManager/)

The documentation includes:
- Detailed class descriptions
- Method signatures and parameters
- Usage examples and best practices
- Architecture diagrams
- Performance considerations

### Quick Links
- [API Reference](https://swilltec.github.io/ProcessThreadManager/annotated.html)
- [Class List](https://swilltec.github.io/ProcessThreadManager/classes.html)
- [File Documentation](https://swilltec.github.io/ProcessThreadManager/files.html)

## 📦 Prerequisites

### Required
- **Operating System**: FreeBSD 11.0+ (or Linux/macOS with POSIX support)
- **Compiler**: 
  - g++ 10.0+ with C++20 support, or
  - clang++ 10.0+ with C++20 support
- **Build System**: Make
- **Libraries**: 
  - pthread (POSIX threads)
  - rt (realtime extensions)

### Optional
- **Doxygen** - For generating documentation locally
- **Graphviz** - For documentation diagrams
- **Valgrind** - For memory leak detection (Linux)
- **GDB** - For debugging

### FreeBSD Installation
```bash
# Install required packages
sudo pkg install gcc gmake git

# Optional: Install documentation tools
sudo pkg install doxygen graphviz

# Optional: Install debugging tools
sudo pkg install gdb
```

### Linux (Debian/Ubuntu) Installation
```bash
# Update package list
sudo apt-get update

# Install build essentials
sudo apt-get install build-essential git

# Optional: Install documentation tools
sudo apt-get install doxygen graphviz

# Optional: Install debugging tools
sudo apt-get install gdb valgrind
```

### macOS Installation
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Optional: Install documentation tools with Homebrew
brew install doxygen graphviz
```

## 🚀 Installation

### Clone the Repository

```bash
git clone https://github.com/swilltec/ProcessThreadManager.git
cd ProcessThreadManager
```

### Project Structure

```
ProcessThreadManager/
│
├── Makefile                    # Build configuration
├── Doxyfile                    # Documentation generation config
├── README.md                   # This file
├── ABOUT.md                    # Project information
│
├── include/                    # Public header files
│   ├── ProcessManager.h        # Process lifecycle management
│   ├── ThreadPool.h            # Thread pool with task queue
│   ├── IPC.h                   # Inter-process communication
│   └── Synchronization.h       # Synchronization primitives
│
├── src/                        # Implementation files
│   ├── ProcessManager.cpp      # Process management implementation
│   ├── ThreadPool.cpp          # Thread pool implementation
│   ├── IPC.cpp                 # IPC mechanisms implementation
│   └── Synchronization.cpp     # Synchronization implementation
│
├── test/                       # Test suite
│   └── main.cpp                # Comprehensive test program
│
├── build/                      # Build artifacts (generated)
│   ├── obj/                    # Object files
│   └── test_manager            # Compiled executable
│
└── docs/                       # Generated documentation (via Doxygen)
    └── html/
        └── index.html          # Documentation entry point
```

## 🔨 Building

### Quick Build

```bash
# Build the project
make

# Build with parallel jobs for faster compilation
make -j$(nproc)  # Linux
make -j$(sysctl -n hw.ncpu)  # FreeBSD/macOS
```

### Available Make Targets

```bash
# Building
make              # Build the project
make all          # Same as make
make rebuild      # Clean and rebuild
make debug        # Build with debug symbols and run GDB

# Cleaning
make clean        # Remove build artifacts
make clean-ipc    # Clean IPC resources (shared memory, semaphores)
make clean-all    # Clean everything

# Running
make run          # Build and run all tests
make test1        # Run Process Management test
make test2        # Run Thread Pool test
make test3        # Run IPC test
make test4        # Run Synchronization test
make test5        # Run all tests sequentially

# Debugging & Analysis
make valgrind     # Run with Valgrind (memory checker)
make check-leaks  # Check for memory leaks in all tests
make asan         # Build and run with AddressSanitizer
make tsan         # Build and run with ThreadSanitizer
make ubsan        # Build and run with UBSanitizer

# Documentation
make docs         # Generate documentation with Doxygen
make docs-open    # Generate and open documentation
make docs-clean   # Remove generated documentation

# Code Quality
make format       # Format code with clang-format
make lint         # Run clang-tidy linter
make stats        # Show code statistics

# Information
make info         # Show project information
make help         # Show all available targets
```

### Build Configuration

The Makefile supports several build configurations:

```bash
# Compiler selection
CXX=g++ make              # Use GCC (default)
CXX=clang++ make          # Use Clang

# Optimization levels
CXXFLAGS="-O3" make       # Maximum optimization
CXXFLAGS="-O0 -g" make    # No optimization, debug symbols
```

### Verify Build

```bash
# Check if executable was created
ls -l build/test_manager

# Expected output:
-rwxr-xr-x  1 user  group  XXXXXX Dec 12 12:00 build/test_manager
```

## 🧪 Running Tests

### Interactive Mode

```bash
# Run the test program
./build/test_manager

# Or use make
make run
```

You'll see an interactive menu:

```
╔════════════════════════════════════════╗
║  Process & Thread Management Library  ║
║           Test Suite                   ║
╚════════════════════════════════════════╝

Select test to run:
  1. Process Management
  2. Thread Pool
  3. Inter-Process Communication (IPC)
  4. Synchronization Primitives
  5. Run All Tests
  0. Exit

Choice: _
```

### Command-Line Mode

```bash
# Run specific tests directly
./build/test_manager 1    # Test process management
./build/test_manager 2    # Test thread pool
./build/test_manager 3    # Test IPC mechanisms
./build/test_manager 4    # Test synchronization
./build/test_manager 5    # Run all tests

# Or use make targets
make test1    # Process Management
make test2    # Thread Pool
make test3    # IPC
make test4    # Synchronization
make test-all # All tests sequentially
```

### Example Test Output

```
========================================
  TEST 1: PROCESS MANAGEMENT
========================================
Main process PID: 12345
Created process 'Worker-1' with PID: 12346
Created process 'Worker-2' with PID: 12347
Process 12346 (Worker-1) terminated with status: 0
Process 12347 (Worker-2) terminated with status: 0
✓ Process management test completed
```

## 📚 Library Components

### 1. ProcessManager (`include/ProcessManager.h`)

Manages process creation, monitoring, and termination with full lifecycle tracking.

**Key Classes:**
- `Process` - Represents a single process with metadata
- `ProcessManager` - Centralized process management

**Process States:**
```cpp
enum class ProcessState {
    CREATED,      // Just created via fork()
    RUNNING,      // Currently executing
    BLOCKED,      // Waiting for I/O or resources
    READY,        // Ready to run
    TERMINATED    // Finished execution
};
```

**Key Features:**
- Automatic zombie process prevention
- Signal-based termination (SIGTERM/SIGKILL)
- Exit status tracking
- Bulk operations (waitForAll, terminateAll)

### 2. ThreadPool (`include/ThreadPool.h`)

Implements a fixed-size pool of worker threads with task queue management.

**Key Features:**
- Type-safe task submission via templates
- Future-based result retrieval
- Exception handling in worker threads
- Graceful shutdown with task completion
- Real-time thread state monitoring

**Thread States:**
```cpp
enum class ThreadState {
    IDLE,         // Waiting for tasks
    RUNNING,      // Executing a task
    BLOCKED,      // Waiting for resources
    TERMINATED    // Shut down
};
```

### 3. IPC - Inter-Process Communication (`include/IPC.h`)

**Available Mechanisms:**

| Mechanism | Latency | Throughput | Complexity | Use Case |
|-----------|---------|------------|------------|----------|
| **Unnamed Pipe** | ~1µs | High | Simple | Parent-child, moderate data |
| **Named Pipe (FIFO)** | ~5µs | Medium | Simple | Unrelated processes |
| **Shared Memory** | ~100ns | Very High | Medium | Large data, frequent access |
| **Message Queue** | ~10µs | Medium | Medium | Structured messages |

**Classes:**
- `Pipe` - Unnamed pipe for parent-child communication
- `NamedPipe` - FIFO for any process communication
- `SharedMemory` - POSIX shared memory wrapper
- `MessageQueue` - System V message queue wrapper

### 4. Synchronization (`include/Synchronization.h`)

**Primitives Available:**

| Primitive | Purpose | Thread Safe | Overhead | Use Case |
|-----------|---------|-------------|----------|----------|
| **SafeMutex** | Mutual exclusion | ✅ Yes | Low | Critical sections |
| **SafeLockGuard** | RAII mutex lock | ✅ Yes | Very Low | Automatic unlock |
| **Semaphore** | Resource counting | ✅ Yes | Low | Producer-consumer |
| **RWLock** | Reader-writer | ✅ Yes | Medium | Read-heavy workloads |
| **Barrier** | Thread coordination | ✅ Yes | Medium | Parallel algorithms |
| **ConditionVariable** | Thread signaling | ✅ Yes | Low | Event notification |
| **SpinLock** | Busy-wait lock | ✅ Yes | Very Low | Short critical sections |

**Key Features:**
- Deadlock detection in SafeMutex
- Timeout support
- RAII-based resource management
- Writer-preference in RWLock

## 💡 Usage Examples

[Keep all your existing examples - they are excellent and comprehensive]

## 🎓 Learning Objectives

[Keep this section as is - very well structured]

## 🔬 Core Concepts Demonstrated

[Keep this section as is]

## 📖 API Reference

For complete API documentation with detailed descriptions, see: [https://swilltec.github.io/ProcessThreadManager/](https://swilltec.github.io/ProcessThreadManager/)

[Keep the API reference section as is]

## 🔒 Thread Safety

[Keep this section as is]

## ⚡ Performance Considerations

[Keep this section as is]

## 🔧 Troubleshooting

[Keep this section as is]

## 🤝 Contributing

Contributions are welcome! Please follow these guidelines:

### Code Style

- Use 4 spaces for indentation
- Follow existing code formatting
- Add Doxygen comments for all public APIs
- Write clear commit messages

### Submitting Changes

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Building Documentation Locally

```bash
# Generate documentation
make docs

# View documentation
make docs-open
```

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.


## 📚 Additional Resources

### Documentation
- [Full API Documentation](https://swilltec.github.io/ProcessThreadManager/) - Complete reference with examples
- [FreeBSD Handbook](https://docs.freebsd.org/en/books/handbook/) - Operating system guide
- [POSIX Threads Programming](https://hpc-tutorials.llnl.gov/posix/) - POSIX threads tutorial
- [Linux man pages](https://man7.org/linux/man-pages/) - System call reference

### Recommended Reading
- **"Operating System Concepts" by Silberschatz, Galvin, Gagne**
    - Chapter 3: Processes
    - Chapter 4: Threads
    - Chapter 5: Process Synchronization
    - Chapter 6: Deadlocks

- **"UNIX Network Programming" by W. Richard Stevens**
    - Volume 2: Interprocess Communications

- **"C++ Concurrency in Action" by Anthony Williams**
    - Modern C++ threading and synchronization

### Online Resources
- [C++20 Reference](https://en.cppreference.com/w/cpp/20)
- [POSIX.1-2017](https://pubs.opengroup.org/onlinepubs/9699919799/)
- [FreeBSD Architecture Handbook](https://docs.freebsd.org/en/books/arch-handbook/)
- [concurrency-ts Reference](https://en.cppreference.com/w/cpp/experimental/concurrency)

### Related Topics
- Process scheduling algorithms
- Memory management and virtual memory
- File systems and I/O
- Network programming with sockets
- Real-time operating systems

---

## 📞 Support

For questions, issues, or feature requests:

1. Check the [Documentation](https://swilltec.github.io/ProcessThreadManager/)
2. Review the [Troubleshooting](#troubleshooting) section
3. Search existing [Issues](https://github.com/swilltec/ProcessThreadManager/issues)
4. Open a new issue if needed

---

**Last Updated:** December 12, 2025  
**Version:** 1.0.0  
**Documentation:** https://swilltec.github.io/ProcessThreadManager/

---

Made with ❤️ for Operating Systems education
