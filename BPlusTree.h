#ifndef B_PLUS_TREE_BPLUSTREE_H
#define B_PLUS_TREE_BPLUSTREE_H

#include <fstream>
#include <iostream>
#include <time.h>
#include <vector>
#include <cstring>
#include <algorithm>
#include <list>
#include "../map/map.hpp"
#include "../map/utility.hpp"
#include "../map/exceptions.hpp"


const int MAX_KEY = 21; //B+树的阶
const int string_size = 70;
const int the_size = 20;//缓存大小


class mystring{
public:
    char c[70] = {'\0'};
public:
    mystring() = default;
    mystring(std::string s){
        std::copy(s.begin(), s.end(), c);
    }
    bool operator < (const mystring & other) const {
        int result = std::strcmp(c, other.c);
        return (result < 0);
    }
    bool operator > (const mystring & other) const {
        int result = std::strcmp(c, other.c);
        return (result > 0);
    }
    bool operator == (const mystring & other) const {
        int result = std::strcmp(c, other.c);
        return (result == 0);
    }
    bool operator != (const mystring & other) const {
        return !(*this == other);
    }
    mystring & operator = (const mystring & other){
        for(int i = 0; i < string_size - 1; ++i){
            c[i] = other.c[i];
        }
        return *this;
    }
    friend std::ostream& operator<<(std::ostream& out,const mystring &s){
        out << s.c;
        return out;
    }
};

template<typename keytype, typename valuetype>
struct Node{
    keytype first;
    valuetype second;
    int pos = -1;           //记录节点在文件中的存储位置
    Node() = default;
    Node(keytype key, valuetype val){
        first = key;
        second = val;
    }
    bool operator < (const Node& other) const {
        if(first < other.first) return true;
        else if(first > other.first) return false;
        else return (second < other.second);
        return true;
    }
    bool operator > (const Node& other) const {
        if(first > other.first) return true;
        else if(first < other.first) return false;
        else {
            if(second < other.second || second == other.second) return false;
            else return true;
        }
        return true;
    }
    bool operator == (const Node& other) const {
        if(first == other.first && second == other.second) return true;
        return false;
    }
    bool operator != (const Node& other) const {
        return !(*this == other);
    }
    Node& operator = (const Node &other){
        if(this == &other) return *this;
        first = other.first;
        second = other.second;
        return *this;
    }
};

template<typename keytype, typename valuetype>
struct Mynode{
    Node<keytype, valuetype> node;
    int fpos;  //父亲节点BPlusNode的位置
    int pos;   //现在BPlusNode的位置，没用到
    bool exist;
};

//这里的Node实际上是key，存放int只是为了防止重复。测试点中为string
//这里myvalue是对应的value。测试点中为int

//BPlusNode为树中的节点
template<typename keytype, typename valuetype>
struct BPlusNode{
    Node<keytype, valuetype> key[MAX_KEY] ; //关键字域，存放这个节点所存储的key
    int Ptr[MAX_KEY+1] = {};//指针域，存放这个节点的儿子在文件中的位置
    int num;           //关键字数
    int pos;           //指示BPlusNode在文件的什么位置
    bool isleaf;       //指示这时的节点是不是在底部，0表示不是，1表示是
    int next_pos = -1; //下一个Node在文件中的位置
    int last_pos = -1; //上一个Node在文件中的位置
};

// template<typename keytype, typename valuetype>
// class memory_river{
// public:
//     std::list<BPlusNode<keytype, valuetype>> mylist;
//     class Compare1 {
//     public:
//         bool operator () (const int &lhs, const int &rhs) const {
//             return lhs < rhs;
//         }
//     };
//     sjtu::map<int, typename std::list<BPlusNode<keytype, valuetype>>::iterator, Compare1> mymap;
// };

template<typename keytype, typename valuetype>
class BPlusTree{
public:
    std::string Nfile_name;
    std::string Ifile_name;
    std::fstream Nfile; //存放Node的file
    std::fstream Ifile; //存放特征信息root和num的file
    int root = -1;      //存放root的位置
    int num;            //表示整个树里面有多少个数据
    int file_tail;      //存放Nfile文件的末尾位置
    Mynode<keytype, valuetype> tmp;
    // memory_river<keytype, valuetype> mycache;

public:
    BPlusTree() = default;
    BPlusTree(std::string name1, std::string name2);
    ~BPlusTree();
    void writeNode(BPlusNode<keytype, valuetype> &n, int pos_in);
    void readNode(BPlusNode<keytype, valuetype> &n, int pos_in);
    bool Insert(Node<keytype, valuetype> &n);
    bool insert(int cur, Node<keytype, valuetype> &n);
    void Remove(Node<keytype, valuetype> &n);
    void remove(int cur, Node<keytype, valuetype> &n);
    void spilit(BPlusNode<keytype, valuetype> &father, BPlusNode<keytype, valuetype> &current, int index);
    void search(Node<keytype, valuetype> &n, Mynode<keytype, valuetype> &result, int &p);
    int create();
    int myfind(const Node<keytype, valuetype>* key, int num, const keytype & s);
    int myfind_node(const Node<keytype, valuetype>* key, int num, const Node<keytype, valuetype> & s);
    std::pair<BPlusNode<keytype, valuetype>, int> find_node(int p, const keytype & s);
    std::vector<valuetype> final_find(const keytype & s);
    int modify_find(Node<keytype, valuetype>* key, int num, Node<keytype, valuetype>& origin_node, Node<keytype, valuetype>& tran_node);
    void modify_node(Node<keytype, valuetype>& origin_node, Node<keytype, valuetype>& tran_node, int p);
    void modify(Node<keytype, valuetype>& origin_node, Node<keytype, valuetype>& tran_node);
    void case1(BPlusNode<keytype, valuetype> &x, BPlusNode<keytype, valuetype> &y, int &i, int &cur);
    void case2(BPlusNode<keytype, valuetype> &x, BPlusNode<keytype, valuetype> &y, BPlusNode<keytype, valuetype> &lchild, int &i, int &j, int &cur);
    void case3(BPlusNode<keytype, valuetype> &x, BPlusNode<keytype, valuetype> &y, BPlusNode<keytype, valuetype> &lchild, int &i, int &j, int &cur);
    void case4(BPlusNode<keytype, valuetype> &x, BPlusNode<keytype, valuetype> &y, BPlusNode<keytype, valuetype> &rchild, int &i, int &j, int &cur);
    void case5(BPlusNode<keytype, valuetype> &x, BPlusNode<keytype, valuetype> &y, BPlusNode<keytype, valuetype> &rchild, int &i, int &j, int &cur);
};

#endif //B_PLUS_TREE_BPLUSTREE_H