//
// Created by rene on 23. 11. 2020.
//

#ifndef IMS_DATACOLLECTOR_H
#define IMS_DATACOLLECTOR_H

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <random>
#include <cmath>
#include <map>
#include "Worker.h"

using namespace std;

class DataCollector {
public:
    explicit DataCollector();

    void increment_total_infected();

    int total_infected;

    int initial_S;

    void increment_dead();

    int total_dead;

    void increment_initial_S();

    void update_data(Worker *pWorker);

    map<string, int> current_data;

    vector<int> mild;
    vector<int> severe;
    vector<int> asymptomatic;
    vector<int> death;
    vector<int> S;
    vector<int> I;
    vector<int> R;
    vector<int> WM;
    vector<int> SD;
    map<string, vector<int>> data_history;

    map<string, int> adv_infection_data;

    map<string, vector<int>> adv_infection_data_history;

    void update_adv_infection_data(Worker *pWorker);

    void reset(int timestamp, bool last);

//    void add_lifetime_infected(int i, map<string, int> map);

    int save_experiment;

    int basic_to_print;

    void reset_data_options(bool hist);

    int lifetime_infected_bin_size;

    map<string, int> lifetime_infected_bin_avgs[ITERATIONS];

    map<string, int> last_bin_avgs;

    void add_lifetime_infected(int i, map<string, int> map);

    vector<map<string, int>> current_bin_lifetime_infected;

    void increment_initial_infected();

    int initial_infected;

    void increment_newly_infected();

    int newly_infected;

    int get_total_infected();

    void add_lifetime_infected(Worker *worker);

    void increment_initial_recovered();

    int initial_recovered;
};



#endif //IMS_DATACOLLECTOR_H
