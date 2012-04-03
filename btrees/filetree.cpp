#include <cstdio>
#include <cstring>

#include "fileio.h"

#define FileIO MappedFileIO

class FileTree
{
    public:
        typedef void (*walk_callback)(const char *key);

        struct NodeHeader
        {
            #define OFFS__NODEHEADER_PARENT_OFFS  0
            uint32 parent_offs;
            #define OFFS__NODEHEADER_LEFT_OFFS  4
            uint32 left_offs;
            #define OFFS__NODEHEADER_RIGHT_OFFS  8
            uint32 right_offs;

            #define OFFS__NODEHEADER_COLOR  12
            uint8 color;

            #define OFFS__NODEHEADER_KEY_LEN  13
            uint8 key_len;

            #define OFFS__NODEHEADER_KEY  14

            NodeHeader()
                : parent_offs(0), left_offs(0), right_offs(0),
                  color(0), key_len(0)
            {
            }

        } __attribute__ ((packed));


        class NodeDesc
        {
            private:
                FileIO* io;
                uint32 offs;
                NodeHeader header;

            public:
                NodeDesc(FileIO* io, uint32 offs)
                    : io(io), offs(offs)
                {
                    if (offs == 0)
                        fprintf(stderr, "ERROR: NodeDesc(0)\n");

                    header = io->read<NodeHeader>(offs);
                }


                uint32 offset() { return offs; }


                uint8 key_len() { return header.key_len; }
                void key(char* buf)
                {
                    io->read_chars(offs + OFFS__NODEHEADER_KEY, buf, header.key_len);
                    buf[header.key_len] = 0;
                }


                uint32 parent_offs() { return header.parent_offs; }
                uint32 left_offs() { return header.left_offs; }
                uint32 right_offs() { return header.right_offs; }


                bool has_parent() { return header.parent_offs != 0; }
                NodeDesc parent() { return NodeDesc(io, header.parent_offs); }
                bool has_left() { return header.left_offs != 0; }
                NodeDesc left() { return NodeDesc(io, header.left_offs); }
                bool has_right() { return header.right_offs != 0; }
                NodeDesc right() { return NodeDesc(io, header.right_offs); }

                void setParent(uint32 val)
                {
                    header.parent_offs = val;
                    io->write<uint32>(offs + OFFS__NODEHEADER_PARENT_OFFS, val);
                }
                void setLeft(uint32 val)
                {
                    header.left_offs = val;
                    io->write<uint32>(offs + OFFS__NODEHEADER_LEFT_OFFS, val);
                }
                void setRight(uint32 val)
                {
                    header.right_offs = val;
                    io->write<uint32>(offs + OFFS__NODEHEADER_RIGHT_OFFS, val);
                }

                uint8 color() { return header.color; }
                void setColor(uint8 color)
                {
                    header.color = color;
                    io->write<uint8>(offs + OFFS__NODEHEADER_COLOR, color);
                }

                bool has_grandparent()
                {
                    if (has_parent())
                    {
                        NodeDesc p = parent();
                        return p.has_parent();
                    }
                    else
                        return false;
                }
                NodeDesc grandparent()
                {
                    return parent().parent();
                }

                bool has_uncle()
                {
                    if (! has_grandparent())
                        return false;

                    NodeDesc g = grandparent();
                    if (g.left_offs() == parent_offs())
                        return g.has_right();
                    else
                        return g.has_left();
                }
                NodeDesc uncle()
                {
                    NodeDesc g = grandparent();
                    if (g.left_offs() == parent_offs())
                        return g.right();
                    else
                        return g.left();
                }
        };


        void walk(walk_callback callback)
        {
            uint32 root = rootOffs();
            if (root)
                node_walk(root, callback);
        }

    public:
        FileTree(const char* tree_filename)
            : tree_io(tree_filename)
        {
            if (tree_io.size() == 0)
                setRootOffs(0);
        }

        ~FileTree()
        {
            printf("~FileTree, root = 0x%x\n", rootOffs());
        }


        void add(const char* key)
        {
            if (rootOffs() == 0)
                setRootOffs(createNode(0, key));
            else
                node_add(rootOffs(), key);

            while (true)
            {
                NodeDesc r(&tree_io, rootOffs());
                if (r.has_parent())
                    setRootOffs(r.parent_offs());
                else
                    break;
            }
        }



        void print()
        {
            uint32 parent = rootOffs();
            if (parent == 0)
                printf("<empty>\n");
            else
                node_print(parent, 0);
        }


    private:
        FileIO tree_io;

        void setRootOffs(uint32 parent)
        {
            tree_io.write<uint32>(0, parent);
        }

        uint32 rootOffs()
        {
            return tree_io.read<uint32>(0);
        }

        uint32 createNode(uint32 parent, const char *key, uint8 color = 0)
        {
            uint32 offs = tree_io.size();

            NodeHeader header;
            header.parent_offs = parent;
            header.color = color;
            header.key_len = strlen(key);

            tree_io.write<NodeHeader>(offs, header);
            tree_io.write_chars(offs + OFFS__NODEHEADER_KEY, key, header.key_len);

            return offs;
        }


        void node_add(uint32 offs, const char* key)
        {
            NodeDesc d(&tree_io, offs);
            char node_key[d.key_len()+1];
            d.key(node_key);

            int cmp = strcmp(key, node_key);
            if (cmp == 0)
            {
                // printf("Found: 0x%0x\n", offs);
            }
            else if (cmp < 0)
            {
                if (d.has_left())
                    node_add(d.left_offs(), key);
                else
                {
                    uint32 child = createNode(offs, key, 1);
                    d.setLeft(child);
                    node_fix_tree(child);
                }
            }
            else
            {
                if (d.has_right())
                    node_add(d.right_offs(), key);
                else
                {
                    uint32 child = createNode(offs, key, 1);
                    d.setRight(child);
                    node_fix_tree(child);
                }
            }
        }




        void node_fix_tree(uint32 offs)
        {
            NodeDesc d(&tree_io, offs);

            if (d.color() == 0)
                return;

            if (! d.has_parent())
            {
                // Case 1
                d.setColor(0);
                return;
            }

            NodeDesc parent = d.parent();
            if (parent.color() == 0)
                // Case 2
                return;

            if (d.has_uncle())
            {
                NodeDesc uncle = d.uncle();
                if (uncle.color() != 0)
                {
                    // Case 3
                    parent.setColor(0);
                    uncle.setColor(0);
                    NodeDesc gp = d.grandparent();
                    gp.setColor(1);
                    node_fix_tree(gp.offset());
                    return;
                }
            }

            NodeDesc grandparent = d.grandparent();

            if (offs == parent.right_offs() && parent.offset() == grandparent.left_offs())
            {
                // Case 4a
                rotate_left(parent.offset());
                node_fix_tree(d.parent_offs());
                return;
            }
            if (offs == parent.left_offs() && parent.offset() == grandparent.right_offs())
            {
                // Case 4b
                rotate_right(parent.offset());
                node_fix_tree(d.parent_offs());
                return;
            }

            // Case 5
            parent.setColor(0);
            grandparent.setColor(1);
            if (offs == parent.left_offs() && parent.offset() == grandparent.left_offs())
                rotate_right(grandparent.offset());
            else
                rotate_left(grandparent.offset());
        }


        void rotate_left(uint32 offs)
        {
            NodeDesc d(&tree_io, offs);
            if (d.has_parent())
            {
                NodeDesc parent = d.parent();
                if (offs == parent.left_offs())
                    parent.setLeft(d.right_offs());
                else
                    parent.setRight(d.right_offs());
            }
            d.right().setParent(d.parent_offs());

            d.setParent(d.right_offs());
            d.setRight(d.right().left_offs());
            d.parent().setLeft(offs);
            if (d.has_right())
                d.right().setParent(offs);
        }


        void rotate_right(uint32 offs)
        {
            NodeDesc d(&tree_io, offs);
            if (d.has_parent())
            {
                NodeDesc parent = d.parent();
                if (offs == parent.left_offs())
                    parent.setLeft(d.left_offs());
                else
                    parent.setRight(d.left_offs());
            }
            d.left().setParent(d.parent_offs());

            d.setParent(d.left_offs());
            d.setLeft(d.left().right_offs());
            d.parent().setRight(offs);
            if (d.has_left())
                d.left().setParent(offs);
        }



        void node_print(uint32 offs, int indent)
        {
            for (int i = 0; i < indent; i++) printf(" ");

            if (offs == 0)
            {
                printf("-\n");
                return;
            }

            NodeHeader header = tree_io.read<NodeHeader>(offs);
            char key[header.key_len+1];
            tree_io.read_chars(offs + OFFS__NODEHEADER_KEY, key, header.key_len);
            key[header.key_len] = 0;

            if (header.color)
                printf("*");
            printf("%s\n", key);

            if (header.left_offs != 0  ||  header.right_offs != 0)
            {
                node_print(header.left_offs, indent + 2);
                node_print(header.right_offs, indent + 2);
            }
        }

        void node_walk(uint32 offs, walk_callback callback)
        {
            NodeDesc d(&tree_io, offs);

            if (d.has_left()) node_walk(d.left_offs(), callback);

            char key[d.key_len()+1];
            d.key(key);
            callback(key);

            if (d.has_right()) node_walk(d.right_offs(), callback);
        }
};


#include <iostream>
#include <fstream>
#include <string>

void callback(const char* key)
{
    printf("%s ", key);
}

int main(int argc, char *argv[])
{
    {
        FileTree tree("t.bin");

        std::ifstream wordsfile("words.txt");
        std::string word;
        while (true)
        {
            std::getline(wordsfile, word);
            if (wordsfile.eof()) break;

            tree.add(word.c_str());
        }
    }

//    printf("\n------------\n");
//
//    {
//        FileTree tree("t.bin");
//        tree.walk(callback); printf("\n");
//        printf("\n");
//    }
}
