CC = gcc # Compiler
TARGET = main # Define the Target Executable
INCL = includes # Define Includes
SRCS =  main.c \
	src/heap.c
OBJS = $(SRCS:.c=.o) # Defining the Objects
CFLAGS = -std=c99 -Wall -I$(INCL) # Compiler Flags

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
