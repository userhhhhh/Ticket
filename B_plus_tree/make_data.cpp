// 这个程序用于随机生成测试数据

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
#include <bits/stdc++.h>

std::map<std::string, std::set<int>> m;
std::vector<Node> alused;
int value;
BPlusTree t;
int mymax = 30000;
int getRandomNode(const std::vector<Node>& nodeSet) {
    int size = nodeSet.size();
    int tmp = rand() % size;
    return tmp;
}

int main(){
    freopen("0.out","w",stdout);
    srand(3); 
    int x, a, b, c, v;
    //生成数x在 1 - mymax 之间
    BPlusTree mytree;

    mytree.Nfile.open("Nfile.txt",std::ios::out);
    mytree.Nfile.close();
    mytree.Ifile.open("Ifile.txt",std::ios::out | std::ios::in);
    mytree.Ifile.close();
    mytree.root = -1;
    mytree.num = 0;
    mytree.file_tail = 0;
    mytree.Nfile.open("Nfile.txt",std::ios::in | std::ios::out);

    int count = 0;

    // for(int i = 1; i < 100; ++i){
    //     std::string s = std::to_string(i);
    //     Node n(s);
    //     n.second = i;
    //     mytree.Insert(n);
    // }
    // std::vector<int> ans;
    // for(int i = 1; i < 100; ++i){
    //     ans = mytree.final_find(std::to_string(i));
    //     std::cout << ans[0] << ' ' << ans.size() << '\n';
    // }

    // for(int i = 100; i >= 1; --i){
    //     std::string s = std::to_string(i);
    //     Node n(s);
    //     n.second = i;
    //     mytree.Remove(n);
    // }
    // 备注：最简单的操作

    std::vector<int> vv;
    while(true){
        x = rand() % 3 + 1;
        count++;
        if(count <= 30){
            x = 1;
        }
        if(x == 1){
            std::string s = "";
            a = rand() % 20 + 1;
            b = rand() % 20 + 1;
            c = rand() % 20 + 1;
            s += (a + 'a');
            s += (b + 'a');
            s += (c + 'a');
            v = rand() % mymax + 1;
            
            while(find(vv.begin(),vv.end(),v) != vv.end()){
                v = rand() % mymax + 1;
            }
            vv.push_back(v);

            Node n(s);
            n.second = v;
            std::cout << "insert" << " " << s << " " << v << std::endl;
            alused.push_back(n);

            mytree.Insert(n);
            m[n.first].insert(n.second);
            
        }
        else if (x == 2){
            if(alused.empty()){
                continue;
            }
            int tmp = getRandomNode(alused);
            Node n = alused[tmp];
            std::cout << "delete" << " " << n.first << " " << n.second << std::endl;
            mytree.Remove(n);
            m[n.first].erase(n.second);
        }
        else{
            if(alused.empty()){
                continue;
            }
            int tmp = getRandomNode(alused);
            Node n = alused[tmp];
            std::cout << "find" << " " << n.first << std::endl;
            std::vector<int> ans;
            ans = mytree.final_find(n.first);
            if(ans.empty()) continue;
            std::sort(ans.begin(),ans.end());
            if((int)ans.size() != (int)m[n.first].size()) {
                std::cout << "hhhhhhhhhhhhh" << std::endl;
                for (auto it = m[n.first].begin(); it != m[n.first].end(); ++it) {
                    std::cout << *it << " ";
                }
                std::cout << std::endl;
                for(int i = 0; i < ans.size(); ++i){
                    std::cout << ans[i] << " ";
                }
                std::cout << std::endl;
                return 0;
            }
            int i = 0;
            for (auto it = m[n.first].begin(); it != m[n.first].end(); ++it, ++i) {
                if(*it != ans[i]){
                    std::cout << "hhhhhhhhhhhhh" << std::endl;
                    for (auto it = m[n.first].begin(); it != m[n.first].end(); ++it) {
                        std::cout << *it << " ";
                    }
                    std::cout << std::endl;
                    for(int i = 0; i < ans.size(); ++i){
                        std::cout << ans[i] << " ";
                    }
                    std::cout << std::endl;
                    return 0;
                }
            }
        }
    }
}