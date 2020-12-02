//
// Created by rene on 23. 11. 2020.
//

#include "DataCollector.h"


DataCollector::DataCollector() {

    DataCollector::reset_data_options(true);

    DataCollector::total_infected = 0;

    DataCollector::initial_infected = 0;

    DataCollector::initial_S = 0;

    DataCollector::initial_recovered = 0;

    DataCollector::total_dead = 0;

    DataCollector::newly_infected = 0;

    DataCollector::lifetime_infected_bin_size = 5;

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

    DataCollector::lifetime_infected_bin_avgs;

    DataCollector::last_bin_avgs = {
            {"total", 0},
            {"SD", 0},
            {"not SD", 0},
            {"WM", 0},
            {"not WM", 0},
            {"both", 0},
            {"neither", 0}
    };

    // Uvidime
    DataCollector::current_bin_lifetime_infected;

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

void DataCollector::add_lifetime_infected(Worker *worker) {
    bool SD_D = worker->SD > worker->not_SD;
    bool WM_D = worker->WM > worker->not_WM;
    bool both = SD_D and WM_D;
    bool neither = not SD_D and not WM_D;
    this->current_bin_lifetime_infected.push_back({{"total", worker->num_people_infected},
                                                   {"SD", SD_D},
                                                   {"not_SD", !SD_D},
                                                   {"WM", WM_D},
                                                   {"not_WM", !WM_D},
                                                   {"both", both},
                                                   {"neither", neither},
                                                    });
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
//    if (timestamp % this->lifetime_infected_bin_size == 0 and timestamp!= 0){
//        this->lifetime_infected_bin_avgs[timestamp] = {};
//        if(this->current_bin_lifetime_infected.empty()){
//            for(auto & i : this->last_bin_avgs){
//                this->lifetime_infected_bin_avgs[timestamp][i.first] = i.second;
//            }
//        }
//        else{
//            for(auto & i : this->last_bin_avgs){
//                vector<int>bin_arr;
//                int sum = 0;
//                for(auto & dic : this->current_bin_lifetime_infected) {
//                    if (dic[i.first]) {
//                        bin_arr.push_back(dic["total"]);
//                        sum += dic["total"];
//                    }
//                }
//                if(!bin_arr.empty()){
//                    this->lifetime_infected_bin_avgs[timestamp][i.first] = this->last_bin_avgs[i.first];
//                    continue;
//                }
//                double bin_avg = double(sum) / (bin_arr.size());
//                this->lifetime_infected_bin_avgs[timestamp][i.first] = bin_avg;
//                this->last_bin_avgs[i.first] = bin_avg;
//            }
//        }
//        this->current_bin_lifetime_infected.clear();
//    }

    this->reset_data_options(false);

    if (last){
        double SAR = double(this->total_infected) / double(this->initial_S);
        cout<<"Secondary Attack Rate (SAR): "<<this->total_infected<<" / "<<this->initial_S<<" = "<<SAR<<endl;
//        vector<map<string, vector<int>>> RO_hist;
//        RO_hist.push_back({{"total", {}}, {"SD", {}}, {"not_SD", {}}, {"WM", {}}, {"not_WM", {}},
//                           {"both", {}}, {"neither", {}},
//                           });
//        vector<map<string, vector<int>>> ROS_hist;
//        ROS_hist.push_back({{"total", {}}, {"SD", {}}, {"not_SD", {}}, {"WM", {}}, {"not_WM", {}},
//                           {"both", {}}, {"neither", {}},
//                          });
//        vector<int>RO_xvals = {};
//        for(auto & i : this->lifetime_infected_bin_avgs){
//            for(auto & x_val : i) {
//                RO_xvals.push_back(x_val.second);
//                int S = this->data_history["S"].at(x_val.second);
//            }
//        }
    }
    // TODO continue
}

void DataCollector::increment_initial_infected() {
    this->initial_infected += 1;
}

void DataCollector::increment_newly_infected() {
    this->newly_infected += 1;
}

void DataCollector::increment_initial_recovered() {
    this->initial_recovered += 1;
}


