CFLAGS = -Wall -Iinclude
LDFLAGS = -lncurses

compile: main.o ui.o engine.o timing.o prints.o savefile.o
	gcc main.o ui.o engine.o timing.o prints.o savefile.o -o game_of_life $(LDFLAGS)

main.o: src/main.c include/engine.h
	gcc $(CFLAGS) -c src/main.c -o main.o

ui.o: src/ui.c include/ui.h include/engine.h include/config.h include/prints.h include/savefile.h
	gcc $(CFLAGS) -c src/ui.c -o ui.o

engine.o: src/engine.c include/engine.h
	gcc $(CFLAGS) -c src/engine.c -o engine.o

timing.o: src/timing.c include/timing.h
	gcc $(CFLAGS) -c src/timing.c -o timing.o

prints.o: src/prints.c include/prints.h 
	gcc $(CFLAGS) -c src/prints.c -o prints.o

savefile.o: src/savefile.c include/savefile.h
	gcc $(CFLAGS) -c src/savefile.c -o savefile.o


clean:
	rm -f *.o game_of_life test_engine

# Tests

test: test_engine.o engine.o unity.o
	gcc test_engine.o engine.o unity.o -o test_engine $(LDFLAGS)
	./test_engine

test_engine.o: tests/test_engine.c include/engine.h
	gcc $(CFLAGS) -c tests/test_engine.c -o test_engine.o

unity.o: lib/unity/unity.c lib/unity/unity.h lib/unity/unity_internals.h
	gcc $(CFLAGS) -c lib/unity/unity.c -o unity.o
