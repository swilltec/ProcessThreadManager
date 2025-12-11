#ifndef PROCESS_THREAD_MANAGER_PROCESSMANAGER_H
#define PROCESS_THREAD_MANAGER_PROCESSMANAGER_H

#include <functional>
#include <vector>
#include <unordered_map>
#include <string>
#include <sys/types.h>

namespace PTManager {

    enum class ProcessState {
        CREATED,
        RUNNING,
        BLOCKED,
        READY,
        TERMINATED
    };

    class Process {
    public:
        pid_t pid;
        ProcessState state;
        std::string name;
        int exitStatus;

        Process(pid_t p, const std::string& n);
        ~Process() = default;
    };

    class ProcessManager {
    private:
        std::unordered_map<pid_t, Process> processes;

    public:
        ProcessManager() = default;
        ~ProcessManager();

        // Process creation and management
        pid_t createProcess(const std::string& name,
                           std::function<int()> task);

        bool waitForProcess(pid_t pid, int* status = nullptr);
        bool terminateProcess(pid_t pid, int signal = 15);
        bool killProcess(pid_t pid);

        ProcessState getProcessState(pid_t pid);
        std::vector<pid_t> getAllProcesses() const;
        void waitForAll();
        void terminateAll();

        // Process information
        void printProcessInfo(pid_t pid);
        void printAllProcesses();
    };

} // namespace PTManager

#endif //PROCESS_THREAD_MANAGER_PROCESSMANAGER_H