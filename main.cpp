#include "library.hpp"

std::pair<int, int> Time::gettime(const std::string &str)
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

void Time::check_time(const std::string &str)
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

std::string Time::format_time(const std::pair<int, int> &time)
{
    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", time.first, time.second);
    return std::string(buffer);
}
std::pair<int, int> Time::time_diff(const std::pair<int, int> &start, const std::pair<int, int> &end)
{
    int start_min = start.first * 60 + start.second;
    int end_min = end.first * 60 + end.second;
    int diff_min = end_min - start_min;

    if (diff_min < 0)
    {
        diff_min += 24 * 60;
    }
    return {diff_min / 60, diff_min % 60};
}

void Computer_club::print_event(int id, const std::pair<int, int> &time, const std::string &client_name, int table_num = 0)
{
    std::cout << Time::format_time(time) << " " << id << " " << client_name;
    if (table_num != 0)
    {
        std::cout << " " << table_num;
    }
    std::cout << std::endl;
}

void Computer_club::calculate_revenue(Table &table, const std::pair<int, int> &end_time)
{
    if (table.occupied_since.first == 0 && table.occupied_since.second == 0)
        return;

    auto diff = Time::time_diff(table.occupied_since, end_time);

    table.usage.first += diff.first;
    table.usage.second += diff.second;
    if (table.usage.second >= 60)
    {
        table.usage.first += table.usage.second / 60;
        table.usage.second %= 60;
    }

    int hours = diff.first + (diff.second > 0 ? 1 : 0);
    table.revenue += hours * price;
}

void Computer_club::print_error(const std::pair<int, int> &time, const std::string &str)
{
    std::cout << Time::format_time(time) << " 13 " << str << std::endl;
}

void Computer_club::end_work()
{
    std::vector<std::string> remaining_clients(present_clients.begin(), present_clients.end());
    std::sort(remaining_clients.begin(), remaining_clients.end());

    for (const auto &client : remaining_clients)
    {
        print_event(11, time_work_end, client);

        for (auto &table : tables)
        {
            if (table.current_client == client)
            {
                calculate_revenue(table, time_work_end);
                table.current_client.clear();
                table.occupied_since = {0, 0};
            }
        }
    }

    for (auto &table : tables)
    {
        std::cout << table.number << " " << table.revenue << " "
                  << Time::format_time(table.usage) << std::endl;
    }
}

void Computer_club::handle_client_arrival(const Event &n)
{
    if (present_clients.find(n.client_name) != present_clients.end())
    {
        print_error(n.time_event, "YouShallNotPass");
        return;
    }

    if (n.time_event.first < time_work_start.first ||
        (n.time_event.first == time_work_start.first &&
         n.time_event.second < time_work_start.second) ||
        n.time_event.first > time_work_end.first ||
        (n.time_event.first == time_work_end.first &&
         n.time_event.second > time_work_end.second))
    {
        print_error(n.time_event, "NotOpenYet");
        return;
    }

    present_clients.insert(n.client_name);
}

void Computer_club::handle_client_seat(const Event &event)
{
    if (present_clients.find(event.client_name) == present_clients.end())
    {
        print_error(event.time_event, "ClientUnknown");
        return;
    }

    if (event.table_number < 1 || event.table_number > count_table)
    {
        print_error(event.time_event, "Неверный номер стола");
        return;
    }

    Table &target_table = tables[event.table_number - 1];

    if (!target_table.current_client.empty() && target_table.current_client != event.client_name)
    {
        print_error(event.time_event, "PlaceIsBusy");
        return;
    }

    for (auto &table : tables)
    {
        if (table.current_client == event.client_name && table.number != event.table_number)
        {
            calculate_revenue(table, event.time_event);
            table.current_client.clear();
            break;
        }
    }

    target_table.current_client = event.client_name;
    target_table.occupied_since = event.time_event;
}

void Computer_club::handle_client_wait(const Event &n)
{
    bool has_free_tables = false;
    for (const auto &table : tables)
    {
        if (table.current_client.empty())
        {
            has_free_tables = true;
            break;
        }
    }

    if (has_free_tables)
    {
        print_error(n.time_event, "ICanWaitNoLonger!");
        return;
    }

    if (waiting_queue.size() >= count_table)
    {
        present_clients.erase(n.client_name);
        print_event(11, n.time_event, n.client_name);
        return;
    }

    waiting_queue.push(n.client_name);
}

void Computer_club::handle_client_leaves(const Event &event)
{
    if (present_clients.find(event.client_name) == present_clients.end())
    {
        print_error(event.time_event, "ClientUnknown");
        return;
    }

    for (auto &table : tables)
    {
        if (table.current_client == event.client_name)
        {
            calculate_revenue(table, event.time_event);
            table.current_client.clear();
            table.occupied_since = {0, 0};
            present_clients.erase(event.client_name);

            if (!waiting_queue.empty())
            {
                std::string next_client = waiting_queue.front();
                waiting_queue.pop();
                table.current_client = next_client;
                table.occupied_since = event.time_event;
                print_event(12, event.time_event, next_client, table.number);
            }
            break;
        }
    }
}

void Computer_club::read_file(const std::string &filename)
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

        std::cout << Time::format_time(time_work_start) << std::endl;

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
                    throw std::invalid_argument("Неправильное имя клиента");
                }
            }

            timecurrent = event.time_event;

            std::cout << Time::format_time(event.time_event) << " " << event.id << " " << event.client_name;
            if (event.table_number != 0)
            {
                std::cout << " " << event.table_number;
            }
            std::cout << std::endl;

            switch (event.id)
            {
            case 1:
                handle_client_arrival(event);
                break;
            case 2:
                handle_client_seat(event);
                break;
            case 3:
                handle_client_wait(event);
                break;
            case 4:
                handle_client_leaves(event);
                break;
            default:
                break;
            }
        }

        end_work();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Ошибка, добавьте файл для чтения \n";
        return 1;
    }
    Computer_club club;
    club.read_file(argv[1]);
    return 0;
}