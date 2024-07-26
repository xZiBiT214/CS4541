CC = gcc # Compiler
CFLAGS = -std=c99 -Wall # Compiler Flags
TARGET = main # Define the Target Executable
SRCS = main.c # Defining the Sources
OBJS = $(SRCS:.c=.o) # Defining the Objects

# Default Target
all: $(TARGET) 

# Linking the Executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compiling the Source File(s) into Object File(s)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Executing the Project
run: $(TARGET)
	./$(TARGET)

# Clean the Project
clean:
	rm -f $(TARGET) $(OBJS)
