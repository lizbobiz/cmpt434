# CMPT 434
# Assignment 2
# 
# Lizzie Adams
# ema651
# 11139300

# Compiler
CC = gcc

# Preprocessor flags
CPPFLAGS := -Wall -pedantic -g -I.

# Compiler flags
CFLAGS := -ansi

# Linker flags
LDFLAGS := -L.

# Library flags
LDLIBS := -lstrconvert -lqueue

# Default target
all: sender receiver

# Executable recipes
sender: sender.o libstrconvert.a libqueue.a
	$(CC) -o $@ $< $(LDFLAGS) $(LDLIBS)

receiver: receiver.o libstrconvert.a libqueue.a
	$(CC) -o $@ $< $(LDFLAGS) $(LDLIBS)

# Object file pattern recipe
%.o: %.c %.h param.h
	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $<

# Static library recipe
lib%.a: %.o
	ar -rcs $@ $^
	ranlib $@

# Clean
clean:
	rm -f *.o *.a sender receiver

# Phony targets
.PHONY: all clean
