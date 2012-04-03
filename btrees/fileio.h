#pragma once

#include "types.h"

class StdFileIO
{
    private:
        FILE* f;

    public:
        StdFileIO(const char *filename)
        {
            f = fopen(filename, "r+");
            if (f == 0)
            {
                f = fopen(filename, "w+");
                if (f == 0)
                    fprintf(stderr, "Cannot open file: %s\n", filename);
            }
        }

        ~StdFileIO()
        {
            printf("closing file\n");
            fclose(f);
        }

        template <typename T>
        T read(off_t o)
        {
            fseek(f, o, SEEK_SET);
            T buf;
            fread(&buf, sizeof(T), 1, f);
            return buf;
        }

        void read_chars(off_t o, char* buffer, uint32 len)
        {
            fseek(f, o, SEEK_SET);
            fread(buffer, len, 1, f);
        }

        template <typename T>
        void write(off_t o, const T& value)
        {
            fseek(f, o, SEEK_SET);
            fwrite(&value, sizeof(T), 1, f);
        }

        void write_chars(off_t o, const char* chars, uint32 len)
        {
            fseek(f, o, SEEK_SET);
            fwrite(chars, len, 1, f);
        }

        uint32 size()
        {
            fseek(f, 0, SEEK_END);
            return ftell(f);
        }
};


#include <map>

class PagedFileIO
{
    #define PAGE_SIZE   4096
    #define PAGE_MASK   0x00000fff

    private:
        FILE* f;

        struct Page
        {
            bool dirty;
            uint8 data[4096];

            Page(): dirty(false) { }
        };

        std::map<uint32, Page*> pages;

        uint32 _size;

    public:
        PagedFileIO(const char *filename)
        {
            f = fopen(filename, "r+");
            if (f == 0)
            {
                f = fopen(filename, "w+");
                if (f == 0)
                    fprintf(stderr, "Cannot open file: %s\n", filename);
            }

            fseek(f, 0, SEEK_END);
            _size = ftell(f);
        }

        ~PagedFileIO()
        {
            for (std::map<uint32, Page*>::const_iterator i = pages.begin(); i != pages.end(); i++)
            {
                if (i->second->dirty)
                {
                    fseek(f, i->first, SEEK_SET);
                    if (_size - i->first >= PAGE_SIZE)
                        fwrite(i->second->data, PAGE_SIZE, 1, f);
                    else
                        fwrite(i->second->data, _size - i->first, 1, f);
                }
                delete i->second;
            }
        }

        uint32 size() { return _size; }

        inline uint32 pagestart(uint32 offs)
        {
            return offs & (~PAGE_MASK);
        }
        inline uint32 inpageoffs(uint32 offs)
        {
            return offs & PAGE_MASK;
        }

        Page* loadpage(uint32 addr)
        {
            uint32 start = pagestart(addr);
            if (pages.find(start) == pages.end())
            {
                Page* page = new Page();
                fseek(f, start, SEEK_SET);
                fread(page->data, PAGE_SIZE, 1, f);
                pages[start] = page;
                return page;
            }
            else
                return pages[start];
        }

        template <typename T>
        T read(off_t o)
        {
            uint32 size = sizeof(T);

            Page* page = loadpage(o);

            uint32 inoffs = inpageoffs(o);

            if (inoffs + size <= PAGE_SIZE)
            {
                T buf;
                memcpy(&buf, &page->data[inoffs], sizeof(T));
                return buf;
            }
            else
            {
                char buf[size];
                int partsize = PAGE_SIZE - inoffs;
                memcpy(&buf, &page->data[inoffs], partsize);
                Page* next = loadpage(o + size);
                memcpy(&buf[partsize], next->data, size - partsize);
                return *reinterpret_cast<T*>(&buf);
            }
        }

        template <typename T>
        void write(off_t o, const T& value)
        {
            uint32 size = sizeof(T);

            if (o + size > _size) _size = o + size;

            Page* page = loadpage(o);
            uint32 inoffs = inpageoffs(o);

            page->dirty = true;

            if (inoffs + size <= PAGE_SIZE)
            {
                memcpy(&page->data[inoffs], &value, sizeof(T));
            }
            else
            {
                const char *ptr = reinterpret_cast<const char*>(&value);
                int partsize = PAGE_SIZE - inoffs;
                memcpy(&page->data[inoffs], ptr, partsize);
                Page* next = loadpage(o + size);
                next->dirty = true;
                memcpy(next->data, &ptr[partsize], size - partsize);
            }
        }

        void read_chars(off_t o, char* buffer, uint32 len)
        {
            Page* page = loadpage(o);
            uint32 inoffs = inpageoffs(o);

            if (inoffs + len <= PAGE_SIZE)
                memcpy(buffer, &page->data[inoffs], len);
            else
            {
                int partsize = PAGE_SIZE - inoffs;
                memcpy(buffer, &page->data[inoffs], partsize);
                Page* next = loadpage(o + len);
                memcpy(&buffer[partsize], next->data, len - partsize);
            }
        }

        void write_chars(off_t o, const char* chars, uint32 len)
        {
            if (o + len > _size) _size = o + len;

            Page* page = loadpage(o);
            uint32 inoffs = inpageoffs(o);

            page->dirty = true;

            if (inoffs + len <= PAGE_SIZE)
                memcpy(&page->data[inoffs], chars, len);
            else
            {
                int partsize = PAGE_SIZE - inoffs;
                memcpy(&page->data[inoffs], chars, partsize);
                Page* next = loadpage(o + len);
                next->dirty = true;
                memcpy(next->data, &chars[partsize], len - partsize);
            }
        }
};



#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

class MappedFileIO
{
    private:
        int fd;
        char* map;
        uint32 _size;

    public:
        MappedFileIO(const char* filename)
        {
            fd = open(filename, O_RDWR | O_CREAT, 0644);
            if (fd == -1)
            {
                fprintf(stderr, "Cannot open file: %s\n", filename);
                return;
            }

            struct stat statbuf;
            fstat(fd, &statbuf);
            _size = statbuf.st_size;

            map = (char*)mmap(0, 10 * 1024 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (map == MAP_FAILED)
            {
                fprintf(stderr, "mmap failed\n");
                return;
            }
        }

        ~MappedFileIO()
        {
            munmap(map, 10*1024*1024);
            close(fd);
        }

        uint32 size() { return _size; }

        template <typename T>
        T read(off_t o)
        {
            T buf;
            memcpy(&buf, &map[o], sizeof(T));
            return buf;
        }

        template <typename T>
        void write(off_t o, const T& value)
        {
            if (o + sizeof(T) > _size)
            {
                _size = o + sizeof(T);
                ftruncate(fd, _size);
            }

            memcpy(&map[o], &value, sizeof(T));
        }

        void read_chars(off_t o, char* buffer, uint32 len)
        {
            memcpy(buffer, &map[o], len);
        }

        void write_chars(off_t o, const char* buffer, uint32 len)
        {
            if (o + len > _size)
            {
                _size = o + len;
                ftruncate(fd, _size);
            }

            memcpy(&map[o], buffer, len);
        }
};
