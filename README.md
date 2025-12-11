# Process and Thread Management Library for FreeBSD

A comprehensive C++ library demonstrating process management, thread pooling, inter-process communication (IPC), and synchronization primitives for FreeBSD/Unix environments.

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Project Structure](#project-structure)
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

## 🎯 Overview

This project implements a user-level library for managing processes and threads in FreeBSD, focusing on:

- **Process lifecycle management** - Creation, monitoring, and termination
- **Thread pooling** - Efficient worker thread reuse and task scheduling
- **Inter-process communication** - Multiple IPC mechanisms (pipes, shared memory)
- **Synchronization primitives** - Thread-safe operations with mutexes, semaphores, and more
- **Concurrency control** - Race condition prevention and deadlock detection

## ✨ Features

### Process Management
- ✅ Create and manage multiple processes with fork()
- ✅ Track process states (CREATED, RUNNING, BLOCKED, READY, TERMINATED)
- ✅ Wait for process completion with status codes
- ✅ Graceful termination (SIGTERM) and forced kill (SIGKILL)
- ✅ Process information and state monitoring

### Thread Pool
- ✅ Fixed-size worker thread pool for resource efficiency
- ✅ Task queue with automatic distribution
- ✅ Future-based asynchronous result retrieval
- ✅ Thread state monitoring (IDLE, RUNNING, BLOCKED, TERMINATED)
- ✅ Graceful shutdown with task completion

### Inter-Process Communication (IPC)
- ✅ **Unnamed Pipes** - Fast parent-child communication
- ✅ **Named Pipes (FIFO)** - Filesystem-based process communication
- ✅ **Shared Memory** - High-performance memory sharing via POSIX shm
- ✅ **Message Queues** - Structured message passing (System V IPC)

### Synchronization Primitives
- ✅ **SafeMutex** - Mutex with deadlock detection and timeout
- ✅ **Semaphore** - POSIX semaphore wrapper for resource counting
- ✅ **Reader-Writer Lock** - Multiple readers, exclusive writer access
- ✅ **Barrier** - Synchronization point for multiple threads
- ✅ **Condition Variables** - Thread signaling and coordination
- ✅ **SpinLock** - Low-latency busy-wait locking

## 📦 Prerequisites

### Required
- **Operating System**: FreeBSD 11.0+ (or Linux/macOS with POSIX support)
- **Compiler**: g++ 7.0+ or clang++ 5.0+ with C++17 support
- **CMake**: 3.15 or higher
- **Libraries**: pthread, rt (realtime extensions)

### Optional
- **Git**: For version control
- **Make**: Build automation (installed with CMake)

### FreeBSD Installation
```bash
# Install required packages
pkg install cmake gcc git

# Or use system clang (already installed)
pkg install cmake git
```

### Linux (Ubuntu/Debian) Installation
```bash
sudo apt-get update
sudo apt-get install build-essential cmake git


# Or use system clang (already installed)

# Install CMake 3.15+ from Kitware (Optional)
sudo curl -fsSL https://apt.kitware.com/keys/kitware-archive-latest.asc \
  | sudo gpg --dearmor -o /usr/share/keyrings/kitware-archive-keyring.gpg

echo "deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] \
  https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main" \
  | sudo tee /etc/apt/sources.list.d/kitware.list

sudo apt update
sudo apt install cmake
```

### macOS Installation
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install CMake
brew install cmake
```

## 🚀 Installation

### 1. Clone or Create Project Structure

```bash
# Create project directory
mkdir process_thread_manager
cd process_thread_manager

# Create subdirectories
mkdir -p include src test
```

### 2. Copy Files

Place the following files in their respective directories:

```
process_thread_manager/
├── CMakeLists.txt              # Build configuration
├── README.md                   # This file
├── include/
│   ├── ProcessManager.h      # Process management header
│   ├── ThreadPool.h          # Thread pool header
│   ├── IPC.h                 # IPC mechanisms header
│   └── Synchronization.h     # Synchronization primitives header
├── src/
│   ├── ProcessManager.cpp      # Process management implementation
│   ├── ThreadPool.cpp          # Thread pool implementation
│   ├── IPC.cpp                 # IPC implementation
│   └── Synchronization.cpp     # Synchronization implementation
└── test/
    └── main.cpp                # Test suite
```

## 🏗️ Project Structure

```
process_thread_manager/
│
├── CMakeLists.txt              # CMake build configuration
├── README.md                   # Documentation (this file)
│
├── include/                    # Public header files
│   ├── ProcessManager.h      # Process creation and lifecycle management
│   ├── ThreadPool.h          # Thread pool with task queue
│   ├── IPC.h                 # Inter-process communication classes
│   └── Synchronization.h     # Thread synchronization primitives
│
├── src/                        # Implementation files
│   ├── ProcessManager.cpp      # Process management implementation
│   ├── ThreadPool.cpp          # Thread pool implementation
│   ├── IPC.cpp                 # IPC mechanisms implementation
│   └── Synchronization.cpp     # Synchronization primitives implementation
│
└── test/                       # Test suite
    └── main.cpp                # Comprehensive test program with menu
```

## 🔨 Building

### Quick Build

```bash
# From project root directory
mkdir build && cd build
cmake ..
make

# Or use make with parallel jobs
make -j$(nproc)  # Linux
make -j$(sysctl -n hw.ncpu)  # FreeBSD/macOS
```

### Build Options

```bash
# Debug build (with debug symbols)
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Release build (optimized)
cmake -DCMAKE_BUILD_TYPE=Release ..
make

# Clean build
make clean
rm -rf build/*
```

### Verify Build

```bash
# After successful build, you should see:
ls -l test_manager

# Expected output:
-rwxr-xr-x  1 user  group  123456 Dec 11 12:00 test_manager
```

## 🧪 Running Tests

### Interactive Mode

```bash
# Run the test program
./test_manager

# You'll see a menu:
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
# Run specific test directly
./test_manager 1    # Test process management
./test_manager 2    # Test thread pool
./test_manager 3    # Test IPC mechanisms
./test_manager 4    # Test synchronization
./test_manager 5    # Run all tests

# Example output:
./test_manager 1
========================================
  TEST 1: PROCESS MANAGEMENT
========================================
Main process PID: 12345
...
✓ Process management test completed
```

## 📚 Library Components

### 1. ProcessManager

Manages process creation, monitoring, and termination.

**Key Classes:**
- `Process` - Represents a single process with state tracking
- `ProcessManager` - Manages multiple processes

**Process States:**
```cpp
enum class ProcessState {
    CREATED,      // Just forked, not yet running
    RUNNING,      // Currently executing
    BLOCKED,      // Waiting for I/O or resources
    READY,        // Ready to run
    TERMINATED    // Finished execution
};
```

### 2. ThreadPool

Implements a fixed-size pool of worker threads for task execution.

**Key Features:**
- Task queue with FIFO scheduling
- Future-based result retrieval
- Thread state monitoring
- Graceful shutdown

**Thread States:**
```cpp
enum class ThreadState {
    IDLE,         // Waiting for tasks
    RUNNING,      // Executing a task
    BLOCKED,      // Waiting for resources
    TERMINATED    // Shut down
};
```

### 3. IPC (Inter-Process Communication)

**Available Mechanisms:**

| Mechanism | Use Case | Performance | Complexity |
|-----------|----------|-------------|------------|
| **Unnamed Pipe** | Parent-child only | Fast | Simple |
| **Named Pipe (FIFO)** | Any processes | Medium | Simple |
| **Shared Memory** | High-throughput | Very Fast | Medium |
| **Message Queue** | Structured data | Medium | Medium |

### 4. Synchronization

**Primitives Available:**

| Primitive | Purpose | Use Case |
|-----------|---------|----------|
| **SafeMutex** | Mutual exclusion | Critical sections |
| **Semaphore** | Resource counting | Producer-consumer |
| **RWLock** | Reader-writer | Read-heavy workloads |
| **Barrier** | Synchronization point | Parallel algorithms |
| **ConditionVariable** | Thread signaling | Event notification |
| **SpinLock** | Low-latency lock | Short critical sections |

## 💡 Usage Examples

### Example 1: Create and Manage Processes

```cpp
#include "ProcessManager.h"

int main() {
    ProcessManager pm;
    
    // Create a worker process
    pid_t pid = pm.createProcess("Worker-1", []() {
        // Process work
        std::cout << "Worker process running!" << std::endl;
        sleep(2);
        return 0;  // Exit code
    });
    
    // Wait for completion
    int status;
    pm.waitForProcess(pid, &status);
    std::cout << "Process exited with code: " << status << std::endl;
    
    return 0;
}
```

### Example 2: Thread Pool with Tasks

```cpp
#include "ThreadPool.h"
#include <iostream>

int fibonacci(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; ++i) {
        int temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

int main() {
    // Create pool with 4 worker threads
    ThreadPool pool(4);
    
    // Submit tasks and get futures
    auto future1 = pool.enqueue(fibonacci, 30);
    auto future2 = pool.enqueue(fibonacci, 35);
    auto future3 = pool.enqueue([]() {
        std::cout << "Lambda task executing" << std::endl;
        return 42;
    });
    
    // Get results
    std::cout << "Fib(30) = " << future1.get() << std::endl;
    std::cout << "Fib(35) = " << future2.get() << std::endl;
    std::cout << "Lambda result = " << future3.get() << std::endl;
    
    return 0;
}
```

### Example 3: Unnamed Pipe Communication

```cpp
#include "IPC.h"
#include <unistd.h>

int main() {
    Pipe pipe;
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process - reader
        pipe.closeWrite();
        std::string msg = pipe.readString();
        std::cout << "Child received: " << msg << std::endl;
        pipe.close();
        exit(0);
    } else {
        // Parent process - writer
        pipe.closeRead();
        pipe.writeString("Hello from parent!");
        pipe.close();
        waitpid(pid, nullptr, 0);
    }
    
    return 0;
}
```

### Example 4: Shared Memory

```cpp
#include "IPC.h"
#include <unistd.h>

struct Data {
    int id;
    char message[256];
};

int main() {
    const std::string shmName = "/my_shared_memory";
    const size_t shmSize = sizeof(Data);
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child - reader
        sleep(1);  // Wait for parent to write
        
        SharedMemory shm(shmName, shmSize);
        shm.open();
        shm.map();
        
        Data* data = shm.getAs<Data>();
        std::cout << "Child read: ID=" << data->id 
                  << ", Message=" << data->message << std::endl;
        
        shm.unmap();
        shm.close();
        exit(0);
    } else {
        // Parent - writer
        SharedMemory shm(shmName, shmSize);
        shm.create();
        shm.map();
        
        Data* data = shm.getAs<Data>();
        data->id = 42;
        strcpy(data->message, "Hello via shared memory!");
        
        waitpid(pid, nullptr, 0);
        
        shm.unmap();
        shm.close();
        shm.unlink();
    }
    
    return 0;
}
```

### Example 5: Producer-Consumer with Semaphore

```cpp
#include "ThreadPool.h"
#include "Synchronization.h"
#include <vector>

int main() {
    const int bufferSize = 5;
    std::vector<int> buffer(bufferSize);
    int writePos = 0, readPos = 0;
    
    Semaphore empty(bufferSize);  // Empty slots
    Semaphore full(0);            // Full slots
    SafeMutex mutex;
    
    ThreadPool pool(2);
    
    // Producer
    auto producer = pool.enqueue([&]() {
        for (int i = 0; i < 20; ++i) {
            empty.wait();  // Wait for empty slot
            
            {
                SafeLockGuard lock(mutex);
                buffer[writePos] = i;
                writePos = (writePos + 1) % bufferSize;
            }
            
            full.post();  // Signal full slot
        }
    });
    
    // Consumer
    auto consumer = pool.enqueue([&]() {
        for (int i = 0; i < 20; ++i) {
            full.wait();  // Wait for full slot
            
            int item;
            {
                SafeLockGuard lock(mutex);
                item = buffer[readPos];
                readPos = (readPos + 1) % bufferSize;
            }
            
            std::cout << "Consumed: " << item << std::endl;
            empty.post();  // Signal empty slot
        }
    });
    
    producer.get();
    consumer.get();
    
    return 0;
}
```

### Example 6: Reader-Writer Lock

```cpp
#include "ThreadPool.h"
#include "Synchronization.h"

int main() {
    std::string sharedData = "Initial data";
    RWLock rwLock;
    ThreadPool pool(6);
    
    std::vector<std::future<void>> futures;
    
    // Create 4 reader threads
    for (int i = 0; i < 4; ++i) {
        futures.push_back(pool.enqueue([&, i]() {
            rwLock.readLock();
            std::cout << "Reader " << i << ": " << sharedData << std::endl;
            rwLock.readUnlock();
        }));
    }
    
    // Create 2 writer threads
    for (int i = 0; i < 2; ++i) {
        futures.push_back(pool.enqueue([&, i]() {
            rwLock.writeLock();
            sharedData = "Updated by writer " + std::to_string(i);
            std::cout << "Writer " << i << " updated data" << std::endl;
            rwLock.writeUnlock();
        }));
    }
    
    // Wait for all
    for (auto& f : futures) {
        f.get();
    }
    
    return 0;
}
```

### Example 7: Deadlock Prevention

```cpp
#include "Synchronization.h"
#include "ThreadPool.h"

int main() {
    SafeMutex resourceA("Resource_A");
    SafeMutex resourceB("Resource_B");
    
    ThreadPool pool(2);
    
    // Thread 1: Lock A then B
    auto t1 = pool.enqueue([&]() {
        if (resourceA.lock()) {
            std::cout << "Thread 1 locked A" << std::endl;
            
            // Try to lock B with timeout
            if (resourceB.lock(std::chrono::milliseconds(2000))) {
                std::cout << "Thread 1 locked B" << std::endl;
                resourceB.unlock();
            } else {
                std::cout << "Thread 1 timeout on B (deadlock avoided!)" << std::endl;
            }
            
            resourceA.unlock();
        }
    });
    
    // Thread 2: Lock B then A
    auto t2 = pool.enqueue([&]() {
        if (resourceB.lock()) {
            std::cout << "Thread 2 locked B" << std::endl;
            
            if (resourceA.lock(std::chrono::milliseconds(2000))) {
                std::cout << "Thread 2 locked A" << std::endl;
                resourceA.unlock();
            } else {
                std::cout << "Thread 2 timeout on A (deadlock avoided!)" << std::endl;
            }
            
            resourceB.unlock();
        }
    });
    
    t1.get();
    t2.get();
    
    return 0;
}
```

## 🎓 Learning Objectives

This project demonstrates understanding of:

### 1. Process Management
- ✅ Process creation using `fork()`
- ✅ Process lifecycle and state transitions
- ✅ Parent-child relationships
- ✅ Process termination and exit codes
- ✅ Waiting for child processes (`waitpid`)
- ✅ Signal handling (`SIGTERM`, `SIGKILL`)

### 2. Thread Management
- ✅ Thread creation and lifecycle
- ✅ Thread pooling for resource efficiency
- ✅ Task scheduling and distribution
- ✅ Thread state monitoring
- ✅ Graceful shutdown procedures

### 3. Concurrency Control
- ✅ **Race Conditions**: Understanding and prevention
- ✅ **Critical Sections**: Protection with mutexes
- ✅ **Deadlocks**: Detection and prevention strategies
- ✅ **Synchronization**: Proper use of primitives

### 4. Inter-Process Communication
- ✅ **Pipes**: Unidirectional data flow
- ✅ **Named Pipes**: Process-independent communication
- ✅ **Shared Memory**: High-performance data sharing
- ✅ **Message Queues**: Structured message passing

### 5. Resource Management
- ✅ RAII (Resource Acquisition Is Initialization)
- ✅ Proper cleanup and resource deallocation
- ✅ Memory management best practices
- ✅ File descriptor management

## 🔬 Core Concepts Demonstrated

### Race Conditions

**Problem:**
```cpp
int counter = 0;

// Thread 1
counter++;

// Thread 2
counter++;

// Expected: 2
// Actual: 1 or 2 (race condition!)
```

**Solution:**
```cpp
SafeMutex mutex;
int counter = 0;

// Thread 1
{
    SafeLockGuard lock(mutex);
    counter++;
}

// Thread 2
{
    SafeLockGuard lock(mutex);
    counter++;
}

// Result: Always 2 (correct!)
```

### Deadlock Scenario

**Problem:**
```cpp
// Thread 1          // Thread 2
lock(A)              lock(B)
lock(B)              lock(A)  // DEADLOCK!
```

**Solution with SafeMutex:**
```cpp
// Thread 1
if (mutexA.lock(timeout)) {
    if (mutexB.lock(timeout)) {
        // Critical section
        mutexB.unlock();
    } else {
        // Timeout - avoid deadlock
    }
    mutexA.unlock();
}
```

### Producer-Consumer Pattern

```
Producer                 Buffer              Consumer
   |                      [ ]                   |
   |--[produce item]----> [X]                   |
   |                      [X]                   |
   |                      [X] <--[consume]------|
   |                      [ ]                   |
```

Synchronized with semaphores:
- `empty`: Counts available slots
- `full`: Counts filled slots
- `mutex`: Protects buffer access

## 📖 API Reference

### ProcessManager

```cpp
class ProcessManager {
public:
    // Create a new process
    pid_t createProcess(const std::string& name, std::function<int()> task);
    
    // Wait for specific process
    bool waitForProcess(pid_t pid, int* status = nullptr);
    
    // Terminate process
    bool terminateProcess(pid_t pid, int signal = SIGTERM);
    bool killProcess(pid_t pid);
    
    // Query state
    ProcessState getProcessState(pid_t pid);
    std::vector<pid_t> getAllProcesses() const;
    
    // Bulk operations
    void waitForAll();
    void terminateAll();
    
    // Information
    void printProcessInfo(pid_t pid);
    void printAllProcesses();
};
```

### ThreadPool

```cpp
class ThreadPool {
public:
    // Constructor
    explicit ThreadPool(size_t numThreads);
    
    // Submit task
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>;
    
    // Pool information
    size_t getPoolSize() const;
    size_t getActiveTasks() const;
    size_t getQueuedTasks();
    
    // Management
    void waitForCompletion();
    void shutdown();
    
    // Monitoring
    ThreadState getThreadState(size_t id);
    void printThreadStates();
};
```

### IPC Classes

```cpp
// Unnamed Pipe
class Pipe {
public:
    Pipe();
    
    int getReadFd() const;
    int getWriteFd() const;
    
    void closeRead();
    void closeWrite();
    void close();
    
    ssize_t write(const void* data, size_t size);
    ssize_t read(void* buffer, size_t size);
    
    bool writeString(const std::string& str);
    std::string readString(size_t maxSize = 4096);
};

// Named Pipe (FIFO)
class NamedPipe {
public:
    NamedPipe(const std::string& pipePath);
    
    bool create(mode_t mode = 0666);
    bool openForReading();
    bool openForWriting();
    void close();
    bool remove();
    
    ssize_t write(const void* data, size_t size);
    ssize_t read(void* buffer, size_t size);
    
    bool writeString(const std::string& str);
    std::string readString(size_t maxSize = 4096);
};

// Shared Memory
class SharedMemory {
public:
    SharedMemory(const std::string& shmName, size_t shmSize);
    
    bool create(mode_t mode = 0666);
    bool open();
    bool map(int prot = PROT_READ | PROT_WRITE);
    void unmap();
    void close();
    bool unlink();
    
    void* getAddress() const;
    size_t getSize() const;
    
    bool writeData(const void* data, size_t dataSize, size_t offset = 0);
    bool readData(void* buffer, size_t dataSize, size_t offset = 0);
    
    template<typename T>
    T* getAs();
};
```

### Synchronization Primitives

```cpp
// SafeMutex with deadlock detection
class SafeMutex {
public:
    explicit SafeMutex(const std::string& name = "");
    
    bool lock(std::chrono::milliseconds timeout = std::chrono::milliseconds(5000));
    bool tryLock();
    void unlock();
    
    bool isLocked() const;
    std::thread::id getOwner() const;
};

// RAII Lock Guard
class SafeLockGuard {
public:
    explicit SafeLockGuard(SafeMutex& m);
    ~SafeLockGuard();
};

// Semaphore
class Semaphore {
public:
    explicit Semaphore(unsigned int value = 0, const std::string& name = "");
    
    bool wait();
    bool tryWait();
    bool timedWait(std::chrono::milliseconds timeout);
    bool post();
    
    int getValue();
};

// Reader-Writer Lock
class RWLock {
public:
    RWLock();
    
    void readLock();
    void readUnlock();
    void writeLock();
    void writeUnlock();
};

// Barrier
class Barrier {
public:
    explicit Barrier(size_t numThreads);
    
    void wait();
    void reset();
};
```

## 🔒 Thread Safety

### Thread-Safe Components
- ✅ `ThreadPool` - All public methods are thread-safe
- ✅ `SafeMutex` - Inherently thread-safe
- ✅ `Semaphore` - Thread-safe operations
- ✅ `RWLock` - Thread-safe read/write operations
- ✅ `Barrier` - Thread-safe synchronization

### Not Thread-Safe (Use Mutexes)
- ❌ `ProcessManager` - Not designed for multi-threaded access
- ❌ `Pipe` / `NamedPipe` - Use external synchronization if shared
- ❌ `SharedMemory` - Data access needs external synchronization

### Best Practices

```cpp
// ✅ GOOD: Protect shared data
SafeMutex mutex;
int sharedCounter = 0;

void incrementCounter() {
    SafeLockGuard lock(mutex);
    sharedCounter++;
}

// ❌ BAD: Unprotected access
int sharedCounter = 0;

void incrementCounter() {
    sharedCounter++;  // RACE CONDITION!
}
```

## ⚡ Performance Considerations

### Thread Pool Sizing

```cpp
// CPU-bound tasks
ThreadPool pool(std::thread::hardware_concurrency());

// I/O-bound tasks
ThreadPool pool(std::thread::hardware_concurrency() * 2);

// Mixed workload
ThreadPool pool(std::thread::hardware_concurrency() + 2);
```

### IPC Performance Comparison

| Mechanism | Latency | Throughput | Use Case |
|-----------|---------|------------|----------|
| **Shared Memory** | ~100ns | Very High | Large data, frequent access |
| **Unnamed Pipe** | ~1µs | High | Parent-child, moderate data |
| **Named Pipe** | ~5µs | Medium | Any processes, moderate data |
| **Message Queue** | ~10µs | Medium | Structured messages |

### Lock Granularity

```cpp
// ✅ GOOD: Fine-grained locking
{
    SafeLockGuard lock(mutex);
    sharedData++;  // Quick operation
}

// ❌ BAD: Coarse-grained locking
{
    SafeLockGuard lock(mutex);
    complexCalculation();  // Locks for too long!
    sharedData++;
}
```

## 🔧 Troubleshooting

### Common Issues

#### 1. Compilation Errors

**Error:** `error: 'thread' in namespace 'std' does not name a type`

**Solution:**
```bash
# Ensure you're using C++17 and pthread flag
g++ -std=c++17 -pthread main.cpp -o program
```

**Error:** `undefined reference to 'pthread_create'`

**Solution:**
```bash
# Link pthread library
g++ main.cpp -pthread -o program
```

#### 2. Runtime Issues

**Error:** Shared memory permission denied

**Solution:**
```bash
# Check shared memory permissions
ls -la /dev/shm/

# Clean up old shared memory segments
rm /dev/shm/test_shm
```

**Error:** Too many open files

**Solution:**
```bash
# Increase file descriptor limit
ulimit -n 4096

# Or permanently in /etc/security/limits.conf
* soft nofile 4096
* hard nofile 8192
```

#### 3. Deadlock Detection

If program hangs:
```bash
# Get process ID
ps aux | grep test_manager

# Check thread states
gdb -p <PID>
(gdb) info threads
(gdb) thread apply all bt

# Or use system tools
# FreeBSD
procstat -k <PID>

# Linux
pstack <PID>
```

### Debugging Tips

```cpp
// Enable verbose output
#define DEBUG_MODE 1

#if DEBUG_MODE
    std::cout << "[DEBUG] Thread " << std::this_thread::get_id() 
              << " acquired lock" << std::endl;
#endif
```

### Memory Leaks

```bash
# Use Valgrind (Linux)
valgrind --leak-check=full ./test_manager

# Or Address Sanitizer
g++ -fsanitize=address -g main.cpp -o program
./program
```

## 🤝 Contributing

### Code Style

- Use 4 spaces for indentation
- Opening braces on same line
- Meaningful variable names
- Comment complex logic

```cpp
// Good
void processData(const std::vector<int>& data) {
    for (const auto& item : data) {
        // Process each item
    }
}

// Bad
void pd(const std::vector<int>& d) {
    for(auto i:d){processItem(i);}
}
```

### Adding New Features

1. Create header in `include/`
2. Implement in `src/`
3. Add tests in `test/main.cpp`
4. Update `CMakeLists.txt`
5. Document in README

## 📝 License

This project is created for educational purposes.

**Use for:**
- ✅ Learning operating system concepts
- ✅ Academic assignments
- ✅ Personal projects
- ✅ Teaching materials

**Restrictions:**
- ❌ Do not submit as your own work without understanding
- ❌ Commercial use without proper licensing

## 👨‍💻 Author

Created as a comprehensive demonstration of process and thread management concepts for FreeBSD/Unix systems.

## 📚 Additional Resources

### Recommended Reading
- **"Operating System Concepts" by Silberschatz, Galvin, Gagne** - Chapter 3 (Processes), Chapter 4 (Threads)
- **"UNIX Network Programming" by W. Richard Stevens** - IPC mechanisms
- **"C++ Concurrency in Action" by Anthony Williams** - Modern C++ threading

### Online Resources
- [POSIX Threads Programming](https://computing.llnl.gov/tutorials/pthreads/)
- [FreeBSD Handbook - Processes](https://docs.freebsd.org/en/books/handbook/)
- [Linux man pages](https://man7.org/linux/man-pages/)

### Related Topics
- Scheduling algorithms
- Memory management
- File systems
- Network programming

---

## 📞 Support

For questions or issues:
1. Check the [Troubleshooting](#troubleshooting) section
2. Review example code in `test/main.cpp`
3. Consult FreeBSD/POSIX documentation

---

**Last Updated:** December 2025  
**Version:** 1.0.0