#ifndef TICKET_TRAINSYSTEM_H
#define TICKET_TRAINSYSTEM_H
// #include "/home/hqs123/class_code/Ticket/B_plus_tree/BPlusTree.h"
// #include "/home/hqs123/class_code/Ticket/B_plus_tree/BPlusTree.cpp"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <bits/stdc++.h>
#include "usersystem.h"
#include "command.hpp"

int hash_string(const std::string & s);
int hash_mystring(const mystring & s);

class time_data{
public:
    int date = 0;
    int time = 0;
public:
    time_data operator + (int other){
        time_data old;
        old.date = date;
        old.time = time + other;
        old.date += old.time / 1440;
        old.time = old.time % 1440;
        return old;
    }
    time_data operator + (const time_data & other){
        time_data old;
        old.date = date + other.date;
        old.time = time + other.time;
        if(old.time >= 1440){
            old.time = old.time - 1440;
            old.date ++;
        }
        return old;
    }
    int operator - (const time_data & other){
        int ans = (date - other.date) * 1440 + time - other.time;
        return ans;
    }
    friend std::ostream& operator<<(std::ostream& out,const time_data &s){
        Reshape rshp;
        out << rshp.form2(s.date) << " " << rshp.form1(s.time);
        return out;
    }
};

struct train_data{
    int stationNum;
    mystring station_name[101];
    int stations[101] = {};//维护哈希值
    int prices[101] = {};  //维护前缀和,0_base
    int stopoverTimes[101] = {};
    int travelTimes[101] = {};
    time_data arrive_time[101];
    time_data leave_time[101];
};

class seat_data{
public:
    int seat_num[101];
public:
    seat_data() = default;
    seat_data(int seat_num_max){
        for(int i = 0; i < 101; ++i) seat_num[i] = seat_num_max;
    }
};

#pragma pack(1)
class Seat{
public:
    int station_num;
    int price[101];      //price的前缀和
    int stations[101];   //哈希值
    seat_data data[101]; //index为日期转化成的int，value为存放的数值
public:
    Seat(int seatnum_in, int stationnum_in){
        station_num = stationnum_in;
        for(int i = 0; i < 101; ++i)
            for(int j = 0; j < 101; ++j)
                data[i].seat_num[j] = seatnum_in;
    }
};
#pragma pack()

struct station_data{
    int stations[101];
    int num;
};

class TrainInfor{
public:
    mystring trainID;
    int stationNum;
    int seatNum;  
    int startTime;
    int saleDate_start;
    int saleDate_end;
    int data_pos = -1;        //指向train_data存放位置的指针
    int seat_pos = -1;        //指向seat存放位置的指针
    bool release = false;
    char type;
public:
    bool operator < (const TrainInfor & other) const {
        return trainID < other.trainID;
    }
    bool operator == (const TrainInfor & other) const {
        return trainID == other.trainID;
    }
};

class TrainStation{
public:
    mystring trainID;
    mystring station_name;
    int hash_trainID;
    int seat_pos;
    int data_pos;
    int index;
    int saleDate_start;
    int saleDate_end;
    int start_;           // 整个列车在当天的发车时间
    time_data start_time; // 发车时间，始发站为 0，就是 leave
    time_data end_time;   // 停靠时间，始发站为 0，就是 arrive   
public:
    TrainStation() = default;
    TrainStation(const TrainInfor & infor, mystring name_in, int index_in, time_data time_arrive, time_data time_leave){
        trainID = infor.trainID;
        station_name = name_in;
        hash_trainID = hash_mystring(trainID);
        seat_pos = infor.seat_pos;
        data_pos = infor.data_pos;
        index = index_in;
        saleDate_start = infor.saleDate_start;
        saleDate_end = infor.saleDate_end;
        start_ = infor.startTime;
        start_time = time_leave;
        end_time = time_arrive;
    }
    bool operator < (const TrainStation & other) const {
        return (hash_trainID < other.hash_trainID);
    }
    bool operator > (const TrainStation & other) const {
        return (hash_trainID > other.hash_trainID);
    }
    bool operator == (const TrainStation & other) const {
        return (hash_trainID == other.hash_trainID);
    }
};

class Ticket{
public: 
    mystring trainID;
    mystring start_station;
    mystring end_station;
    time_data start_time;
    time_data end_time;
    int time;
    int cost;
    int seat_num;
    int date;                     // 始发站的日期
    int ticket_start;             // 整个列车在当天的发车时间
public:
    Ticket() = default;
    Ticket(TrainStation& start, TrainStation& end, int date_in, int cost_in, int seat_in){
        date = date_in;
        trainID = start.trainID;
        start_station = start.station_name;
        end_station = end.station_name;
        start_time.date = start.start_time.date;
        start_time.time = start.start_time.time;
        end_time.date = end.end_time.date;
        end_time.time = end.end_time.time;
        time = end_time - start_time;
        cost = cost_in;
        seat_num = seat_in;
        ticket_start = start.start_;
    }
    Ticket(TrainInfor & infor, train_data & data, int date_in, int index1, int index2, int cost_in, int seat_in){
        date = date_in;
        trainID = infor.trainID;
        start_station = data.station_name[index1];
        end_station = data.station_name[index2];
        start_time = data.leave_time[index1];
        end_time = data.arrive_time[index2];
        time = end_time - start_time;
        cost = cost_in;
        seat_num = seat_in;
        ticket_start = infor.startTime;
    }
    Ticket& operator = (const Ticket & other){
        trainID = other.trainID;
        start_station = other.start_station;
        end_station = other.end_station;
        start_time = other.start_time;
        end_time = other.end_time;
        time = other.time;
        cost = other.cost;
        seat_num = other.seat_num;
        date = other.date;
        ticket_start = other.ticket_start;
        return *this;
    }
    // friend std::ostream& operator<<(std::ostream& out,const Ticket &s){
    //     Reshape rshp;
    //     out << s.trainID << " " << s.start_station << " " << rshp.form2(s.start_time.date) << " " << rshp.form1(s.start_time.time);
    //     out << " -> ";
    //     out << s.end_station << " " << rshp.form2(s.end_time.date) << " " << rshp.form1(s.end_time.time) << " ";
    //     out << s.cost << " " << s.seat_num;
    //     return out;
    // }
};

// 添加 <trainID> 为 -i，<stationNum> 为 -n，<seatNum> 为 -m，<stations> 为 -s，<prices> 为 -p，<startTime> 为 -x，<travelTimes> 为 -t，
// <stopoverTimes> 为 -o，<saleDate> 为 -d，<type> 为 -y 的车次。 
// 由于 -s、-p、-t、-o 和 -d 由多个值组成，输入时两个值之间以 | 隔开（仍是一个不含空格的字符串）。
class Trainsystem{
public:
    BPlusTree<mystring, TrainInfor> train_map;
    BPlusTree<int, TrainStation> station_train_map;
    std::fstream seat_file;
    std::fstream train_data_file;
    std::fstream tail_file;      //存放seat_file和train_data_file的tail
    int seat_tail = 0;
    int train_data_tail = 0;
public:
    Trainsystem(std::string str1, std::string str2, std::string str3, std::string str4);
    void write_train_data(train_data & data_in, int pos_in);
    void read_train_data(train_data & data_in, int pos_in);
    void write_seat(Seat & seat_in, int pos_in);
    void read_seat_data(seat_data & seat_in, int pos_in, int index);
    void write_seat_data(seat_data & seat_in, int pos_in, int index);
    int read_price(int pos, int index1, int index2);
    bool add_train(mystring & i, int n, int m, std::string & s, std::string & p, std::string & x, std::string & t, std::string & o, std::string & d, char y);
    bool delete_train(mystring & i);
    bool release_train(mystring & i);
    std::pair<std::pair<int, TrainInfor>, train_data> query_train(mystring & i, std::string & d);
    bool myupdate(TrainStation & t, int my_s, int my_t);
    std::vector<Ticket> query_ticket(std::string & s, std::string & t, std::string & d, int order);
    void get_station(TrainStation & t, std::vector<station_data>& v);
    std::pair<bool, std::pair<Ticket, Ticket>> check(int date1, int leave_index, int arrive_index, TrainInfor & train_leave_infor, train_data& train_data_leave, seat_data& seat_data_leave, TrainInfor & train_arrive_infor, train_data& train_data_arrive, int order);
    std::pair<bool, std::pair<Ticket, Ticket>> query_transfer(std::string & s, std::string & t, std::string & d, int order);
    void print_train(std::pair<std::pair<int, TrainInfor>, train_data> & train_in, int d);
    void print_ticket(const Ticket & s);
    ~Trainsystem();
};

#endif //TICKET_TRAINSYSTEM_H
