/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <cstdarg>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"
#include <cassert>
#include <iostream>

namespace sjtu {

    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    >
    class map {
    public:
        
        typedef pair<const Key, T> value_type;

        struct Node {
            value_type *data;
            Node *parent;
            Node *left;
            Node *right;
            int balance;

            Node(const value_type &data_in) {
                data = new value_type(data_in.first, data_in.second);
                left = nullptr;
                right = nullptr;
                parent = nullptr;
                balance = 0;
            }

            Node() : data(nullptr) {
                left = nullptr;
                right = nullptr;
                parent = nullptr;
                balance = 0;
            }

            Node(Node *other) {
                value_type add_data(other->data->first, other->data->second);
                data = new value_type(add_data);
                left = nullptr;
                right = nullptr;
                parent = nullptr;
                balance = 0;
            }

            bool operator==(const Node &other) {
                if (data->first == other.data->first && data->second == other.data->second) {
                    return true;
                } else return false;
            }
        };

    public:
        Node *root;
        int num;

        class const_iterator;

        class iterator {
        public:
            Node *node;
            const map *ptr;
        public:
            iterator() : node(nullptr) {}

            iterator(const iterator &other) : node(other.node), ptr(other.ptr) {}

            iterator(Node *node_in, const map *m) : node(node_in), ptr(m) {}

            iterator operator++(int) {
                iterator old = iterator(*this);
                if (node == nullptr) {
                    throw invalid_iterator();
                }
                if (node->right != nullptr) {
                    node = node->right;
                    while (node->left != nullptr) {
                        node = node->left;
                    }
                } else {
                    Node *cur_parent = node->parent;
                    while (cur_parent != nullptr && node == cur_parent->right) {
                        node = cur_parent;
                        cur_parent = cur_parent->parent;
                    }
                    node = cur_parent;
                }
                return old;
            }

            iterator &operator++() {
                if (node == nullptr) {
                    throw invalid_iterator();
                }
                if (node->right != nullptr) {
                    node = node->right;
                    while (node->left != nullptr) {
                        node = node->left;
                    }
                } else {
                    Node *cur_parent = node->parent;
                    while (cur_parent != nullptr && node == cur_parent->right) {
                        node = cur_parent;
                        cur_parent = cur_parent->parent;
                    }
                    node = cur_parent;
                }
                return *this;
            }

            iterator operator--(int) {
                Node *tmp1 = ptr->root;
                if (tmp1 == nullptr) throw invalid_iterator();
                while (tmp1->left != nullptr) {
                    tmp1 = tmp1->left;
                }
                if(tmp1 == node) throw invalid_iterator();
			
                iterator old = iterator(*this);
                if (node == nullptr) {
                    Node *tmp = ptr->root;
                    while (tmp->right != nullptr) {
                        tmp = tmp->right;
                    }
                    node = tmp;
                    return *this;
                }
                if (node->left != nullptr) {
                    node = node->left;
                    while (node->right != nullptr) {
                        node = node->right;
                    }
                } else {
                    Node *cur_parent = node->parent;
                    while (cur_parent != nullptr && node == cur_parent->left) {
                        node = cur_parent;
                        cur_parent = cur_parent->parent;
                    }
                    node = cur_parent;
                }
                return old;
            }

            iterator &operator--() {
                Node *tmp1 = ptr->root;
                if (tmp1 == nullptr) throw invalid_iterator();
                while (tmp1->left != nullptr) {
                    tmp1 = tmp1->left;
                }
                if(tmp1 == node) throw invalid_iterator();
				
                if (node == nullptr) {
                    Node *tmp = ptr->root;
                    while (tmp->right != nullptr) {
                        tmp = tmp->right;
                    }
                    node = tmp;
                    return *this;
                }
                if (node->left != nullptr) {
                    node = node->left;
                    while (node->right != nullptr) {
                        node = node->right;
                    }
                } else {
                    Node *cur_parent = node->parent;
                    while (cur_parent != nullptr && node == cur_parent->left) {
                        node = cur_parent;
                        cur_parent = cur_parent->parent;
                    }
                    node = cur_parent;
                }
                return *this;
            }

            value_type &operator*() const {
                return *(node->data);
            }

            bool operator==(const iterator &rhs) const {
                return ((node == rhs.node) && (ptr == rhs.ptr));
            }

            bool operator==(const const_iterator &rhs) const {
                return ((node == rhs.node) && (ptr == rhs.ptr));
            }

            bool operator!=(const iterator &rhs) const {
                return !(*this == rhs);
            }

            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }

            value_type *operator->() const noexcept {
                return node->data;
            }
        };

        class const_iterator {
        public:
            Node *node;
            const map *ptr;
        public:
            const_iterator() : node(nullptr) {}

            const_iterator(const const_iterator &other) : node(other.node), ptr(other.ptr) {}

            const_iterator(const iterator &other) : node(other.node), ptr(other.ptr) {}

            const_iterator(Node *node_in, const map *m) : node(node_in), ptr(m) {}

            const_iterator operator++(int) {
                const_iterator old = const_iterator(*this);
                if (node == nullptr) {
                    throw invalid_iterator();
                }
                if (node->right != nullptr) {
                    node = node->right;
                    while (node->left != nullptr) {
                        node = node->left;
                    }
                } else {
                    Node *cur_parent = node->parent;
                    while (cur_parent != nullptr && node == cur_parent->right) {
                        node = cur_parent;
                        cur_parent = cur_parent->parent;
                    }
                    node = cur_parent;
                }
                return old;
            }

            const_iterator &operator++() {
                if (node == nullptr) {
                    throw invalid_iterator();
                }
                if (node->right != nullptr) {
                    node = node->right;
                    while (node->left != nullptr) {
                        node = node->left;
                    }
                } else {
                    Node *cur_parent = node->parent;
                    while (cur_parent != nullptr && node == cur_parent->right) {
                        node = cur_parent;
                        cur_parent = cur_parent->parent;
                    }
                    node = cur_parent;
                }
                return *this;
            }

            const_iterator operator--(int) {
                Node *tmp1 = ptr->root;
                if (tmp1 == nullptr) throw invalid_iterator();
                while (tmp1->left != nullptr) {
                    tmp1 = tmp1->left;
                }
                if(tmp1 == node) throw invalid_iterator();
				
                const_iterator old = const_iterator(*this);
                if (node == nullptr) {
                    Node *tmp = ptr->root;
                    while (tmp->right != nullptr) {
                        tmp = tmp->right;
                    }
                    node = tmp;
                    return *this;
                }
                if (node->left != nullptr) {
                    node = node->left;
                    while (node->right != nullptr) {
                        node = node->right;
                    }
                } else {
                    Node *cur_parent = node->parent;
                    while (cur_parent != nullptr && node == cur_parent->left) {
                        node = cur_parent;
                        cur_parent = cur_parent->parent;
                    }
                    node = cur_parent;
                }
                return old;
            }

            const_iterator &operator--() {
                Node *tmp1 = ptr->root;
                if (tmp1 == nullptr) throw invalid_iterator();
                while (tmp1->left != nullptr) {
                    tmp1 = tmp1->left;
                }
                if(tmp1 == node) throw invalid_iterator();
				
                if (node == nullptr) {
                    Node *tmp = ptr->root;
                    while (tmp->right != nullptr) {
                        tmp = tmp->right;
                    }
                    node = tmp;
                    return *this;
                }
                if (node->left != nullptr) {
                    node = node->left;
                    while (node->right != nullptr) {
                        node = node->right;
                    }
                } else {
                    Node *cur_parent = node->parent;
                    while (cur_parent != nullptr && node == cur_parent->left) {
                        node = cur_parent;
                        cur_parent = cur_parent->parent;
                    }
                    node = cur_parent;
                }
                return *this;
            }

            const value_type &operator*() const {
                return *(node->data);
            }

            bool operator==(const iterator &rhs) const {
                return ((node == rhs.node) && (ptr == rhs.ptr));
            }

            bool operator==(const const_iterator &rhs) const {
                return ((node == rhs.node) && (ptr == rhs.ptr));
            }

            bool operator!=(const iterator &rhs) const {
                return !(*this == rhs);
            }

            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }

            value_type *operator->() const noexcept {
                return (node->data);
            }
        };

    public:
        Node *copyTree(Node *n) {
            if (n == nullptr) return nullptr;
            Node *newNode = new Node(n);
            newNode->balance = n->balance;
            newNode->left = copyTree(n->left);
            newNode->right = copyTree(n->right);
            if (newNode->left != nullptr) newNode->left->parent = newNode;
            if (newNode->right != nullptr) newNode->right->parent = newNode;
            return newNode;
        }

        map() : root(nullptr), num(0) {}

        map(const map &other) {
            root = copyTree(other.root);
            num = other.num;
        }

        map &operator=(const map &other) {
            if (this == &other) {
                return *this;
            }
            clear();
            root = copyTree(other.root);
            num = other.num;
            return *this;
        }

        ~map() {
            clear();
        }

        T &at(const Key &key) {
            Node *tmp = find(key, root);
            if (tmp == nullptr) {
                throw index_out_of_bound();
            }
            return tmp->data->second;
        }

        const T &at(const Key &key) const {
            Node *tmp = find(key, root);
            if (tmp == nullptr) {
                throw index_out_of_bound();
            }
            return tmp->data->second;
        }

        T &operator[](const Key &key) {
            Node *t = find(key, root);
            if (t == nullptr) {
                value_type *tmp = new value_type(key, T());
                insert(*tmp);
                delete tmp;
                Node *n = find(key, root);
                return n->data->second;
            } else {
                return t->data->second;
            }
        }

        const T &operator[](const Key &key) const {
            Node *t = find(key, root);
            if (t == nullptr) {
                throw index_out_of_bound();
            } else {
                return t->data->second;
            }
        }

        iterator begin() {
            Node *tmp = root;
            if (tmp == nullptr) return iterator(nullptr, this);
            while (tmp->left != nullptr) {
                tmp = tmp->left;
            }
            iterator ans(tmp, this);
            return ans;
        }

        const_iterator cbegin() const {
            Node *tmp = root;
            if (tmp == nullptr) return const_iterator(nullptr, this);
            while (tmp->left != nullptr) {
                tmp = tmp->left;
            }
            const_iterator ans(tmp, this);
            return ans;
        }

        iterator end() {
            return iterator(nullptr, this);
        }

        const_iterator cend() const {
            return const_iterator(nullptr, this);
        }

        bool empty() const {
            return (num == 0);
        }

        size_t size() const {
            return size_t(num);
        }

        void clear(Node *&t) {
            if (t == nullptr) return;
            clear(t->left);
            clear(t->right);
            delete t->data;
            t->data = nullptr;
            delete t;
            t = nullptr;
            --num;
            t = nullptr;
        }

        void clear() {
            clear(root);
        }

        void rotateLL(Node *p) {
            Node *cur = p->left;
            Node *curR = cur->right;
            Node *pp = p->parent;

            //修改节点的链接关系
            p->left = curR;
            if (curR) curR->parent = p; //错误：curR可能为nullptr
            cur->right = p;
            p->parent = cur;
            //cur与pp的链接
            if (pp == nullptr) {//cur为根节点
                root = cur;
                cur->parent = nullptr;
            } else {//cur为普通节点
                cur->parent = pp;
                if (p == pp->left) pp->left = cur;
                else pp->right = cur;
            }

            //更新平衡因子
            p->balance = 0;
            cur->balance = 0;
        }

        void rotateRR(Node *p) {
            Node *cur = p->right;
            Node *curL = cur->left;
            Node *pp = p->parent;

            p->right = curL;
            if (curL) curL->parent = p;
            cur->left = p;
            p->parent = cur;

            if (pp == nullptr) {
                root = cur;
                cur->parent = nullptr;
            } else {
                cur->parent = pp;
                if (p == pp->left) pp->left = cur;
                else pp->right = cur;
            }

            p->balance = 0;
            cur->balance = 0;
        }

        void rotateLR(Node *p) {
            Node *cur = p->left;
            Node *curR = cur->right;
            int balance_ = curR->balance;

            rotateRR(p->left);
            rotateLL(p);

            //处理平衡因子
            if (balance_ == 1) {
                curR->balance = 0;
                cur->balance = -1;
                p->balance = 0;
            } else if (balance_ == -1) {
                curR->balance = 0;
                cur->balance = 0;
                p->balance = 1;
            }
        }

        void rotateRL(Node *p) {
            Node *cur = p->right;
            Node *curL = cur->left;
            int balance_ = curL->balance;

            rotateLL(p->right);
            rotateRR(p);

            //处理平衡因子
            if (balance_ == -1) {
                curL->balance = 0;
                cur->balance = 1;
                p->balance = 0;
            } else if (balance_ == 1) {
                curL->balance = 0;
                cur->balance = 0;
                p->balance = -1;
            }
        }

        pair<iterator, bool> insert(const value_type &value) {
            pair<iterator, bool> ans;
            if (root == nullptr) {
                root = new Node(value);
                ans.first = iterator(root, this);
                ans.second = true;
                num++;
                return ans;
            }

            //先找插入的位置，p为父亲节点
            Node *p = nullptr;
            Node *cur = root;
            while (cur) {
                if (Compare()(value.first, cur->data->first)) {
                    p = cur;
                    cur = cur->left;
                } else if (Compare()(cur->data->first, value.first)) {
                    p = cur;
                    cur = cur->right;
                } else {
                    ans.first = iterator(cur, this);
                    ans.second = false;
                    return ans;
                }
            }

            //插入元素，分为插入到p的左边和右边两种情况
            cur = new Node(value);
            Node* ans1 = cur;
            if (Compare()(value.first, p->data->first)) {
                p->left = cur;
                cur->parent = p;
            } else {
                p->right = cur;
                cur->parent = p;
            }

            //更新平衡因子
            while (p) {

                //先更新p的平衡因子
                if (p->left == cur) p->balance--;
                if (p->right == cur) p->balance++;

                //判断p的平衡因子，决定是否要向上找，是否要旋转
                if (p->balance == 0) {
                    break;
                } else if (p->balance == 1 || p->balance == -1) {
                    cur = p;
                    p = p->parent;
                } else if (p->balance == -2) {
                    if (cur->balance == -1) rotateLL(p);//balance为-1
                    else rotateLR(p);                  //balance为 1
                    break;
                } else if (p->balance == 2) {
                    if (cur->balance == 1) rotateRR(p);//balance为-1
                    else rotateRL(p);                 //balance为1
                    break;
                }
            }
            ans.first = iterator(ans1, this);
            ans.second = true;
            num++;
            // check(root);
            return ans;
        }

        int check(Node *t) {
            // if (t == nullptr) return 0;
            // int l = check(t->left), r = check(t->right);
            // if (l == -1 || r == -1) {
            //     return -1;
            // }
            // if (t->balance != r - l) {
            //     return -1;
            // }
            // return std::max(l, r) + 1;
            return 1;
// //        std::cout<<t->data->second<<" ";
//     //    std::cout<<t-><<" ";

        }
        void myswap(Node*& n1,Node*& n2){
            Node* tmp = new Node();
            tmp->left = n2->left;
            tmp->right = n2->right;
            tmp->parent = n2->parent;
            tmp->balance = n2->balance;
            n2->left = n1->left;
            n2->right = n1->right;
            n2->parent = n1->parent;
            n2->balance = n1->balance;
            n1->left = tmp->left;
            n1->right = tmp->right;
            n1->parent = tmp->parent;
            n1->balance = tmp->balance;
            delete tmp;
        }

        bool remove(const value_type &x) {
            Node *p = nullptr;
            Node *cur = root;
            Node *delp = nullptr;
            Node *delc = nullptr;

            //先找到该删除的节点，然后部分处理，找到真正要删除的节点
            while (cur) {
                if (Compare()(x.first, cur->data->first)) {
                    p = cur;
                    cur = cur->left;
                } else if (Compare()(cur->data->first, x.first)) {
                    p = cur;
                    cur = cur->right;
                } else {//现在找到了待删除的节点
                    if (cur->left == nullptr) {
                        if (cur == root) {
                            root = root->right;
                            if (root) root->parent = nullptr;
                            delete cur->data;
                            cur->data = nullptr;
                            delete cur;
                            cur = nullptr;
                            return true;
                        } else {
                            delp = p;
                            delc = cur;
                        }
                        break;
                    } else if (cur->right == nullptr) {
                        if (cur == root) {
                            root = root->left;
                            if (root) root->parent = nullptr;
                            delete cur->data;
                            cur->data = nullptr;
                            delete cur;
                            cur = nullptr;
                            return true;
                        } else {
                            delp = p;
                            delc = cur;
                        }
                        break;
                    } else {
                        Node *subp = cur;
                        Node *subc = cur->right;
                        while (subc->left) {
                            subp = subc;
                            subc = subc->left;
                        }
                        if(cur == root){
                            root = subc;
                        }
                        if(subc == cur->right){
                            Node* origin_right = subc->right;
                            Node* origin_left = subc->left;
                            subc->parent = cur->parent;
                            if(subc->parent != nullptr){
                                if(subc->parent->right == cur) subc->parent->right = subc;
                                else subc->parent->left = subc;
                            }
                            subc->right = cur;
                            subc->left = cur->left;
                            if(subc->left != nullptr) subc->left->parent = subc;
                            cur->parent = subc;
                            cur->left = origin_left;
                            cur->right = origin_right;
                            if(cur->right != nullptr) cur->right->parent = cur;
                            if(cur->left != nullptr) cur->left->parent = cur;
                            int subc_balance = subc->balance;
                            subc->balance = cur->balance;
                            cur->balance = subc_balance;
                            delete cur->data;
                            cur->data = nullptr;
                            cur->data = new value_type(subc->data->first,subc->data->second);
                            delp = subc;
                            delc = cur;
                            break;
                        }
                        else{
                            myswap(cur,subc);
                            if(cur->parent != nullptr){
                                if(cur->parent->right == subc) cur->parent->right = cur;
                                else cur->parent->left = cur;
                            }
                            if(cur->left != nullptr) cur->left->parent = cur;
                            if(cur->right != nullptr) cur->right->parent = cur;
                            if(subc->parent != nullptr){
                                if(subc->parent->right == cur) subc->parent->right = subc;
                                else subc->parent->left = subc;
                            }
                            if(subc->left != nullptr) subc->left->parent = subc;
                            if(subc->right != nullptr) subc->right->parent = subc;
                            delete cur->data;
                            cur->data = nullptr;
                            cur->data = new value_type(subc->data->first,subc->data->second);
                            delp = subp;
                            delc = cur;
                            break;
                        }
                    }
                }
            }
            if (delp == nullptr) return false;

            Node *_del = delc;
            Node *_delp = delp;
            //更新平衡因子
            while (delc != root) {
                //先更新一下父亲节点的平衡因子
                if (delc == delp->left) delp->balance++;
                else if (delc == delp->right) delp->balance--;
                //然后判断平衡情况，决定是否向上继续操作
                if (delp->balance == -1 || delp->balance == 1) break;
                else if (delp->balance == 0) {
                    delc = delp;
                    delp = delp->parent;
                } else if (delp->balance == -2) {  //左边高
                    if (delp->left->balance == -1) {
                        Node *tmp = delp->left;
                        rotateLL(delp);
                        delp = tmp;
                    } else if (delp->left->balance == 1) {
                        Node *tmp = delp->left->right;
                        rotateLR(delp);
                        delp = tmp;
                    } else {
                        Node *tmp = delp->left;
                        rotateLL(delp);
                        delp = tmp;
                        delp->balance = 1;
                        delp->right->balance = -1;
                        break;
                    }
                    delc = delp;
                    delp = delp->parent;
                } else if (delp->balance == 2) {
                    if (delp->right->balance == 1) {
                        Node *tmp = delp->right;
                        rotateRR(delp);
                        delp = tmp;
                    } else if (delp->right->balance == -1) {
                        Node *tmp = delp->right->left;
                        rotateRL(delp);
                        delp = tmp;
                    } else {
                        Node *tmp = delp->right;
                        rotateRR(delp);
                        delp = tmp;
                        delp->balance = -1;
                        delp->left->balance = 1;
                        break;
                    }
                    delc = delp;
                    delp = delp->parent;
                }
            }

            //删除元素
            if (_del->left == nullptr) {
                if (_del == _delp->left) {
                    _delp->left = _del->right;
                    if (_del->right) _del->right->parent = _delp;
                } else {
                    _delp->right = _del->right;
                    if (_del->right) _del->right->parent = _delp;
                }
            } else {
                if (_del == _delp->left) {
                    _delp->left = _del->left;
                    if (_del->left) _del->left->parent = _delp;
                } else {
                    _delp->right = _del->left;
                    if (_del->left) _del->left->parent = _delp;
                }
            }
            delete _del->data;
            _del->data = nullptr;
            delete _del;
            _del = nullptr;
            // static int xx = 0;
            // ++xx;
            // if (xx % 100 == 0) {
            //     check(root);
            // }
            return true;
        }

        bool judge_equal(const Key &k1, const Key &k2) const {
            if ((Compare()(k1, k2) == false) && (Compare()(k2, k1) == false)) {
                return true;
            } else return false;
        }

        Node *find(const Key &key, Node *t) const {
            if (t == nullptr || judge_equal(t->data->first, key)) {
                return t;
            } else if (Compare()(key, t->data->first)) {
                return find(key, t->left);
            } else {
                return find(key, t->right);
            }
        }
        void erase(iterator pos) {
            if(pos.ptr != this){
                throw invalid_iterator();
            }
            if(pos.node == nullptr){
                throw container_is_empty();
            }
            if (!remove(*(pos.node->data))) {
                throw invalid_iterator();
            } else num--;
        } 
        
        size_t count(const Key &key) const {
            Node *tmp = find(key, root);
            if (tmp == nullptr) return 0;
            else return 1;
        }

        iterator find(const Key &key) {
            iterator ans = iterator(nullptr, this);
            ans.node = find(key, root);
            return ans;
        }

        const_iterator find(const Key &key) const {
            const_iterator ans = const_iterator(nullptr, this);
            ans.node = find(key, root);
            return ans;
        }
    };
}

//错误1：自己写的一些函数要声明 const
//错误2：insert之类的函数传入 Node* 要加上引用
//错误3：将迭代器的end设置为nullptr，然后在--函数内进行特判
//错误4：迭代器内存储的 map* 要声明成const，因为传入的map可能为const
//错误5：在交换节点的时候不能只交换内部的值，而是要交换整个节点，因为迭代器指向的是地址，所以要保证每个值对应的地址不变
//错误6：在交换节点时要考虑两节点直接相连的情况，这时的交换关系会发生改变
//错误7：交换节点时要考虑节点为根节点的情况，这时要重新对root赋值
//错误8：交换节点时要更新完全，比如将原来的父亲节点的父亲节点的子节点改变
//备注：借鉴了csdn的AVL树文章

#endif
