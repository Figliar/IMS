//
// Created by rene on 23. 11. 2020.
//

#include "Worker.h"

using namespace std;

int policy = POLICY_SAFETY_MEDIUM;

int get_n(const periods& p, char c){
    int result;
    if(c == 'l')
        for(const auto& i: p.num){
            result = i;
        }
    else{
        for(const auto& i: p.num){
            result = i;
            break;
        }
    }
    return result;
}

std::string get_p(const periods& p, char c){
    std::string result;
    if(c == 'l')
        for(const auto& i: p.period){
            result = i;
        }
    else{
        for(const auto& i: p.period){
            result = i;
            break;
        }
    }
    return result;
}

int random_int(int min, int max){
    std::random_device os_seed;
    const uint_least32_t seed = os_seed();
    std::mt19937 generator(seed);
    std::uniform_int_distribution< uint_least32_t > distribute(min, max);
    return distribute(generator);
//    return min + rand() / (RAND_MAX / (max - min  + 1) +1);
}

Worker::Worker(position position, int id) {
    int random = random_int(1, INITIAL_INFECTION_PROBABILITY);
    Worker::id = id;
    if(random <= 5) {
        Worker::infected = true;
        Worker::susceptible = false;
        Worker::recovered = false;
    }else if(random <= 90){
        Worker::susceptible = true;
        Worker::recovered = false;
        Worker::infected = false;
    }else{
        Worker::recovered = true;
        Worker::susceptible = false;
        Worker::infected = false;

    }
    Worker::pos = position;
    Worker::age = random_int(MIN_AGE, MAX_AGE);
    Worker::dead = false;
    Worker::wear_mask = (random_int(1, 100) <= policy);
    Worker::wear_mask_before_symptoms = this->wear_mask;
    Worker::social_distance = (random_int(1, 100) <= policy);
    Worker::social_distance_before_symptoms = this->social_distance;
    Worker::infection_step = -1;
    Worker::altruistic = (random_int(1, 100) <= ALTRUISTIC_PROBABILITY);
    Worker::num_people_infected = 0;
    Worker::movement_prob = MOVE_PROBABILITY;
    Worker::low_movement_prob = ALTRUISTIC_MOVEMENT_PROBABILITY;
    Worker::last_position = nullptr;

    /*
     * Creating Infection periods
     */
    Worker::incubation_period_duration = random_int(MIN_INCUBATION_DURATION, MAX_INCUBATION_DURATION);
    Worker::infectious_period_duration = random_int(MIN_INFECTIOUS_DURATION, MAX_INFECTIOUS_DURATION);
    Worker::infectious_start_before_symptoms = random_int(MIN_INFECTIOUS_START_BEFORE_SYMPTOMS, MAX_INFECTIOUS_START_BEFORE_SYMPTOMS);
    Worker::severe_symptoms_start = random_int(MIN_SEVERE_SYMPTOMS_START, MAX_SEVERE_SYMPTOMS_START);
    Worker::fatality_occur = random_int(DEATH_OCCURENCE_MIN,DEATH_OCCURENCE_MAX + 1);
    Worker::infectious_period_start = Worker::incubation_period_duration - Worker::infectious_start_before_symptoms;
    Worker::removed_period_start = Worker::infectious_period_duration + Worker::infectious_period_start;
    Worker::total_length_infection = TOTAL_LENGTH_INFECTION;
    Worker::total_length = Worker::total_length_infection;

    Worker::infectious_periods->num.push_back(0);
    Worker::infectious_periods->period.emplace_back("latent");
    Worker::infectious_periods->num.push_back(Worker::infectious_period_start);
    Worker::infectious_periods->period.emplace_back("infectious");
    Worker::infectious_periods->num.push_back(Worker::removed_period_start);
    Worker::infectious_periods->period.emplace_back("remove");
    Worker::infectious_periods->num.push_back(Worker::total_length);
    Worker::infectious_periods->period.emplace_back("recover");

    /*
     * Creating Symptoms periods
     */

    Worker::symptoms_periods->num.push_back(0);
    Worker::symptoms_periods->period.emplace_back("incubation");
    Worker::symptoms_start = Worker::incubation_period_duration;
    if(Worker::symptoms_start < Worker::infectious_period_start)
        cout<<"Symptoms ("<<Worker::symptoms_start<<") start during infectious stage (starts "<<infectious_period_start<<")"<<endl;
    if(Worker::symptoms_start > Worker::removed_period_start)
        cout<<"Symptoms ("<<Worker::symptoms_start<<") start during infectious stage (starts "<<infectious_period_start<<")"<<endl;
    // 1) Some people are asymptomatic (and have no mild or severe symptoms, and also cant die)
    if(random_int(1, 100) <= 35) {
        Worker::symptoms_periods->num.push_back(Worker::symptoms_start);
        Worker::symptoms_periods->period.emplace_back("asymptomatic");
    }
    // 2) If they arent asymptomatic they start with mild
    else{
        Worker::symptoms_periods->num.push_back(Worker::symptoms_start);
        Worker::symptoms_periods->period.emplace_back("mild");
        // 3) Can have severe (or not)
        if(random_int(1,100) <= 81){
            Worker::severe_start_abs = Worker::severe_symptoms_start + symptoms_start;
            Worker::symptoms_periods->num.push_back(Worker::severe_start_abs);
            Worker::symptoms_periods->period.emplace_back("severe");
            // 4) Can die (or not)
            if(random_int(1, 100) < 50){
                Worker::fatality_start_abs = Worker::fatality_occur + Worker::severe_symptoms_start + Worker::symptoms_start;
                Worker::symptoms_periods->num.push_back(Worker::fatality_start_abs);
                Worker::symptoms_periods->period.emplace_back("death");
            }
        }
    }
    // 5) If didnt die then recover
    if(get_p(*Worker::symptoms_periods, 'l') != "death"){
        Worker::symptoms_periods->num.push_back(Worker::total_length);
        Worker::symptoms_periods->period.emplace_back("recover");
    }

//    Worker::possible_permutations.push_back({"incubation", "asymptomatic", "recover"});
//    Worker::possible_permutations.push_back({"incubation", "mild", "recover"});
//    Worker::possible_permutations.push_back({"incubation", "mild", "severe", "death"});
//    Worker::possible_permutations.push_back({"incubation", "mild", "severe", "recover"});
//    pristup k polozkam
//    for(auto &i: possible_permutations)
//        cout<<i.array[2]<<endl;

    Worker::current_symptom_stage = "NONE";
    Worker::current_infection_stage = "NONE";

    /*
     * for datacollector
     */
    Worker::SD = 0;
    Worker::not_SD = 0;
    Worker::WM = 0;
    Worker::not_WM = 0;
}

bool Worker::get_infected() {
    if(this->infected or this->recovered or this->infectious_spots->empty()){
        return false;
    }
    float p;
    float sum = 0.0;
    for(auto i = this->infectious_spots->begin(); i < this->infectious_spots->end(); i++){
        sum += float(BASE_INFECTION_PROBABILITY) + float(-1. * (int(i->wear_mask) * float(MASK_INFECTION_DECREASE_PROB)));
    }
    p = sum / this->infectious_spots->size();
    auto infection_probability = float(pow(1 - (1 - p), this->infectious_spots->size()) * 100);
    if (random_int(1, 100) < infection_probability) {
        this->infected = true;
        this->susceptible = true;
        this->current_symptom_stage = get_p(*this->symptoms_periods, 'f');
        this->current_infection_stage = get_p(*this->infectious_periods, 'f');
        this->infectious_periods->period.erase(this->infectious_periods->period.begin());
        this->infectious_periods->num.erase(this->infectious_periods->num.begin());
        this->symptoms_periods->period.erase(this->symptoms_periods->period.begin());
        this->symptoms_periods->num.erase(this->symptoms_periods->num.begin());
        this->infection_step = 0;
//        data_collector.increment_total_infected();
        return true;
    }
    return false;
}

bool Worker::progress_infection() {
    cout<<"progress_infection(start)"<<endl;
    if(!this->infected) {
        cout << "!this->infected;"<<endl;
        return false;
    }

    if(this->is_infectious()){
        if(this->social_distance)
            this->SD += 1;
        else
            this->not_SD += 1;
        if(this->wear_mask)
            this->WM += 1;
        else
            this->not_WM += 1;
    }

    this->infection_step += 1;
    bool new_infection_stage = false;
    bool new_symptoms_stage = false;

    cout<<"bool new_symptoms_stage = false;"<<endl;
    if(this->infection_step == get_n(*this->infectious_periods, 'f')){
        this->current_infection_stage = get_p(*this->infectious_periods, 'f');
        this->infectious_periods->num.erase(this->infectious_periods->num.begin());
        this->infectious_periods->period.erase(this->infectious_periods->period.begin());
        new_infection_stage = true;
    }

    cout<<"new_infection_stage = true;"<<endl;
    if(this->infection_step == get_n(*this->symptoms_periods, 'f')){
        this->current_symptom_stage = get_p(*this->infectious_periods, 'f');
        this->symptoms_periods->num.erase(this->symptoms_periods->num.begin());
        this->symptoms_periods->period.erase(this->symptoms_periods->period.begin());
        new_symptoms_stage = true;
    }

    cout<<"new_symptoms_stage = true;"<<endl;
    if(this->current_symptom_stage == "death"){
        return true;
    }

    bool new_SD = false;

    if(this->current_symptom_stage == "mild" and this->altruistic and new_symptoms_stage){
        this->movement_prob = ALTRUISTIC_MOVEMENT_PROBABILITY;
        this->wear_mask = true;
        new_SD = (!this->social_distance);
        this->social_distance = true;
    }
    else if(this->current_symptom_stage == "severe" and new_symptoms_stage){
        this->movement_prob = 0;
        this->wear_mask = true;
        new_SD = (!this->social_distance);
        this->social_distance = true;
    }

    if (this->current_infection_stage == "recover" and new_infection_stage){
        this->recovered = true;
        this->infected = false;
        this->susceptible = false;
        this->wear_mask = this->wear_mask_before_symptoms;
        new_SD =
                this->social_distance != this->social_distance_before_symptoms && this->social_distance_before_symptoms;
        this->social_distance = this->social_distance_before_symptoms;
        this->movement_prob = this->removed_period_start;
        this->current_symptom_stage = "NONE";

//        data_collector.add_lifetime_infected(this->num_people_infected, this->infectious_days_info);
        //todo data_collector
    }
    return false;
}


position Worker::get_position() const{
    return this->pos;
}

void Worker::set_position(position p){
    this->pos = p;
}

bool Worker::is_infectious() const{
    return this->current_infection_stage == "infectious";
}