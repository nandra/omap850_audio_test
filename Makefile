#CC = arm-none-linux-gnueabi-gcc
CC = gcc

#LIB = /home/marek.nandra/projects/omap850_audio_test/wav
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

