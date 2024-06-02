#include <iostream>
#include <string>
#include <vector>
#include "BPlusTree.h"
#include <list>

template<typename keytype, typename valuetype>
BPlusTree<keytype, valuetype>::BPlusTree(std::string name1, std::string name2){
    Nfile_name = name1;
    Ifile_name = name2;
    //打开存放Node的file
    Nfile.open(name1,std::ios::in | std:: ios::out);
    if (!Nfile.good()){
        Nfile.open(name1, std::ios::out);
        Nfile.close();
        Nfile.open(name1,std::ios::in | std:: ios::out);
    }
    //打开存放information的file
    Ifile.open(name2,std::ios::in | std:: ios::out);
    if (!Ifile.good()){
        Ifile.open(name2, std::ios::out);
        Ifile.close();
        Ifile.open(name2);
    }
    Ifile.seekg(0, std::ios::end);
    int Ifile_size = Ifile.tellg();
    if (Ifile_size == 0){
        root = -1;
        num = 0;
        file_tail = 0;
    }
    else{
        Ifile.seekg(0);
        Ifile.read(reinterpret_cast<char*>(&root),sizeof(int));
        Ifile.read(reinterpret_cast<char*>(&num), sizeof(int));
        Ifile.read(reinterpret_cast<char*>(&file_tail), sizeof(int));
    }
}

template<typename keytype, typename valuetype>
BPlusTree<keytype, valuetype>::~BPlusTree(){
    if (Nfile.is_open()) Nfile.close();
    if (Ifile.is_open()){
        Ifile.close();
        Ifile.open(Ifile_name, std::ios::out | std::ios::trunc);
        Ifile.seekp(0);
        Ifile.write(reinterpret_cast<char*>(&root), sizeof(int));
        Ifile.write(reinterpret_cast<char*>(&num), sizeof(int));
        Ifile.write(reinterpret_cast<char*>(&file_tail), sizeof(int));
        Ifile.close();
    }
}

template<typename keytype, typename valuetype>
int BPlusTree<keytype, valuetype>::create(){
    int ori_tail = file_tail;
    file_tail += sizeof(BPlusNode<keytype, valuetype>);
    return ori_tail;
}

template<typename keytype, typename valuetype>
void BPlusTree<keytype, valuetype>::writeNode(BPlusNode<keytype, valuetype> &n, int pos_in){
    n.pos = pos_in;
    if(mycache.mymap.count(pos_in)){
        mycache.mylist.erase(mycache.mymap[n.pos]);
        mycache.mylist.push_front(n);
        mycache.mymap[n.pos] = mycache.mylist.begin();
        return;
    }
    mycache.mylist.push_front(n);
    mycache.mymap[n.pos] = mycache.mylist.begin();
    if(mycache.mymap.size() > 200){
        BPlusNode<keytype, valuetype> the_back = mycache.mylist.back();
        mycache.mymap.erase(mycache.mymap.find(the_back.pos));
        Nfile.seekp(the_back.pos);
        Nfile.write(reinterpret_cast<char*>(&the_back), sizeof(the_back));
        mycache.mylist.pop_back();
    }
    // Nfile.seekp(pos_in);
    // Nfile.write(reinterpret_cast<char*>(&n), sizeof(n));
}

template<typename keytype, typename valuetype>
void BPlusTree<keytype, valuetype>::readNode(BPlusNode<keytype, valuetype> &n, int pos_in){
    if(mycache.mymap.count(pos_in)){
        n = *(mycache.mymap[pos_in]);
        mycache.mylist.erase(mycache.mymap[pos_in]);
        mycache.mylist.push_front(n);
        mycache.mymap[pos_in] = mycache.mylist.begin();
        return;
    }
    Nfile.seekg(pos_in);
    Nfile.read(reinterpret_cast<char*>(&n), sizeof(n));
    mycache.mylist.push_front(n);
    mycache.mymap[pos_in] = mycache.mylist.begin();
    if(mycache.mymap.size() > 200){
        BPlusNode<keytype, valuetype> the_back = mycache.mylist.back();
        mycache.mymap.erase(mycache.mymap.find(the_back.pos));
        Nfile.seekp(the_back.pos);
        Nfile.write(reinterpret_cast<char*>(&the_back), sizeof(the_back));
        mycache.mylist.pop_back();
    }
}

template<typename keytype, typename valuetype>
bool BPlusTree<keytype, valuetype>::Insert(Node<keytype, valuetype> &n){
    if(root == -1){
        BPlusNode<keytype, valuetype> tmp;
        tmp.isleaf = true;
        tmp.key[0] = n;
        tmp.num = 1;
        writeNode(tmp, 0);
        root = 0;
        file_tail = sizeof(tmp);
        num = 1;
        return true;
    }
    BPlusNode<keytype, valuetype> root_in;
    readNode(root_in,root);
    if(root_in.num == MAX_KEY){
        BPlusNode<keytype, valuetype> newroot;
        newroot.num = 0;
        newroot.isleaf = false;
        newroot.Ptr[0] = root;
        spilit(newroot, root_in, 0);
        writeNode(root_in, root);
        root = create();
        writeNode(newroot, root);
    }
    if(insert(root, n)){
        num ++;
        return true;
    } 
    return false;
}

//插入函数，保证 cur的位置的变量为 BPlusNode 类型的
template<typename keytype, typename valuetype>
bool BPlusTree<keytype, valuetype>::insert(int cur, Node<keytype, valuetype> &n){
    BPlusNode<keytype, valuetype> x;
    readNode(x, cur);
    int i = 0;
    for(; i < x.num && x.key[i] < n; i++);
    if(i < x.num && x.isleaf && x.key[i] == n){
        return false;
    }
    //假如现在节点不是底部节点
    if(!x.isleaf){
        BPlusNode<keytype, valuetype> y;
        readNode(y, x.Ptr[i]);
        if(y.num == MAX_KEY){ //假如节点已满，则裂块
            spilit(x, y, i);
            writeNode(x, cur);
            writeNode(y, x.Ptr[i]);
        }
        if(n < x.key[i] || n == x.key[i] || i == x.num){
            bool tmp = insert(x.Ptr[i], n);
            if(!tmp) return false;
        }
        else{
            bool tmp = insert(x.Ptr[i+1], n);
            if(!tmp) return false;
        }
    }
        //假如现在节点是叶节点
    else{
        for(int j = x.num; j > i; --j){
            x.key[j] = x.key[j-1];
            x.Ptr[j] = x.Ptr[j-1];
        }
        x.key[i] = n;
        // for(int w = 0; w < string_size; ++w){
        //     x.key[i].first[w] = n.first[w];
        // }
        // x.key[i].second = n.second;
        x.num++;
        writeNode(x, cur);
    }
    return true;
}

template<typename keytype, typename valuetype>
void BPlusTree<keytype, valuetype>::spilit(BPlusNode<keytype, valuetype> &father, BPlusNode<keytype, valuetype> &current, int index){
    int half = MAX_KEY / 2;
    int i;
    for(i = father.num; i > index; --i){
        father.key[i] = father.key[i-1];
        father.Ptr[i+1] = father.Ptr[i];
    }
    father.num++;
    BPlusNode<keytype, valuetype> t;
    int newnode = create();
    father.key[index] = current.key[half];
    father.Ptr[index+1] = newnode;
    for(i = half + 1; i < MAX_KEY; ++i){
        t.key[i-half-1] = current.key[i];
        t.Ptr[i-half-1] = current.Ptr[i];
    }
    t.num = MAX_KEY - half - 1;
    t.Ptr[t.num] = current.Ptr[MAX_KEY];
    t.isleaf = current.isleaf;
    t.next_pos = current.next_pos;
    t.last_pos = current.pos;
    t.pos = newnode;
    current.next_pos = t.pos;
    current.num = half + 1;
    writeNode(t, newnode);
}

template<typename keytype, typename valuetype>
void BPlusTree<keytype, valuetype>::Remove(Node<keytype, valuetype> &n){
    num--;
    remove(root, n);
    BPlusNode<keytype, valuetype> r;
    readNode(r, root);
    //错误：如果删除key以后根节点不是叶节点且内部所含key为0，则删除根节点
    if(!r.isleaf && r.num == 0){
        root = r.Ptr[0];
    }
}

template<typename keytype, typename valuetype>
void BPlusTree<keytype, valuetype>::remove(int cur, Node<keytype, valuetype> &n){
    int i, j;
    BPlusNode<keytype, valuetype> x;
    readNode(x, cur);
    for(i = 0; i < x.num && x.key[i] < n; ++i);
    //找到第一个大于等于 n 的位置
    if(i < x.num && x.key[i] == n){//找到了key
        if(!x.isleaf){    //x为内节点
            BPlusNode<keytype, valuetype> y;
            readNode(y, x.Ptr[i]);
            if(y.isleaf){ //x的孩子y为叶节点
                if(y.num > MAX_KEY / 2){
                    case1(x, y, i, cur);
                    return;
                }
                else{
                    if(i > 0){ //有左兄弟节点
                        BPlusNode<keytype, valuetype> lchild;
                        readNode(lchild, x.Ptr[i-1]);
                        if(lchild.num > MAX_KEY / 2)
                            case2(x, y, lchild, i, j, cur);
                        else case3(x, y, lchild, i, j, cur);
                    }
                    else{      //没有左兄弟节点
                        BPlusNode<keytype, valuetype> rchild;
                        readNode(rchild, x.Ptr[i+1]);
                        if(rchild.num > MAX_KEY / 2)
                            case4(x, y, rchild, i, j, cur);
                        else case5(x, y, rchild, i, j, cur);
                    }
                }
            }
            else{         //x的孩子y为内节点
                //找到key的节点的左兄弟，将左兄弟取代它原来的位置
                //先创建一个n1，因为后续操作会涉及node的修改
                int p1;
                Node<keytype, valuetype> n1;
                n1 = n;
                // for(int w = 0; w < string_size; ++w){
                //     n1.first[w] = n.first[w];
                // }
                // n1.second = n.second;
                Mynode<keytype, valuetype> result;
                if(tmp.node == n){
                    result = tmp;
                }else{
                    search(n1, result, p1);
                }
                BPlusNode<keytype, valuetype> last;
                readNode(last, result.fpos);
                x.key[i] = last.key[last.num - 2];
                tmp = result;
                writeNode(x, cur);
                if(y.num > MAX_KEY / 2){}
                else{ //key不过半，将兄弟节点的某一个key移到孩子
                    if(i > 0){ //x.key[i]有左兄弟
                        BPlusNode<keytype, valuetype> lchild;
                        readNode(lchild, x.Ptr[i-1]);
                        if(lchild.num > MAX_KEY / 2){
                            for(j = y.num; j > 0; j--){
                                y.key[j] = y.key[j-1];
                                y.Ptr[j+1] = y.Ptr[j];
                            }
                            y.Ptr[1] = y.Ptr[0];
                            y.key[0] = x.key[i-1];
                            y.Ptr[0] = lchild.Ptr[lchild.num - 1];
                            y.num++;
                            x.key[i-1] = lchild.key[lchild.num-2];
                            lchild.num--;
                            writeNode(x, cur);
                            writeNode(lchild, x.Ptr[i-1]);
                            writeNode(y, x.Ptr[i]);
                        }
                        else{
                            for(j = 0; j < y.num; ++j){
                                lchild.key[lchild.num + j] = y.key[j];
                                lchild.Ptr[lchild.num + j] = y.Ptr[j];
                            }
                            lchild.Ptr[lchild.num + j] = y.Ptr[j];
                            lchild.num += y.num;
                            lchild.next_pos = y.next_pos;
                            for(j = i - 1; j < x.num - 1; ++j){
                                x.key[j] = x.key[j + 1];
                                x.Ptr[j + 1] = x.Ptr[j + 2];
                            }
                            x.num--;
                            writeNode(x, cur);
                            writeNode(lchild, x.Ptr[i - 1]);
                            i--;
                        }
                    }
                    else{      //x.key[i]只有右兄弟
                        BPlusNode<keytype, valuetype> rchild;
                        readNode(rchild, x.Ptr[i + 1]);
                        if(rchild.num > MAX_KEY / 2){
                            y.key[y.num] = rchild.key[0];
                            y.num++;
                            y.Ptr[y.num] = rchild.Ptr[1];
                            x.key[i] = rchild.key[0];
                            for(j = 0; j < rchild.num - 1; j++){
                                rchild.key[j] = rchild.key[j + 1];
                                rchild.Ptr[j] = rchild.Ptr[j + 1];
                            }
                            rchild.Ptr[j] = rchild.Ptr[j + 1];
                            rchild.num--;
                            writeNode(x, cur);
                            writeNode(y, x.Ptr[i]);
                            writeNode(rchild, x.Ptr[i + 1]);
                        }
                        else{
                            for(j = 0; j < rchild.num; ++j){
                                y.key[y.num + j] = rchild.key[j];
                                y.Ptr[y.num + j] = rchild.Ptr[j];
                            }
                            y.Ptr[y.num + j] = rchild.Ptr[j];
                            y.num += rchild.num;
                            y.next_pos = rchild.next_pos;
                            for(j = i; j < x.num - 1; j++){
                                x.key[j] = x.key[j + 1];
                                x.Ptr[j + 1] = x.Ptr[j + 2];
                            }
                            x.num--;
                            writeNode(x, cur);
                            writeNode(y, x.Ptr[i]);
                        }
                    }
                }
            }
            remove(x.Ptr[i], n);
        }
        else{
            for(j = i; j < x.num - 1; ++j){
                x.key[j] = x.key[j + 1];
                x.Ptr[j] = x.Ptr[j + 1];
            }
            x.num--;
            writeNode(x, cur);
            return;
        }
    }
    else{                 //在当前节点没找到key
        if(!x.isleaf){    //没找到key，则key在Ptr[i]指向的节点中
            BPlusNode<keytype, valuetype> y;
            readNode(y, x.Ptr[i]);
            if(!y.isleaf){//孩子节点为内节点
                if(y.num > MAX_KEY / 2) {}
                else{     //孩子节点的key数量不够，将兄弟节点的key移到孩子
                    if(i > 0){ //x.key[i]有左兄弟
                        BPlusNode<keytype, valuetype> lchild;
                        readNode(lchild, x.Ptr[i-1]);
                        if(lchild.num > MAX_KEY / 2){
                            for(j = y.num; j > 0; j--){
                                y.key[j] = y.key[j-1];
                                y.Ptr[j+1] = y.Ptr[j];
                            }
                            y.Ptr[1] = y.Ptr[0];
                            y.key[0] = x.key[i-1];
                            y.Ptr[0] = lchild.Ptr[lchild.num - 1];
                            y.num++;
                            x.key[i-1] = lchild.key[lchild.num - 2];
                            lchild.num--;
                            writeNode(x, cur);
                            writeNode(lchild, x.Ptr[i-1]);
                            writeNode(y, x.Ptr[i]);
                        }
                        else{
                            for(j = 0; j < y.num; ++j){
                                lchild.key[lchild.num + j] = y.key[j];
                                lchild.Ptr[lchild.num + j] = y.Ptr[j];
                            }
                            lchild.Ptr[lchild.num + j] = y.Ptr[j];
                            lchild.num += y.num;
                            lchild.next_pos = y.next_pos;
                            for(j = i - 1; j < x.num - 1; j++){
                                x.key[j] = x.key[j + 1];
                                x.Ptr[j + 1] = x.Ptr[j + 2];
                            }
                            x.num--;
                            writeNode(x, cur);
                            writeNode(lchild, x.Ptr[i - 1]);
                            i--;
                        }
                    }
                    else{      //x.key[i]只有右兄弟
                        BPlusNode<keytype, valuetype> rchild;
                        readNode(rchild, x.Ptr[i + 1]);
                        if(rchild.num > MAX_KEY / 2){
                            y.key[y.num] = rchild.key[0];
                            y.num++;
                            y.Ptr[y.num] = rchild.Ptr[1];
                            x.key[i] = rchild.key[0];
                            for(j = 0; j < rchild.num - 1; ++j){
                                rchild.key[j] = rchild.key[j + 1];
                                rchild.Ptr[j] = rchild.Ptr[j + 1];
                            }
                            rchild.Ptr[j] = rchild.Ptr[j + 1];
                            rchild.num--;
                            writeNode(x, cur);
                            writeNode(y, x.Ptr[i]);
                            writeNode(rchild, x.Ptr[i + 1]);
                        }
                        else{
                            for(j = 0; j < rchild.num; ++j){
                                y.key[y.num + j] = rchild.key[j];
                                y.Ptr[y.num + j] = rchild.Ptr[j];
                            }
                            y.Ptr[y.num + j] = rchild.Ptr[j];
                            y.num += rchild.num;
                            y.next_pos = rchild.next_pos;
                            for(j = i; j < x.num - 1; ++j){
                                x.key[j] = x.key[j + 1];
                                x.Ptr[j + 1] = x.Ptr[j + 2];
                            }
                            x.num--;
                            writeNode(x, cur);
                            writeNode(y, x.Ptr[i]);
                        }
                    }
                }
            }
            else{         //孩子节点为叶节点
                if(y.num > MAX_KEY / 2){}
                else{
                    if(i > 0){ //有左兄弟
                        BPlusNode<keytype, valuetype> lchild;
                        readNode(lchild, x.Ptr[i - 1]);
                        if(lchild.num > MAX_KEY / 2){
                            for(j = y.num; j > 0; --j){
                                y.key[j] = y.key[j - 1];
                                y.Ptr[j] = y.Ptr[j - 1];
                            }
                            y.key[0] = x.key[i - 1];
                            y.Ptr[0] = lchild.Ptr[lchild.num - 1];
                            y.num++;
                            lchild.num--;
                            x.key[i - 1] = lchild.key[lchild.num - 1];
                            writeNode(lchild, x.Ptr[i - 1]);
                            writeNode(y, x.Ptr[i]);
                            writeNode(x, cur);
                        }
                        else{
                            for(j = 0; j < y.num; ++j){
                                lchild.key[lchild.num + j] = y.key[j];
                                lchild.Ptr[lchild.num + j + 1] = y.Ptr[j + 1];
                            }
                            lchild.num += y.num;
                            lchild.Ptr[MAX_KEY] = y.Ptr[MAX_KEY];
                            lchild.next_pos = y.next_pos;
                            for(j = i - 1; j < x.num - 1; ++j){
                                x.key[j] = x.key[j + 1];
                                x.Ptr[j + 1] = x.Ptr[j + 2];
                            }
                            x.num--;
                            writeNode(lchild, x.Ptr[i - 1]);
                            writeNode(x, cur);
                            i--;
                        }
                    }
                    else{      //只有右兄弟
                        BPlusNode<keytype, valuetype> rchild;
                        readNode(rchild, x.Ptr[i + 1]);
                        if(rchild.num > MAX_KEY / 2){
                            x.key[i] = rchild.key[0];
                            y.key[y.num] = rchild.key[0];
                            y.Ptr[y.num] = rchild.Ptr[0];
                            y.num++;
                            y.Ptr[y.num] = rchild.Ptr[1];
                            for(j = 0; j < rchild.num - 1; j++){
                                rchild.key[j] = rchild.key[j + 1];
                                rchild.Ptr[j] = rchild.Ptr[j + 1];
                            }
                            rchild.Ptr[j] = rchild.Ptr[j + 1];
                            rchild.num--;
                            writeNode(x, cur);
                            writeNode(rchild, x.Ptr[i + 1]);
                            writeNode(y, x.Ptr[i]);
                        }
                        else{
                            for(j = 0; j < rchild.num; j++){
                                y.key[y.num + j] = rchild.key[j];
                                y.Ptr[y.num + j + 1] = rchild.Ptr[j + 1];
                            }
                            y.num += rchild.num;
                            y.Ptr[MAX_KEY] = rchild.Ptr[MAX_KEY];
                            y.next_pos = rchild.next_pos;
                            for(j = i; j < x.num - 1; ++j){
                                x.key[j] = x.key[j + 1];
                                x.Ptr[j + 1] = x.Ptr[j + 2];
                            }
                            x.num--;
                            writeNode(x, cur);
                            writeNode(y, x.Ptr[i]);
                        }
                    }
                }
            }
            remove(x.Ptr[i], n);
        }
        else{
            num ++;
            //不可能
        }
    }
}

template<typename keytype, typename valuetype>
void BPlusTree<keytype, valuetype>::search(Node<keytype, valuetype> &n, Mynode<keytype, valuetype> &result, int &p){
    BPlusNode<keytype, valuetype> x;
    int current = root;
    int i;
    do{
        readNode(x, current);
        for(i = 0; i < x.num && x.key[i] < n; ++i);
        p = i;
        result.fpos = current;
        if(i < x.num && x.isleaf && x.key[i] == n){
            result.node.pos = x.Ptr[p];
            result.exist = true;
            result.node = n;
            return;
        }
        current = x.Ptr[i];
    }while(!x.isleaf);
    result.exist = false;
}

template<typename keytype, typename valuetype>
void BPlusTree<keytype, valuetype>::case1(BPlusNode<keytype, valuetype> &x, BPlusNode<keytype, valuetype> &y, int &i, int &cur){
    x.key[i] = y.key[y.num - 2];
    y.num--;
    writeNode(x, cur);
    writeNode(y, x.Ptr[i]);
}

template<typename keytype, typename valuetype>
void BPlusTree<keytype, valuetype>::case2(BPlusNode<keytype, valuetype> &x, BPlusNode<keytype, valuetype> &y, BPlusNode<keytype, valuetype> &lchild, int &i, int &j, int &cur){
    for(j = y.num; j > 0; j--){
        y.key[j] = y.key[j-1];
        y.Ptr[j] = y.Ptr[j-1];
    }
    y.key[0] = x.key[i-1];
    y.Ptr[0] = lchild.Ptr[lchild.num-1];
    y.num++;
    lchild.num--;
    x.key[i-1] = lchild.key[lchild.num-1];
    x.key[i] = y.key[y.num-2];
    writeNode(x, cur);
    writeNode(lchild, x.Ptr[i-1]);
    writeNode(y, x.Ptr[i]);
}

template<typename keytype, typename valuetype>
void BPlusTree<keytype, valuetype>::case3(BPlusNode<keytype, valuetype> &x, BPlusNode<keytype, valuetype> &y, BPlusNode<keytype, valuetype> &lchild, int &i, int &j, int &cur){
    for(j = 0; j < y.num; ++j){
        lchild.key[lchild.num + j] = y.key[j];
        lchild.Ptr[lchild.num + j] = y.Ptr[j];
    }
    lchild.num += y.num;
    lchild.Ptr[MAX_KEY] = y.Ptr[MAX_KEY];
    for(j = i-1; j < x.num-1; j++){
        x.key[j] = x.key[j+1];
        x.Ptr[j+1] = x.Ptr[j+2];
    }
    x.num--;
    x.key[i-1] = lchild.key[lchild.num-2];
    lchild.next_pos = y.next_pos;
    writeNode(x, cur);
    writeNode(lchild, x.Ptr[i-1]);
    i--;
}

template<typename keytype, typename valuetype>
void BPlusTree<keytype, valuetype>::case4(BPlusNode<keytype, valuetype> &x, BPlusNode<keytype, valuetype> &y, BPlusNode<keytype, valuetype> &rchild, int &i, int &j, int &cur){
    x.key[i] = rchild.key[0];
    y.key[y.num] = rchild.key[0];
    y.Ptr[y.num] = rchild.Ptr[0];
    y.num++;
    for(j = 0; j < rchild.num-1; ++j){
        rchild.key[j] = rchild.key[j+1];
        rchild.Ptr[j] = rchild.Ptr[j+1];
    }
    rchild.Ptr[rchild.num - 1] = rchild.Ptr[rchild.num];
    rchild.num--;
    writeNode(x, cur);
    writeNode(y, x.Ptr[i]);
    writeNode(rchild, x.Ptr[i+1]);
}

template<typename keytype, typename valuetype>
void BPlusTree<keytype, valuetype>::case5(BPlusNode<keytype, valuetype> &x, BPlusNode<keytype, valuetype> &y, BPlusNode<keytype, valuetype> &rchild, int &i, int &j, int &cur){
    for(j = 0; j < rchild.num; ++j){
        y.key[y.num+j] = rchild.key[j];
        y.Ptr[y.num+j] = rchild.Ptr[j];
    }
    y.num += rchild.num;
    y.Ptr[MAX_KEY] = rchild.Ptr[MAX_KEY];
    y.Ptr[y.num] = rchild.Ptr[rchild.num];
    for( j = i; j < x.num - 1; j ++)  {
        x.key[j] = x.key[j+1];
        x.Ptr[j+1] = x.Ptr[j+2];
    }
    x.num --;
    y.next_pos = rchild.next_pos;
    writeNode(x, cur);
    writeNode(y, x.Ptr[i]);
}

//找到第一个大于等于该 index 的节点，则前一个节点小于 index
template<typename keytype, typename valuetype>
int BPlusTree<keytype, valuetype>::myfind(const Node<keytype, valuetype>* key, int num, const keytype & s){
    // std::string tmp(key[num - 1].first);
    if(key[num - 1].first < s) return num;  //错误：如果key 大于数组中的所有元素
    int l = 0, r = num - 1;
    while(l < r){
        int mid = (l + r) >> 1;
        // std::string str(key[mid].first);
        if(key[mid].first > s || key[mid].first == s) r = mid;
        else l = mid + 1;
    }
    return r;
}

template<typename keytype, typename valuetype>
int BPlusTree<keytype, valuetype>::myfind_node(const Node<keytype, valuetype>* key, int num, const Node<keytype, valuetype> & s){
    if(key[num - 1] < s) return num;  //错误：如果key 大于数组中的所有元素
    int l = 0, r = num - 1;
    while(l < r){
        int mid = (l + r) >> 1;
        if(key[mid] > s || key[mid] == s) r = mid;
        else l = mid + 1;
    }
    return r;
}

//返回值为BPlusNode以及寻找的值所在的key的index
template<typename keytype, typename valuetype>
std::pair<BPlusNode<keytype, valuetype>, int> BPlusTree<keytype, valuetype>::find_node(int p, const keytype & s){
    BPlusNode<keytype, valuetype> n;
    readNode(n, p);
    int pos = myfind(n.key, n.num, s);
    if(n.isleaf) return {n, pos};
    else return find_node(n.Ptr[pos], s);
}

template<typename keytype, typename valuetype>
std::vector<valuetype> BPlusTree<keytype, valuetype>::final_find(const keytype & s){
    std::vector<valuetype> ans;
    ans.clear();
    if(root == -1) return ans;
    std::pair<BPlusNode<keytype, valuetype>, int> mypair;
    mypair = find_node(root, s);
    BPlusNode<keytype, valuetype> n = mypair.first;
    if(n.key[mypair.second].first != s) return ans;
    int p = mypair.second;
    while(true){
        if(p < n.num){
            if(s != n.key[p].first) break;
            ans.push_back(n.key[p].second);
            ++p;
        }
        else{
            if(n.next_pos == -1) return ans;
            readNode(n, n.next_pos);
            p = 0;
        }
    }
    return ans;
}

template<typename keytype, typename valuetype>
int BPlusTree<keytype, valuetype>::modify_find(Node<keytype, valuetype>* key, int num, Node<keytype, valuetype>& origin_node, Node<keytype, valuetype>& tran_node){
    if(key[num - 1] < origin_node) return num;  //错误：如果 key 大于数组中的所有元素
    int l = 0, r = num - 1;
    while(l < r){
        int mid = (l + r) >> 1;
        if(key[mid] >= origin_node) r = mid;
        else l = mid + 1;
    }
    if(key[r] == origin_node){
        key[r] = tran_node;
    }
    return r;
}

template<typename keytype, typename valuetype>
void BPlusTree<keytype, valuetype>::modify_node(Node<keytype, valuetype>& origin_node, Node<keytype, valuetype>& tran_node, int p){
    BPlusNode<keytype, valuetype> n;
    readNode(n, p);
    int pos = myfind_node(n.key, n.num, origin_node);
    if(pos != n.num && n.key[pos] == origin_node){
        n.key[pos] = tran_node;
        writeNode(n, n.pos);
    }
    // writeNode(n, n.pos);
    if(n.isleaf) return;
    else modify_node(origin_node, tran_node, n.Ptr[pos]);
}

template<typename keytype, typename valuetype>
void BPlusTree<keytype, valuetype>::modify(Node<keytype, valuetype>& origin_node, Node<keytype, valuetype>& tran_node){
    modify_node(origin_node, tran_node, root);
}