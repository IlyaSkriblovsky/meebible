#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>

#include "BTree.h"

typedef BTree<std::string, std::list<int> > Index;

class Serializer
{
    private:
        Index *tree;
        FILE *f;


    public:
        Serializer(Index *tree)
            : tree(tree), f(0)
        {
        }

        ~Serializer()
        {
            fclose(f);
        }

        unsigned int saveNode(Index::Node* node)
        {
            if (node == 0) return 0;

            unsigned int left_offset = saveNode(node->left);
            unsigned int right_offset = saveNode(node->right);

            unsigned int this_offset = ftell(f);
            fwrite(&left_offset, sizeof(left_offset), 1, f);
            fwrite(&right_offset, sizeof(right_offset), 1, f);

            unsigned char key_len = node->key.length();
            const char* key = node->key.c_str();
            fwrite(&key_len, 1, 1, f);
            fwrite(key, 1, key_len, f);

            unsigned int docid_count = node->value.size();
            fwrite(&docid_count, sizeof(unsigned int), 1, f);
            for (std::list<int>::const_iterator i = node->value.begin(); i != node->value.end(); i++)
            {
                int docid = *i;
                fwrite(&docid, sizeof(int), 1, f);
            }

            return this_offset;
        }

        void save(const char *filename)
        {
            f = fopen(filename, "wb");

            unsigned int signature = 0xc0d0e0f0;
            fwrite(&signature, sizeof(signature), 1, f);

            unsigned int root_offset = saveNode(tree->root);
            fseek(f, 0, SEEK_SET);
            fwrite(&root_offset, sizeof(unsigned int), 1, f);
        }
};


int main()
{
    BTree<std::string, std::list<int> > tree;

    std::ifstream wordsfile("words.txt");
    std::string word;
    int i = 0;
    while (true)
    {
        std::getline(wordsfile, word);
        if (wordsfile.eof()) break;

        i++;
        std::list<int>& doclist = tree.search_or_add(word);
        doclist.push_back(i);
    }
    std::cout << "total = " << i << "\n";

    std::list<int>* doclist = tree.search_ptr("Иегова");
    if (doclist == 0)
        std::cout << "not found\n";
    else
    {
        for (std::list<int>::const_iterator i = doclist->begin(); i != doclist->end(); i++)
            std::cout << *i << " ";
        std::cout << "\n";
        std::cout << doclist->size();
        std::cout << "\n";
    }


    Serializer serializer(&tree);
    serializer.save("tree.dat");
}
