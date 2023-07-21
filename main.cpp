#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

struct Event {
    string time;
    int id;
    string name;
    int table;
};

struct Table {
    std::string client;
    std::string start_time;
    std::string time_spent;
    int revenue;
};

string timeSum(string time1, string time2) {
    if (time1 == ""){
        time1 = "00:00";
    }
    int hours1 = stoi(time1.substr(0, 2));
    int minutes1 = stoi(time1.substr(3, 2));
    int hours2 = stoi(time2.substr(0, 2));
    int minutes2 = stoi(time2.substr(3, 2));

    int totalMinutes = (hours1 + hours2) * 60 + minutes1 + minutes2;
    int hours = totalMinutes / 60;
    int minutes = totalMinutes % 60;

    string result = "";
    if (hours < 10) {
        result += "0";
    }
    result += to_string(hours);
    result += ":";
    if (minutes < 10) {
        result += "0";
    }
    result += to_string(minutes);

    return result;
}

int timeToMinutes(string time) {
    int hours = stoi(time.substr(0, 2));
    int minutes = stoi(time.substr(3, 2));
    return hours * 60 + minutes;
}

string minutesToTime(int minutes) {
    int hours = minutes / 60;
    int mins = minutes % 60;
    return (hours < 10 ? "0" : "") + to_string(hours) + ":" + (mins < 10 ? "0" : "") + to_string(mins);
}

string timeDifference(string time1, string time2) {

    int minutes1 = timeToMinutes(time1);
    int minutes2 = timeToMinutes(time2);
    int diff = abs(minutes1 - minutes2);
    return minutesToTime(diff);
}

int main(int argc, char *argv[]) {
    std::ifstream file(argv[1]);
    int num_tables;
    std::string start_time, end_time;
    double cost_per_hour;
    file >> num_tables >> start_time >> end_time >> cost_per_hour;

    vector<Table> tables(num_tables);
    map<std::string, int> clients;
    std::vector<Event> events;
    vector<string> queue;

    std::string line;
    while (std::getline(file, line)) {
        Event event;
        file >> event.time >> event.id >> event.name;
        if (event.id == 2) {
            file >> event.table;
        }
        events.push_back(event);
    }
    cout << start_time << endl;

    for (const auto &event: events) {
        cout << event.time << ' ' << event.id << ' ' << event.name;
        if (event.id == 1) {
            if (clients[event.name]) {
                cout << endl << event.time << " 13 YouShallNotPass" << endl;
                continue;
            }
            if (stoi(event.time.substr(0, 2)) < stoi(start_time.substr(0, 2))) {
                cout << endl << event.time << " 13 NotOpenYet" << endl;
                continue;
            }
            clients[event.name] = -1;

        } else if (event.id == 2) {
            cout << ' ' << event.table;
            if (clients[event.name] == event.table) {
                cout << event.time << " 13 PlaceIsBusy" << endl;
                continue;
            }
            if (clients[event.name] != -1) {
                std::string start = tables[clients[event.name] - 1].start_time;
                std::string end = event.time;
                std::string timespent = timeDifference(start, end);
                tables[clients[event.name] - 1].time_spent = timeSum(tables[clients[event.name] - 1].time_spent,
                                                                     timespent);
                int hours = stoi(timespent.substr(0, 2));
                if (stoi(timespent.substr(3, 2))) {
                    hours++;
                }
                tables[clients[event.name] - 1].revenue += hours * cost_per_hour;
                tables[clients[event.name] - 1].client.clear();
                clients[event.name] = -1;
                if (not(queue.empty())) {
                    tables[event.table - 1].client = queue[0];
                    tables[event.table - 1].start_time = event.time;
                    clients[queue[0]] = event.table;
                    queue[0].erase();
                }
            }
            if (tables[event.table - 1].client.empty()) {
                tables[event.table - 1].client = event.name;
                tables[event.table - 1].start_time = event.time;
                clients[event.name] = event.table;
            } else {
                cout << endl << event.time << " 13 PlaceIsBusy" << endl;
                continue;
            }
        } else if (event.id == 3) {
            for (int i = 0; i <= size(tables); i++) {
                if (tables[i].client.empty()) {
                    cout << endl << event.time << " 13 ICanWaitNoLonger!";
                    break;
                }
            }
            queue.push_back(event.name);
            if (queue.size() > num_tables) {
                queue.erase(queue.end());
                clients.erase(event.name);
            }
        } else if (event.id == 4) {
            if (clients[event.name] != -1) {
                std::string start = tables[clients[event.name] - 1].start_time;
                std::string end = event.time;
                std::string timespent = timeDifference(start, end);
                tables[clients[event.name] - 1].time_spent = timeSum(tables[clients[event.name] - 1].time_spent,
                                                                     timespent);
                int hours = stoi(timespent.substr(0, 2));
                if (stoi(timespent.substr(3, 2))) {
                    hours++;
                }
                tables[clients[event.name] - 1].revenue += hours * cost_per_hour;
                tables[clients[event.name] - 1].client.clear();
                int now_table = clients[event.name];
                clients[event.name] = -1;
                if (count(queue.begin(), queue.end(), event.name) > 0) {
                    erase(queue, event.name);
                }
                if (not(queue.empty())) {
                    tables[now_table - 1].client = queue[0];
                    tables[now_table - 1].start_time = event.time;
                    clients[queue[0]] = now_table;
                    cout << endl << event.time << " 12 " << queue[0] << ' ' << now_table << endl;
                    queue.erase(queue.begin());
                    continue;
                }
            } else {
                cout << endl << event.time << " 13 ClientUnknown" << endl;
                continue;
            }
        }
        cout << endl;
    }

    for (const auto &client: clients) {
        if (client.second != -1) {
            cout << end_time << " 11 " << client.first << endl;
            std::string start = tables[client.second - 1].start_time;
            std::string timespent = timeDifference(start, end_time);
            tables[client.second - 1].time_spent = timeSum(tables[client.second - 1].time_spent, timespent);
            int hours = stoi(timespent.substr(0, 2));
            if (stoi(timespent.substr(3, 2))) {
                hours++;
            }
            tables[client.second - 1].revenue += hours * cost_per_hour;
            tables[client.second - 1].client.clear();
        }
    }
    cout << end_time << endl;

    for (int i = 0; i < tables.size(); i++) {
        std::cout << i + 1 << ' ' << tables[i].revenue << ' ' << tables[i].time_spent << endl;
    }
    cin >> cost_per_hour;
    return 0;
}
