#include <cstdio>
#include <cstring>

#include "IndexTree.h"


void callback(const char* key, int key_len)
{
    printf("%.*s ", key_len, key);
}


#include <iostream>
#include <fstream>
#include <string>

int main()
{
    IndexTree tree("t");

    if (tree.empty())
    {
        std::ifstream wordsfile("words.txt");
        std::string word;
        int i = 1;
        while (true)
        {
            std::getline(wordsfile, word);
            if (wordsfile.eof()) break;

            tree.add(word.c_str(), MatchEntry(i, 1));

            i++;
        }
    }


    std::list<MatchEntry> found = tree.search_prefix("Аарон");
    std::cout << "Found " << found.size() << ":\n\t";
    for (std::list<MatchEntry>::const_iterator i = found.begin(); i != found.end(); i++)
        std::cout << i->docid << ":" << i->token_no << "  ";
    std::cout << "\n";
}
