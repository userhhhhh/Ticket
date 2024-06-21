#include "ticketsystem.h"
#include "ticketsystem.cpp"
#include "trainsystem.h"
#include "trainsystem.cpp"
#include "ticketsystem.h"
#include "usersystem.h"
#include "map.hpp"
#include "utility.hpp"
#include "exceptions.hpp"
// #include "command.hpp"

class Compare {
public:
	bool operator () (const std::string &lhs, const std::string &rhs) const {
		return lhs < rhs;
	}
};
void hhh(){
    std::ofstream ofs("data.txt", std::ios::trunc);
    ofs.close();
    std::ofstream ofs1("infor.txt", std::ios::trunc);
    ofs.close();
    std::ofstream ofs2("train.txt", std::ios::trunc);
    ofs.close();
    std::ofstream ofs3("traininfor.txt", std::ios::trunc);
    ofs.close();
    std::ofstream ofs4("station.txt", std::ios::trunc);
    ofs.close();
    std::ofstream ofs5("anothertrain.txt", std::ios::trunc);
    ofs.close();
    std::ofstream ofs6("orderindex.txt", std::ios::trunc);
    ofs.close();
    std::ofstream ofs7("order.txt", std::ios::trunc);
    ofs.close();
    std::ofstream ofs8("waitindex.txt", std::ios::trunc);
    ofs.close();
    std::ofstream ofs9("wait.txt", std::ios::trunc);
    ofs.close();
}
int main(){
//     freopen("/home/hqs123/class_code/tmp-hw/data/1.in","r",stdin);
//     freopen("0.out","w",stdout);
    std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    // Usersystem usersystem("data.txt", "infor.txt");

//     hhh();
    ticketsystem system("data.txt", "infor.txt", "train.txt", "traininfor.txt", "station.txt", "anothertrain.txt", "orderindex.txt", "order.txt", "waitindex.txt", "wait.txt");
    std::string cmd;
    sjtu::map<std::string, bool, Compare> mymap;
    Command command;
    while(getline(std::cin, cmd)){
        if(cmd[cmd.size() - 1] == '\r') cmd.pop_back();
        std::string time_stamp;
        int i = 1;
        while (cmd[i] != ']') {
            time_stamp += cmd[i];
            ++i;
        }
//             if(time_stamp == "366616"){
//                 std::cout << "hhhhh" <<std::endl;
//             }
        i += 2;
        std::string str = cmd.substr(i, 5);
        if(str == "exit " || str == "exit"){
            std::cout << "[" << time_stamp << "]" << " ";
            std::cout << "bye" << std::endl;
            // if(system.usersystem.user_map.Nfile.is_open()) system.usersystem.user_map.Nfile.close();
            // if(system.usersystem.user_map.Ifile.is_open()) system.usersystem.user_map.Ifile.close();
            // if(system.trainsystem.train_map.Nfile.is_open()) system.trainsystem.train_map.Nfile.close();
            // if(system.trainsystem.train_map.Ifile.is_open()) system.trainsystem.train_map.Ifile.close();
            // if(system.trainsystem.station_train_map.Nfile.is_open()) system.trainsystem.station_train_map.Nfile.close();
            // if(system.trainsystem.station_train_map.Ifile.is_open()) system.trainsystem.station_train_map.Ifile.close();
            // if(system.trainsystem.seat_file.is_open()) system.trainsystem.seat_file.close();
            // if(system.trainsystem.train_data_file.is_open()) system.trainsystem.train_data_file.close();
            // if(system.trainsystem.tail_file.is_open()) system.trainsystem.tail_file.close();
            // if(system.order_map.Nfile.is_open()) system.order_map.Nfile.close();
            // if(system.order_map.Ifile.is_open()) system.order_map.Ifile.close();
            // if(system.waitlist_map.Nfile.is_open()) system.waitlist_map.Nfile.close();
            // if(system.waitlist_map.Ifile.is_open()) system.waitlist_map.Ifile.close();
            // exit(0);
		    return 0;
        }
        else{
            std::vector<std::string> ans = command.GetTokens(cmd, ' ');
            std::cout << ans[0] << " ";
            std::string instruction = ans[1];
            int i = 2;
            int thesize = ans.size();
            if(instruction == "add_user"){
                std::string c, u, p, n, m;
                int g;
                while(i < thesize){
                    if (ans[i] == "-c") c = ans[++i];
                    else if (ans[i] == "-u") u = ans[++i];
                    else if (ans[i] == "-p") p = ans[++i];
                    else if (ans[i] == "-n") n = ans[++i];
                    else if (ans[i] == "-m") m = ans[++i];
                    else if (ans[i] == "-g") {
                        g = command.stringToint(ans[++i]);
                    }
                    ++i;
                }
                auto data = mymap.find(c);
                //没登录且num为0；没登录且num不为0；登录且num不为0
                if(data == mymap.end() && system.usersystem.user_map.num == 0){
                    mystring my_u(u);
                    User tmp(u, p, n, m, 10);
                    Node<mystring, User> add_data(my_u, tmp);
                    system.usersystem.user_map.Insert(add_data);
                    std::cout << "0" << std::endl;
                    continue;
                }
                if(data == mymap.end() && system.usersystem.user_map.num != 0){
                    std::cout << "-1" << std::endl;
                    continue;
                }
                if(system.usersystem.add_user(c, u, p, n, m, g)) std::cout << "0" << std::endl;
                else std::cout << "-1" << std::endl; 
            }
            else if(instruction == "login"){
                std::string u, p;
                while(i < thesize){
                    if (ans[i] == "-u") u = ans[++i];
                    else if (ans[i] == "-p") p = ans[++i];
                    ++i;
                }
                if(mymap.find(u) != mymap.end() || system.usersystem.login(u, p) == false){
                    std::cout << "-1" << std::endl;
                    continue;
                }
                mymap[u] = true;
                std::cout << "0" << std::endl;
            }
            else if(instruction == "logout"){
                std::string u = ans[++i];
                auto data = mymap.find(u);
                if(data == mymap.end() || system.usersystem.logout(u) == false){
                    std::cout << "-1" << std::endl;
                    continue;
                }
                mymap.erase(data);
                std::cout << "0" << std::endl;
            }
            else if(instruction == "query_profile"){
                std::string c, u;
                while (i < thesize) {
                    if (ans[i] == "-c") c = ans[++i];
                    else if (ans[i] == "-u") u = ans[++i];
                    ++i;
                }
                if(mymap.find(c) == mymap.end()){
                    std::cout << "-1\n";
                    continue;
                }
                auto query_user = system.usersystem.query_profile(c, u);
                if (!query_user.first) std::cout << "-1\n";
                else std::cout << query_user.second;
            }
            else if(instruction == "modify_profile"){
                std::string c, u, p, n, m;
                int g = -1;
                while (i < thesize) {
                    if (ans[i] == "-c") c = ans[++i];
                    else if (ans[i] == "-u") u = ans[++i];
                    else if (ans[i] == "-p") p = ans[++i];
                    else if (ans[i] == "-n") n = ans[++i];
                    else if (ans[i] == "-m") m = ans[++i];
                    else if (ans[i] == "-g") {
                        g = command.stringToint(ans[++i]);
                    }
                    ++i;
                }
                if(mymap.find(c) == mymap.end()){
                    std::cout << "-1" << std::endl; 
                    continue;
                }
                auto find_user = system.usersystem.modify_profile(c, u, p, n, m, g);
                if(find_user.first) std::cout << find_user.second;
                else std::cout << "-1" << std::endl; 
            }
            else if(instruction == "add_train"){
                std::string s, p, x, t, o, d, y, index;
                int n, m;
                while(i < thesize){
                    if(ans[i] == "-s") s = ans[++i];
                    else if(ans[i] == "-p") p = ans[++i];
                    else if(ans[i] == "-x") x = ans[++i];
                    else if(ans[i] == "-t") t = ans[++i];
                    else if(ans[i] == "-o") o = ans[++i];
                    else if(ans[i] == "-d") d = ans[++i];
                    else if(ans[i] == "-i") index = ans[++i];
                    else if(ans[i] == "-n") n = command.stringToint(ans[++i]);
                    else if(ans[i] == "-m") m = command.stringToint(ans[++i]);
                    else if(ans[i] == "-y") y = ans[++i];
                    ++i;
                }
                mystring my_i(index);
                char my_y = y[0];
                if(system.trainsystem.add_train(my_i, n, m, s, p, x, t, o, d, my_y)) std::cout << "0" << std::endl;
                else std::cout << "-1" << std::endl; 
            }
            else if(instruction == "delete_train"){
                std::string id(ans[++i]);
                mystring my_id(id);
                if(system.trainsystem.delete_train(my_id)) std::cout << "0" << std::endl;
                else std::cout << "-1" << std::endl; 
            }
            else if(instruction == "release_train"){
                std::string id(ans[++i]);
                mystring my_id(id);
                if(system.trainsystem.release_train(my_id)) std::cout << "0" << std::endl;
                else std::cout << "-1" << std::endl; 
            }
            else if(instruction == "query_train"){
                std::string id, d;
                while(i < thesize){
                    if(ans[i] == "-i") id = ans[++i];
                    else if(ans[i] == "-d") d = ans[++i];
                    i++;
                }
                mystring my_id(id);
                int my_d = command.myprocess(d);
                auto ans = system.trainsystem.query_train(my_id, d);
                system.trainsystem.print_train(ans, my_d);
            }
            else if(instruction == "query_ticket"){
                std::string s, t, d, p;
                int order;
                while(i < thesize){
                    if(ans[i] == "-s") s = ans[++i];
                    else if(ans[i] == "-t") t = ans[++i];
                    else if(ans[i] == "-d") d = ans[++i];
                    else if(ans[i] == "-p") {
                        p = ans[++i];
                        if(p == "time") order = 1;
                        if(p == "cost") order = 2;
                    }
                    i++;
                }
                auto ticket_vector = system.trainsystem.query_ticket(s, t, d, order);
                int ticket_vector_size = ticket_vector.size();
                std::cout << ticket_vector_size << std::endl;
                for(int i = 0; i < ticket_vector_size; ++i){
                    system.trainsystem.print_ticket(ticket_vector[i]);
                    std::cout << std::endl;
                }
            }
            else if(instruction == "query_transfer"){
                std::string s, t, d, p;
                int order;
                while(i < thesize){
                    if(ans[i] == "-s") s = ans[++i];
                    else if(ans[i] == "-t") t = ans[++i];
                    else if(ans[i] == "-d") d = ans[++i];
                    else if(ans[i] == "-p") {
                        p = ans[++i];
                        if(p == "time") order = 1;
                        if(p == "cost") order = 2;
                    }
                    i++;
                }
                auto the_ans = system.trainsystem.query_transfer(s, t, d, order);
                if(!the_ans.first) std::cout << "0" << std::endl;
                else{
                    system.trainsystem.print_ticket(the_ans.second.first);
                    std::cout << std::endl;
                    system.trainsystem.print_ticket(the_ans.second.second);
                    std::cout << std::endl;
                }
            }
            else if(instruction == "buy_ticket"){
                std::string u, id, d, f, t, q;
                int n;
                bool my_q = false;
                while(i < thesize){
                    if(ans[i] == "-u") u = ans[++i];
                    else if(ans[i] == "-i") id = ans[++i];
                    else if(ans[i] == "-d") d = ans[++i];
                    else if(ans[i] == "-f") f = ans[++i];
                    else if(ans[i] == "-t") t = ans[++i];
                    else if(ans[i] == "-n") n = command.stringToint(ans[++i]);
                    else if(ans[i] == "-q"){
                        q = ans[++i];
                        if(q == "true") my_q = true;
                        if(q == "false") my_q = false;
                    }
                    i++;
                }
                if(mymap.find(u) == mymap.end()){
                    std::cout << "-1" << std::endl;
                    continue;
                }
                int time_stamp_in = command.stringToint(time_stamp);
                int the_ans = system.buy_ticket(u, id, d, n, f, t, my_q, time_stamp_in);
                if(the_ans == -1) std::cout << "-1" << std::endl;
                else if(the_ans == 0) std::cout << "queue" << std::endl;
                else std::cout << the_ans << std::endl;
            }
            else if(instruction == "query_order"){
                std::string u(ans[++i]);
                if(mymap.find(u) == mymap.end()){
                    std::cout << "-1" << std::endl;
                    continue;
                }
                auto the_ans = system.query_order(u);
                if(!the_ans.first) std::cout << "-1" << std::endl;
                else{
                    std::cout << the_ans.second.size() << std::endl;
                    for(int j = the_ans.second.size() - 1; j >= 0; --j){
                        system.print_order(the_ans.second[j]);
                        std::cout << std::endl;
                    }
                }
            }
            else if(instruction == "refund_ticket"){
                std::string u;
                int n = 1;
                while(i < thesize){
                    if(ans[i] == "-u") u = ans[++i];
                    else if(ans[i] == "-n") n = command.stringToint(ans[++i]);
                    i++;
                }
                if(mymap.find(u) == mymap.end()){
                    std::cout << "-1" << std::endl;
                    continue;
                }
                if(system.refund_ticket(u, n)) std::cout << "0" << std::endl;
                else std::cout << "-1" << std::endl;
            }
        }
    }
}
