#pragma once

#include <list>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

#include "MappedFile.h"

template <uint16 signature, uint8 version, typename Header, typename Entry, uint16 chunk_size>
class FileTree
{
    public:
        typedef void (*walk_callback)(const char* key, int len);


    private:
        struct BaseHeader
        {
            uint16 sig;
            uint8 ver;
            uint8 clean;
            uint32 root_off;
            Header header;
            uint32 file_size;
        } __attribute__((packed));


        struct Node
        {
            uint32 parent_off;
            uint32 left_off;
            uint32 right_off;

            uint32 data_off;

            uint8 color;
            uint8 key_len;
            char key[];
        } __attribute__((packed));



        struct DataChunk
        {
            uint32 next_off;

            uint8 entry_count;

            Entry entries[chunk_size];
        } __attribute__((packed));

    public:

        FileTree(const char *filename)
            :file(filename)
        {
            baseheader = file.at<BaseHeader>(0);

            if (file.size() < sizeof(BaseHeader))
                clear();
            else
            {
                if (file.size() < baseheader->file_size ||
                    baseheader->sig != signature ||
                    baseheader->ver != version ||
                    (baseheader->clean & 0x1) != 0x1)
                {
                    clear();
                }
                else
                {
                    if (file.size() > baseheader->file_size)
                        file.resize(baseheader->file_size);
                }
            }
        }

        ~FileTree()
        {
        }


        void sync()
        {
            file.sync();
        }


        Header* header()
        {
            return &baseheader->header;
        }


        bool empty()
        {
            return baseheader->root_off == 0;
        }


        void clear()
        {
            printf("Tree cleared\n");

            file.resize(sizeof(BaseHeader));
            memset(baseheader, 0, sizeof(BaseHeader));
            baseheader->sig = signature;
            baseheader->ver = version;
            baseheader->clean = 1;
            baseheader->file_size = file.size();
        }


        void add(const char* key, const Entry& entry)
        {
            baseheader->clean = 0;

            Node* node = 0;

            if (root())
                node = node_search_insert(root(), key, strlen(key));
            else
            {
                node = create_node(0, key);
                setRoot(node);
            }

            node_add_match_entry(node, entry);

            // Root node may become non-root due to balancing while adding new node
            Node* r = root();
            while (r->parent_off)
                r = node_parent(r);
            setRoot(r);

            baseheader->clean = 1;
        }

        std::list<Entry> search(const char* key)
        {
            if (baseheader->root_off == 0) return std::list<Entry>();

            Node* node = node_search(root(), key, strlen(key));
            if (node == 0) return std::list<Entry>();

            std::list<Entry> result;

            DataChunk* chunk = data_chunk_at(node->data_off);
            while (chunk)
            {
                for (int i = 0; i < chunk->entry_count; i++)
                    result.push_back(chunk->entries[i]);
                chunk = data_chunk_at(chunk->next_off);
            }

            return result;
        }

        std::list<Entry> search_prefix(const char *key)
        {
            if (baseheader->root_off == 0) return std::list<Entry>();

            std::list<Entry> result;
            node_search_prefix(root(), key, strlen(key), &result);
            return result;
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
        MappedFile file;

        BaseHeader* baseheader;



        inline Node* node_at(uint32 offset)
        {
            if (offset)
                return file.at<Node>(offset);
            return 0;
        }

        inline uint32 node_off(Node* node)
        {
            if (node)
                return file.off(node);
            return 0;
        }

        inline DataChunk* data_chunk_at(uint32 offset)
        {
            if (offset) return file.at<DataChunk>(offset);
            return 0;
        }

        inline Node* root()
        {
            return node_at(baseheader->root_off);
        }

        inline void setRoot(Node* node)
        {
            baseheader->root_off = file.off(node);
        }



        Node* create_node(Node* parent, const char* key, uint8 color = 0)
        {
            // FIXME: align node here
            int key_len = strlen(key);
            uint32 offs = file.size();
            file.grow(sizeof(Node) + key_len);
            baseheader->file_size = file.size();

            Node* node = node_at(offs);
            node->parent_off = node_off(parent);
            node->left_off = node->right_off = 0;
            node->data_off = 0;
            node->color = color;
            node->key_len = key_len;
            memcpy(node->key, key, key_len);

            return node;
        }


        inline static int strnmcmp(const char* s1, int l1, const char* s2, int l2)
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

        inline static int strnmcmp_prefix(const char* p, int lp, const char* s, int ls)
        {
            if (lp <= ls)
                return strncmp(p, s, lp);
            else
            {
                int r = strncmp(p, s, ls);
                if (r != 0) return r;
                return +1;
            }
        }



        Node* node_search(Node* node, const char* key, int key_len)
        {
            int r = strnmcmp(key, key_len, node->key, node->key_len);

            if (r == 0)
                return node;
            else if (r < 0)
            {
                if (node->left_off)
                    return node_search(node_at(node->left_off), key, key_len);
                else
                    return 0;
            }
            else
            {
                if (node->right_off)
                    return node_search(node_at(node->right_off), key, key_len);
                else
                    return 0;
            }
        }

        void node_search_prefix(Node* node, const char* key, int key_len, std::list<Entry>* list)
        {
            int r = strnmcmp_prefix(key, key_len, node->key, node->key_len);

            if (r <= 0)
            {
                if (node->left_off)
                    node_search_prefix(node_at(node->left_off), key, key_len, list);
            }

            if (r == 0)
            {
                DataChunk *chunk = data_chunk_at(node->data_off);
                while (chunk)
                {
                    for (int i = 0; i < chunk->entry_count; i++)
                        list->push_back(chunk->entries[i]);
                    chunk = data_chunk_at(chunk->next_off);
                }
            }

            if (r >= 0)
            {
                if (node->right_off)
                    node_search_prefix(node_at(node->right_off), key, key_len, list);
            }
        }



        Node* node_search_insert(Node* node, const char* key, int key_len)
        {
            int r = strnmcmp(key, key_len, node->key, node->key_len);

            if (r == 0)
                return node;
            else if (r < 0)
            {
                if (node->left_off)
                    return node_search_insert(node_at(node->left_off), key, key_len);
                else
                {
                    Node* child = create_node(node, key, 1);
                    node->left_off = node_off(child);
                    node_fix(child);
                    return child;
                }
            }
            else
            {
                if (node->right_off)
                    return node_search_insert(node_at(node->right_off), key, key_len);
                else
                {
                    Node* child = create_node(node, key, 1);
                    node->right_off = node_off(child);
                    node_fix(child);
                    return child;
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



        inline Node* node_parent(Node* node)
        {
            if (node->parent_off)
                return node_at(node->parent_off);
            return 0;
        }
        inline Node* node_left(Node* node)
        {
            if (node->left_off)
                return node_at(node->left_off);
            return 0;
        }
        inline Node* node_right(Node* node)
        {
            if (node->right_off)
                return node_at(node->right_off);
            return 0;
        }
        inline Node* node_grandparent(Node* node)
        {
            Node* p = node_parent(node);
            if (p)
                return node_parent(p);
            return 0;
        }
        inline Node* node_uncle(Node* node)
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



        uint32 create_data_chunk()
        {
            uint32 offs = file.size();
            file.grow(sizeof(DataChunk));
            baseheader->file_size = file.size();

            DataChunk* chunk = data_chunk_at(offs);
            chunk->next_off = 0;
            chunk->entry_count = 0;

            return offs;
        }

        void node_add_match_entry(Node* node, const Entry& entry)
        {
            if (node->data_off == 0)
                node->data_off = create_data_chunk();

            DataChunk* chunk = data_chunk_at(node->data_off);

            if (chunk->entry_count == chunk_size)
            {
                node->data_off = create_data_chunk();
                DataChunk* new_chunk = data_chunk_at(node->data_off);
                new_chunk->next_off = file.off(chunk);
                chunk = new_chunk;
            }

            chunk->entries[chunk->entry_count++] = entry;
        }
};
