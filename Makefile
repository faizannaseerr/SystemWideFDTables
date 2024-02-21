CC = gcc
CFLAGS = -Wall -g -std=c99

all: showFDtables

showFDtables: showFDtables.o
	$(CC) $(CFLAGS) $< -o $@

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f showFDtables.o showFDtables compositeTable.txt compositeTable.bin