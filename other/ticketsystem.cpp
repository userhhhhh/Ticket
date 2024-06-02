#include "ticketsystem.h"
#include "trainsystem.h"
#include "usersystem.h"
// #include "command.hpp"

ticketsystem::ticketsystem(std::string str1, std::string str2, std::string str3, std::string str4, std::string str5, std::string str6, std::string str7, std::string str8, std::string str9, std::string str10)
    : usersystem(str1, str2), trainsystem(str3, str4, str5, str6), order_map(str7, str8), waitlist_map(str9, str10) {}

int ticketsystem::buy_ticket(std::string &u, std::string &i, std::string &d, int n, std::string &f, std::string &t, bool q, int time_in){
    int my_u = hash_string(u);
    int my_i = hash_string(i);
    int my_f = hash_string(f);
    int my_t = hash_string(t);
    Command command1;
    int date = command1.myprocess(d);
    mystring mystring_u(u);
    mystring mystring_i(i);
    auto ans1 = usersystem.user_map.final_find(mystring_u);
    auto ans2 = trainsystem.train_map.final_find(mystring_i);
    if(ans1.empty() || ans2.empty() || ans2[0].release == false) return -1;
    auto & train_find = ans2[0];
    if(n > ans2[0].seatNum) return -1;
    
    train_data train_data_in;
    trainsystem.read_train_data(train_data_in, train_find.data_pos);
    int leave_index = -1, arrive_index = -1;
    for(int i = 0; i < train_find.stationNum; ++i){
        if(train_data_in.stations[i] == my_f){
            leave_index = i;
        }
        else if(train_data_in.stations[i] == my_t){
            arrive_index = i;
            break;
        }
    }
    if(arrive_index == -1 || leave_index == -1 || arrive_index <= leave_index) return -1;

    date -= train_data_in.leave_time[leave_index].date;
    if(train_data_in.leave_time[leave_index].time + ans2[0].startTime >= 1440) date--;
    if(date < train_find.saleDate_start || date > train_find.saleDate_end) return -1;
    seat_data seat_in;
    trainsystem.read_seat_data(seat_in, train_find.seat_pos, date);

    int ticket_seat = INT_MAX;
    for(int i = leave_index; i <= arrive_index - 1; ++i)
        ticket_seat = std::min(ticket_seat, seat_in.seat_num[i]);
    int ticket_price;
    if(leave_index == 0) ticket_price = train_data_in.prices[arrive_index - 1];
    else ticket_price = train_data_in.prices[arrive_index - 1] - train_data_in.prices[leave_index - 1];

    bool whether_enough = true;
    Order order_in(train_find, train_data_in, date, leave_index, arrive_index, ticket_price, ticket_seat, n);
    order_in.time_stamp = time_in;
    for(int i = leave_index; i <= arrive_index - 1; ++i){
        if(seat_in.seat_num[i] < n){
            whether_enough = false;
            break;
        }
    }
    if(whether_enough){
        order_in.status = 1;
        for(int i = leave_index; i <= arrive_index - 1; ++i)
            seat_in.seat_num[i] -= n;
        trainsystem.write_seat_data(seat_in, ans2[0].seat_pos, date);
    }
    else{
        if(!q) return -1;
        if(q){
            order_in.status = 2;
            Waitlist wait_data;
            wait_data.leave_index = leave_index;
            wait_data.arrive_index = arrive_index;
            wait_data.num = n;
            wait_data.username = mystring_u;
            wait_data.time_stamp = time_in;
            wait_data.date = date;
            Node<mystring, Waitlist> node(mystring_i, wait_data);
            waitlist_map.Insert(node);
        }
    }
    Node<mystring, Order> node1(mystring_u, order_in);
    order_map.Insert(node1);
    if(order_in.status == 1) return order_in.cost * order_in.num;
    return 0;
}

void ticketsystem::print_order(const Order &order){
    if(order.status == 1) std::cout << "[success] ";
    if(order.status == 2) std::cout << "[pending] ";
    if(order.status == 3) std::cout << "[refunded] ";
    std::cout << order.trainID << " " << order.start_station << " ";
    time_data actual;
    actual.date = order.date;
    actual.time = order.ticket_start;
    std::cout << actual + order.start_time;
    std::cout << " -> ";
    std::cout << order.end_station << " ";
    std::cout << actual + order.end_time << " ";
    std::cout << order.cost << " " << order.num;
}

std::pair<bool, std::vector<Order>> ticketsystem::query_order(const std::string &u) {
    mystring my_u(u);
    auto tmp = usersystem.checkuser(my_u);
    if(!tmp) return {false, {}};
    auto ans = order_map.final_find(my_u);
    std::sort(ans.begin(), ans.end(), [](const Order& order1, const Order & order2){
        return order1.time_stamp < order2.time_stamp;
    });
    return {true, ans};
}

void ticketsystem::judge_compensate(Waitlist & wait, seat_data & seat_in, mystring& trainID_in){
    for(int i = wait.leave_index; i <= wait.arrive_index - 1; ++i){
        if(wait.num > seat_in.seat_num[i]) return;
    }
    for(int i = wait.leave_index; i <= wait.arrive_index - 1; ++i){
        seat_in.seat_num[i] -= wait.num;
    }
    Node<mystring, Waitlist> node(trainID_in, wait);
    waitlist_map.Remove(node);
    auto tmp_vector = order_map.final_find(wait.username);
    Order origin_order;
    for(int i = 0; i < tmp_vector.size(); ++i){
        if(tmp_vector[i].time_stamp == wait.time_stamp){
            origin_order = tmp_vector[i];
            break;
        }
    }
    Order tran_order = origin_order;
    tran_order.status = 1;
    Node<mystring, Order> origin_node(wait.username, origin_order);
    Node<mystring, Order> tran_node(wait.username, tran_order);
    order_map.modify(origin_node, tran_node);
}

bool ticketsystem::refund_ticket(std::string & u, int n){
    mystring my_u(u);
    if(!usersystem.checkuser(my_u)) return false;
    auto ans = order_map.final_find(my_u);
    std::sort(ans.begin(), ans.end(), [](const Order& order1, const Order & order2){
        return order1.time_stamp < order2.time_stamp;
    });
    int index = ans.size() - n;
    if (index < 0) return false;

    Order order = ans[index];
    if(order.status == 3) return false;
    if(order.status == 2){
        Waitlist tmp;
        tmp.time_stamp = order.time_stamp;
        tmp.username = my_u;
        Node<mystring, Waitlist> node(order.trainID, tmp);
        waitlist_map.Remove(node);
        Node<mystring, Order> origin_node(my_u, order);
        order.status = 3;
        Node<mystring, Order> tran_node(my_u, order);
        order_map.modify(origin_node, tran_node);
        return true;
    }
//    TrainInfor tmp;
    auto ans1 = trainsystem.train_map.final_find(order.trainID);
    seat_data seat_in;
    trainsystem.read_seat_data(seat_in, ans1[0].seat_pos, order.date);
    for(int i = order.leave_index; i <= order.arrive_index - 1; ++i){
        seat_in.seat_num[i] += order.num;
    }

    Node<mystring, Order> origin_node(my_u, order);
    order.status = 3;
    Node<mystring, Order> tran_node(my_u, order);
    order_map.modify(origin_node, tran_node);

    auto waiting_list = waitlist_map.final_find(order.trainID);
    std::sort(waiting_list.begin(), waiting_list.end(), [](const Waitlist & w1, const Waitlist & w2){
        return w1.time_stamp < w2.time_stamp;
    });
    for(int i = 0; i < waiting_list.size(); ++i){
        if(waiting_list[i].date != order.date) continue;
        judge_compensate(waiting_list[i], seat_in, order.trainID);
    }
    trainsystem.write_seat_data(seat_in, ans1[0].seat_pos, order.date);
    return true;
}