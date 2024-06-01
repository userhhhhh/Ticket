#include "BPlusTree.h"
#include "BPlusTree.cpp"


BPlusTree<mystring, int> tree("Nfile.txt", "Ifile.txt");

int main(){

    //   freopen("0.in","r",stdin);
    //   freopen("0.out","w",stdout);
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    //debug
//    tree.Nfile.open("Nfile.txt",std::ios::out | std::ios::in);
//    tree.Nfile.close();
//    tree.Ifile.open("Ifile.txt",std::ios::out | std::ios::in);
//    tree.Ifile.close();
//    tree.root = -1;
//    tree.num = 0;
//    tree.file_tail = 0;
    // 备注：上面的代码用于清空文件，调试
//    tree.Nfile.open("Nfile.txt",std::ios::in | std:: ios::out);

    mystring str("!");
    Node<mystring, int> tmp(str, -10000);
    // tmp.second = -10000;
    tree.Insert(tmp);
    BPlusNode<mystring, int> nn1;
    tree.Nfile.seekg(0);
    tree.Nfile.read(reinterpret_cast<char*>(&nn1), sizeof(nn1));
    int tmpp = sizeof (nn1);

    int command_count;
    std::cin >> command_count;
    for (int i = 0; i < command_count; ++i){
        std::string s1, s2;
        int val;
        std::cin >> s1;
        if(s1 == "insert"){
            BPlusNode<mystring, int> nn;
            tree.Nfile.seekg(0);
            tree.Nfile.read(reinterpret_cast<char*>(&nn), sizeof(nn));

            std::cin >> s2 >> val;
            mystring str1(s2);
            Node<mystring, int> n(str1, val);
            // n.second = val;
            tree.Insert(n);
        }
        else if(s1 == "delete"){
            std::cin >> s2 >> val;
            mystring str2(s2);
            Node<mystring, int> n(str2, val);
            tree.Remove(n);
        }
        else if(s1 == "find"){
            std::cin >> s2;
            mystring str2(s2);
            // Node<mystring, int> n(str2, val);
            std::vector<int> && ans = tree.final_find(str2);
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