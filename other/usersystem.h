#ifndef TICKET_USERSYSTEM_H
#define TICKET_USERSYSTEM_H

#include "BPlusTree.h"
#include "BPlusTree.cpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <bits/stdc++.h>
#include <cstring>

void mycopy(char* a, const char* b){
    int i = 0;
    while(b[i] != '\0'){
        a[i] = b[i];
        i++;
    }
    a[i] = '\0';
}

class User{
public:
    char username[22] = {'\0'};
    char password[33] = {'\0'};
    char name[35] = {'\0'};
    char mailAddr[33] = {'\0'};
    int privilege;
    bool log_in = false;
public:
    User() = default;
    User(const std::string & s1, const std::string & s2, const std::string & s3, const std::string & s4, int pri, bool log = false): privilege(pri), log_in(log) {
//        int i = 0;
//        for (; s1[i] != '\0'; ++i) username[i] = s1[i];
//        username[i] = '\0';
//        i = 0;
//        for (; s2[i] != '\0'; ++i) password[i] = s2[i];
//        password[i] = '\0';
//        i = 0;
//        for (; s3[i] != '\0'; ++i) name[i] = s3[i];
//        name[i] = '\0';
//        i = 0;
//        for (; s4[i] != '\0'; ++i) mailAddr[i] = s4[i];
//        mailAddr[i] = '\0';
        
        mycopy(username, s1.c_str());
        mycopy(password, s2.c_str());
        mycopy(name, s3.c_str());
        mycopy(mailAddr, s4.c_str());
        // std::copy(s1.begin(), s1.end(), username);
        // std::copy(s2.begin(), s2.end(), password);
        // std::copy(s3.begin(), s3.end(), name);
        // std::copy(s4.begin(), s4.end(), mailAddr);
    }
    User& operator = (const User & other){
        std::memcpy(username, other.username, 22);
        std::memcpy(password, other.password, 33);
        std::memcpy(name, other.name,35);
        std::memcpy(mailAddr, other.mailAddr, 33);
        privilege = other.privilege;
        // log_in = other.log_in;
        return *this;
    }
    bool operator < (const User & other) const {
        return strcmp(username, other.username);
    }
    bool operator == (const User & other) const {
        return (strcmp(username, other.username) == 0);
    }
    friend std::ostream &operator<<(std::ostream & os, const User & obj){
        os << obj.username << ' ' << obj.name << ' ' << obj.mailAddr << ' '  << obj.privilege << '\n';
        return os;
    }
};

class Usersystem{
public:
    BPlusTree<mystring, User> user_map;
public:
    Usersystem(std::string str1, std::string str2): user_map(str1, str2){};
    bool checkuser(const mystring & s){
        std::vector<User> && ans = user_map.final_find(s);
        if(ans.empty()) return false;
        // if(!ans[0].log_in) return false;
        return true;
    }
    bool add_user(const std::string & c, const std::string & u, const std::string & p, const std::string & n, const std::string & m, int g){
        mystring my_c(c);
        mystring my_u(u);
        // if(user_map.num == 0){
        //     User tmp(u, p, n, m, 10);
        //     Node<mystring, User> add_data(my_u, tmp);
        //     return user_map.Insert(add_data);
        // }
        std::vector<User> && ans = user_map.final_find(my_c);
        if(ans.empty()) return false;
        //if(!ans[0].log_in) return false;
        if(ans[0].privilege <= g) return false;
        User tmp(u, p, n, m, g);
        Node<mystring, User> add_data(my_u, tmp);
        return user_map.Insert(add_data);
    }
    bool login(const std::string & u, const std::string & p){
        if(user_map.num == 0) return false;
        mystring my_u(u);
        mystring my_p(p);
        std::pair<BPlusNode<mystring, User>, int> modify_data;
        modify_data = user_map.find_node(user_map.root, my_u);
        if(modify_data.first.key[modify_data.second].first != my_u) return false;
        User& modify_user = modify_data.first.key[modify_data.second].second;
        std::string tmp(modify_user.password);
        if(tmp != p) return false;
        // if(modify_user.log_in) return false;
        // modify_data.first.key[modify_data.second].second.log_in = true;
        // user_map.writeNode(modify_data.first, modify_data.first.pos);
        return true;
    }
    bool logout(const std::string & u){
        mystring my_u(u);
        std::pair<BPlusNode<mystring, User>, int> modify_data;
        modify_data = user_map.find_node(user_map.root, my_u);
        if(modify_data.first.key[modify_data.second].first != my_u) return false;
        // User& modify_user = modify_data.first.key[modify_data.second].second;
        // std::string tmp(modify_user.password);
        // if(!modify_user.log_in) return false;
        // modify_user.log_in = false;
        // user_map.writeNode(modify_data.first, modify_data.first.pos);
        return true;
    }
    std::pair<bool, User> query_profile(const std::string & c, const std::string & u){
        mystring my_c(c);
        mystring my_u(u);

        std::pair<BPlusNode<mystring, User>, int> log_data;
        log_data = user_map.find_node(user_map.root, my_c);
        auto & log_user = log_data.first.key[log_data.second].second;
        auto & log_key = log_data.first.key[log_data.second].first;
        std::string log_key_str(log_key.c);
        if(log_key_str != c) return {false, {}};

        std::pair<BPlusNode<mystring, User>, int> query_data;
        query_data = user_map.find_node(user_map.root, my_u);
        auto & query_user = query_data.first.key[query_data.second].second;
        auto & query_key = query_data.first.key[query_data.second].first;
        std::string query_key_str(query_key.c);
        if(query_key_str != u) return {false, {}};

        // if(!log_user.log_in) return {false, {}};
        if(log_user.privilege <= query_user.privilege && c != u) return {false, {}};
        return {true, query_user};
    }
    std::pair<bool, User> modify_profile(const std::string & c, const std::string & u, const std::string & p, const std::string & n, const std::string & m, int g){
        mystring my_c(c);
        mystring my_u(u);
        std::pair<BPlusNode<mystring, User>, int> log_data;
        log_data = user_map.find_node(user_map.root, my_c);
        auto & log_user = log_data.first.key[log_data.second].second;
        auto & log_key = log_data.first.key[log_data.second].first;
        std::string log_key_str(log_key.c);
        if(log_key_str != c) return {false, {}};

        std::pair<BPlusNode<mystring, User>, int> query_data;
        query_data = user_map.find_node(user_map.root, my_u);
        auto & query_user = query_data.first.key[query_data.second].second;
        auto & query_key = query_data.first.key[query_data.second].first;
        auto & query_node = query_data.first.key[query_data.second];
        std::string query_key_str(query_key.c);
        if(query_key_str != u) return {false, {}};

        // if(!log_user.log_in) return {false, {}};
        if(log_user.privilege <= query_user.privilege && c != u) return {false, {}};
        if(log_user.privilege <= g) return {false, {}};

        User new_user(query_user);
        if(p != ""){
            for(int i = 0; i < 33; ++i) new_user.password[i] = '\0';
            std::copy(p.begin(), p.end(), new_user.password);
        } 
        if(n != ""){
            for(int i = 0; i < 35; ++i) new_user.name[i] = '\0';
            std::copy(n.begin(), n.end(), new_user.name);
        } 
        if(m != ""){
            for(int i = 0; i < 33; ++i) new_user.mailAddr[i] = '\0';
            std::copy(m.begin(), m.end(), new_user.mailAddr);
        } 
        if(g != -1) new_user.privilege = g;
        Node<mystring, User> tran_node(my_u, new_user);
        user_map.modify(query_node, tran_node);
        return {true, new_user};
    }
};

#endif //TICKET_USERSYSTEM_H
