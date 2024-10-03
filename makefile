# Compiler and flags
CC = gcc
# remove -Wpedantic for {} empty initializer
CFLAGS += -Wall -Wextra - \
          -Wformat=2 -Wno-unused-parameter -Wshadow \
          -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
          -Wredundant-decls -Wnested-externs -Wmissing-include-dirs
# Add linker flags here if needed
LDFLAGS = -lm  
# Directories
# GCC warnings that Clang doesn't provide:
ifeq ($(CC),gcc)
    CFLAGS += -Wjump-misses-init -Wlogical-op
endif

SRCDIR = webserver
OUTDIR = $(SRCDIR)/out

# Source files and corresponding object files
SOURCES = $(SRCDIR)/server.c $(SRCDIR)/utils.c $(SRCDIR)/endpoints.c
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
