#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <sys/time.h>

#include "BTree.h"


std::vector<int> result;

void callback(const int& key, const int& value)
{
    (void)value;
    std::cout << key << " ";
    result.push_back(key);
}

void scallback(const std::string& key, const int& value)
{
    (void)value;
    std::cout << key << " ";
}

int main()
{
    struct timeval time;
    gettimeofday(&time, 0);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

    BTree<int, int> tree;

    std::vector<int> items;
    for (int i = 0; i < 20; i++)
        items.push_back(i);

    random_shuffle(items.begin(), items.end());

    for (std::vector<int>::const_iterator i = items.begin(); i != items.end(); i++)
        std::cout << *i << " ";
    std::cout << "\n";


    for (unsigned int i = 0; i < items.size(); i++)
    {
        tree.add(items[i], 1);
    }

    int key = 9;
    std::cout << "search(" << key << ") = " << tree.search(key) << "\n";

    result.clear();
    tree.walk(callback);
    std::cout << "\n";

    for (int i = 0; i < (signed)result.size(); i++)
        if (result[i] != i)
            printf("WRONG\n");



    BTree<std::string, int> stree;
    stree.add("one", 1);
    stree.add("two", 2);
    stree.add("three", 3);
    stree.add("four", 4);
    stree.add("five", 5);
    stree.walk(scallback);
    std::cout << "\n";
}
