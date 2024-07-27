CC = gcc # Compiler
TARGET = exec # Define the Target Executable
INCL = includes # Define Includes
SRCS =  main.c \
	src/heap.c
OBJS = $(SRCS:.c=.o) # Defining the Objects
CFLAGS = -std=c99 -Wall -g -I$(INCL) # Compiler Flags
INPT_FILE = inpt_ex.txt # Input File Example (Can Modify if you want another File or Manually Run it)

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
	./$(TARGET) $(INPT_FILE)

# Clean the Project
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean run
