#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
class Time
{
private:
public:
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
    }
};
class Computer_club
{
private:
    std::ifstream file;
    int count_table;
    int price;
    std::vector<std::string> events;

public:
    void read_file(const std::string &filename)
    {

        std::string line;
        std::string start_time;
        std::string end_time;
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
            if (!std::getline(file, line))
                throw std::runtime_error("Не удалось считать данные");
            std::istringstream iss(line);
            if (!(iss >> start_time >> end_time))
                throw std::invalid_argument("Неверный формат времени работы, время нужно указывать через пробел");
            Time::check_time(start_time);
            Time::check_time(end_time);
            if (!std::getline(file, line))
                throw std::runtime_error("Не удалось считать данные");
            price = std::stoi(line);
            if (price <= 0)
                throw std::invalid_argument("Цена не может быть меньше 0");
            while (std::getline(file, line))
            {
                events.push_back(line)
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Ошибка: " << e.what() << std::endl;
            exit(1);
        }
    }
    void processing()
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