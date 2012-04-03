all: btree search filetree mmap filetree2

CXXFLAGS = -pipe -Wall -O2

btree: main.cpp BTree.h
	g++ $(CXXFLAGS) main.cpp -o $@

search: search.cpp
	g++ $(CXXFLAGS) search.cpp -o $@

filetree: filetree.cpp fileio.h
	g++ $(CXXFLAGS) filetree.cpp -o $@

mmap: mmap.cpp
	g++ $(CXXFLAGS) mmap.cpp -o $@

filetree2: filetree2.cpp
	g++ $(CXXFLAGS) filetree2.cpp -o $@

clean:
	rm -f *.o btree search filetree mmap filetree2

new: clean all
