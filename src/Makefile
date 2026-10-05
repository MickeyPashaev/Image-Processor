CC = gcc

CFLAGS = -fno-common -std=c11 -g -Wall -Werror -fsanitize=address
LDFLAGS = -fsanitize=address -static-libasan
LDLIBS = -lpng -lm


TARGET = cw
SRCS = main.c image.c utils.c image_io.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(TARGET) $(LDLIBS)

main.o: main.c image.h utils.h types.h image_io.h
	$(CC) $(CFLAGS) -c main.c

image.o: image.c image.h types.h
	$(CC) $(CFLAGS) -c image.c

utils.o: utils.c utils.h types.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f $(OBJS) $(TARGET)
