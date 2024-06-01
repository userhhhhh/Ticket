#ifndef TICKET_TICKETSYSTEM_H
#define TICKET_TICKETSYSTEM_H

#include "trainsystem.h"
// #include "usersystem.h"

class Order{
public:
    mystring trainID;
    mystring start_station;
    mystring end_station;
    time_data start_time;
    time_data end_time;
    int time;
    int cost;
    int seat_num;

    int date;                     // 发车的日期
    int ticket_start; 

    int status;                   // 1为 sussess，2为 pending，3为 refunded
    int num;                      //该订单的买票数量
    int time_stamp;
    int leave_index;
    int arrive_index;

public:
    Order() = default;
    Order(const TrainInfor & infor, const train_data & data, int date_in, int leave_index_in, int arrive_index_in, int cost_in, int seat_num_in, int n){
        trainID = infor.trainID;
        start_station = data.station_name[leave_index_in];
        end_station = data.station_name[arrive_index_in];
        start_time = data.leave_time[leave_index_in];
        end_time = data.arrive_time[arrive_index_in];
        time = (end_time.date - start_time.date) * 1440 + end_time.time - start_time.time;
        cost = cost_in;
        seat_num = seat_num_in;
        date = date_in;
        ticket_start = infor.startTime;
        num = n;
        leave_index = leave_index_in;
        arrive_index = arrive_index_in;
    }
    bool operator < (const Order & other) const {
        if(trainID != other.trainID) return (trainID < other.trainID);
        else return (time_stamp < other.time_stamp);
    }
    bool operator == (const Order & other) const {
        return (trainID == other.trainID && time_stamp == other.time_stamp && status == other.status);
    }
};

class Waitlist{
public:
    mystring username;
    int leave_index;
    int arrive_index;
    int num;
    int time_stamp;
public:
    bool operator < (const Waitlist & other) const {
        if(username != other.username) return (username < other.username);
        else return (time_stamp < other.time_stamp);
    }
    bool operator == (const Waitlist & other) const {
        return (username == other.username && time_stamp == other.time_stamp);
    }
};

class ticketsystem{
public:
    Usersystem usersystem;
    Trainsystem trainsystem;
    BPlusTree<mystring, Order> order_map;
    BPlusTree<mystring, Waitlist> waitlist_map;
public:
    ticketsystem(std::string str1, std::string str2, std::string str3, std::string str4, std::string str5, std::string str6, std::string str7, std::string str8, std::string str9, std::string str10);
    int buy_ticket(std::string &u, std::string &i, std::string &d, int n, std::string &f, std::string &t, bool q, int time_in);
    std::pair<bool, std::vector<Order>> query_order(const std::string &u);
    bool refund_ticket(std::string & u, int n);
    void print_order(const Order & order);
};

#endif //TICKET_TICKETSYSTEM_H
