CC = gcc
TARGET = checked

# Build the project
all:
	$(CC) ./src/* -o $(TARGET)

# Remove old build artifacts
clean:
	rm $(TARGET)

# Run the program
run:
	./${TARGET}