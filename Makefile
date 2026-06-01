CFLAGS = -Wall -Iinclude
LDFLAGS = -lncurses

compile: main.o ui.o engine.o timing.o
	gcc main.o ui.o engine.o timing.o -o game_of_life $(LDFLAGS)

main.o: src/main.c include/engine.h
	gcc $(CFLAGS) -c src/main.c -o main.o

ui.o: src/ui.c include/ui.h include/engine.h include/config.h
	gcc $(CFLAGS) -c src/ui.c -o ui.o

engine.o: src/engine.c include/engine.h
	gcc $(CFLAGS) -c src/engine.c -o engine.o

timing.o: src/timing.c include/timing.h
	gcc $(CFLAGS) -c src/timing.c -o timing.o

clean:
	rm -f *.o game_of_life lib/unity/unity.o

# Tests

test: test_engine.o engine.o lib/unity/unity.o
	echo "Tests in Makefile einbauen!"

test_engine.o: test_engine.c engine.h
	echo "Tests in Makefile einbauen!"

lib/unity/unity.o: lib/unity/unity.c lib/unity/unity.h lib/unity/unity_internals.h
	echo "Tests in Makefile einbauen!"
