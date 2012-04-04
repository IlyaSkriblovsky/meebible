all: filetree2

CXXFLAGS = -pipe -Wall -O3

main.o: main.cpp MappedFile.h FileTree.h IndexTree.h
MappedFile.o: MappedFile.cpp

filetree2: main.o MappedFile.o
	g++ $(CXXFLAGS) $^ -o $@

clean:
	rm -f *.o btree search filetree mmap filetree2

new: clean all
