#pragma once

#include <cstdio>

class MappedFile
{
    public:
        MappedFile(const char* filename);
        ~MappedFile();

        inline unsigned char* map() { return _map; }
        inline unsigned int size() { return _size; }

        void sync();

        inline void resize(unsigned int size)
        {
            _size = size;
            if (size > map_size)
                fprintf(stderr, "MappedFile::resize > max_map_size\n");
        }

        inline void grow(unsigned int amount)
        {
            resize(_size + amount);
        }

        template <typename T>
        inline T* at(unsigned int offset)
        {
            return (T*)&_map[offset];
        }

        inline unsigned int off(void* p)
        {
            return (unsigned char*)p - _map;
        }

    private:
        static const unsigned int map_size = 10*1024*1024;

        int _fd;
        unsigned int _size;

        unsigned char* _map;
};
