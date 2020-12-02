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
    /*
     * Konstruktor
     */
    explicit DataCollector();

    /*
     * Updatuje aktualne data o stave vo fabrike
     */
    void update_data(Worker *pWorker);

    /*
     * Znova inicializuje premenne s datami na prazdne
     */
    void reset_data_options(bool hist);

    /*
     * Vrati pocet vsetkych nakazenych
     */
    int get_total_infected();

    /*
     * Zvysi pocet prvotnych nachylnych
     */
    void increment_initial_S();

    /*
     * Updatuje pokrocile informacie infikovanych
     */
    void update_adv_infection_data(Worker *pWorker);

    /*
     * Zvysi pocet mrtvych
     */
    void increment_dead();

    /*
     * Reset medzi iteraciami
     */
    void reset(int timestamp, bool last);

    /*
     * Zvysi pocet prvotnych infikovanych
     */
    void increment_initial_infected();

    /*
     * Zvysi pocet novych infikovanych
     */
    void increment_newly_infected();

    int total_infected;

    int initial_S;

    int total_dead;

    map<string, int> current_data;

    map<string, vector<int>> data_history;

    map<string, int> adv_infection_data;

    map<string, vector<int>> adv_infection_data_history;

    int initial_infected;

    int newly_infected;

};

#endif //IMS_DATACOLLECTOR_H
