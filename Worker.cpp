//
// Created by rene on 23. 11. 2020.
//

#include "Worker.h"

using namespace std;

/*
 * Nastavujeme ake vysoke percento pracovnikov bude dodrziavat restrikcie
 */
int policy = POLICY_SAFETY_VERYHIGH;

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
    /*
     * Inicializujeme stav (nakazeny alebo náchylný)
     */
    int random = random_int(1, 100);
    Worker::id = id;
    if(random <= (INITIAL_INFECTION_PROBABILITY)) {
        Worker::infected = true;
        Worker::susceptible = !this->infected;
        Worker::recovered = false;
    }else{
        Worker::recovered = false;
        Worker::infected = false;
        Worker::susceptible = !this->infected;
    }

    /*
     * Predpokladame ze vyzdraveny sa uz nemoze znova nakazit
     */
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
    Worker::movement_prob_before_symptoms = this->movement_prob;

    /*
     * Pre datacollector informacie o nakazenom obdobi
     */
    Worker::SD = 0;
    Worker::not_SD = 0;
    Worker::WM = 0;
    Worker::not_WM = 0;

    /*
     * Vytvor periody infekcie
     */
    Worker::incubation_period_duration = random_int(MIN_INCUBATION_DURATION, MAX_INCUBATION_DURATION + 1);
    Worker::infectious_period_duration = random_int(MIN_INFECTIOUS_DURATION, MAX_INFECTIOUS_DURATION + 1);
    Worker::infectious_start_before_symptoms = random_int(MIN_INFECTIOUS_START_BEFORE_SYMPTOMS, MAX_INFECTIOUS_START_BEFORE_SYMPTOMS + 1);
    Worker::severe_symptoms_start = random_int(MIN_SEVERE_SYMPTOMS_START, MAX_SEVERE_SYMPTOMS_START + 1);
    Worker::fatality_occur = random_int(DEATH_OCCURRENCE_MIN,DEATH_OCCURRENCE_MAX + 1);

    Worker::infectious_period_start = this->incubation_period_duration - this->infectious_start_before_symptoms;
    Worker::removed_period_start = this->infectious_period_duration + this->infectious_period_start;
    Worker::total_length_infection = TOTAL_LENGTH_INFECTION;
    Worker::total_length = Worker::total_length_infection;

    Worker::infectious_periods->num.push_back(0);
    Worker::infectious_periods->period.emplace_back("latent");
    Worker::infectious_periods->num.push_back(this->infectious_period_start);
    Worker::infectious_periods->period.emplace_back("infectious");
    Worker::infectious_periods->num.push_back(this->removed_period_start);
    Worker::infectious_periods->period.emplace_back("remove");
    Worker::infectious_periods->num.push_back(this->total_length);
    Worker::infectious_periods->period.emplace_back("recover");

    /*
     * Vytvor periody symptomov
     */

    Worker::symptoms_periods->num.push_back(0);
    Worker::symptoms_periods->period.emplace_back("incubation");
    Worker::symptoms_start = this->incubation_period_duration;

    // 1) Niektori ludia budu asymptomaticki (nemaju ziande priznaky a nemozu zomriet)
    if(random_int(1, 100) <= ASYMPTOMATIC_PROBABILITY) {
        Worker::symptoms_periods->num.push_back(this->symptoms_start);
        Worker::symptoms_periods->period.emplace_back("asymptomatic");
    }
    // 2) Ak nie je asymptomaticky tak zacina s jemnym priebehom
    else{
        Worker::symptoms_periods->num.push_back(this->symptoms_start);
        Worker::symptoms_periods->period.emplace_back("mild");
        // 3) Moze mat tazky priebeh (or not)
        if(random_int(1,100) <= SEVERITY_PROBABILITY){
            Worker::severe_start_abs = this->severe_symptoms_start + this->symptoms_start;
            Worker::symptoms_periods->num.push_back(this->severe_start_abs);
            Worker::symptoms_periods->period.emplace_back("severe");
            // 4) Moze zomriet (alebo aj nie)
            if(random_int(1, 100) <= DEATH_PROBABILITY){
                Worker::fatality_start_abs = this->fatality_occur + this->severe_symptoms_start + this->symptoms_start;
                Worker::symptoms_periods->num.push_back(this->fatality_start_abs);
                Worker::symptoms_periods->period.emplace_back("death");
            }
        }
    }
    // 5) Ak nezomrel ale sa vyzdravel
    if(get_p(*Worker::symptoms_periods, 'l') != "death"){
        Worker::symptoms_periods->num.push_back(this->total_length);
        Worker::symptoms_periods->period.emplace_back("recover");
    }

    Worker::current_symptom_stage = "NONE";
    Worker::current_infection_stage = "NONE";

}

bool Worker::get_infected() {
    if(this->infected or this->recovered or this->infectious_spots->empty()){
        return false;
    }
    double sum = 0.0;
    for(auto i = this->infectious_spots->begin(); i < this->infectious_spots->end(); i++){
        sum += double(BASE_INFECTION_PROBABILITY) + double(-1. * (int(i->wear_mask) * double(MASK_INFECTION_DECREASE_PROB)));
    }

    // 1 - (1 - p) ^ r
    // p => sanca nakazenia jedne osoby, r => pocet infekcnych ludi
    // Ak infekcny clovek ma masku, znizuje sancu nakazenia
    double infection_probability;
    double p = sum / double(this->infectious_spots->size());
    infection_probability = double(pow(1 - (1 - p), this->infectious_spots->size()) * 100);

    if (random_int(1, 100) < int(infection_probability)) {
        this->infected = true;
        this->susceptible = true;
        this->current_symptom_stage = get_p(*this->symptoms_periods, 'f');
        this->current_infection_stage = get_p(*this->infectious_periods, 'f');
        this->infectious_periods->period.erase(this->infectious_periods->period.begin());
        this->infectious_periods->num.erase(this->infectious_periods->num.begin());
        this->symptoms_periods->period.erase(this->symptoms_periods->period.begin());
        this->symptoms_periods->num.erase(this->symptoms_periods->num.begin());
        this->infection_step = 0;
        return true;
    }
    return false;
}

bool Worker::progress_infection() {
    if(!this->infected) {
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

    /*
     * Urobime krok v infekcii
     */
    this->infection_step += 1;
    bool new_infection_stage = false;
    bool new_symptoms_stage = false;

    /*
     * Kontrolujeme ci to ovplyvni stadium infekcie a symptomov
     */
    if(this->infection_step == get_n(*this->infectious_periods, 'f')){
        this->current_infection_stage = get_p(*this->infectious_periods, 'f');
        this->infectious_periods->num.erase(this->infectious_periods->num.begin());
        this->infectious_periods->period.erase(this->infectious_periods->period.begin());
        new_infection_stage = true;
    }

    if(this->infection_step == get_n(*this->symptoms_periods, 'f')){
        this->current_symptom_stage = get_p(*this->symptoms_periods, 'f');
        this->symptoms_periods->num.erase(this->symptoms_periods->num.begin());
        this->symptoms_periods->period.erase(this->symptoms_periods->period.begin());
        new_symptoms_stage = true;
    }

    /*
     * Ak zomrel
     */
    if(this->current_symptom_stage == "death"){
        this->susceptible = false;
        return true;
    }

    /*
     * Klasicky priebeh infekcie
     */
    if(this->current_symptom_stage == "mild" and this->altruistic and new_symptoms_stage){
        this->movement_prob = this->low_movement_prob;
        this->wear_mask = true;
        this->social_distance = true;
    }
    else if(this->current_symptom_stage == "severe" and new_symptoms_stage){
        this->movement_prob = 0;
        this->wear_mask = true;
        this->social_distance = true;
    }
    if (this->current_infection_stage == "recover" and new_infection_stage){
        this->recovered = true;
        this->infected = false;
        this->susceptible = false;
        this->wear_mask = this->wear_mask_before_symptoms;
        this->social_distance = this->social_distance_before_symptoms;
        this->movement_prob = this->movement_prob_before_symptoms;
        this->current_symptom_stage = "NONE";
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