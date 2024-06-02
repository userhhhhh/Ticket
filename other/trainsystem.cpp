// #include "/home/hqs123/class_code/Ticket/B_plus_tree/BPlusTree.h"
// #include "/home/hqs123/class_code/Ticket/B_plus_tree/BPlusTree.cpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <bits/stdc++.h>
#include <iterator>
#include <sstream>
#include <utility>
#include <vector>
#include "trainsystem.h"
// #include "command.hpp"

const int M = 1e9 + 7;
const int B = 233;
int hash_string(const std::string & s){
    int res = 0;
    for (int i = 0; i < s.size(); ++i) {
    res = ((long long)res * B + s[i]) % M;
    }
    return res;
}
int hash_mystring(const mystring & s){
    std::string str = s.c;
    return hash_string(str);
}

Command command;
Reshape reshape;

Trainsystem::Trainsystem(std::string str1, std::string str2, std::string str3, std::string str4)
    : train_map(str1, str2), station_train_map(str3, str4) {
    seat_file.open("seat.txt",std::ios::in | std:: ios::out);
    if (!seat_file.good()){ 
        seat_file.open("seat.txt", std::ios::out);
        seat_file.close();
        seat_file.open("seat.txt", std::ios::in | std:: ios::out);
    }
    train_data_file.open("train_data.txt",std::ios::in | std:: ios::out);
    if (!train_data_file.good()){ 
        train_data_file.open("train_data.txt", std::ios::out);
        train_data_file.close();
        train_data_file.open("train_data.txt", std::ios::in | std:: ios::out);
    }
    tail_file.open("tail.txt",std::ios::in | std:: ios::out);
    if (!tail_file.good()){ 
        tail_file.open("tail.txt", std::ios::out);
        tail_file.close();
        tail_file.open("tail.txt");
    }
    tail_file.seekg(0, std::ios::end);
    int tail_file_size = tail_file.tellg();
    if (tail_file_size == 0){
        seat_tail = 0;
        train_data_tail = 0;
    }
    else{
        tail_file.seekg(0);
        tail_file.read(reinterpret_cast<char*>(&seat_tail),sizeof(int));
        tail_file.read(reinterpret_cast<char*>(&train_data_tail), sizeof(int));
    }
}
Trainsystem::~Trainsystem(){
    if(seat_file.is_open()) seat_file.close();
    if(train_data_file.is_open()) train_data_file.close();
    if (tail_file.is_open()){
        tail_file.close();
        tail_file.open("tail.txt", std::ios::out | std::ios::trunc);
        tail_file.seekp(0);
        tail_file.write(reinterpret_cast<char*>(&seat_tail), sizeof(int));
        tail_file.write(reinterpret_cast<char*>(&train_data_tail), sizeof(int));
        tail_file.close();
    }
}

void Trainsystem::write_train_data(train_data & data_in, int pos){
    train_data_file.seekp(pos);
    train_data_file.write(reinterpret_cast<char*>(&data_in), sizeof(data_in));
}
void Trainsystem::read_train_data(train_data & data_in, int pos){
    train_data_file.seekg(pos);
    train_data_file.read(reinterpret_cast<char*>(&data_in), sizeof(data_in));
}
void Trainsystem::write_seat(Seat & seat_in, int pos_in){
    seat_file.seekp(pos_in);
    seat_file.write(reinterpret_cast<char*>(&seat_in), sizeof(seat_in));
}
void Trainsystem::read_seat_data(seat_data & seat_in, int pos_in, int index){
    int pos = pos_in + sizeof(int) * 203 + index * sizeof(seat_data);
    seat_file.seekg(pos);
    seat_file.read(reinterpret_cast<char*>(&seat_in), sizeof(seat_in));
}
void Trainsystem::write_seat_data(seat_data & seat_in, int pos_in, int index){
    int pos = pos_in + sizeof(int) * 203 + index * sizeof(seat_data);
    seat_file.seekp(pos);
    seat_file.write(reinterpret_cast<char*>(&seat_in), sizeof(seat_in));
}

// 添加 <trainID> 为 -i，<stationNum> 为 -n，<seatNum> 为 -m，<stations> 为 -s，<prices> 为 -p，<startTime> 为 -x，<travelTimes> 为 -t，
// <stopoverTimes> 为 -o，<saleDate> 为 -d，<type> 为 -y 的车次。 
// 由于 -s、-p、-t、-o 和 -d 由多个值组成，输入时两个值之间以 | 隔开（仍是一个不含空格的字符串）。
bool Trainsystem::add_train(mystring & i, int n, int m, std::string & s, std::string & p, std::string & x, std::string & t, std::string & o, std::string & d, char y){
    auto ans = train_map.final_find(i);
    if(!ans.empty()) return false;
    TrainInfor infor_in;
    infor_in.trainID = i;
    infor_in.stationNum = n;
    infor_in.seatNum = m;

    train_data data_in;
    data_in.stationNum = n;

    std::vector<std::string> splitStrings;
    std::vector<int> spilitNums;
    std::istringstream iss(s);
    std::string token;
    while (std::getline(iss, token, '|')) {
        splitStrings.push_back(token);
        spilitNums.push_back(hash_string(token));
    }
    for(int i = 0; i < spilitNums.size(); ++i){
        mystring tmp(splitStrings[i]);
        data_in.station_name[i] = tmp;
        data_in.stations[i] = spilitNums[i];
    }

    // 下标图例
    // station   0       1       2       3
    // price         0       1       2
    // travel        0       1       2
    // stop              0       1
    // price         0       1       2

    std::vector<int> array;
    command.spilit1(p, array);
    data_in.prices[0] = array[0];
    for(int i = 1; i < array.size(); ++i){
        data_in.prices[i] = array[i] + data_in.prices[i - 1];
    }
    infor_in.startTime = command.spilit2(x);
    command.spilit1(t, array);
    for(int i = 0; i < array.size(); ++i){
        data_in.travelTimes[i] = array[i];
    }
    command.spilit1(o, array);
    for(int i = 0; i < array.size(); ++i){
        data_in.stopoverTimes[i] = array[i];
    }
    data_in.leave_time[0].date = 0;
    data_in.leave_time[0].time = 0;
    for(int i = 1; i < data_in.stationNum - 1; ++i){
        data_in.arrive_time[i] = data_in.leave_time[i - 1] + data_in.travelTimes[i - 1];
        data_in.leave_time[i] = data_in.arrive_time[i] + data_in.stopoverTimes[i - 1];
    }
    data_in.arrive_time[data_in.stationNum - 1] = data_in.leave_time[data_in.stationNum - 2] + data_in.travelTimes[data_in.stationNum - 2];

    auto date = command.spilit3(d);
    infor_in.saleDate_start = date.first;
    infor_in.saleDate_end = date.second;
    infor_in.type = y;

    infor_in.data_pos = train_data_tail;
    // infor_in.seat_pos = seat_tail;
    write_train_data(data_in, train_data_tail);
    train_data_tail += sizeof(data_in);

    Node<mystring, TrainInfor> node(i, infor_in);
    int tmp = train_map.Insert(node);
    //auto ans1 = train_map.final_find(i);
    return tmp;
}

bool Trainsystem::delete_train(mystring & i){
    auto ans1 = train_map.find_node(train_map.root, i);
    auto & ans = ans1.first.key[ans1.second];
    if(ans.first != i) return false;
    if(ans.second.release) return false;
    train_map.Remove(ans);
    return true;
}

bool Trainsystem::release_train(mystring & i){
//    auto ans111 = train_map.final_find(i);
    auto ans1 = train_map.find_node(train_map.root, i);
    auto ans = ans1.first.key[ans1.second];
    auto ans_node = ans1.first.key[ans1.second];
    auto & ans_infor = ans_node.second;
    train_data ans_data;
    read_train_data(ans_data, ans_infor.data_pos);
    int pos_in = ans1.first.pos;
    if(ans.first != i) return false;
    if(ans.second.release) return false;
    ans_node.second.release = true;

    Seat my_seat(ans_infor.seatNum, ans_infor.stationNum);
    // seat_data data_in(ans_infor.seatNum, ans_infor.stationNum);
    for(int i = 0; i < ans_infor.stationNum - 1; ++i){
        my_seat.price[i] = ans_data.prices[i];
        my_seat.stations[i] = ans_data.stations[i];
    }
    my_seat.stations[ans_infor.stationNum - 1] = ans_data.stations[ans_infor.stationNum - 1];
    write_seat(my_seat, seat_tail);
    ans_node.second.seat_pos = seat_tail;
    seat_tail += sizeof(my_seat);

    train_map.modify(ans, ans_node);
    //    auto ans11 = train_map.final_find(i);

    for(int i = 0; i < ans_infor.stationNum; ++i){
        TrainStation ts(ans_infor, ans_data.station_name[i], i, ans_data.arrive_time[i], ans_data.leave_time[i]);
        Node<int, TrainStation> node;
        node.first = hash_mystring(ans_data.station_name[i]);
        node.second = ts;
        station_train_map.Insert(node);
    }

    return true;
}

void Trainsystem::print_train(std::pair<std::pair<int, TrainInfor>, train_data> & train_in, int d) {
    auto & judge = train_in.first.first;
    auto & infor = train_in.first.second;
    auto & train_get = train_in.second;
    seat_data seat_get;
    int cur_date = -1;
    if(judge == -1){
        std::cout << "-1" << std::endl;
        return;
    }
    std::cout << infor.trainID << " " << infor.type << std::endl;
    for(int i = 0; i < infor.stationNum; ++i){
        std::cout << train_get.station_name[i] << " ";
        if(i == 0){
            std::cout << "xx-xx xx:xx ";
        }
        else{
            time_data actual;
            actual.date = d;
            actual.time = infor.startTime;
            std::cout << actual + train_get.arrive_time[i] << " ";
        }
        std::cout << "-> ";
        if(i == train_get.stationNum - 1){
            std::cout << "xx-xx xx:xx ";
        }
        else{
            time_data actual;
            actual.date = d;
            actual.time = infor.startTime;
            time_data my_ans = actual + train_get.leave_time[i];
            if(infor.release == true){
                if(cur_date != my_ans.date){
                    read_seat_data(seat_get, infor.seat_pos, d);
                    cur_date = my_ans.date;
                }
            }
            std::cout << my_ans << " ";
        }
        if(i == 0){
            std::cout << 0 << " ";
        } else {
            std::cout << train_get.prices[i - 1] << " ";
        }
        if(infor.release == false){
            if(i == infor.stationNum - 1) std::cout << "x";
            else std::cout << infor.seatNum;
        }
        else{
            if(i == infor.stationNum - 1) std::cout << "x";
            else std::cout << seat_get.seat_num[i];
        }
        std::cout << std::endl;
    }
}

void Trainsystem::print_ticket(const Ticket &s){
    Reshape rshp;
    std::cout << s.trainID << " " << s.start_station << " ";
    time_data actual;
    actual.date = s.date;
    actual.time = s.ticket_start;
    std::cout << actual + s.start_time;
    std::cout << " -> ";
    std::cout << s.end_station << " ";
    std::cout << actual + s.end_time << " ";
    std::cout << s.cost << " " << s.seat_num;
}

std::pair<std::pair<int, TrainInfor>, train_data> Trainsystem::query_train(mystring & i, std::string & d){
    auto ans1 = train_map.final_find(i);
    if(ans1.empty()) return {{-1, TrainInfor()}, {}};
    auto & ans = ans1[0];
    int start_time = command.myprocess(d);
    if(start_time < ans.saleDate_start || start_time > ans.saleDate_end) return {{-1, TrainInfor()}, train_data()};
 
    train_data data_in;
    // seat_data seat_in;
    read_train_data(data_in, ans.data_pos);
    // read_seat_data(seat_in, ans.seat_pos);

    if(ans.release)  return {{1, ans}, data_in};
    return {{0, ans}, data_in};
}

void mysort(std::vector<Ticket> & v, int order){
    if(order == 1){
        std::sort(v.begin(), v.end(), [](const Ticket& a, const Ticket& b) {
            if(a.time != b.time) return a.time < b.time;
            else return a.trainID < b.trainID;
        });
    }
    if(order == 2){
        std::sort(v.begin(), v.end(), [](const Ticket& a, const Ticket& b) {
            if(a.cost != b.cost) return a.cost < b.cost;
            else return a.trainID < b.trainID;
        });
    }
}

//pos为Seat在文件中的位置，index1是起始站的index，index2是终止站的index
int Trainsystem::read_price(int pos, int index1, int index2){
    int myprice[101];
    int pos_in = pos + sizeof(int);
    seat_file.seekg(pos_in);
    seat_file.read(reinterpret_cast<char*>(& myprice), sizeof(int) * 101);
    if(index1 == 0) return myprice[index2 - 1];
    else return myprice[index2 - 1] - myprice[index1 - 1];
}

std::vector<Ticket> Trainsystem::query_ticket(std::string & s, std::string & t, std::string & d, int order){
    int my_s = hash_string(s);
    int my_t = hash_string(t);
    int date = command.myprocess(d);
    std::vector<Ticket> v;
    auto ans1 = station_train_map.final_find(my_s);
    auto ans2 = station_train_map.final_find(my_t);
    if(ans1.empty() || ans2.empty()) return {};
    std::sort(ans1.begin(), ans1.end());
    std::sort(ans2.begin(), ans2.end());
    int i = 0, j = 0;
    while(i < ans1.size() && j < ans2.size()){
        auto & s_i = ans1[i];
        auto & t_j = ans2[j];
        if(s_i < t_j){
            i++;
        }
        else if(s_i > t_j){
            j++;
        }
        else{
            if(s_i.index < t_j.index){
                int my_start = date - s_i.start_time.date;
                if(s_i.start_time.time + s_i.start_ >= 1440) my_start--;
                //不确定
                if(s_i.saleDate_start <= my_start && t_j.saleDate_end >= my_start){
                    int ticket_price = read_price(s_i.seat_pos, s_i.index, t_j.index);
                    int ticket_seat = INT_MAX;
                    seat_data seat_read;
                    read_seat_data(seat_read, s_i.seat_pos, my_start);
                    for(int w = s_i.index; w <= t_j.index - 1; ++w){
                        ticket_seat = std::min(ticket_seat, seat_read.seat_num[w]);
                    }
                    Ticket ticket_in(s_i, t_j, my_start, ticket_price, ticket_seat);
                    v.push_back(ticket_in);
                }
            }
            i++;
            j++;
        }
    }
    mysort(v, order);
    return v;
}

void Trainsystem::get_station(TrainStation & t, std::vector<station_data>& v){
    auto ans1 = train_map.final_find(t.trainID);
    train_data data_in;
    read_train_data(data_in, ans1[0].data_pos);
    station_data new_data;
    new_data.num = data_in.stationNum;
    std::copy(std::begin(data_in.stations), std::end(data_in.stations), std::begin(new_data.stations));
    v.push_back(new_data);
}

void mysort2(std::pair<Ticket, Ticket> & origin, std::pair<Ticket, Ticket> & other, int order){
    time_data origin_actual_end;
    origin_actual_end.date = origin.second.end_time.date;
    origin_actual_end.time = origin.second.end_time.time;
    origin_actual_end = origin_actual_end + origin.second.ticket_start;
    time_data origin_actual_start;
    origin_actual_start.date = origin.first.start_time.date;
    origin_actual_start.time = origin.first.start_time.time;
    origin_actual_start = origin_actual_start + origin.first.ticket_start;

    time_data other_actual_end;
    other_actual_end.date = other.second.end_time.date;
    other_actual_end.time = other.second.end_time.time;
    other_actual_end = other_actual_end + other.second.ticket_start;
    time_data other_actual_start;
    other_actual_start.date = other.first.start_time.date;
    other_actual_start.time = other.first.start_time.time;
    other_actual_start = other_actual_start + other.first.ticket_start;

    int original_time = origin_actual_end - origin_actual_start;
    int original_cost = origin.first.cost + origin.second.cost;
    int other_time = other_actual_end - other_actual_start;
    int other_cost = other.first.cost + other.second.cost;

    if(order == 1){
        if(original_time < other_time) return;
        else if(original_time > other_time){
            origin = other; return;
        } 
        else{
            if(original_cost < other_cost) return;
            else if(original_cost > other_cost){
                origin = other; return;
            } 
            else {}
        }
    }
    else if(order == 2){
        if(original_cost < other_cost) return;
        else if(original_cost > other_cost){
            origin = other; return;
        }
        else{
            if(original_time < other_time) return;
            else if(original_time > other_time){
                origin = other; return;
            } 
            else {}
        }
    }
    else {}
    if(origin.first.trainID < other.first.trainID) return;
    else if(other.first.trainID < origin.first.trainID){
        origin = other; return;
    }
    else{
        if(origin.second.trainID < other.second.trainID) return;
        else if(other.second.trainID < origin.second.trainID){
            origin = other; return;
        }
        else{}
    }
    return;
}


std::pair<bool, std::pair<Ticket, Ticket>> Trainsystem::check(int date_in1, int leave_index, int arrive_index, TrainInfor & train_leave_infor, train_data& train_data_leave, seat_data& seat_data_leave, TrainInfor & train_arrive_infor, train_data& train_data_arrive, int order){
    std::pair<bool, std::pair<Ticket, Ticket>> ans;
    bool flag = false;
    for(int i = leave_index + 1; i < train_data_leave.stationNum; ++i){
        for(int j = arrive_index - 1; j >= 0; --j){
            if(train_data_leave.stations[i] == train_data_arrive.stations[j]){
                int target_date_min; //在满足第一辆车到达中转的情况下，第二辆的最早发车日期
                int target_date;     //第二辆车的发车日期
                time_data mid_arrive_time = train_data_leave.arrive_time[i] + train_leave_infor.startTime;   //到达中转，指定了天数和时间
                time_data mid_leave_time = train_data_arrive.leave_time[j] + train_arrive_infor.startTime;   //离开中转，没有指定哪一天，但可以得出相对天数和时间
                // time_data target_arrive_time = train_data_arrive.arrive_time[arrive_index] + train_arrive_infor.startTime;   //到达目的地的相对天数和时间
                if(mid_arrive_time.time <= mid_leave_time.time){
                    target_date_min = date_in1 + mid_arrive_time.date - mid_leave_time.date;
                }
                else{
                    target_date_min = date_in1 + mid_arrive_time.date - mid_leave_time.date;
                    target_date_min--;
                }
                if(target_date_min > train_arrive_infor.saleDate_end) continue;
                if(target_date_min >= train_arrive_infor.saleDate_start) target_date = target_date_min;
                else target_date = train_arrive_infor.saleDate_start;

                int ticket_price1 = read_price(train_leave_infor.seat_pos, leave_index, i);
                int ticket_seat1 = INT_MAX;
                for(int w = leave_index; w <= i - 1; ++w){
                    ticket_seat1 = std::min(ticket_seat1, seat_data_leave.seat_num[w]);
                }
                Ticket ticket_in1(train_leave_infor, train_data_leave, date_in1, leave_index, i, ticket_price1, ticket_seat1);

                int ticket_price2 = read_price(train_arrive_infor.seat_pos, j, arrive_index);
                seat_data seat_data_arrive;
                read_seat_data(seat_data_arrive, train_arrive_infor.seat_pos, target_date);
                int ticket_seat2 = INT_MAX;
                for(int w = j; w <= arrive_index - 1; ++w){
                    ticket_seat2 = std::min(ticket_seat2, seat_data_arrive.seat_num[w]);
                }
                Ticket ticket_in2(train_arrive_infor, train_data_arrive, target_date, j, arrive_index, ticket_price2, ticket_seat2);

                if(!flag){
                    ans.second.first = ticket_in1;
                    ans.second.second = ticket_in2;
                    flag = true;
                }
                else{
                    std::pair<Ticket, Ticket> other = std::make_pair(ticket_in1, ticket_in2);
                    mysort2(ans.second, other, order);
                }
            }
        }
    }
    if(flag) ans.first = true;
    return ans;
}

std::pair<bool, std::pair<Ticket, Ticket>> Trainsystem::query_transfer(std::string & s, std::string & t, std::string & d, int order){
    int my_s = hash_string(s);
    int my_t = hash_string(t);
    int date = command.myprocess(d);
    std::pair<Ticket, Ticket> final_ans;
    // ans1 和 ans2 是经过这个 station 的 train
    // v1 和 v2 分别存储了每个 train 的所有 station 信息
    auto ans1 = station_train_map.final_find(my_s);
    auto ans2 = station_train_map.final_find(my_t);

    for(int i = 0; i < ans1.size(); ++i){
        int tmp_date1 = date;
        train_data train_data_leave;
        seat_data seat_data_leave;
        auto pair_get1 = train_map.find_node(train_map.root, ans1[i].trainID);
        auto & train_leave_infor = pair_get1.first.key[pair_get1.second].second;

        read_train_data(train_data_leave, ans1[i].data_pos);
        tmp_date1 -= train_data_leave.leave_time[ans1[i].index].date;
        if(ans1[i].start_ + train_data_leave.leave_time[ans1[i].index].time >= 1440) tmp_date1--;
        if(tmp_date1 < ans1[i].saleDate_start || tmp_date1 > ans1[i].saleDate_end) continue;
        read_seat_data(seat_data_leave, ans1[i].seat_pos, tmp_date1);

        for(int j = 0; j < ans2.size(); ++j){
            train_data train_data_arrive;
            auto pair_get2 = train_map.find_node(train_map.root, ans2[j].trainID);
            auto & train_arrive_infor = pair_get2.first.key[pair_get2.second].second;
            read_train_data(train_data_arrive, ans2[j].data_pos);

            // seat_data seat_data_arrive;
            // int tmp_date2 = date;
            // tmp_date2 -= train_data_arrive.arrive_time[ans2[j].index].date;
            // if(ans2[j].start_ + train_data_arrive.arrive_time[ans2[j].index].time >= 1440) tmp_date2--;
            // read_seat_data(seat_data_arrive, ans2[j].seat_pos, tmp_date2);

            auto ans = check(tmp_date1, ans1[i].index, ans2[j].index, train_leave_infor, train_data_leave, seat_data_leave,train_arrive_infor, train_data_arrive, order);
            mysort2(final_ans, ans.second, order);
        }
    }
    return {true, final_ans};
}

