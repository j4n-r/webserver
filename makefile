# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm  # Add linker flags here if needed

# Directories
SRCDIR = webserver
OUTDIR = $(SRCDIR)/out

# Source files and corresponding object files
SOURCES = $(SRCDIR)/server.c $(SRCDIR)/utils.c
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OUTDIR)/%.o,$(SOURCES))

# Target executable
TARGET = $(SRCDIR)/server

# Default rule
all: $(TARGET)

# Linking the executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compiling source files to object files
$(OUTDIR)/%.o: $(SRCDIR)/%.c | $(OUTDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure output directory exists
$(OUTDIR):
	mkdir -p $(OUTDIR)

# Clean up build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Run the server
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean run
