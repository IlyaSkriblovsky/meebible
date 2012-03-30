FLAGS = -Wall -O3 -I../src -DSQLITE_ENABLE_FTS3 -DSQLITE_ENABLE_FTS3_PARENTHESIS -DSQLITE_ENABLE_ICU

LIBS = -ldl -licui18n -licuuc

all: ftstest shell

sqlite3.o: sqlite3.c
	gcc -c -pthread $(FLAGS) $^

ftstest: main.cpp sqlite3.o tokenizer.cpp ../src/unisimple/utf8.cpp ../src/unisimple/unisimple.cpp
	gcc -o $@ -pthread $(FLAGS) $^ $(LIBS)

shell: shell.c sqlite3.o tokenizer.cpp ../src/unisimple/utf8.cpp ../src/unisimple/unisimple.cpp
	gcc -o $@ -pthread $(FLAGS) $^ $(LIBS)

clean:
	rm -f ftstest shell *.o a.out

new: clean all
