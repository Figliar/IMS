//
// Created by rene on 23. 11. 2020.
//

#include "DataCollector.h"


DataCollector::DataCollector() {

    DataCollector::reset_data_options(true);

    DataCollector::total_infected = 0;

    DataCollector::initial_infected = 0;

    DataCollector::initial_S = 0;

    DataCollector::total_dead = 0;

    DataCollector::newly_infected = 0;

    DataCollector::adv_infection_data = {
            {"total", 0},
            {"SD", 0},
            {"not_SD", 0}
    };

    DataCollector::adv_infection_data_history = {
            {"total", {}},
            {"SD", {}},
            {"not_SD", {}}
    };

}

void DataCollector::update_data(Worker *pWorker) {
    this->current_data["S"] += int(pWorker->susceptible);
    this->current_data["I"] += int(pWorker->infected);
    if(pWorker->infected)
        this->update_adv_infection_data(pWorker);
    this->current_data["R"] += int(pWorker->recovered);
    this->current_data["WM"] += int(pWorker->wear_mask);
    this->current_data["SD"] += int(pWorker->social_distance);

    if(pWorker->current_symptom_stage == "mild")
        this->current_data["mild"] += 1;
    if(pWorker->current_symptom_stage == "severe")
        this->current_data["severe"] += 1;
    if(pWorker->current_symptom_stage == "asymptomatic")
        this->current_data["asymptomatic"] += 1;
}

void DataCollector::reset_data_options(bool hist){
    this->current_data = {
            {"mild",         0},
            {"severe",       0},
            {"asymptomatic", 0},
            {"death",        0},
            {"S",            0},
            {"I",            0},
            {"R",            0},
            {"WM",           0},
            {"SD",           0},
    };
    if(hist) {
        this->data_history["mild"] = {};
        this->data_history["severe"] = {};
        this->data_history["asymptomatic"] = {};
        this->data_history["death"] = {};
        this->data_history["S"] = {};
        this->data_history["I"] = {};
        this->data_history["R"] = {};
        this->data_history["WM"] = {};
        this->data_history["SD"] = {};
    }
}

int DataCollector::get_total_infected() {
    this->total_infected = this->newly_infected + this->initial_infected;
    return this->total_infected;
}

void DataCollector::increment_initial_S() {
    this->initial_S +=1;
}

void DataCollector::update_adv_infection_data(Worker *pWorker) {
    this->adv_infection_data["total"] += 1;
    if(pWorker->social_distance)
        this->adv_infection_data["SD"] += 1;
    else
        this->adv_infection_data["not_SD"] += 1;
}

void DataCollector::increment_dead() {
    this->current_data["death"] += 1;
    this->total_dead += 1;
}

void DataCollector::reset(int timestamp, bool last) {

    for(auto & i : this->current_data) {
        this->data_history[i.first].push_back(i.second);
    }
    for(auto & i : this->adv_infection_data){
        this->adv_infection_data_history[i.first].push_back(i.second);
        this->adv_infection_data[i.first] = 0;
    }

    this->reset_data_options(false);

    if (last){
        double SAR = double(this->newly_infected) / double(this->initial_S);
        cout<<"Secondary Attack Rate (SAR): "<<this->newly_infected<<" / "<<this->initial_S<<" = "<<SAR<<endl;
    }
}

void DataCollector::increment_initial_infected() {
    this->initial_infected += 1;
}

void DataCollector::increment_newly_infected() {
    this->newly_infected += 1;
}
