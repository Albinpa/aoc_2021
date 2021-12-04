# Generated from generate_skeleton.sh
# Builds and runs program made out of day4.cpp with files from ../shared/

CC = g++
CFLAGS  = -g -Wall -Werror -Wextra -std=c++2a

TARGET = day4
LIBRARY = ../shared

all: $(TARGET)
	./$(TARGET)

$(TARGET): $(TARGET).cpp $(LIBRARY)/*
	$(CC) $(CFLAGS) -I $(LIBRARY) -o $(TARGET) $(TARGET).cpp

clean:
	$(RM) $(TARGET)
