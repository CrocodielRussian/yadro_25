#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

class Time {
    private:
        int hours, minutes;
    public:
        Time() : hours(0), minutes(0) {}

        Time(const std::string& time_str) {
            if (time_str.size() != 5 || time_str[2] != ':') throw std::runtime_error("Invalid time format");
            hours = std::stoi(time_str.substr(0, 2));
            minutes = std::stoi(time_str.substr(3, 2));
            if (hours > 23 || minutes > 59) throw std::runtime_error("Invalid time value");
        }
        ~Time(){}

        bool operator<(const Time& other) const {
            return hours * 60 + minutes < other.hours * 60 + other.minutes;
        }

        bool operator>=(const Time& other) const {
            return hours * 60 + minutes >= other.hours * 60 + other.minutes;
        }

        std::string to_string() const {
            std::ostringstream oss;
            oss << std::setfill('0') << std::setw(2) << hours << ":" << std::setw(2) << minutes;
            return oss.str();
        }

        int diff_in_minutes(const Time& other) const {
            return (hours - other.hours) * 60 + (minutes - other.minutes);
        }
};

struct Event {
    Time time;
    int id;
    std::string client;
    int table = 0;
    Event(const std::string& line, int line_num) {
        std::istringstream iss(line);
        std::string time_str;
        if (!(iss >> time_str >> id)) {
            throw std::runtime_error("Invalid event format at line " + std::to_string(line_num));
        }
        try {
            time = Time(time_str);
        } catch (...) {
            throw std::runtime_error("Invalid time format at line " + std::to_string(line_num));
        }
        if (id < 1 || id > 4) {
            throw std::runtime_error("Invalid event ID at line " + std::to_string(line_num));
        }
        if (!(iss >> client)) {
            throw std::runtime_error("Invalid client name at line " + std::to_string(line_num));
        }
        for (char c : client) {
            if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' || c == '-')) {
                throw std::runtime_error("Invalid client name at line " + std::to_string(line_num));
            }
        }
        if (id == 2) {
            if (!(iss >> table) || table <= 0) {
                throw std::runtime_error("Invalid table number at line " + std::to_string(line_num));
            }
        }
    }
    std::string to_string() const {
        std::ostringstream oss;
        oss << time.to_string() << " " << id << " " << client;
        if (id == 2 || id == 12) oss << " " << table;
        return oss.str();
    }
};

struct Table {
    bool occupied = false;
    std::string client;
    Time start_time;
    int total_minutes = 0;
    int revenue = 0;
};

class ComputerClub {
    private:
        int num_tables, hourly_rate;
        Time start_time, end_time;
        std::vector<Table> tables;
        std::map<std::string, bool> clients_in_club;
        std::queue<std::string> waiting_queue;
        std::vector<std::string> events_log;

        void log_event(const std::string& event) {
            events_log.push_back(event);
        }

        void log_error(const Time& time, const std::string& error) {
            std::ostringstream oss;
            oss << time.to_string() << " 13 " << error;
            log_event(oss.str());
        }

        void update_table_usage(int table_idx, const Time& end) {
            if (tables[table_idx].occupied) {
                int minutes = end.diff_in_minutes(tables[table_idx].start_time);
                tables[table_idx].total_minutes += minutes;
                int hours = (minutes + 59) / 60;
                tables[table_idx].revenue += hours * hourly_rate;
            }
        }

        void assign_table(const Time& time, const std::string& client, int table_idx) {
            tables[table_idx].occupied = true;
            tables[table_idx].client = client;
            tables[table_idx].start_time = time;
            std::ostringstream oss;
            oss << time.to_string() << " 12 " << client << " " << (table_idx + 1);
            log_event(oss.str());
        }

        void process_client_leaving(const Time& time, const std::string& client, bool is_end_of_day = false) {
            clients_in_club[client] = false;
            for (size_t i = 0; i < tables.size(); i++) {
                if (tables[i].occupied && tables[i].client == client) {
                    update_table_usage(i, time);
                    tables[i].occupied = false;
                    tables[i].client.clear();
                    if (!waiting_queue.empty()) {
                        std::string next_client = waiting_queue.front();
                        waiting_queue.pop();
                        assign_table(time, next_client, i);
                    }
                }
            }
            if (!is_end_of_day) {
                std::ostringstream oss;
                oss << time.to_string() << " 11 " << client;
                log_event(oss.str());
            }
        }

    public:
        ComputerClub(const std::string& filename) {
            std::ifstream file(filename);
            if (!file.is_open()) throw std::runtime_error("Cannot open file");

            std::string line;
            int cnt_line = 0;

            if (!std::getline(file, line)) throw std::runtime_error("Missing number of tables");
            cnt_line++;
            std::istringstream iss(line);
            if (!(iss >> num_tables) || num_tables <= 0) {
                throw std::runtime_error("Invalid number of tables at line 1");
            }
            tables.resize(num_tables);

            if (!std::getline(file, line)) throw std::runtime_error("Missing time configuration");
            cnt_line++;
            iss.clear();
            iss.str(line);
            std::string start_str, end_str;
            if (!(iss >> start_str >> end_str)) {
                throw std::runtime_error("Invalid time format at line 2");
            }
            try {
                start_time = Time(start_str);
                end_time = Time(end_str);
            } catch (...) {
                throw std::runtime_error("Invalid time format at line 2");
            }

            if (!std::getline(file, line)) throw std::runtime_error("Missing hourly rate");
            cnt_line++;
            iss.clear();
            iss.str(line);
            if (!(iss >> hourly_rate) || hourly_rate <= 0) {
                throw std::runtime_error("Invalid hourly rate at line 3");
            }
            
            Time last_time("00:00");
            while (file >> line) {
                cnt_line++;
                if (line.empty()) continue;
                try {
                    Event event(line, cnt_line);
                    if (event.time < last_time) {
                        throw std::runtime_error("Events out of chronological order at line " + std::to_string(cnt_line));
                    }
                    last_time = event.time;
                    process_event(event);
                } catch (const std::runtime_error& e) {
                    throw e;
                }
            }

            std::vector<std::string> remaining_clients;
            for (const auto& client : clients_in_club) {
                if (client.second) remaining_clients.push_back(client.first);
            }
            std::sort(remaining_clients.begin(), remaining_clients.end());
            for (const auto& client : remaining_clients) {
                process_client_leaving(end_time, client, true);
                std::ostringstream oss;
                oss << end_time.to_string() << " 11 " << client;
                log_event(oss.str());
            }
        }

        ~ComputerClub(){}

        void process_event(const Event& event){
            log_event(event.to_string());

            if (event.time < start_time || event.time >= end_time) {
                log_error(event.time, "NotOpenYet");
                return;
            }

            switch (event.id) {
                case 1: { 
                    if (clients_in_club[event.client]) {
                        log_error(event.time, "YouShallNotPass");
                    } else {
                        clients_in_club[event.client] = true;
                    }
                    break;
                }
                case 2: {
                    if (!clients_in_club[event.client]) {
                        log_error(event.time, "ClientUnknown");
                        break;
                    }
                    if (event.table < 1 || event.table > num_tables) {
                        log_error(event.time, "PlaceIsBusy");
                        break;
                    }
                    int table_idx = event.table - 1;
                    if (tables[table_idx].occupied) {
                        log_error(event.time, "PlaceIsBusy");
                        break;
                    }
                    for (size_t i = 0; i < tables.size(); i++) {
                        if (tables[i].occupied && tables[i].client == event.client) {
                            update_table_usage(i, event.time);
                            tables[i].occupied = false;
                            tables[i].client.clear();
                            if (!waiting_queue.empty()) {
                                std::string next_client = waiting_queue.front();
                                waiting_queue.pop();
                                assign_table(event.time, next_client, i);
                            }
                        }
                    }
                    assign_table(event.time, event.client, table_idx);
                    break;
                }
                case 3: {
                    if (!clients_in_club[event.client]) {
                        log_error(event.time, "ClientUnknown");
                        break;
                    }
                    bool has_free_table = false;
                    for (const auto& table : tables) {
                        if (!table.occupied) {
                            has_free_table = true;
                            break;
                        }
                    }
                    if (has_free_table) {
                        log_error(event.time, "ICanWaitNoLonger!");
                        break;
                    }
                    if (waiting_queue.size() >= static_cast<size_t>(num_tables)) {
                        process_client_leaving(event.time, event.client);
                    } else {
                        waiting_queue.push(event.client);
                    }
                    break;
                }
                case 4: {
                    if (!clients_in_club[event.client]) {
                        log_error(event.time, "ClientUnknown");
                        break;
                    }
                    process_client_leaving(event.time, event.client);
                    break;
                }
            }
        }

        void print_report(){
            std::cout << start_time.to_string() << '\n';
            for (const auto& event : events_log) {
                std::cout << event << '\n';
            }
            std::cout << end_time.to_string() << '\n';
            for (size_t i = 0; i < tables.size(); i++) {
                int hours = tables[i].total_minutes / 60;
                int minutes = tables[i].total_minutes % 60;
                std::cout << (i + 1) << " " << tables[i].revenue << " "
                        << std::setfill('0') << std::setw(2) << hours << ":"
                        << std::setw(2) << minutes << '\n';
            }
        }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    try {
        ComputerClub club(argv[1]);
        club.print_report();
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}