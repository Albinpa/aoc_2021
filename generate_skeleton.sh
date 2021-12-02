if [ $# -eq 0 ]
  then
    echo "No arguments supplied"
fi

THIS_FILE="$(basename -- $0)"
DIR_NAME=$1

if [[ -d "${DIR_NAME}" ]]
then
    echo "${DIR_NAME} already on your filesystem."
    exit 1
fi

mkdir ${DIR_NAME}
cd ${DIR_NAME}

# create Makefile
cat <<EOT >> Makefile
# Generated from ${THIS_FILE}
# Builds and runs program made out of ${DIR_NAME}.cpp with files from ../shared/

CC = g++
CFLAGS  = -g -Wall -Werror -Wextra -std=c++2a

TARGET = ${DIR_NAME}
LIBRARY = ../shared

all: \$(TARGET)
	./\$(TARGET)

\$(TARGET): \$(TARGET).cpp \$(LIBRARY)/*
	\$(CC) \$(CFLAGS) -I \$(LIBRARY) -o \$(TARGET) \$(TARGET).cpp

clean:
	\$(RM) \$(TARGET)
EOT

# create main cpp file
cat <<EOT >> ${DIR_NAME}.cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "aoc_library.hpp"


int main()
{
    const auto lines = aoc::get_lines("input.txt");
    aoc::print(lines);
    return 0;
}
EOT

# create skeleton input file
cat <<EOT >> input.txt
Almost empty file made by ${THIS_FILE}
EOT
