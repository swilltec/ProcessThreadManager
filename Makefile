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
HEADERS := $(wildcard $(INC_DIR)/*.h)

# Include paths
INCLUDES := -I$(INC_DIR)

# Colors
RED := \033[0;31m
GREEN := \033[0;32m
YELLOW := \033[0;33m
BLUE := \033[0;34m
MAGENTA := \033[0;35m
CYAN := \033[0;36m
NC := \033[0m

# ============================================================================
# Main Targets
# ============================================================================

.PHONY: all
all: banner check-compiler $(TARGET)
	@echo "$(GREEN)✓ Build complete!$(NC)"
	@echo "$(BLUE)Run with: ./$(TARGET)$(NC)"

# Banner
.PHONY: banner
banner:
	@echo "$(CYAN)======================================$(NC)"
	@echo "$(CYAN)  Building Process/Thread Manager$(NC)"
	@echo "$(CYAN)  C++20 Standard$(NC)"
	@echo "$(CYAN)======================================$(NC)"
	@echo ""

# Check compiler
.PHONY: check-compiler
check-compiler:
	@echo "$(YELLOW)Checking compiler...$(NC)"
	@$(CXX) --version | head -1
	@echo ""

# === FIXED: Ensure both build/ and build/obj/ exist ===
$(BUILD_DIR):
	@echo "$(YELLOW)Creating build directories...$(NC)"
	@mkdir -p $(BUILD_DIR)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Build executable
$(TARGET): $(BUILD_DIR) $(OBJ_DIR) $(ALL_OBJECTS)
	@echo "$(YELLOW)Linking $(TARGET)...$(NC)"
	@$(CXX) $(CXXFLAGS) $(ALL_OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "$(GREEN)✓ Executable created$(NC)"

# Compile src/*.cpp
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(OBJ_DIR)
	@echo "$(YELLOW)Compiling $<...$(NC)"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile test/*.cpp
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp $(HEADERS) | $(OBJ_DIR)
	@echo "$(YELLOW)Compiling $<...$(NC)"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ============================================================================
# Cleaning
# ============================================================================

.PHONY: clean
clean:
	@echo "$(RED)Cleaning build artifacts...$(NC)"
	@rm -rf $(BUILD_DIR)
	@echo "$(GREEN)✓ Clean complete!$(NC)"

.PHONY: clean-ipc
clean-ipc:
	@echo "$(RED)Cleaning IPC resources...$(NC)"
	@-rm -f /dev/shm/test_* 2>/dev/null || true
	@-rm -f /dev/shm/sem.* 2>/dev/null || true
	@-rm -f /tmp/test_fifo* 2>/dev/null || true
	@-ipcs -m 2>/dev/null | grep $(USER) | awk '{print $$2}' | xargs -r ipcrm -m 2>/dev/null || true
	@-ipcs -s 2>/dev/null | grep $(USER) | awk '{print $$2}' | xargs -r ipcrm -s 2>/dev/null || true
	@echo "$(GREEN)✓ IPC resources cleaned!$(NC)"

.PHONY: clean-all
clean-all: clean clean-ipc
	@echo "$(GREEN)✓ Full clean complete!$(NC)"

.PHONY: rebuild
rebuild: clean all

# ============================================================================
# Running
# ============================================================================

.PHONY: run
run: $(TARGET)
	@echo ""
	@echo "$(CYAN)======================================$(NC)"
	@echo "$(CYAN)  Running All Tests$(NC)"
	@echo "$(CYAN)======================================$(NC)"
	@./$(TARGET)

.PHONY: test1 test2 test3 test4 test5
test1: $(TARGET) ; @./$(TARGET) 1
test2: $(TARGET) ; @./$(TARGET) 2
test3: $(TARGET) ; @./$(TARGET) 3
test4: $(TARGET) ; @./$(TARGET) 4
test5: $(TARGET) ; @./$(TARGET) 5

.PHONY: test-all
test-all: $(TARGET)
	@for i in 1 2 3 4 5; do \
		echo "$(CYAN)>>> Test $$i <<<$(NC)"; \
		./$(TARGET) $$i || exit 1; \
	done
	@echo "$(GREEN)✓ All tests passed!$(NC)"

# ============================================================================
# Debugging, Tools, Info
# ============================================================================

debug: CXXFLAGS += -g -O0 -DDEBUG
debug: clean $(TARGET)
	@gdb ./$(TARGET)

asan: CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer -g
asan: LDFLAGS += -fsanitize=address
asan: clean $(TARGET)
	@./$(TARGET)

tsan: CXXFLAGS += -fsanitize=thread -fno-omit-frame-pointer -g
tsan: LDFLAGS += -fsanitize=thread
tsan: clean $(TARGET)
	@./$(TARGET)

ubsan: CXXFLAGS += -fsanitize=undefined -fno-omit-frame-pointer -g
ubsan: LDFLAGS += -fsanitize=undefined
ubsan: clean $(TARGET)
	@./$(TARGET)

format:
	@echo "Formatting code..."
	@if command -v clang-format >/dev/null; then \
		find $(SRC_DIR) $(INC_DIR) $(TEST_DIR) -name "*.cpp" -o -name "*.hpp" | \
			xargs clang-format -i; \
	else echo "clang-format not installed"; fi

lint:
	@if command -v clang-tidy >/dev/null; then \
		find $(SRC_DIR) $(TEST_DIR) -name "*.cpp" | xargs clang-tidy -p=$(BUILD_DIR); \
	else echo "clang-tidy not installed"; fi

# ============================================================================
# Dependency Tracking
# ============================================================================

-include $(ALL_OBJECTS:.o=.d)

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) -MM -MT $(@:.d=.o) $< > $@

$(OBJ_DIR)/%.d: $(TEST_DIR)/%.cpp | $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) -MM -MT $(@:.d=.o) $< > $@

# ============================================================================
# Docs
# ============================================================================

docs:
	doxygen Doxyfile

docs-open: docs
	open docs/html/index.html

docs-clean:
	rm -rf docs/

.DEFAULT_GOAL := all