CC = gcc
CFLAGS = -Wall -Iinclude
LDFLAGS = -lncurses

.PHONY: all compile test

all: compile test

compile: main.o ui.o engine.o timing.o render.o savefile.o
	$(CC) main.o ui.o engine.o timing.o render.o savefile.o -o game_of_life $(LDFLAGS)

main.o: src/main.c include/engine.h
	$(CC) $(CFLAGS) -c src/main.c -o main.o

ui.o: src/ui.c include/ui.h include/engine.h include/config.h include/render.h include/savefile.h
	$(CC) $(CFLAGS) -c src/ui.c -o ui.o

engine.o: src/engine.c include/engine.h
	$(CC) $(CFLAGS) -c src/engine.c -o engine.o

timing.o: src/timing.c include/timing.h
	$(CC) $(CFLAGS) -c src/timing.c -o timing.o

render.o: src/render.c include/render.h 
	$(CC) $(CFLAGS) -c src/render.c -o render.o

savefile.o: src/savefile.c include/savefile.h
	$(CC) $(CFLAGS) -c src/savefile.c -o savefile.o


clean:
	rm -f *.o game_of_life test_engine test_savefile

# Tests

test: test_engine.o test_savefile.o engine.o savefile.o unity.o
	$(CC) test_engine.o engine.o unity.o -o test_engine $(LDFLAGS)
	$(CC) test_savefile.o engine.o savefile.o unity.o -o test_savefile $(LDFLAGS)
	./test_engine
	./test_savefile

test_engine.o: tests/test_engine.c include/engine.h
	$(CC) $(CFLAGS) -c tests/test_engine.c -o test_engine.o

test_savefile.o: tests/test_savefile.c include/engine.h include/savefile.h
	$(CC) $(CFLAGS) -c tests/test_savefile.c -o test_savefile.o

unity.o: lib/unity/unity.c lib/unity/unity.h lib/unity/unity_internals.h
	$(CC) $(CFLAGS) -c lib/unity/unity.c -o unity.o
