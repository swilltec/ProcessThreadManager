#include "ProcessManager.h"
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <cstring>


namespace PTManager {

/**
 * @brief Constructs a Process object with initial metadata
 *
 * @param p Process ID (PID) assigned by the operating system
 * @param n Human-readable name for process identification
 *
 * Initializes the process in CREATED state with undefined exit status.
 * The state will be updated to RUNNING after the process actually starts.
 */
Process::Process(pid_t p, const std::string& n)
    : pid(p), state(ProcessState::CREATED), name(n), exitStatus(-1) {}

/**
 * @brief Destructor that ensures all managed processes are cleaned up
 *
 * Terminates all running processes gracefully (SIGTERM), then forcefully (SIGKILL)
 * if necessary, and waits for them to complete. Prevents zombie processes.
 */
ProcessManager::~ProcessManager() {
    terminateAll();
}

/**
 * @brief Creates a new child process to execute a task
 *
 * @param name Human-readable identifier for the process
 * @param task Function object containing the code to execute in the child process
 * @return PID of the created child process, or -1 on failure
 *
 * Uses fork() to create a child process. The child executes the task and exits
 * with the task's return value. The parent records the process metadata and
 * continues execution. The task function should not return to caller - it will
 * be terminated with exit().
 */
pid_t ProcessManager::createProcess(const std::string& name,
                                    std::function<int()> task) {
    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Fork failed: " << strerror(errno) << std::endl;
        return -1;
    }

    if (pid == 0) {
        // Child process
        int exitCode = task();
        exit(exitCode);
    }

    // Parent process
    Process proc(pid, name);
    proc.state = ProcessState::RUNNING;
    processes.emplace(pid, proc);

    std::cout << "Created process '" << name << "' with PID: " << pid << std::endl;
    return pid;
}

/**
 * @brief Waits for a specific process to terminate and retrieves its exit status
 *
 * @param pid Process ID to wait for
 * @param status Optional pointer to store the exit status (can be nullptr)
 * @return true if process was found and successfully waited for, false otherwise
 *
 * Blocks until the specified process terminates. Updates internal process state
 * and extracts the exit code if the process exited normally. Returns false if
 * the PID is not managed by this ProcessManager.
 */
bool ProcessManager::waitForProcess(pid_t pid, int* status) {
    auto it = processes.find(pid);
    if (it == processes.end()) {
        return false;
    }

    int wstatus;
    pid_t result = waitpid(pid, &wstatus, 0);

    if (result == pid) {
        it->second.state = ProcessState::TERMINATED;
        it->second.exitStatus = WIFEXITED(wstatus) ? WEXITSTATUS(wstatus) : -1;

        if (status) {
            *status = it->second.exitStatus;
        }

        std::cout << "Process " << pid << " (" << it->second.name
                  << ") terminated with status: " << it->second.exitStatus << std::endl;
        return true;
    }

    return false;
}

/**
 * @brief Sends a signal to a managed process
 *
 * @param pid Process ID to send signal to
 * @param signal Signal number (e.g., SIGTERM, SIGKILL, SIGSTOP)
 * @return true if signal was successfully sent, false if process not found or kill() failed
 *
 * Does not wait for process termination - only sends the signal.
 * The process may ignore or handle certain signals (except SIGKILL and SIGSTOP).
 */
bool ProcessManager::terminateProcess(pid_t pid, int signal) {
    auto it = processes.find(pid);
    if (it == processes.end()) {
        return false;
    }

    if (kill(pid, signal) == 0) {
        std::cout << "Sent signal " << signal << " to process " << pid << std::endl;
        return true;
    }

    return false;
}

/**
 * @brief Forcefully terminates a process using SIGKILL
 *
 * @param pid Process ID to kill
 * @return true if SIGKILL was successfully sent, false otherwise
 *
 * SIGKILL cannot be caught or ignored - guarantees process termination.
 * Use terminateProcess(pid, SIGTERM) for graceful shutdown when possible.
 */
bool ProcessManager::killProcess(pid_t pid) {
    return terminateProcess(pid, SIGKILL);
}

/**
 * @brief Queries and updates the current state of a process
 *
 * @param pid Process ID to check
 * @return Current ProcessState, or TERMINATED if PID not found
 *
 * Uses non-blocking waitpid() with WNOHANG to check if the process has terminated
 * without blocking. Updates internal state if termination is detected.
 * Does not affect zombie processes - they remain until explicitly waited for.
 */
ProcessState ProcessManager::getProcessState(pid_t pid) {
    auto it = processes.find(pid);
    if (it == processes.end()) {
        return ProcessState::TERMINATED;
    }

    // Check if process is still running
    int status;
    pid_t result = waitpid(pid, &status, WNOHANG);

    if (result == pid) {
        it->second.state = ProcessState::TERMINATED;
        it->second.exitStatus = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    } else if (result == 0) {
        it->second.state = ProcessState::RUNNING;
    }

    return it->second.state;
}

/**
 * @brief Retrieves a list of all managed process IDs
 *
 * @return Vector containing PIDs of all processes tracked by this manager
 *
 * Includes both running and terminated processes that haven't been removed
 * from the internal tracking map.
 */
std::vector<pid_t> ProcessManager::getAllProcesses() const {
    std::vector<pid_t> pids;
    for (const auto& pair : processes) {
        pids.push_back(pair.first);
    }
    return pids;
}

/**
 * @brief Waits for all managed processes to terminate
 *
 * Blocks until every non-terminated process has exited.
 * Skips processes already marked as TERMINATED to avoid waiting on
 * already-reaped processes.
 */
void ProcessManager::waitForAll() {
    for (auto& pair : processes) {
        if (pair.second.state != ProcessState::TERMINATED) {
            waitForProcess(pair.first);
        }
    }
}

/**
 * @brief Terminates all managed processes gracefully, then forcefully if needed
 *
 * First attempts graceful termination by sending SIGTERM to all processes,
 * waits 1 second for them to clean up, then sends SIGKILL to any survivors.
 * Finally waits for all processes to be reaped to prevent zombies.
 *
 * This two-phase approach allows processes to perform cleanup operations
 * before being forcefully killed.
 */
void ProcessManager::terminateAll() {
    for (auto& pair : processes) {
        if (pair.second.state != ProcessState::TERMINATED) {
            terminateProcess(pair.first, SIGTERM);
        }
    }

    // Wait a bit and then force kill if necessary
    sleep(1);

    for (auto& pair : processes) {
        if (pair.second.state != ProcessState::TERMINATED) {
            killProcess(pair.first);
        }
    }

    waitForAll();
}

/**
 * @brief Prints detailed information about a specific process
 *
 * @param pid Process ID to display information for
 *
 * Displays PID, name, current state, and exit status (if terminated).
 * Prints "not found" message if the PID is not managed by this ProcessManager.
 */
void ProcessManager::printProcessInfo(pid_t pid) {
    auto it = processes.find(pid);
    if (it == processes.end()) {
        std::cout << "Process " << pid << " not found" << std::endl;
        return;
    }

    const Process& proc = it->second;
    std::cout << "Process " << proc.pid << " (" << proc.name << "):" << std::endl;
    std::cout << "  State: ";

    switch (proc.state) {
        case ProcessState::CREATED: std::cout << "CREATED"; break;
        case ProcessState::RUNNING: std::cout << "RUNNING"; break;
        case ProcessState::BLOCKED: std::cout << "BLOCKED"; break;
        case ProcessState::READY: std::cout << "READY"; break;
        case ProcessState::TERMINATED: std::cout << "TERMINATED"; break;
    }

    std::cout << std::endl;
    if (proc.state == ProcessState::TERMINATED) {
        std::cout << "  Exit Status: " << proc.exitStatus << std::endl;
    }
}

/**
 * @brief Prints a summary of all managed processes
 *
 * Displays total process count followed by detailed information for each process.
 * Useful for debugging and monitoring the state of all child processes.
 */
void ProcessManager::printAllProcesses() {
    std::cout << "\n=== Process Manager Status ===" << std::endl;
    std::cout << "Total processes: " << processes.size() << std::endl;

    for (const auto& pair : processes) {
        printProcessInfo(pair.first);
    }
    std::cout << "============================\n" << std::endl;
}

} // namespace PTManager