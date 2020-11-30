//
// Created by rene on 23. 11. 2020.
//

#ifndef IMS_WORKER_H
#define IMS_WORKER_H
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

#define GRID_WIDTH 75
#define GRID_HEIGHT 75

#define WORKERS 500
#define ITERATIONS 50

#define INITIAL_INFECTION_PROBABILITY 100 // 100 / 1000 = 0.1 * 5 = 0.5
#define MAX_AGE 80
#define MIN_AGE 18
#define MOVE_PROBABILITY 50 //%
#define ALTRUISTIC_PROBABILITY 75
#define BASE_INFECTION_PROBABILITY 0.20
#define MASK_INFECTION_DECREASE_PROB 0.10

#define AREA 1
#define MOVE_LENGTH 6
#define ARRAY_SIZE int(pow(AREA * 2 + 1, 2) - 1)

#define MIN_INCUBATION_DURATION 4
#define MAX_INCUBATION_DURATION 6
#define MIN_INFECTIOUS_DURATION 8
#define MAX_INFECTIOUS_DURATION 9
#define MIN_INFECTIOUS_START_BEFORE_SYMPTOMS 2
#define MAX_INFECTIOUS_START_BEFORE_SYMPTOMS 3
#define MIN_SEVERE_SYMPTOMS_START 2
#define MAX_SEVERE_SYMPTOMS_START 4
#define TOTAL_LENGTH_INFECTION 14
#define DEATH_OCCURENCE_MIN 2
#define DEATH_OCCURENCE_MAX 4

#define ALTRUISTIC_MOVEMENT_PROBABILITY 25

#define POLICY_SAFETY_VERYHIGH 75
#define POLICY_SAFETY_HIGH 50
#define POLICY_SAFETY_MEDIUM 25
#define POLICY_SAFETY_LOW 10


struct position{
    int x;
    int y;
};

struct periods {
    std::vector<int> num;
    std::vector<std::string> period;
};

//struct permutations {
//    std::string array[4];
//};

int random_int(int, int);

std::string get_p(const periods& p, char c);

int get_n(const periods& p, char c);

using namespace std;

class Worker {
public:

    explicit Worker(position position, int id);

    bool progress_infection();

    bool get_infected();

    int age;

    position pos{};

    position get_position() const;

    void set_position(position p);

    int incubation_period_duration;

    int infectious_period_duration;

    int infectious_start_before_symptoms;

    int severe_symptoms_start;

    int infectious_period_start;

    int total_length_infection;

    int removed_period_start;

    periods *infectious_periods = new periods;

    int total_length;

    periods *symptoms_periods = new periods;

    int symptoms_start;

    int severe_start_abs;

    int fatality_start_abs;

    int fatality_occur;

    bool is_infectious() const;

    std::string current_infection_stage;

    std::string current_symptom_stage;

//    std::vector<permutations> possible_permutations;

    bool wear_mask;

    int infection_step;

    bool altruistic;

    int movement_prob;

    bool social_distance;

    bool recovered;

    bool infected;

    bool wear_mask_before_symptoms;

    bool social_distance_before_symptoms;

    int id;

    int num_people_infected;

    // TODO free()
    vector<position> *empty_spots = new vector<position>;

    vector<Worker> *infectious_spots = new vector<Worker>;

    int low_movement_prob;

    bool dead;

    bool susceptible;

    position *last_position = new position;

    map<string, int> *infectious_days_info = new map<string, int>;

    int SD;
    int not_SD;
    int not_WM;
    int WM;
};

#endif //IMS_WORKER_H
