# ============================================================================
# Makefile for Process/Thread Manager (C++20)
# ============================================================================

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -pthread
LDFLAGS := -pthread -lrt

# Directories
SRC_DIR := src
INC_DIR := include
TEST_DIR := test
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj

# Target executable
TARGET := $(BUILD_DIR)/test_manager

# Source files
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
TEST_SOURCES := $(wildcard $(TEST_DIR)/*.cpp)
ALL_SOURCES := $(SOURCES) $(TEST_SOURCES)

# Object files
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TEST_OBJECTS := $(TEST_SOURCES:$(TEST_DIR)/%.cpp=$(OBJ_DIR)/%.o)
ALL_OBJECTS := $(OBJECTS) $(TEST_OBJECTS)

# Header files
HEADERS := $(wildcard $(INC_DIR)/*.hpp)

# Include paths
INCLUDES := -I$(INC_DIR)

# Colors for output
RED := \033[0;31m
GREEN := \033[0;32m
YELLOW := \033[0;33m
BLUE := \033[0;34m
MAGENTA := \033[0;35m
CYAN := \033[0;36m
NC := \033[0m # No Color

# ============================================================================
# Main Targets
# ============================================================================

# Default target
.PHONY: all
all: banner check-compiler $(TARGET)
	@echo "$(GREEN)✓ Build complete!$(NC)"
	@echo "$(BLUE)Run with: ./$(TARGET)$(NC)"
	@echo ""

# Banner
.PHONY: banner
banner:
	@echo "$(CYAN)======================================$(NC)"
	@echo "$(CYAN)  Building Process/Thread Manager$(NC)"
	@echo "$(CYAN)  C++20 Standard$(NC)"
	@echo "$(CYAN)======================================$(NC)"
	@echo ""

# Check compiler version
.PHONY: check-compiler
check-compiler:
	@echo "$(YELLOW)Checking compiler...$(NC)"
	@$(CXX) --version | head -1
	@echo ""

# Create build directories
$(BUILD_DIR):
	@echo "$(YELLOW)Creating build directories...$(NC)"
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OBJ_DIR)

# Build target executable
$(TARGET): $(BUILD_DIR) $(ALL_OBJECTS)
	@echo "$(YELLOW)Linking $(TARGET)...$(NC)"
	@$(CXX) $(CXXFLAGS) $(ALL_OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "$(GREEN)✓ Executable created$(NC)"

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(BUILD_DIR)
	@echo "$(YELLOW)Compiling $<...$(NC)"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile test files
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp $(HEADERS) | $(BUILD_DIR)
	@echo "$(YELLOW)Compiling $<...$(NC)"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ============================================================================
# Cleaning Targets
# ============================================================================

# Clean build artifacts
.PHONY: clean
clean:
	@echo "$(RED)Cleaning build artifacts...$(NC)"
	@rm -rf $(BUILD_DIR)
	@echo "$(GREEN)✓ Clean complete!$(NC)"

# Clean system resources (shared memory, semaphores, FIFOs)
.PHONY: clean-ipc
clean-ipc:
	@echo "$(RED)Cleaning IPC resources...$(NC)"
	@-rm -f /dev/shm/test_* 2>/dev/null || true
	@-rm -f /dev/shm/sem.* 2>/dev/null || true
	@-rm -f /tmp/test_fifo* 2>/dev/null || true
	@-ipcs -m 2>/dev/null | grep $(USER) | awk '{print $$2}' | xargs -r ipcrm -m 2>/dev/null || true
	@-ipcs -s 2>/dev/null | grep $(USER) | awk '{print $$2}' | xargs -r ipcrm -s 2>/dev/null || true
	@echo "$(GREEN)✓ IPC resources cleaned!$(NC)"

# Full clean (build + IPC)
.PHONY: clean-all
clean-all: clean clean-ipc
	@echo "$(GREEN)✓ Full clean complete!$(NC)"

# Rebuild everything
.PHONY: rebuild
rebuild: clean all

# ============================================================================
# Running Targets
# ============================================================================

# Run the program
.PHONY: run
run: $(TARGET)
	@echo ""
	@echo "$(CYAN)======================================$(NC)"
	@echo "$(CYAN)  Running All Tests$(NC)"
	@echo "$(CYAN)======================================$(NC)"
	@./$(TARGET)

# Run specific tests
.PHONY: test1 test2 test3 test4 test5
test1: $(TARGET)
	@echo ""
	@echo "$(BLUE)======================================$(NC)"
	@echo "$(BLUE)  Test 1: Process Management$(NC)"
	@echo "$(BLUE)======================================$(NC)"
	@./$(TARGET) 1

test2: $(TARGET)
	@echo ""
	@echo "$(BLUE)======================================$(NC)"
	@echo "$(BLUE)  Test 2: Thread Pool$(NC)"
	@echo "$(BLUE)======================================$(NC)"
	@./$(TARGET) 2

test3: $(TARGET)
	@echo ""
	@echo "$(BLUE)======================================$(NC)"
	@echo "$(BLUE)  Test 3: IPC$(NC)"
	@echo "$(BLUE)======================================$(NC)"
	@./$(TARGET) 3

test4: $(TARGET)
	@echo ""
	@echo "$(BLUE)======================================$(NC)"
	@echo "$(BLUE)  Test 4: Synchronization$(NC)"
	@echo "$(BLUE)======================================$(NC)"
	@./$(TARGET) 4

test5: $(TARGET)
	@echo ""
	@echo "$(BLUE)======================================$(NC)"
	@echo "$(BLUE)  Test 5: All Tests$(NC)"
	@echo "$(BLUE)======================================$(NC)"
	@./$(TARGET) 5

# Run all tests sequentially
.PHONY: test-all
test-all: $(TARGET)
	@echo ""
	@echo "$(MAGENTA)======================================$(NC)"
	@echo "$(MAGENTA)  Running All Tests Sequentially$(NC)"
	@echo "$(MAGENTA)======================================$(NC)"
	@for i in 1 2 3 4 5; do \
		echo ""; \
		echo "$(CYAN)>>> Test $$i <<<$(NC)"; \
		./$(TARGET) $$i || exit 1; \
	done
	@echo ""
	@echo "$(GREEN)✓ All tests passed!$(NC)"

# ============================================================================
# Debugging Targets
# ============================================================================

# Build with debug symbols
.PHONY: debug
debug: CXXFLAGS += -g -O0 -DDEBUG
debug: clean $(TARGET)
	@echo "$(GREEN)✓ Debug build complete!$(NC)"
	@echo "$(YELLOW)Starting GDB...$(NC)"
	@gdb ./$(TARGET)

# Run with valgrind (memory leak detection)
.PHONY: valgrind
valgrind: $(TARGET)
	@echo "$(YELLOW)Running with Valgrind...$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--suppressions=/usr/share/gdb/auto-load/valgrind.supp 2>/dev/null \
		./$(TARGET) || \
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

# Run valgrind on specific test
.PHONY: valgrind-test1 valgrind-test2 valgrind-test3 valgrind-test4 valgrind-test5
valgrind-test%: $(TARGET)
	@echo "$(YELLOW)Running Test $* with Valgrind...$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET) $*

# Check for memory leaks in all tests
.PHONY: check-leaks
check-leaks: $(TARGET)
	@echo "$(YELLOW)Checking for memory leaks in all tests...$(NC)"
	@for i in 1 2 3 4 5; do \
		echo ""; \
		echo "$(BLUE)>>> Test $$i <<<$(NC)"; \
		valgrind --leak-check=full --error-exitcode=1 --quiet ./$(TARGET) $$i 2>&1 | \
			grep -E "(ERROR SUMMARY|definitely lost|indirectly lost)" || true; \
	done

# Run with address sanitizer
.PHONY: asan
asan: CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer -g
asan: LDFLAGS += -fsanitize=address
asan: clean $(TARGET)
	@echo "$(GREEN)✓ Build with AddressSanitizer complete!$(NC)"
	@./$(TARGET)

# Run with thread sanitizer
.PHONY: tsan
tsan: CXXFLAGS += -fsanitize=thread -fno-omit-frame-pointer -g
tsan: LDFLAGS += -fsanitize=thread
tsan: clean $(TARGET)
	@echo "$(GREEN)✓ Build with ThreadSanitizer complete!$(NC)"
	@./$(TARGET)

# Run with undefined behavior sanitizer
.PHONY: ubsan
ubsan: CXXFLAGS += -fsanitize=undefined -fno-omit-frame-pointer -g
ubsan: LDFLAGS += -fsanitize=undefined
ubsan: clean $(TARGET)
	@echo "$(GREEN)✓ Build with UBSanitizer complete!$(NC)"
	@./$(TARGET)

# ============================================================================
# Information Targets
# ============================================================================

# Show help
.PHONY: help
help:
	@echo "$(CYAN)======================================$(NC)"
	@echo "$(CYAN)  Available Make Targets$(NC)"
	@echo "$(CYAN)======================================$(NC)"
	@echo ""
	@echo "$(GREEN)Building:$(NC)"
	@echo "  make              - Build the project"
	@echo "  make all          - Build the project (same as make)"
	@echo "  make rebuild      - Clean and rebuild"
	@echo "  make debug        - Build with debug symbols and run GDB"
	@echo ""
	@echo "$(GREEN)Cleaning:$(NC)"
	@echo "  make clean        - Remove build artifacts"
	@echo "  make clean-ipc    - Clean IPC resources (shared memory, semaphores)"
	@echo "  make clean-all    - Clean everything (build + IPC)"
	@echo ""
	@echo "$(GREEN)Running:$(NC)"
	@echo "  make run          - Build and run all tests"
	@echo "  make test1        - Run Test 1 (Process Management)"
	@echo "  make test2        - Run Test 2 (Thread Pool)"
	@echo "  make test3        - Run Test 3 (IPC)"
	@echo "  make test4        - Run Test 4 (Synchronization)"
	@echo "  make test5        - Run Test 5 (All Tests)"
	@echo "  make test-all     - Run all tests sequentially"
	@echo ""
	@echo "$(GREEN)Debugging & Analysis:$(NC)"
	@echo "  make valgrind     - Run with Valgrind memory checker"
	@echo "  make check-leaks  - Check for memory leaks in all tests"
	@echo "  make asan         - Build and run with AddressSanitizer"
	@echo "  make tsan         - Build and run with ThreadSanitizer"
	@echo "  make ubsan        - Build and run with UBSanitizer"
	@echo ""
	@echo "$(GREEN)Code Quality:$(NC)"
	@echo "  make format       - Format code with clang-format"
	@echo "  make lint         - Run clang-tidy linter"
	@echo "  make stats        - Show code statistics"
	@echo ""
	@echo "$(GREEN)Information:$(NC)"
	@echo "  make info         - Show project information"
	@echo "  make deps         - Show dependencies"
	@echo "  make help         - Show this help message"
	@echo ""
	@echo "$(GREEN)Installation:$(NC)"
	@echo "  make install-deps-debian  - Install dependencies (Debian/Ubuntu)"
	@echo "  make install-deps-freebsd - Install dependencies (FreeBSD)"
	@echo "  make install-deps-macos   - Install dependencies (macOS)"
	@echo ""

# Show project info
.PHONY: info
info:
	@echo "$(CYAN)======================================$(NC)"
	@echo "$(CYAN)  Project Information$(NC)"
	@echo "$(CYAN)======================================$(NC)"
	@echo ""
	@echo "$(YELLOW)Compiler:$(NC)     $(CXX)"
	@$(CXX) --version | head -1
	@echo "$(YELLOW)C++ Standard:$(NC) C++20"
	@echo "$(YELLOW)Flags:$(NC)        $(CXXFLAGS)"
	@echo "$(YELLOW)Link Flags:$(NC)   $(LDFLAGS)"
	@echo ""
	@echo "$(YELLOW)Source files ($(words $(SOURCES))):$(NC)"
	@for src in $(SOURCES); do echo "  $$src"; done
	@echo ""
	@echo "$(YELLOW)Test files ($(words $(TEST_SOURCES))):$(NC)"
	@for src in $(TEST_SOURCES); do echo "  $$src"; done
	@echo ""
	@echo "$(YELLOW)Header files ($(words $(HEADERS))):$(NC)"
	@for hdr in $(HEADERS); do echo "  $$hdr"; done
	@echo ""
	@echo "$(YELLOW)Object files:$(NC) $(words $(ALL_OBJECTS))"
	@echo "$(YELLOW)Target:$(NC)       $(TARGET)"
	@echo ""

# ============================================================================
# Code Quality Targets
# ============================================================================

# Format code (requires clang-format)
.PHONY: format
format:
	@echo "$(YELLOW)Formatting code with clang-format...$(NC)"
	@if command -v clang-format >/dev/null 2>&1; then \
		find $(SRC_DIR) $(INC_DIR) $(TEST_DIR) -name "*.cpp" -o -name "*.hpp" | \
			xargs clang-format -i --style="{BasedOnStyle: Google, IndentWidth: 4, ColumnLimit: 100}"; \
		echo "$(GREEN)✓ Code formatted!$(NC)"; \
	else \
		echo "$(RED)✗ clang-format not found. Install with:$(NC)"; \
		echo "  Debian/Ubuntu: sudo apt-get install clang-format"; \
		echo "  FreeBSD: sudo pkg install llvm"; \
		echo "  macOS: brew install clang-format"; \
	fi

# Lint code (requires clang-tidy)
.PHONY: lint
lint:
	@echo "$(YELLOW)Running clang-tidy...$(NC)"
	@if command -v clang-tidy >/dev/null 2>&1; then \
		find $(SRC_DIR) $(TEST_DIR) -name "*.cpp" | \
			xargs clang-tidy -p=$(BUILD_DIR) -- $(INCLUDES); \
		echo "$(GREEN)✓ Lint complete!$(NC)"; \
	else \
		echo "$(RED)✗ clang-tidy not found. Install with:$(NC)"; \
		echo "  Debian/Ubuntu: sudo apt-get install clang-tidy"; \
		echo "  FreeBSD: sudo pkg install llvm"; \
		echo "  macOS: brew install llvm"; \
	fi

# Code statistics
.PHONY: stats
stats:
	@echo "$(CYAN)======================================$(NC)"
	@echo "$(CYAN)  Code Statistics$(NC)"
	@echo "$(CYAN)======================================$(NC)"
	@echo ""
	@echo "$(YELLOW)Lines of code (source):$(NC)"
	@wc -l $(SOURCES) 2>/dev/null | tail -1 || echo "  No source files"
	@echo ""
	@echo "$(YELLOW)Lines of code (headers):$(NC)"
	@wc -l $(HEADERS) 2>/dev/null | tail -1 || echo "  No header files"
	@echo ""
	@echo "$(YELLOW)Lines of code (tests):$(NC)"
	@wc -l $(TEST_SOURCES) 2>/dev/null | tail -1 || echo "  No test files"
	@echo ""
	@echo "$(YELLOW)Total lines:$(NC)"
	@cat $(SOURCES) $(HEADERS) $(TEST_SOURCES) 2>/dev/null | wc -l || echo "  0"
	@echo ""
	@echo "$(YELLOW)File counts:$(NC)"
	@echo "  Source files:  $(words $(SOURCES))"
	@echo "  Header files:  $(words $(HEADERS))"
	@echo "  Test files:    $(words $(TEST_SOURCES))"
	@echo "  Total files:   $(words $(ALL_SOURCES) $(HEADERS))"
	@echo ""

# Show dependencies
.PHONY: deps
deps:
	@echo "$(CYAN)======================================$(NC)"
	@echo "$(CYAN)  File Dependencies$(NC)"
	@echo "$(CYAN)======================================$(NC)"
	@echo ""
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MM $(ALL_SOURCES) 2>/dev/null || \
		echo "$(RED)Error generating dependencies$(NC)"

# ============================================================================
# Installation Targets
# ============================================================================

# Install dependencies (Debian/Ubuntu)
.PHONY: install-deps-debian
install-deps-debian:
	@echo "$(YELLOW)Installing dependencies (Debian/Ubuntu)...$(NC)"
	sudo apt-get update
	sudo apt-get install -y \
		build-essential \
		g++-10 \
		cmake \
		valgrind \
		gdb \
		clang-format \
		clang-tidy
	@echo "$(GREEN)✓ Dependencies installed!$(NC)"

# Install dependencies (FreeBSD)
.PHONY: install-deps-freebsd
install-deps-freebsd:
	@echo "$(YELLOW)Installing dependencies (FreeBSD)...$(NC)"
	sudo pkg install -y \
		gcc \
		cmake \
		valgrind \
		gdb \
		llvm
	@echo "$(GREEN)✓ Dependencies installed!$(NC)"

# Install dependencies (macOS)
.PHONY: install-deps-macos
install-deps-macos:
	@echo "$(YELLOW)Installing dependencies (macOS)...$(NC)"
	brew install \
		gcc \
		cmake \
		llvm \
		clang-format
	@echo "$(GREEN)✓ Dependencies installed!$(NC)"

# ============================================================================
# Advanced Targets
# ============================================================================

# Create compilation database for IDEs
.PHONY: compile-db
compile-db:
	@echo "$(YELLOW)Generating compile_commands.json...$(NC)"
	@cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -B$(BUILD_DIR) .
	@ln -sf $(BUILD_DIR)/compile_commands.json .
	@echo "$(GREEN)✓ Compilation database created!$(NC)"

# Watch for changes and rebuild (requires inotify-tools on Linux)
.PHONY: watch
watch:
	@echo "$(YELLOW)Watching for changes... (Ctrl+C to stop)$(NC)"
	@while true; do \
		inotifywait -qq -r -e modify $(SRC_DIR) $(INC_DIR) $(TEST_DIR) 2>/dev/null && \
		clear && \
		make all; \
	done || echo "$(RED)inotify-tools not installed$(NC)"

# Install the executable
.PHONY: install
install: $(TARGET)
	@echo "$(YELLOW)Installing to /usr/local/bin...$(NC)"
	@sudo cp $(TARGET) /usr/local/bin/
	@echo "$(GREEN)✓ Installed!$(NC)"

# Uninstall the executable
.PHONY: uninstall
uninstall:
	@echo "$(YELLOW)Uninstalling from /usr/local/bin...$(NC)"
	@sudo rm -f /usr/local/bin/test_manager
	@echo "$(GREEN)✓ Uninstalled!$(NC)"

# ============================================================================
# Special Targets
# ============================================================================

# Prevent make from deleting intermediate files
.PRECIOUS: $(OBJ_DIR)/%.o

# Phony targets
.PHONY: all clean clean-ipc clean-all rebuild run test1 test2 test3 test4 test5 \
        test-all valgrind debug asan tsan ubsan help info deps stats format lint \
        check-leaks compile-db watch install uninstall banner check-compiler \
        install-deps-debian install-deps-freebsd install-deps-macos \
        valgrind-test1 valgrind-test2 valgrind-test3 valgrind-test4 valgrind-test5

# Default goal
.DEFAULT_GOAL := all

# Dependency tracking
-include $(ALL_OBJECTS:.o=.d)

# Generate dependencies automatically
$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MM -MT $(@:.d=.o) $< > $@

$(OBJ_DIR)/%.d: $(TEST_DIR)/%.cpp | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MM -MT $(@:.d=.o) $< > $@


docs:
	doxygen Doxyfile
	@echo "Documentation generated in doc/html/index.html"

docs-open: docs
	#open docs/html/index.html  # macOS
	 xdg-open docs/html/index.html  # Linux/FreeBSD

docs-clean:
	rm -rf docs/

.PHONY: docs docs-open docs-clean
