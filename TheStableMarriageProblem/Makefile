CC = mpicc
CFLAGS = -Wall
TARGET = stable_marriage

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

clean:
	rm -f $(TARGET)
