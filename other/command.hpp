#include <algorithm>
#include <cstring>
#include <iostream>
#include <bits/stdc++.h>
#include <iterator>
#include <sstream>
#include <vector>
// #include "trainsystem.h"

class Command{
public:
    int stringToint(const std::string &s){
        int ans = 0;
        for(int i = 0; i < s.size(); ++i){
            ans *= 10;
            ans += s[i] - '0';
        }
        return ans;
    }
    std::vector<std::string> GetTokens(const std::string &s, char separator) {
        std::vector<std::string> ans;
        for(int i = 0; i < s.size(); ++i){
            std::string str;
            while(s[i] != separator && i < s.size()){
                str += s[i];
                ++i;
            }
            ans.push_back(str);
        }
        return ans;
    }
    //以'|'为分隔符，分割出 int 数组
    void spilit1(std::string &s, std::vector<int>& v){
        v.clear();
        if(s == "_") return;
        std::string token;
        std::istringstream iss(s);
        while (std::getline(iss, token, '|')) {
            int value = std::stoi(token);
            v.push_back(value);
        }
    }
    //以':'为分隔符，分割出 int 数组，然后转化成 int
    //比如：23:50 = 23 * 60 + 50;
    int spilit2(std::string &s){
        std::vector<int> v;
        std::string token;
        std::istringstream iss(s);
        while (std::getline(iss, token, ':')) {
            int value = std::stoi(token);
            v.push_back(value);
        }
        return (v[0] * 60 + v[1]);
    }
    //用于处理 06-01 的情况
    int myprocess(std::string & s){
        std::vector<int> v;
        std::string token;
        std::istringstream iss(s);
        while (std::getline(iss, token, '-')) {
            int value = std::stoi(token);
            v.push_back(value);
        }
        if(v[0] == 6) return v[1];
        if(v[0] == 7) return (30 + v[1]);
        if(v[0] == 8) return (61 + v[1]);
        return -1;
    }
    //用于 06-01|08-17 情况
    std::pair<int, int> spilit3(std::string &s){
        std::vector<std::string> v;
        std::string token;
        std::istringstream iss(s);
        while (std::getline(iss, token, '|')) {
            v.push_back(token);
        }
        int tmp1 = myprocess(v[0]);
        int tmp2 = myprocess(v[1]);
        return {tmp1, tmp2};
    }
};

class Reshape{
public:
    //处理 23:50 = 23 * 60 + 50 的情况
    std::string form1(int a){
        std::string ans = "";
        int val1 = a / 60;
        int val2 = a % 60;
        ans += val1 / 10 + '0';
        ans += val1 % 10 + '0';
        ans += ':';
        ans += val2 / 10 + '0';
        ans += val2 % 10 + '0';
        return ans;
    }
    //处理 06-01 的情况
    std::string form2(int a){
        std::string ans = "";
        if(a >= 1 && a <= 30){
            ans += "06-";
            int val = a;
            ans += val / 10 + '0';
            ans += val % 10 + '0';
        }
        else if(a >= 31 && a <= 61){
            ans += "07-";
            int val = a - 30;
            ans += val / 10 + '0';
            ans += val % 10 + '0';
        }
        else if(a >= 62 && a <= 92){
            ans += "08-";
            int val = a - 61;
            ans += val / 10 + '0';
            ans += val % 10 + '0';
        }
        return ans;
    }
};