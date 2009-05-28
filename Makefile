CC = arm-none-linux-gnueabi-gcc
CFLAGS = -Wall
LDFLAGS = 


EXEC = audio_test
OBJS = audio_test.o

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
	-rm -f *.h~ *.c~

