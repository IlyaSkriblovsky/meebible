#include <cstdio>
#include <cstring>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd = open("mmap.txt", O_RDWR | O_CREAT, 0644);
    if (fd == -1)
    {
        fprintf(stderr, "Cannot open file\n");
        return 1;
    }

    void* map = mmap(0, 1024, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    if (map == MAP_FAILED)
    {
        fprintf(stderr, "mmap failed\n");
        return 1;
    }

    ftruncate(fd, 3);

    char* chars = (char*)map;
    chars[0] = 'X';

    close(fd);
}
