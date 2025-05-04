#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <map>
#include <set>
class Time
{
private:
public:
    static std::pair<int, int> gettime(const std::string &str)
    {
        check_time(str);
        int hours = std::stoi(str.substr(0, 2));
        int minutes = std::stoi(str.substr(3, 2));
        if (hours < 0 || hours > 23)
        {
            throw std::invalid_argument("Часы должны быть между 00 и 23");
        }

        if (minutes < 0 || minutes > 59)
        {
            throw std::invalid_argument("Минуты должны быть между 00 и 59");
        }
        return {hours, minutes};
    }
    static void check_time(const std::string &str)
    {
        if (str.length() != 5 || str[2] != ':')
        {
            throw std::invalid_argument("Время должно быть в формате XX:XX");
        }

        for (int i : {0, 1, 3, 4})
        {
            if (!isdigit(str[i]))
            {
                throw std::invalid_argument("Время должно содержать только цифры и двоеточие");
            }
        }
    }
};
struct Table
{
    int number;
    int revenue = 0;
    std::string current_client;
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

public:
    void handle_client_arrival(const Event &n)
    {
        for (const auto &c : events)
        {
            if (c.client_name == n.client_name)
            {
                throw std::runtime_error("YouShallNotPass");
            }
        }
        if (n.time_event.first < time_work_start.first ||
            (n.time_event.first == time_work_start.first &&
             n.time_event.second < time_work_start.second) ||
            n.time_event.first > time_work_end.first ||
            (n.time_event.first == time_work_end.first &&
             n.time_event.second > time_work_end.second))
        {
            throw std::invalid_argument("NotOpenYet");
        }
    }
    void handle_client_seat(const Event &event)
    {
        bool client_exists = false;
        bool client_already_seated = false;
        int current_table = 0;

        for (const auto &e : events)
        {
            if (e.client_name == event.client_name)
            {
                client_exists = true;
                if (e.id == 2)
                {
                    client_already_seated = true;
                    current_table = e.table_number;
                }
            }
        }

        if (!client_exists)
        {
            throw std::runtime_error("ClientUnknown");
        }

        if (event.table_number < 1 || event.table_number > count_table)
        {
            throw std::invalid_argument("Неверный номер стола");
        }

        for (auto &table : tables)
        {
            if (table.number == event.table_number)
            {
                if (!table.current_client.empty() && table.current_client != event.client_name)
                {
                    throw std::runtime_error("PlaceIsBusy");
                }
            }
        }

        if (client_already_seated)
        {
            for (auto &table : tables)
            {
                if (table.number == current_table)
                {
                    table.current_client.clear();
                    break;
                }
            }
        }

        for (auto &table : tables)
        {
            if (table.number == event.table_number)
            {
                table.current_client = event.client_name;
                break;
            }
        }
    }
    void handle_client_wait(const struct Event &n)
    {
        for (auto &table : tables)
        {
            if (table.current_client == " ")
            {
                throw std::runtime_error("ICanWaitNoLonger!");
            }
        }
        int count = 0;
        for (auto &ev : events)
        {
            if (ev.id == 3)
            {
                count++;
            }
        }
        if (count > count_table)
        {
            // клиент уходит и генереруется id11
        }
    }
    void handle_client_leaves(const Event &event)
    { // доделать
        for (const auto &e : events)
        {
            if (event.client_name != e.client_name)
            {
                throw std::runtime_error("ClientUnknown");
            }
        }
    }
    void read_file(const std::string &filename)
    {
        std::pair<int, int> timecurrent = {0, 0};
        std::string line;
        file.open(filename);
        try
        {
            if (!file.is_open())
            {
                throw std::runtime_error("Не удалось открыть файл");
            }

            if (!std::getline(file, line))
            {
                throw std::runtime_error("Не удалось считать данные");
            }
            count_table = std::stoi(line);
            if (count_table <= 0)
                throw std::invalid_argument("Количество столов не может быть меньше 1");
            tables.resize(count_table);
            for (int i = 0; i < count_table; ++i)
            {
                tables[i].number = i + 1;
            }
            if (!std::getline(file, line))
                throw std::runtime_error("Не удалось считать данные");
            std::istringstream time_iss(line);
            std::string start_str, end_str;
            if (!(time_iss >> start_str >> end_str))
                throw std::invalid_argument("Неверный формат времени работы");
            time_work_start = Time::gettime(start_str);
            time_work_end = Time::gettime(end_str);
            if (!std::getline(file, line))
                throw std::runtime_error("Не удалось считать данные");
            price = std::stoi(line);
            if (price <= 0)
                throw std::invalid_argument("Цена не может быть меньше 0");
            while (std::getline(file, line))
            {
                if (line.empty())
                    continue;

                Event event;
                std::istringstream iss(line);
                std::string time_str;

                if (!(iss >> time_str >> event.id))
                {
                    throw std::invalid_argument("Неверный формат события");
                }

                Time::check_time(time_str);
                event.time_event = Time::gettime(time_str);
                if (event.time_event.first < timecurrent.first ||
                    (event.time_event.first == timecurrent.first &&
                     event.time_event.second < timecurrent.second))
                {
                    throw std::invalid_argument("События должны идти последовательно во времени");
                }

                if (event.id == 1 || event.id == 3 || event.id == 4)
                {
                    if (!(iss >> event.client_name))
                    {
                        throw std::invalid_argument("Не указано имя клиента");
                    }
                }
                else if (event.id == 2)
                {
                    if (!(iss >> event.client_name >> event.table_number))
                    {
                        throw std::invalid_argument("Неверный формат события для ID 2");
                    }
                    if (event.table_number > count_table || event.table_number < 1)
                    {
                        throw std::invalid_argument("Неверный номер стола");
                    }
                }
                else
                {
                    throw std::invalid_argument("Неизвестный ID события");
                }

                for (char c : event.client_name)
                {
                    if (!(islower(c) || isdigit(c) || c == '_' || c == '-'))
                    {
                        throw std::invalid_argument("Неправильное имя клента");
                    }
                }
                timecurrent = event.time_event;
                events.push_back(event);

                switch (event.id)
                {
                case 1:
                    handle_client_arrival(event);
                    break;
                case 2:
                    handle_client_seat(event);
                    break;
                case 3:
                    handle_client_leaves(event);
                    break;
                case 4:
                    break;
                default:
                    break;
                }
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Ошибка: " << e.what() << std::endl;
            exit(1);
        }
    }
};

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Ошибка, добавьте файл для чтения \n";
        return 1;
    }
    Computer_club club;
    club.read_file(argv[1]);
}