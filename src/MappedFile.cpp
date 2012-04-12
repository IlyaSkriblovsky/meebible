#include "MappedFile.h"

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


MappedFile::MappedFile(const char* filename)
{
    _fd = open(filename, O_RDWR | O_CREAT, 0644);

    struct stat statbuf;
    fstat(_fd, &statbuf);
    _size = statbuf.st_size;

    _map = (unsigned char*)mmap(0, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
    if (_map == MAP_FAILED)
        fprintf(stderr, "mmap() failed\n");

    if (ftruncate(_fd, map_size) != 0)
        fprintf(stderr, "ftruncate() failed\n");
}

MappedFile::~MappedFile()
{
    munmap(_map, _size);

    if (ftruncate(_fd, _size) != 0)
        fprintf(stderr, "ftruncate() failed\n");

    close(_fd);
}

void MappedFile::sync()
{
    msync(_map, _size, MS_ASYNC);
}
