# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -Wall -Iheaders

# Directories
INCLUDE_DIR = headers
SOURCE_DIR = sources

# Automatically find all C source files in the sources directory
SOURCES_SERVER = RTMServer.c $(wildcard $(SOURCE_DIR)/*.c)
SOURCES_CLIENT = client.c $(wildcard $(SOURCE_DIR)/*.c)

# Executables
SERVER_EXE = server
CLIENT_EXE = client

# Default target: Build both server and client
all: $(SERVER_EXE) $(CLIENT_EXE)

# Build the server
$(SERVER_EXE): $(SOURCES_SERVER)
	$(CC) $(CFLAGS) $(SOURCES_SERVER) -o $(SERVER_EXE)

# Build the client
$(CLIENT_EXE): $(SOURCES_CLIENT)
	$(CC) $(CFLAGS) $(SOURCES_CLIENT) -o $(CLIENT_EXE)

# Clean up the compiled files
clean:
	rm -f $(SERVER_EXE) $(CLIENT_EXE)

.PHONY: all clean run
