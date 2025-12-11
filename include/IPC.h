#ifndef PROCESS_THREAD_MANAGER_IPC_H
#define PROCESS_THREAD_MANAGER_IPC_H

#include <string>
#include <vector>
#include <memory>
#include <sys/types.h>

namespace PTManager {

// Unnamed Pipe
class Pipe {
private:
    int fds[2];
    bool isOpen;

public:
    Pipe();
    ~Pipe();

    // Disable copy
    Pipe(const Pipe&) = delete;
    Pipe& operator=(const Pipe&) = delete;

    int getReadFd() const { return fds[0]; }
    int getWriteFd() const { return fds[1]; }

    void closeRead();
    void closeWrite();
    void close();

    ssize_t write(const void* data, size_t size);
    ssize_t read(void* buffer, size_t size);

    // Helper for string communication
    bool writeString(const std::string& str);
    std::string readString(size_t maxSize = 4096);
};

// Named Pipe (FIFO)
class NamedPipe {
private:
    std::string path;
    int fd;
    bool isCreated;

public:
    NamedPipe(const std::string& pipePath);
    ~NamedPipe();

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
private:
    std::string name;
    int fd;
    void* addr;
    size_t size;
    bool isCreated;
    bool isMapped;

public:
    SharedMemory(const std::string& shmName, size_t shmSize);
    ~SharedMemory();

    bool create(mode_t mode = 0666);
    bool open();
    bool map(int prot = 3); // PROT_READ | PROT_WRITE
    void unmap();
    void close();
    bool unlink();

    void* getAddress() const { return addr; }
    size_t getSize() const { return size; }

    // Helper methods
    bool writeData(const void* data, size_t dataSize, size_t offset = 0);
    bool readData(void* buffer, size_t dataSize, size_t offset = 0);

    template<typename T>
    T* getAs() { return static_cast<T*>(addr); }
};

// Message Queue wrapper
struct Message {
    long type;
    char data[256];
};

class MessageQueue {
private:
    key_t key;
    int msgid;

public:
    MessageQueue(const std::string& path, int projId);
    ~MessageQueue();

    bool create();
    bool open();
    bool remove();

    bool send(const Message& msg, int flags = 0);
    bool receive(Message& msg, long type = 0, int flags = 0);
};

} // namespace PTManager


#endif //PROCESS_THREAD_MANAGER_IPC_H