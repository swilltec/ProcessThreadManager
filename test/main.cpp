#include "ProcessManager.h"
#include "ThreadPool.h"
#include "IPC.h"
#include "Synchronization.h"
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <random>
#include <vector>
#include <cstring>
#include <sys/wait.h>

using namespace PTManager;

// ============================================================================
// PROCESS MANAGEMENT TESTS
// ============================================================================

int workerProcess(int id, int seconds) {
    std::cout << "[Process " << getpid() << "] Worker " << id
              << " starting, will run for " << seconds << " seconds" << std::endl;

    for (int i = 0; i < seconds; ++i) {
        std::cout << "[Process " << getpid() << "] Worker " << id
                  << " working... (" << (i+1) << "/" << seconds << ")" << std::endl;
        sleep(1);
    }

    std::cout << "[Process " << getpid() << "] Worker " << id
              << " completed!" << std::endl;
    return id * 10;
}

void testProcessManagement() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "  TEST 1: PROCESS MANAGEMENT\n";
    std::cout << "========================================\n";
    std::cout << "Main process PID: " << getpid() << std::endl;

    ProcessManager pm;

    std::cout << "\n--- Creating worker processes ---" << std::endl;
    pid_t p1 = pm.createProcess("Worker-1", []() { return workerProcess(1, 2); });
    pid_t p2 = pm.createProcess("Worker-2", []() { return workerProcess(2, 1); });
    pid_t p3 = pm.createProcess("Worker-3", []() { return workerProcess(3, 3); });

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    pm.printAllProcesses();

    std::cout << "\n--- Waiting for Worker-2 ---" << std::endl;
    int status;
    pm.waitForProcess(p2, &status);
    std::cout << "Worker-2 exited with status: " << status << std::endl;

    std::cout << "\n--- Waiting for remaining processes ---" << std::endl;
    pm.waitForAll();

    pm.printAllProcesses();
    std::cout << "✓ Process management test completed\n" << std::endl;
}

// ============================================================================
// THREAD POOL TESTS
// ============================================================================

int fibonacciTask(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; ++i) {
        int temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

void ioTask(int id, int ms) {
    std::cout << "[Thread " << std::this_thread::get_id()
              << "] Starting I/O task " << id << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    std::cout << "[Thread " << std::this_thread::get_id()
              << "] Completed I/O task " << id << std::endl;
}

void testThreadPool() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "  TEST 2: THREAD POOL\n";
    std::cout << "========================================\n";

    ThreadPool pool(4);

    std::cout << "\n--- Submitting CPU-intensive tasks ---" << std::endl;
    std::vector<std::future<int>> cpuResults;

    for (int i = 0; i < 6; ++i) {
        cpuResults.push_back(
            pool.enqueue([i]() {
                std::cout << "[Thread " << std::this_thread::get_id()
                          << "] Computing Fibonacci(" << (25 + i) << ")" << std::endl;
                int result = fibonacciTask(25 + i);
                return result;
            })
        );
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    pool.printThreadStates();

    std::cout << "\n--- Submitting I/O tasks ---" << std::endl;
    std::vector<std::future<void>> ioResults;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100, 300);

    for (int i = 0; i < 4; ++i) {
        int delay = dis(gen);
        ioResults.push_back(
            pool.enqueue([i, delay]() { ioTask(i, delay); })
        );
    }

    std::cout << "\n--- Collecting results ---" << std::endl;
    for (size_t i = 0; i < cpuResults.size(); ++i) {
        int result = cpuResults[i].get();
        std::cout << "CPU Task " << i << " result: " << result << std::endl;
    }

    for (auto& future : ioResults) {
        future.get();
    }

    pool.waitForCompletion();
    std::cout << "✓ Thread pool test completed\n" << std::endl;
}

// ============================================================================
// IPC TESTS
// ============================================================================

void testUnnamedPipe() {
    std::cout << "\n--- Test: Unnamed Pipe ---" << std::endl;

    Pipe pipe;

    pid_t pid = fork();
    if (pid == 0) {
        pipe.closeWrite();
        std::string msg = pipe.readString();
        std::cout << "[Child] Received: " << msg << std::endl;
        pipe.close();
        exit(0);
    } else {
        pipe.closeRead();
        std::string msg = "Hello from parent via unnamed pipe!";
        pipe.writeString(msg);
        std::cout << "[Parent] Sent: " << msg << std::endl;
        pipe.close();
        waitpid(pid, nullptr, 0);
    }
}

void testNamedPipe() {
    std::cout << "\n--- Test: Named Pipe (FIFO) ---" << std::endl;

    const std::string pipePath = "/tmp/test_fifo";

    pid_t pid = fork();
    if (pid == 0) {
        sleep(1);
        NamedPipe reader(pipePath);
        if (reader.openForReading()) {
            std::string msg = reader.readString();
            std::cout << "[Child] Received via FIFO: " << msg << std::endl;
            reader.close();
        }
        exit(0);
    } else {
        NamedPipe writer(pipePath);
        writer.create();
        if (writer.openForWriting()) {
            std::string msg = "Hello from parent via named pipe!";
            writer.writeString(msg);
            std::cout << "[Parent] Sent via FIFO: " << msg << std::endl;
            writer.close();
        }
        waitpid(pid, nullptr, 0);
        writer.remove();
    }
}

void testSharedMemory() {
    std::cout << "\n--- Test: Shared Memory ---" << std::endl;

    const std::string shmName = "/test_shm";
    const size_t shmSize = 4096;

    pid_t pid = fork();
    if (pid == 0) {
        sleep(1);
        SharedMemory shm(shmName, shmSize);
        if (shm.open() && shm.map()) {
            char buffer[256];
            shm.readData(buffer, sizeof(buffer));
            std::cout << "[Child] Read from shared memory: " << buffer << std::endl;
            shm.unmap();
            shm.close();
        }
        exit(0);
    } else {
        SharedMemory shm(shmName, shmSize);
        if (shm.create() && shm.map()) {
            const char* msg = "Hello from parent via shared memory!";
            shm.writeData(msg, strlen(msg) + 1);
            std::cout << "[Parent] Wrote to shared memory: " << msg << std::endl;
            waitpid(pid, nullptr, 0);
            shm.unmap();
            shm.close();
            shm.unlink();
        }
    }
}

void testIPC() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "  TEST 3: INTER-PROCESS COMMUNICATION\n";
    std::cout << "========================================\n";

    testUnnamedPipe();
    testNamedPipe();
    testSharedMemory();

    std::cout << "✓ IPC test completed\n" << std::endl;
}

// ============================================================================
// SYNCHRONIZATION TESTS
// ============================================================================

void testRaceCondition() {
    std::cout << "\n--- Test: Race Condition Demonstration ---" << std::endl;

    int counter = 0;
    const int iterations = 1000;
    const int numThreads = 10;

    ThreadPool pool(numThreads);
    std::vector<std::future<void>> futures;

    std::cout << "Incrementing counter WITHOUT synchronization..." << std::endl;

    for (int i = 0; i < numThreads; ++i) {
        futures.push_back(pool.enqueue([&counter, iterations]() {
            for (int j = 0; j < iterations; ++j) {
                counter++;  // RACE CONDITION!
            }
        }));
    }

    for (auto& f : futures) {
        f.get();
    }

    std::cout << "Expected: " << (numThreads * iterations)
              << " | Actual: " << counter
              << " | Lost updates: " << (numThreads * iterations - counter) << std::endl;
}

void testMutex() {
    std::cout << "\n--- Test: Mutex Protection ---" << std::endl;

    int counter = 0;
    SafeMutex mutex("counter_mutex");
    const int iterations = 1000;
    const int numThreads = 10;

    ThreadPool pool(numThreads);
    std::vector<std::future<void>> futures;

    std::cout << "Incrementing counter WITH mutex protection..." << std::endl;

    for (int i = 0; i < numThreads; ++i) {
        futures.push_back(pool.enqueue([&counter, &mutex, iterations]() {
            for (int j = 0; j < iterations; ++j) {
                SafeLockGuard lock(mutex);
                counter++;
            }
        }));
    }

    for (auto& f : futures) {
        f.get();
    }

    std::cout << "Expected: " << (numThreads * iterations)
              << " | Actual: " << counter
              << " | Perfect sync: " << (counter == numThreads * iterations ? "YES ✓" : "NO ✗") << std::endl;
}

void testSemaphore() {
    std::cout << "\n--- Test: Semaphore (Producer-Consumer) ---" << std::endl;

    const int bufferSize = 5;
    const int itemCount = 10;

    std::vector<int> buffer(bufferSize);
    int writePos = 0, readPos = 0;

    Semaphore empty(bufferSize, "empty_slots");
    Semaphore full(0, "full_slots");
    SafeMutex mutex("buffer_mutex");

    ThreadPool pool(2);

    auto producer = pool.enqueue([&]() {
        for (int i = 0; i < itemCount; ++i) {
            empty.wait();
            {
                SafeLockGuard lock(mutex);
                buffer[writePos] = i;
                std::cout << "[Producer] Produced: " << i << " at pos " << writePos << std::endl;
                writePos = (writePos + 1) % bufferSize;
            }
            full.post();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });

    auto consumer = pool.enqueue([&]() {
        for (int i = 0; i < itemCount; ++i) {
            full.wait();
            int item;
            {
                SafeLockGuard lock(mutex);
                item = buffer[readPos];
                std::cout << "[Consumer] Consumed: " << item << " from pos " << readPos << std::endl;
                readPos = (readPos + 1) % bufferSize;
            }
            empty.post();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    producer.get();
    consumer.get();
    std::cout << "Producer-Consumer completed successfully!" << std::endl;
}

void testRWLock() {
    std::cout << "\n--- Test: Reader-Writer Lock ---" << std::endl;

    std::string sharedData = "Initial data";
    RWLock rwLock;

    ThreadPool pool(6);
    std::vector<std::future<void>> futures;

    // Readers
    for (int i = 0; i < 4; ++i) {
        futures.push_back(pool.enqueue([&, i]() {
            for (int j = 0; j < 2; ++j) {
                rwLock.readLock();
                std::cout << "[Reader " << i << "] Read: " << sharedData << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                rwLock.readUnlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(30));
            }
        }));
    }

    // Writers
    for (int i = 0; i < 2; ++i) {
        futures.push_back(pool.enqueue([&, i]() {
            rwLock.writeLock();
            sharedData = "Data from writer " + std::to_string(i);
            std::cout << "[Writer " << i << "] Wrote: " << sharedData << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            rwLock.writeUnlock();
        }));
    }

    for (auto& f : futures) {
        f.get();
    }
    std::cout << "Reader-Writer test completed!" << std::endl;
}

void testBarrier() {
    std::cout << "\n--- Test: Barrier Synchronization ---" << std::endl;

    const int numThreads = 4;
    const int phases = 2;

    Barrier barrier(numThreads);
    ThreadPool pool(numThreads);
    std::vector<std::future<void>> futures;

    for (int i = 0; i < numThreads; ++i) {
        futures.push_back(pool.enqueue([&, i]() {
            for (int phase = 0; phase < phases; ++phase) {
                std::cout << "[Thread " << i << "] Phase " << phase << " - Working..." << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100 * (i + 1)));

                std::cout << "[Thread " << i << "] Phase " << phase << " - At barrier" << std::endl;
                barrier.wait();

                std::cout << "[Thread " << i << "] Phase " << phase << " - Proceeding!" << std::endl;
            }
        }));
    }

    for (auto& f : futures) {
        f.get();
    }
    std::cout << "Barrier synchronization completed!" << std::endl;
}

void testDeadlockPrevention() {
    std::cout << "\n--- Test: Deadlock Prevention ---" << std::endl;

    SafeMutex mutex1("Resource_A");
    SafeMutex mutex2("Resource_B");

    ThreadPool pool(2);

    auto t1 = pool.enqueue([&]() {
        std::cout << "[Thread 1] Locking Resource_A..." << std::endl;
        if (mutex1.lock()) {
            std::cout << "[Thread 1] Locked Resource_A" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            std::cout << "[Thread 1] Attempting Resource_B..." << std::endl;
            if (mutex2.lock(std::chrono::milliseconds(2000))) {
                std::cout << "[Thread 1] Locked Resource_B" << std::endl;
                mutex2.unlock();
            } else {
                std::cout << "[Thread 1] Timeout on Resource_B (deadlock avoided!)" << std::endl;
            }
            mutex1.unlock();
        }
    });

    auto t2 = pool.enqueue([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        std::cout << "[Thread 2] Locking Resource_B..." << std::endl;
        if (mutex2.lock()) {
            std::cout << "[Thread 2] Locked Resource_B" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            std::cout << "[Thread 2] Attempting Resource_A..." << std::endl;
            if (mutex1.lock(std::chrono::milliseconds(2000))) {
                std::cout << "[Thread 2] Locked Resource_A" << std::endl;
                mutex1.unlock();
            } else {
                std::cout << "[Thread 2] Timeout on Resource_A (deadlock avoided!)" << std::endl;
            }
            mutex2.unlock();
        }
    });

    t1.get();
    t2.get();
    std::cout << "Deadlock prevention demonstrated!" << std::endl;
}

void testSynchronization() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "  TEST 4: SYNCHRONIZATION PRIMITIVES\n";
    std::cout << "========================================\n";

    testRaceCondition();
    testMutex();
    testSemaphore();
    testRWLock();
    testBarrier();
    testDeadlockPrevention();

    std::cout << "✓ Synchronization test completed\n" << std::endl;
}

// ============================================================================
// MAIN
// ============================================================================

void printMenu() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║  Process & Thread Management Library  ║\n";
    std::cout << "║           Test Suite                   ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Select test to run:\n";
    std::cout << "  1. Process Management\n";
    std::cout << "  2. Thread Pool\n";
    std::cout << "  3. Inter-Process Communication (IPC)\n";
    std::cout << "  4. Synchronization Primitives\n";
    std::cout << "  5. Run All Tests\n";
    std::cout << "  0. Exit\n";
    std::cout << "\nChoice: ";
}

int main(int argc, char* argv[]) {
    // If command-line argument provided, run that test directly
    if (argc > 1) {
        int choice = atoi(argv[1]);

        switch (choice) {
            case 1: testProcessManagement(); break;
            case 2: testThreadPool(); break;
            case 3: testIPC(); break;
            case 4: testSynchronization(); break;
            case 5:
                testProcessManagement();
                testThreadPool();
                testIPC();
                testSynchronization();
                std::cout << "\n╔════════════════════════════════════════╗\n";
                std::cout << "║     ALL TESTS COMPLETED SUCCESSFULLY   ║\n";
                std::cout << "╚════════════════════════════════════════╝\n";
                break;
            default:
                std::cerr << "Invalid test number: " << choice << std::endl;
                return 1;
        }
        return 0;
    }

    // Interactive mode
    while (true) {
        printMenu();

        int choice;
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 0:
                std::cout << "\nExiting...\n";
                return 0;

            case 1:
                testProcessManagement();
                break;

            case 2:
                testThreadPool();
                break;

            case 3:
                testIPC();
                break;

            case 4:
                testSynchronization();
                break;

            case 5:
                testProcessManagement();
                testThreadPool();
                testIPC();
                testSynchronization();
                std::cout << "\n╔════════════════════════════════════════╗\n";
                std::cout << "║     ALL TESTS COMPLETED SUCCESSFULLY   ║\n";
                std::cout << "╚════════════════════════════════════════╝\n";
                break;

            default:
                std::cout << "Invalid choice. Please select 0-5.\n";
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.ignore(10000, '\n');
        std::cin.get();
    }

    return 0;
}