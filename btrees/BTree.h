#ifndef __BTREE_H
#define __BTREE_H

#include <iostream>
#include <cstdlib>


template <typename K, typename V>
class BTree
{
    public:
        typedef void walk_callback(const K& key, const V& value);


        class Node
        {
            public:
                const K key;
                V value;

                Node *parent;
                Node *left;
                Node *right;

                Node(Node *parent, const K& key, const V& value)
                    : key(key), value(value), parent(parent), left(0), right(0), color(false)
                {
                }

                ~Node()
                {
                    delete left;
                    delete right;
                }

                void add(const K& key, const V& value)
                {
                    if (key < this->key)
                    {
                        if (left)
                            left->add(key, value);
                        else
                        {
                            left = new Node(this, key, value);
                            left->color = true;
                            left->fix_tree();
                        }
                    }
                    else
                    {
                        if (right)
                            right->add(key, value);
                        else
                        {
                            right = new Node(this, key, value);
                            right->color = true;
                            right->fix_tree();
                        }
                    }
                }

                void print(int indent)
                {
                    for (int i = 0; i < indent; i++)
                        std::cout << ' ';
                    if (color)
                        std::cout << "*";
                    std::cout << key << "\n";

                    if (left)
                        left->print(indent + 2);
                    else
                        if (right)
                        {
                            for (int i = 0; i < indent+2; i++) std::cout << ' ';
                            std::cout << "-\n";
                        }
                    if (right) right->print(indent + 2);
                }


                Node* search(const K& key)
                {
                    if (key == this->key)
                        return this;

                    if (key < this->key)
                    {
                        if (left)
                            return left->search(key);
                        else
                            return 0;
                    }

                    if (right)
                        return right->search(key);
                    else
                        return 0;
                }

                void walk(walk_callback callback)
                {
                    if (left) left->walk(callback);
                    callback(key, value);
                    if (right) right->walk(callback);
                }


                V& search_or_add(const K& key)
                {
                    if (key == this->key)
                        return value;

                    if (key < this->key)
                    {
                        if (left)
                            return left->search_or_add(key);
                        else
                        {
                            left = new Node(this, key, V());
                            V& result = left->value;
                            left->color = true;
                            left->fix_tree(); // left might be null after this call
                            return result;
                        }
                    }

                    if (right)
                        return right->search_or_add(key);
                    else
                    {
                        right = new Node(this, key, V());
                        V& result = right->value;
                        right->color = true;
                        right->fix_tree(); // right might be null after this call
                        return result;
                    }
                }

            private:

                bool color;


                Node* grandparent()
                {
                    if (parent && parent->parent)
                        return parent->parent;
                    return 0;
                }

                Node* uncle()
                {
                    Node* gp = grandparent();
                    if (gp == 0)
                        return 0;
                    else
                    {
                        if (parent == gp->left)
                            return gp->right;
                        else
                            return gp->left;
                    }
                }


                void fix_tree()
                {
                    if (color == false)
                        return;

                    if (parent == 0)
                    {
                        // Case 1
                        color = false;
                        return;
                    }

                    if (parent->color == false)
                        // Case 2
                        return;

                    Node* u = uncle();
                    if (u && u->color == true)
                    {
                        // Case 3
                        parent->color = false;
                        u->color = false;
                        Node* g = grandparent();
                        g->color = true;
                        g->fix_tree();
                        return;
                    }


                    Node* g = grandparent(); // not null because root is always black

                    if (this == parent->right  &&  parent == g->left)
                    {
                        // Case 4a
                        parent->rotate_left();
                        left->fix_tree();
                        return;
                    }

                    if (this == parent->left  &&  parent == g->right)
                    {
                        // Case 4b
                        parent->rotate_right();
                        right->fix_tree();
                        return;
                    }


                    // Case 5
                    parent->color = false;
                    g->color = true;
                    if (this == parent->left  &&  parent == g->left)
                        g->rotate_right();
                    else // this == parent->right  &&  parent == g->right
                        g->rotate_left();
                }


                void rotate_left()
                {
                    if (parent)
                    {
                        if (this == parent->left)
                            parent->left = right;
                        else
                            parent->right = right;
                    }
                    right->parent = parent;

                    parent = right;
                    right = right->left;
                    parent->left = this;
                    if (right) right->parent = this;
                }

                void rotate_right()
                {
                    if (parent)
                    {
                        if (this == parent->left)
                            parent->left = left;
                        else
                            parent->right = left;
                    }
                    left->parent = parent;

                    parent = left;
                    left = left->right;
                    parent->right = this;
                    if (left) left->parent = this;
                }
        };



        Node* root;


        BTree(): root(0)
        {
        }

        ~BTree()
        {
            delete root;
        }


        void add(const K& key, const V& value)
        {
            if (root == 0)
                root = new Node(0, key, value);
            else
                root->add(key, value);
            while (root->parent) root = root->parent;
        }

        void print()
        {
            if (root == 0)
                std::cout << "<empty>\n";
            else
                root->print(0);
        }

        V search(const K& key)
        {
            if (root == 0)
                return V();
            else
            {
                Node* n = root->search(key);
                return n == 0 ? V() : n->value;
            }
        }
        V* search_ptr(const K& key)
        {
            if (root == 0)
                return 0;
            else
            {
                Node* n = root->search(key);
                return n == 0 ? 0 : &n->value;
            }
        }

        Node* search_node(const K& key)
        {
            if (root == 0)
                return 0;
            else
                return root->search(key);
        }


        void walk(walk_callback callback)
        {
            if (root)
                root->walk(callback);
        }

        V& search_or_add(const K& key)
        {
            if (root == 0)
            {
                root = new Node(0, key, V());
                return root->value;
            }
            else
            {
                V& result = root->search_or_add(key);
                while (root->parent) root = root->parent;
                return result;
            }
        }
};


#endif
