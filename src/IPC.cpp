#include "IPC.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>

namespace PTManager {

// ===== Pipe Implementation =====

/**
 * @brief Constructs an anonymous pipe for inter-process communication
 *
 * Creates a unidirectional communication channel using the pipe() system call.
 * The pipe consists of two file descriptors: fds[0] for reading and fds[1] for writing.
 * Logs error to stderr if pipe creation fails.
 */
Pipe::Pipe() : isOpen(false) {
    if (pipe(fds) == 0) {
        isOpen = true;
    } else {
        std::cerr << "Failed to create pipe: " << strerror(errno) << std::endl;
    }
}

/**
 * @brief Destructor that ensures proper cleanup of pipe resources
 *
 * Automatically closes both ends of the pipe to prevent file descriptor leaks.
 */
Pipe::~Pipe() {
    close();
}

/**
 * @brief Closes the read end of the pipe
 *
 * Invalidates the read file descriptor and prevents further read operations.
 * Safe to call multiple times or on already-closed descriptors.
 */
void Pipe::closeRead() {
    if (isOpen && fds[0] != -1) {
        ::close(fds[0]);
        fds[0] = -1;
    }
}

/**
 * @brief Closes the write end of the pipe
 *
 * Invalidates the write file descriptor and prevents further write operations.
 * Safe to call multiple times or on already-closed descriptors.
 */
void Pipe::closeWrite() {
    if (isOpen && fds[1] != -1) {
        ::close(fds[1]);
        fds[1] = -1;
    }
}

/**
 * @brief Closes both ends of the pipe and marks it as closed
 *
 * Ensures complete cleanup of pipe resources. Sets isOpen to false to prevent
 * operations on a closed pipe.
 */
void Pipe::close() {
    closeRead();
    closeWrite();
    isOpen = false;
}

/**
 * @brief Writes raw binary data to the pipe
 *
 * @param data Pointer to the data buffer to write
 * @param size Number of bytes to write
 * @return Number of bytes written, or -1 on error
 *
 * @note Returns -1 if pipe is closed or write end is invalid
 */
ssize_t Pipe::write(const void* data, size_t size) {
    if (!isOpen || fds[1] == -1) return -1;
    return ::write(fds[1], data, size);
}

/**
 * @brief Reads raw binary data from the pipe
 *
 * @param buffer Pointer to the buffer where data will be stored
 * @param size Maximum number of bytes to read
 * @return Number of bytes read, or -1 on error
 *
 * @note Returns -1 if pipe is closed or read end is invalid
 */
ssize_t Pipe::read(void* buffer, size_t size) {
    if (!isOpen || fds[0] == -1) return -1;
    return ::read(fds[0], buffer, size);
}

/**
 * @brief Writes a string with length-prefixed protocol
 *
 * @param str The string to write
 * @return true if entire string was written successfully, false otherwise
 *
 * Protocol: First writes the length as size_t, then writes the string content.
 * This allows the receiver to know exactly how many bytes to read.
 */
bool Pipe::writeString(const std::string& str) {
    size_t len = str.length();
    if (write(&len, sizeof(len)) != sizeof(len)) return false;
    return write(str.c_str(), len) == static_cast<ssize_t>(len);
}

/**
 * @brief Reads a length-prefixed string from the pipe
 *
 * @param maxSize Maximum allowed string length for safety
 * @return The read string, or empty string on error
 *
 * Protocol: First reads the length, then reads that many bytes.
 * Enforces maxSize limit to prevent memory exhaustion attacks.
 * Allocates buffer with extra byte for null terminator.
 */
std::string Pipe::readString(size_t maxSize) {
    size_t len;
    if (read(&len, sizeof(len)) != sizeof(len)) return "";

    if (len > maxSize) len = maxSize;

    std::vector<char> buffer(len + 1, 0);
    if (read(buffer.data(), len) != static_cast<ssize_t>(len)) return "";

    return std::string(buffer.data(), len);
}

// ===== NamedPipe Implementation =====

/**
 * @brief Constructs a named pipe (FIFO) wrapper
 *
 * @param pipePath Filesystem path where the FIFO will be created
 *
 * Initializes the object but does not create the actual FIFO.
 * Call create() to establish the named pipe in the filesystem.
 */
NamedPipe::NamedPipe(const std::string& pipePath)
    : path(pipePath), fd(-1), isCreated(false) {}

/**
 * @brief Destructor that ensures cleanup of named pipe resources
 *
 * Closes the file descriptor and removes the FIFO from the filesystem
 * if this instance created it (prevents premature removal by readers/writers).
 */
NamedPipe::~NamedPipe() {
    close();
    if (isCreated) {
        remove();
    }
}

/**
 * @brief Creates the named pipe (FIFO) in the filesystem
 *
 * @param mode Permission bits for the FIFO (e.g., 0666)
 * @return true if created or already exists, false on error
 *
 * Uses mkfifo() to create a special file in the filesystem that acts as a pipe.
 * Handles EEXIST gracefully by treating existing FIFOs as success.
 */
bool NamedPipe::create(mode_t mode) {
    if (mkfifo(path.c_str(), mode) == 0) {
        isCreated = true;
        std::cout << "Created named pipe: " << path << std::endl;
        return true;
    }

    if (errno == EEXIST) {
        std::cout << "Named pipe already exists: " << path << std::endl;
        return true;
    }

    std::cerr << "Failed to create named pipe: " << strerror(errno) << std::endl;
    return false;
}

/**
 * @brief Opens the named pipe for reading
 *
 * @return true if successfully opened, false otherwise
 *
 * Blocks until a writer opens the other end of the pipe.
 * This is standard FIFO behavior per POSIX specifications.
 */
bool NamedPipe::openForReading() {
    fd = ::open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        std::cerr << "Failed to open pipe for reading: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Opens the named pipe for writing
 *
 * @return true if successfully opened, false otherwise
 *
 * Blocks until a reader opens the other end of the pipe.
 * This is standard FIFO behavior per POSIX specifications.
 */
bool NamedPipe::openForWriting() {
    fd = ::open(path.c_str(), O_WRONLY);
    if (fd < 0) {
        std::cerr << "Failed to open pipe for writing: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Closes the file descriptor
 *
 * Does not remove the FIFO from the filesystem.
 * Safe to call multiple times.
 */
void NamedPipe::close() {
    if (fd >= 0) {
        ::close(fd);
        fd = -1;
    }
}

/**
 * @brief Removes the named pipe from the filesystem
 *
 * @return true if successfully removed, false otherwise
 *
 * Uses unlink() to delete the FIFO special file.
 * Should only be called by the process that created the FIFO.
 */
bool NamedPipe::remove() {
    if (unlink(path.c_str()) == 0) {
        std::cout << "Removed named pipe: " << path << std::endl;
        return true;
    }
    return false;
}

/**
 * @brief Writes raw binary data to the named pipe
 *
 * @param data Pointer to the data buffer
 * @param size Number of bytes to write
 * @return Number of bytes written, or -1 if pipe not open
 */
ssize_t NamedPipe::write(const void* data, size_t size) {
    if (fd < 0) return -1;
    return ::write(fd, data, size);
}

/**
 * @brief Reads raw binary data from the named pipe
 *
 * @param buffer Pointer to the destination buffer
 * @param size Maximum number of bytes to read
 * @return Number of bytes read, or -1 if pipe not open
 */
ssize_t NamedPipe::read(void* buffer, size_t size) {
    if (fd < 0) return -1;
    return ::read(fd, buffer, size);
}

/**
 * @brief Writes a string with length-prefixed protocol
 *
 * @param str The string to write
 * @return true if entire string was written successfully, false otherwise
 *
 * Protocol: Writes length first (size_t), then string content.
 */
bool NamedPipe::writeString(const std::string& str) {
    size_t len = str.length();
    if (write(&len, sizeof(len)) != sizeof(len)) return false;
    return write(str.c_str(), len) == static_cast<ssize_t>(len);
}

/**
 * @brief Reads a length-prefixed string from the named pipe
 *
 * @param maxSize Maximum allowed string length for safety
 * @return The read string, or empty string on error
 *
 * Protocol: Reads length first, then content.
 * Enforces maxSize limit to prevent memory exhaustion.
 */
std::string NamedPipe::readString(size_t maxSize) {
    size_t len;
    if (read(&len, sizeof(len)) != sizeof(len)) return "";

    if (len > maxSize) len = maxSize;

    std::vector<char> buffer(len + 1, 0);
    if (read(buffer.data(), len) != static_cast<ssize_t>(len)) return "";

    return std::string(buffer.data(), len);
}

// ===== SharedMemory Implementation =====

/**
 * @brief Constructs a shared memory object wrapper
 *
 * @param shmName Name of the shared memory object (must start with '/')
 * @param shmSize Size of the shared memory region in bytes
 *
 * Initializes the object but does not create or map the shared memory.
 * Call create() or open(), then map() to use the shared memory.
 */
SharedMemory::SharedMemory(const std::string& shmName, size_t shmSize)
    : name(shmName), fd(-1), addr(nullptr), size(shmSize),
      isCreated(false), isMapped(false) {}

/**
 * @brief Destructor that ensures complete cleanup of shared memory resources
 *
 * Unmaps the memory region, closes the file descriptor, and unlinks the
 * shared memory object if this instance created it.
 */
SharedMemory::~SharedMemory() {
    unmap();
    close();
    if (isCreated) {
        unlink();
    }
}

/**
 * @brief Creates a new shared memory object
 *
 * @param mode Permission bits (e.g., 0666)
 * @return true if successfully created and sized, false otherwise
 *
 * Creates the shared memory object using shm_open() and sets its size using ftruncate().
 * Both operations must succeed for the method to return true.
 * Cleans up file descriptor on ftruncate() failure.
 */
bool SharedMemory::create(mode_t mode) {
    fd = shm_open(name.c_str(), O_CREAT | O_RDWR, mode);
    if (fd < 0) {
        std::cerr << "Failed to create shared memory: " << strerror(errno) << std::endl;
        return false;
    }

    if (ftruncate(fd, size) < 0) {
        std::cerr << "Failed to set shared memory size: " << strerror(errno) << std::endl;
        ::close(fd);
        fd = -1;
        return false;
    }

    isCreated = true;
    std::cout << "Created shared memory: " << name << " (" << size << " bytes)" << std::endl;
    return true;
}

/**
 * @brief Opens an existing shared memory object
 *
 * @return true if successfully opened, false otherwise
 *
 * Opens the shared memory object with read-write permissions.
 * The object must already exist (created by another process).
 */
bool SharedMemory::open() {
    fd = shm_open(name.c_str(), O_RDWR, 0666);
    if (fd < 0) {
        std::cerr << "Failed to open shared memory: " << strerror(errno) << std::endl;
        return false;
    }

    std::cout << "Opened shared memory: " << name << std::endl;
    return true;
}

/**
 * @brief Maps the shared memory into the process address space
 *
 * @param prot Protection flags (e.g., PROT_READ | PROT_WRITE)
 * @return true if successfully mapped, false otherwise
 *
 * Uses mmap() to make the shared memory accessible as a regular memory region.
 * Must be called after create() or open().
 */
bool SharedMemory::map(int prot) {
    if (fd < 0) return false;

    addr = mmap(nullptr, size, prot, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        std::cerr << "Failed to map shared memory: " << strerror(errno) << std::endl;
        addr = nullptr;
        return false;
    }

    isMapped = true;
    std::cout << "Mapped shared memory at address: " << addr << std::endl;
    return true;
}

/**
 * @brief Unmaps the shared memory from the process address space
 *
 * Releases the memory mapping but does not destroy the shared memory object.
 * Other processes can still access it.
 */
void SharedMemory::unmap() {
    if (isMapped && addr != nullptr) {
        munmap(addr, size);
        addr = nullptr;
        isMapped = false;
    }
}

/**
 * @brief Closes the file descriptor
 *
 * Does not unmap or unlink the shared memory.
 * Safe to call multiple times.
 */
void SharedMemory::close() {
    if (fd >= 0) {
        ::close(fd);
        fd = -1;
    }
}

/**
 * @brief Removes the shared memory object from the system
 *
 * @return true if successfully unlinked, false otherwise
 *
 * Destroys the shared memory object. Should only be called by the creating process.
 * Other processes with existing mappings can continue using them until unmapped.
 */
bool SharedMemory::unlink() {
    if (shm_unlink(name.c_str()) == 0) {
        std::cout << "Unlinked shared memory: " << name << std::endl;
        return true;
    }
    return false;
}

/**
 * @brief Writes data to the shared memory region
 *
 * @param data Pointer to source data
 * @param dataSize Number of bytes to write
 * @param offset Offset within the shared memory region
 * @return true if write successful, false if not mapped or out of bounds
 *
 * Validates that the write operation stays within bounds.
 * Uses memcpy for the actual data transfer.
 */
bool SharedMemory::writeData(const void* data, size_t dataSize, size_t offset) {
    if (!isMapped || offset + dataSize > size) return false;

    memcpy(static_cast<char*>(addr) + offset, data, dataSize);
    return true;
}

/**
 * @brief Reads data from the shared memory region
 *
 * @param buffer Pointer to destination buffer
 * @param dataSize Number of bytes to read
 * @param offset Offset within the shared memory region
 * @return true if read successful, false if not mapped or out of bounds
 *
 * Validates that the read operation stays within bounds.
 * Uses memcpy for the actual data transfer.
 */
bool SharedMemory::readData(void* buffer, size_t dataSize, size_t offset) {
    if (!isMapped || offset + dataSize > size) return false;

    memcpy(buffer, static_cast<char*>(addr) + offset, dataSize);
    return true;
}

// ===== MessageQueue Implementation =====

/**
 * @brief Constructs a System V message queue wrapper
 *
 * @param path File path used for key generation (must exist)
 * @param projId Project identifier for key generation (1-255)
 *
 * Generates a unique IPC key using ftok().
 * The path must exist in the filesystem for ftok() to succeed.
 * Does not create or open the queue; call create() or open() afterward.
 */
MessageQueue::MessageQueue(const std::string& path, int projId)
    : msgid(-1) {
    key = ftok(path.c_str(), projId);
    if (key == -1) {
        std::cerr << "Failed to generate key: " << strerror(errno) << std::endl;
    }
}

/**
 * @brief Destructor
 *
 * Does not automatically remove the message queue to prevent data loss
 * when multiple processes are using it. Call remove() explicitly when needed.
 */
MessageQueue::~MessageQueue() {
    // Don't auto-remove, let user decide
}

/**
 * @brief Creates a new message queue or opens existing one
 *
 * @return true if successful, false otherwise
 *
 * Uses IPC_CREAT flag, so will succeed if queue already exists.
 * Sets queue permissions to 0666 (read/write for all).
 */
bool MessageQueue::create() {
    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid < 0) {
        std::cerr << "Failed to create message queue: " << strerror(errno) << std::endl;
        return false;
    }

    std::cout << "Created message queue with ID: " << msgid << std::endl;
    return true;
}

/**
 * @brief Opens an existing message queue
 *
 * @return true if successful, false otherwise
 *
 * Fails if the queue does not already exist (no IPC_CREAT flag).
 */
bool MessageQueue::open() {
    msgid = msgget(key, 0666);
    if (msgid < 0) {
        std::cerr << "Failed to open message queue: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Removes the message queue from the system
 *
 * @return true if successfully removed, false otherwise
 *
 * Destroys the queue and all pending messages.
 * All processes using this queue will be affected.
 */
bool MessageQueue::remove() {
    if (msgctl(msgid, IPC_RMID, nullptr) == 0) {
        std::cout << "Removed message queue" << std::endl;
        return true;
    }
    return false;
}

/**
 * @brief Sends a message to the queue
 *
 * @param msg The message to send
 * @param flags Send flags (e.g., IPC_NOWAIT for non-blocking)
 * @return true if sent successfully, false otherwise
 *
 * Sends only the data portion of the message structure (sizeof(msg.data)).
 * The message type (msg.mtype) is sent separately by the system.
 * Blocks by default unless IPC_NOWAIT is specified.
 */
bool MessageQueue::send(const Message& msg, int flags) {
    if (msgsnd(msgid, &msg, sizeof(msg.data), flags) == 0) {
        return true;
    }

    std::cerr << "Failed to send message: " << strerror(errno) << std::endl;
    return false;
}

/**
 * @brief Receives a message from the queue
 *
 * @param msg Reference to message structure to fill
 * @param type Message type filter (0 = first message, >0 = specific type, <0 = first with type <= |type|)
 * @param flags Receive flags (e.g., IPC_NOWAIT for non-blocking)
 * @return true if received successfully, false otherwise
 *
 * Blocks by default unless IPC_NOWAIT is specified.
 * The type parameter allows selective message retrieval.
 */
bool MessageQueue::receive(Message& msg, long type, int flags) {
    if (msgrcv(msgid, &msg, sizeof(msg.data), type, flags) >= 0) {
        return true;
    }

    std::cerr << "Failed to rseceive message: " << strerror(errno) << std::endl;
    return false;
}

} // namespace PTManager