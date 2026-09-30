#ifndef TREE_HPP
#define TREE_HPP

#include <vector>
#include <set>
#include <algorithm>
#include <stdio.h>

class Tree
{
public:
    Tree() {};
    virtual ~Tree() = default;
    virtual void Insert(int) = 0;
    virtual size_t Keys_in_interval(int, int) = 0;
    virtual int Keys_small_number(int) = 0;
    virtual size_t Key_smaller(int) = 0;
    virtual void Clear() = 0;
};


class Tree_set : public Tree
{
    std::set<int> set;
public:
    Tree_set() {};
    ~Tree_set() override = default;

    void Insert(int x) override 
    {
        set.insert(x);
    }

    size_t Keys_in_interval(int l_value, int r_value) override
    {
        if (l_value > r_value) return 0;
        auto itL = set.upper_bound(l_value);
        auto itR = set.upper_bound(r_value);
        return distance(itL, itR);
    }

    int Keys_small_number(int index) override
    {
        return *next(set.begin(), index - 1);
    }

    size_t Key_smaller(int key) override
    {
        return distance(set.begin(), set.lower_bound(key));
    }

    void Clear() override
    {
        set.clear();
    }
};

class Tree_vec: public Tree
{
    std::vector<int> v;
public:
    void Insert(int x) override
    {
        auto it = std::lower_bound(v.begin(), v.end(), x);
        if (it == v.end() || *it != x) 
        {
            v.insert(it, x);
        }
    }

    size_t Keys_in_interval(int l_value, int r_value) override
    {
        if (l_value >= r_value) return 0;
        auto itL = std::upper_bound(v.begin(), v.end(), l_value);
        auto itR = std::upper_bound(v.begin(), v.end(), r_value);
        return static_cast<int>(itR - itL);
    }

    int Keys_small_number(int i) override
    {
        return v[i - 1];
    }

    size_t Key_smaller(int x) override
    {
        return std::lower_bound(v.begin(), v.end(), x) - v.begin();
    }

    void Clear() override
    {
        v.clear();
    }
};

class Tree_nat : public Tree
{
    class Node
    {
    public:
        Node(int value) : value(value)
        {
            size = 1;
            left = nullptr;
            right = nullptr;
        };
        ~Node()
        {
            if(left)    delete left;
            if(right)   delete right;
        }
        int value;
        size_t size;
        Node* parent;
        Node* left;
        Node* right;
    };

    Node* root;

    bool Insert_node(int value, Node* node)
    {
        if (value == node->value) return false;
    
        if (value < node->value)
        {
            if (node->left)
            {
                if (!Insert_node(value, node->left)) return false;
            }
            else
            {
                node->left = new Node(value);
                node->left->parent = node;
            }
        }
        else
        {
            if (node->right)
            {
                if (!Insert_node(value, node->right)) return false;
            }
            else
            {
                node->right = new Node(value);
                node->right->parent = node;
            }
        }
    
        node->size++;
        return true;
    }

    int Size(Node* node) { return node ? node->size : 0; }

    int Count_less(Node* node, int value)
    {
        if(!node) return 0;
        if(node->value < value)
            return 1 + Size(node->left) + Count_less(node->right, value);
        else
            return Count_less(node->left, value);
    }

    int Count_less_equal(Node* node, int value)
    {
        if(!node) return 0;
        if(node->value <= value)
            return 1 + Size(node->left) + Count_less_equal(node->right, value);
        else
            return Count_less_equal(node->left, value);
    }

public:
    Tree_nat() { root = nullptr; }
    ~Tree_nat() {delete root;};

    void Insert(int x) override
    {
        if(root) Insert_node(x, root);
        else root = new Node(x);
    }

    size_t Keys_in_interval(int l_value, int r_value) override
    {
        if(r_value < l_value) return 0;

        return Count_less_equal(root, r_value) - Count_less_equal(root, l_value);
    }

    int Keys_small_number(int i) override
    {
        Node* node = root;
        while(node)
        {
            int ls = Size(node->left);
            if(i <= ls)     node = node->left;
            else if (i == ls + 1) return node->value;
            else 
            {
                i -= ls + 1;
                node = node->right;
            }
        }

        std::cerr << "Not found " << std::endl;
        return 0;
    }

    size_t Key_smaller(int x) override
    {
        return Count_less(root, x);
    }

    void Clear() override
    {
        delete root;
        root = nullptr;
    }
};


#endif // TREE_HPP