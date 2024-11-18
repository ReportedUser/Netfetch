# Compiler and flags
CC = gcc
CFLAGS = -I./include/ -lcurl

# Sources
SRC = src/netfetch.c lib/cJSON.c

# Binary
BIN = netfetch

# Installation directories
SYSTEM_BIN_DIR = /usr/local/bin
USER_CONFIG_DIR = $(HOME)/.config/netfetch
CONFIG_FILE = netfetch-services.conf

# Default target
all: $(BIN)

# Build the binary
$(BIN): $(SRC)
	$(CC) -o $(BIN) $(SRC) $(CFLAGS)

# Install the binary to /usr/local/bin and configuration to user's home
install: $(BIN)
	# Install the binary to /usr/local/bin (requires sudo)
	sudo install -Dm755 $(BIN) $(SYSTEM_BIN_DIR)/$(BIN)
	echo "Binary installed to $(SYSTEM_BIN_DIR)/$(BIN)"
	# Install the configuration file to the user's directory
	mkdir -p $(USER_CONFIG_DIR)
	if [ ! -f $(USER_CONFIG_DIR)/$(CONFIG_FILE) ]; then \
		install -Dm644 $(CONFIG_FILE) $(USER_CONFIG_DIR)/$(CONFIG_FILE); \
		echo "Configuration file installed to $(USER_CONFIG_DIR)/$(CONFIG_FILE)"; \
	else \
		echo "Configuration file already exists at $(USER_CONFIG_DIR)/$(CONFIG_FILE). Skipping..."; \
	fi

# Clean up build artifacts
clean:
	rm -f $(BIN)

# Remove the binary and configuration
remove:
	# Remove the binary from /usr/local/bin
	sudo rm -f $(SYSTEM_BIN_DIR)/$(BIN)
	echo "Removed binary $(SYSTEM_BIN_DIR)/$(BIN)"
	# Remove the configuration directory if it exists
	if [ -d $(USER_CONFIG_DIR) ]; then \
		rm -rf $(USER_CONFIG_DIR); \
		echo "Removed configuration directory $(USER_CONFIG_DIR)"; \
	fi

.PHONY: all install clean remove

