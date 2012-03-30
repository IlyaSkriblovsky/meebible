FLAGS = -Wall -O2 -I../src -DSQLITE_ENABLE_FTS3 -DSQLITE_ENABLE_FTS3_PARENTHESIS -DSQLITE_ENABLE_ICU

LIBS = -ldl -licui18n -licuuc

all: ftstest shell

sqlite3.o: ../src/sqlite3/sqlite3.c
	gcc -c -pthread $(FLAGS) $^

ftstest: main.cpp sqlite3.o ../src/unisimple/utf8.cpp ../src/unisimple/unisimple.cpp ../src/unisimple/tokenizer.cpp
	gcc -o $@ -pthread $(FLAGS) $^ $(LIBS)

shell: shell.c sqlite3.o ../src/unisimple/utf8.cpp ../src/unisimple/unisimple.cpp ../src/unisimple/tokenizer.cpp
	gcc -o $@ -pthread $(FLAGS) -I../src/sqlite3 $^ $(LIBS)

clean:
	rm -f ftstest shell *.o a.out

new: clean all
