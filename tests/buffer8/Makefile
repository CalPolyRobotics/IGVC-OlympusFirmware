
SOURCES = main.c ../../utils/buffer8.c

CFLAGS = -g -Wall -Werror -I../../utils/
TARGET = main

all: $(TARGET)

$(TARGET): $(SOURCES)
	gcc $(CFLAGS) -o $@ $(SOURCES)

test: $(TARGET)
	@./main

clean:
	rm $(TARGET)