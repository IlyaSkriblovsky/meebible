#include <cstdio>
#include <cstring>

#include "BTree.h"


class Search
{
    private:
        FILE* f;

        char *key_buf;
        int key_buf_length;


    public:
        Search(const char *filename)
            :key_buf(0), key_buf_length(0)
        {
            f = fopen(filename, "rb");
        }

        ~Search()
        {
            fclose(f);
            free(key_buf);
        }


        void ensure_buf(int len)
        {
            if (len > key_buf_length)
            {
                key_buf_length = len;
                key_buf = (char*)realloc(key_buf, len);
            }
        }


        void search(const char *needle)
        {
            printf("Searching for %s\n", needle);

            fseek(f, 0, SEEK_SET);

            unsigned int root_offset;
            fread(&root_offset, 4, 1, f);

            if (! processNode(root_offset, needle))
                std::cout << "Not found :(\n";
        }


        bool processNode(unsigned int offset, const char *needle)
        {
            fseek(f, offset, SEEK_SET);

            unsigned int left_offset, right_offset;
            unsigned char key_len;

            fread(&left_offset, 4, 1, f);
            fread(&right_offset, 4, 1, f);
            fread(&key_len, 1, 1, f);
            ensure_buf(key_len + 1);
            fread(key_buf, 1, key_len, f);
            key_buf[key_len] = 0;

            int cmp = strcmp(needle, key_buf);

            if (cmp < 0)
            {
                if (left_offset > 0)
                    return processNode(left_offset, needle);
                else
                    return false;
            }
            else if (cmp > 0)
            {
                if (right_offset > 0)
                    return processNode(right_offset, needle);
                else
                    return false;
            }
            else
            {
                unsigned int docid_len;
                fread(&docid_len, 4, 1, f);
                printf("Found %u docs\n", docid_len);
                for (int i = 0; i < docid_len; i++)
                {
                    unsigned int docid;
                    fread(&docid, 4, 1, f);
                    printf("%u ", docid);
                }
                printf("\n");
                return true;
            }
        }
};


int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: search <tree-file> <word>\n");
        return 1;
    }

    Search search(argv[1]);
    search.search(argv[2]);
}
