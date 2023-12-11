CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

all: build

build: 
	gcc $(CFLAGS) stalk.c threads.c list.c -lpthread -o s-talk

run: build
	./s-talk

clean:
	rm -f s-talk 

.PHONY: all build run clean
