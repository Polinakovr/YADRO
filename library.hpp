#ifndef COMPUTER_CLUB_HPP
#define COMPUTER_CLUB_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <set>
#include <queue>

class Time
{
public:
    static std::pair<int, int> gettime(const std::string &str);
    static void check_time(const std::string &str);
    static std::string format_time(const std::pair<int, int> &time);
    static std::pair<int, int> time_diff(const std::pair<int, int> &start, const std::pair<int, int> &end);
};

struct Table
{
    int number;
    int revenue = 0;
    std::string current_client;
    std::pair<int, int> occupied_since;
    std::pair<int, int> usage = {0, 0};
};

struct Event
{
    std::pair<int, int> time_event;
    int id;
    std::string client_name;
    int table_number = 0;
};

class Computer_club
{
private:
    std::ifstream file;
    int count_table;
    int price;
    std::vector<Event> events;
    std::vector<Table> tables;
    std::pair<int, int> time_work_start;
    std::pair<int, int> time_work_end;
    std::queue<std::string> waiting_queue;
    std::set<std::string> present_clients;
    void print_event(int id, const std::pair<int, int> &time, const std::string &client_name, int table_num);
    void calculate_revenue(Table &table, const std::pair<int, int> &end_time);

public:
    void print_error(const std::pair<int, int> &time, const std::string &str);
    void end_work();
    void handle_client_arrival(const Event &n);
    void handle_client_seat(const Event &event);
    void handle_client_wait(const Event &n);
    void handle_client_leaves(const Event &event);
    void read_file(const std::string &filename);
};

#endif