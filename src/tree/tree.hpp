#ifndef TREE_HPP
#define TREE_HPP

#include <vector>
#include <set>
#include <algorithm>

class Tree
{
public:
    Tree() {};
    virtual ~Tree() = default;
    virtual void Insert(int) = 0;
    virtual size_t Keys_in_interval(int, int) {};
    virtual int Keys_small_number(int) {};
    virtual size_t Key_smaller(int) {};
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
        return v[i];
    }

    size_t Key_smaller(int x) override
    {
        return std::lower_bound(v.begin(), v.end(), x) - v.begin();
    }
};


#endif // TREE_HPP