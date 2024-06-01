// 这个程序用于调试
// judgeNode、judgewholeNode、check 用于判断 B+ 树结构是否合理
// traverse、traverseBPlusTree 用于前序输出整个 B+ 树

#include <cstdlib>
#include <iostream>
#include <string>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <vector>
#include "BPlusTree.h"
#include "BPlusTree.cpp"

BPlusTree tree;

bool judgeNode(BPlusNode n, BPlusTree& b){
    if(n.pos != b.root){
        if(n.num < MAX_KEY / 2 - 1 || n.num > MAX_KEY){
            std::cout << "wrong num, here is the pos: " << n.pos << std::endl;
            exit(0);
            return false;
        }
    }
    for(int i = 1; i < n.num ; ++i){
        if(n.key[i] < n.key[i-1]){
            std::cout << "wrong key, the key is not ordered correctly or the key is repeated: " << n.pos <<std::endl;
            exit(0);
            return false;
        }
    }
    if(n.isleaf == false){
        for(int i = 0; i < n.num; ++i){
            if(n.Ptr[i] == n.Ptr[i + 1] && n.Ptr[i] != 0){
                std::cout << "wrong Ptr, same: " << n.pos << std::endl;
                exit(0);
                return false;
            }
        }
        BPlusNode n1;
        for(int i = 0; i < n.num; ++i){
            b.readNode(n1, n.Ptr[i]);
            if(n1.key[n1.num - 1] != n.key[i]){
                BPlusNode tmp1;
                b.readNode(tmp1, 12432);
                std::cout << "wrong son, the key is wrong, here is pos: " << n.pos << " here is index: " << i << std::endl;
                exit(0);
                return false;
            }
        }
        b.readNode(n1, n.Ptr[n.num]);
        if(n1.key[0] < n.key[n.num - 1] || n1.key[0] == n.key[n.num - 1]){
            std::cout << "wrong son, the last key is wrong, here is pos: " << n.pos << std::endl;
            exit(0);
            return false;
        }
    }
    return true;
}
bool judgewholeNode(BPlusNode node, BPlusTree& b) {
    bool flag = judgeNode(node, b);
    if(flag == false) return false;
    if (!node.isleaf) {
        for (int i = 0; i < node.num + 1; ++i) {
            BPlusNode tmp ;
            tree.readNode(tmp, node.Ptr[i]);
            judgeNode(tmp, b);
        }
    }
    return true;
}
bool check(BPlusTree& b){
    BPlusNode tmp;
    b.readNode(tmp, b.root);
    bool flag = judgewholeNode(tmp, b);
    return flag;
}

void traverse(BPlusNode node) {
    for (int i = 0; i < node.num; ++i) {
        std::cout << node.key[i].first << " " << node.key[i].second << " ";
    }
    std::cout << std::endl;

    if (!node.isleaf) {
        for (int i = 0; i < node.num + 1; ++i) {
            BPlusNode tmp ;
            tree.readNode(tmp, node.Ptr[i]);
            traverse(tmp);
        }
    }
}

void traverseBPlusTree() {
    BPlusNode tmp1;
    tree.readNode(tmp1, tree.root);
    traverse(tmp1);
    std::cout<<"----------------" << std::endl;
}

int main(){

    //   freopen("0.in","r",stdin);
    //   freopen("0.out","w",stdout);
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    //debug
    tree.Nfile.open("Nfile.txt",std::ios::out | std::ios::in);
    tree.Nfile.close();
    tree.Ifile.open("Ifile.txt",std::ios::out | std::ios::in);
    tree.Ifile.close();
    tree.root = -1;
    tree.num = 0;
    tree.file_tail = 0;
    tree.Nfile.open("Nfile.txt",std::ios::in | std:: ios::out);

    bool flag = false;

    Node tmp("!");
    tmp.second = -10000;
    tree.Insert(tmp);

    int command_count;
    std::cin >> command_count;
    for (int i = 0; i < command_count; ++i){
        std::string s1, s2;
        int val;
        std::cin >> s1;
        if(s1 == "insert"){
            std::cin >> s2 >> val;
            Node n(s2);
            n.second = val;
         
            // check(tree);
            // if(s2 == "hsn"){
            //     std::cout << "hhh" << std::endl;
            //     flag = true;
            // }
            // if(flag){
            //     traverseBPlusTree();
            // }
            // std::cout << "insert" << " " << s2 << " " << val << std::endl;

            tree.Insert(n);

            // if(flag){
            //     traverseBPlusTree();
            // }
            // check(tree);
        }
        else if(s1 == "delete"){
            std::cin >> s2 >> val;
            Node n(s2);
            n.second = val;

            // if(s2 == "ehe"){
            //     std::cout << "hhh" << std::endl;
            //     flag = true;
            // }
            // if(flag){
            //     traverseBPlusTree();
            // }
            // check(tree);
            // std::cout << "delete" << " " << s2 << " " << val << std::endl;

            tree.Remove(n);
            
            // check(tree);
            // if(flag){
            //     traverseBPlusTree();
            // }
        }
        else if(s1 == "find"){
            std::cin >> s2;
            std::vector<int> && ans = tree.final_find(s2);
            if(ans.empty()){
                std::cout << "null" <<std::endl;
                continue;
            }
            for(int i = 0; i < ans.size(); ++i){
                std::cout << ans[i] << " ";
            }
            std::cout << std::endl;
        }
        else{}
    }
}