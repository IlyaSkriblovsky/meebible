#include <cstdio>
#include <cstring>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

class FileTree2
{
    public:
        typedef void (*walk_callback)(const char* key, int len);


        static const uint32 map_size = 10*1024*1024;

        struct Node
        {
            uint32 parent_off;
            uint32 left_off;
            uint32 right_off;

            uint8 color;
            uint8 key_len;
            char key[];
        } __attribute__((packed));


        FileTree2(const char *filename)
        {
            fd = open(filename, O_RDWR | O_CREAT, 0644);

            struct stat statbuf;
            fstat(fd, &statbuf);
            filesize = statbuf.st_size;

            map = (uint8*)mmap(0, map_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
            if (map == MAP_FAILED)
                fprintf(stderr, "mmap() failed\n");

            if (filesize == 0)
            {
                resize(4);
                *(uint32*)map = 0x00000000;
            }
        }

        ~FileTree2()
        {
            munmap(map, map_size);
            close(fd);
        }


        void add(const char* key)
        {
            if (root())
                node_add(root(), key);
            else
                setRoot(createNode(0, key));

            while (root()->parent_off)
                setRoot(node_parent(root()));
        }

        void print()
        {
            if (root())
                node_print(root(), 0);
            else
                printf("<empty>\n");
        }

        void walk(walk_callback callback)
        {
            if (root())
                node_walk(root(), callback);
        }


    private:
        int fd;
        uint8* map;
        int filesize;


        void resize(uint32 newsize)
        {
            filesize = newsize;
            if (ftruncate(fd, filesize) != 0)
                fprintf(stderr, "ftruncate() failed\n");
        }


        inline Node* node_at(uint32 offset)
        {
            if (offset)
                return reinterpret_cast<Node*>(&map[offset]);
            return 0;
        }

        inline uint32 node_off(Node* node)
        {
            if (node)
                return (uint8*)node - map;
            return 0;
        }

        inline Node* root()
        {
            uint32 root_off = *(uint32*)map;
            if (root_off)
                return node_at(root_off);
            else
                return 0;
        }

        inline void setRoot(Node* node)
        {
            uint32 root_off = (uint8*)node - map;
            *(uint32*)map = root_off;
        }

        Node* createNode(Node* parent, const char* key, uint8 color = 0)
        {
            // FIXME: align node here
            int key_len = strlen(key);
            uint32 offs = filesize;
            resize(filesize + sizeof(Node) + key_len);

            Node* node = node_at(offs);
            node->parent_off = node_off(parent);
            node->left_off = node->right_off = 0;
            node->color = color;
            node->key_len = key_len;
            memcpy(node->key, key, key_len);

            return node;
        }


        inline int strnmcmp(const char* s1, int l1, const char* s2, int l2)
        {
            if (l1 < l2)
            {
                int r = strncmp(s1, s2, l1);
                if (r != 0) return r;
                return -1;
            }
            if (l1 > l2)
            {
                int r = strncmp(s1, s2, l2);
                if (r != 0) return r;
                return +1;
            }
            int r = strncmp(s1, s2, l1);
            return r;
        }



        void node_add(Node* node, const char* key)
        {
            int key_len = strlen(key);
            int r = strnmcmp(key, key_len, node->key, node->key_len);

            if (r == 0)
            {
                // printf("Found!\n");
            }
            else if (r < 0)
            {
                if (node->left_off)
                    node_add(node_at(node->left_off), key);
                else
                {
                    Node* child = createNode(node, key, 1);
                    node->left_off = node_off(child);
                    node_fix(child);
                }
            }
            else
            {
                if (node->right_off)
                    node_add(node_at(node->right_off), key);
                else
                {
                    Node* child = createNode(node, key, 1);
                    node->right_off = node_off(child);
                    node_fix(child);
                }
            }
        }


        void node_print(Node* node, int indent)
        {
            for (int i = 0; i < indent; i++) putchar(' ');
            if (node->color) putchar('*');
            printf("%.*s\n", node->key_len, node->key);

            if (node->left_off || node->right_off)
            {
                if (node->left_off)
                    node_print(node_at(node->left_off), indent + 2);
                else
                {
                    for (int i = 0; i < indent+2; i++) putchar(' ');
                    putchar('-'); putchar('\n');
                }

                if (node->right_off)
                    node_print(node_at(node->right_off), indent + 2);
                else
                {
                    for (int i = 0; i < indent+2; i++) putchar(' ');
                    putchar('-'); putchar('\n');
                }
            }
        }



        Node* node_parent(Node* node)
        {
            if (node->parent_off)
                return node_at(node->parent_off);
            return 0;
        }
        Node* node_left(Node* node)
        {
            if (node->left_off)
                return node_at(node->left_off);
            return 0;
        }
        Node* node_right(Node* node)
        {
            if (node->right_off)
                return node_at(node->right_off);
            return 0;
        }
        Node* node_grandparent(Node* node)
        {
            Node* p = node_parent(node);
            if (p)
                return node_parent(p);
            return 0;
        }
        Node* node_uncle(Node* node)
        {
            Node* gp = node_grandparent(node);
            if (gp)
            {
                if (node->parent_off == gp->left_off)
                    return node_right(gp);
                else
                    return node_left(gp);
            }
            return 0;
        }


        void node_fix(Node* node)
        {
            if (node->color == 0)
                return;

            if (node->parent_off == 0)
            {
                // Case 1
                node->color = 0;
                return;
            }

            if (node_parent(node)->color == false)
                // Case 2
                return;


            Node* u = node_uncle(node);
            if (u && u->color != 0)
            {
                // Case 3
                node_parent(node)->color = 0;
                u->color = 0;
                Node* g = node_grandparent(node);
                g->color = 1;
                node_fix(g);
                return;
            }

            Node* p = node_parent(node);
            Node* g = node_parent(p); // not null because root is always black

            if (node == node_right(p) && node->parent_off == g->left_off)
            {
                // Case 4a
                rotate_left(p);
                node_fix(p);
                return;
            }

            if (node == node_left(p) && node->parent_off == g->right_off)
            {
                // Case 4b
                rotate_right(p);
                node_fix(p);
                return;
            }


            // Case 5
            p->color = 0;
            g->color = 1;
            if (node == node_left(p) && node->parent_off == g->left_off)
                rotate_right(g);
            else
                rotate_left(g);
        }


        void rotate_left(Node* node)
        {
            if (node->parent_off)
            {
                if (node == node_left(node_parent(node)))
                    node_parent(node)->left_off = node->right_off;
                else
                    node_parent(node)->right_off = node->right_off;
            }
            node_right(node)->parent_off = node->parent_off;

            node->parent_off = node->right_off;
            node->right_off = node_right(node)->left_off;
            node_parent(node)->left_off = node_off(node);
            if (node->right_off)
                node_right(node)->parent_off = node_off(node);
        }

        void rotate_right(Node* node)
        {
            if (node->parent_off)
            {
                if (node == node_left(node_parent(node)))
                    node_parent(node)->left_off = node->left_off;
                else
                    node_parent(node)->right_off = node->left_off;
            }
            node_left(node)->parent_off = node->parent_off;

            node->parent_off = node->left_off;
            node->left_off = node_left(node)->right_off;
            node_parent(node)->right_off = node_off(node);
            if (node->left_off)
                node_left(node)->parent_off = node_off(node);
        }


        void node_walk(Node* node, walk_callback callback)
        {
            if (node->left_off) node_walk(node_left(node), callback);
            callback(node->key, node->key_len);
            if (node->right_off) node_walk(node_right(node), callback);
        }
};


void callback(const char* key, int key_len)
{
    for (int i = 0; i < key_len; i++) putchar(key[i]);
    putchar(' ');
}


#include <iostream>
#include <fstream>
#include <string>

int main()
{
    FileTree2 tree("t2.bin");

     std::ifstream wordsfile("words.txt");
     std::string word;
     while (true)
     {
         std::getline(wordsfile, word);
         if (wordsfile.eof()) break;

         tree.add(word.c_str());
     }

    // tree.print();

    // tree.walk(callback);
    // printf("\n");
}
