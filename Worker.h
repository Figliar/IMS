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

// GRID CONSTANTS
#define GRID_WIDTH 75
#define GRID_HEIGHT 75
#define WORKERS 500
#define ITERATIONS 50
#define AREA 1 // ake velke okolie berie do uvahy

// WORKER CONSTANTS
#define MIN_AGE 18
#define MAX_AGE 80
#define MOVE_PROBABILITY 50 // %
#define ALTRUISTIC_MOVEMENT_PROBABILITY 25
#define MOVE_LENGTH 6
#define INITIAL_INFECTION_PROBABILITY 5 // %
#define ALTRUISTIC_PROBABILITY 75       // %
#define POLICY_SAFETY_VERYHIGH 75       // %
#define POLICY_SAFETY_HIGH 50           // %
#define POLICY_SAFETY_MEDIUM 25         // %
#define POLICY_SAFETY_LOW 10            // %

// DISEASE CONSTANTS
#define BASE_INFECTION_PROBABILITY 0.20
#define MASK_INFECTION_DECREASE_PROB 0.10
#define TOTAL_LENGTH_INFECTION 14
#define MIN_INCUBATION_DURATION 4
#define MAX_INCUBATION_DURATION 6
#define MIN_INFECTIOUS_DURATION 8
#define MAX_INFECTIOUS_DURATION 9
#define MIN_INFECTIOUS_START_BEFORE_SYMPTOMS 2
#define MAX_INFECTIOUS_START_BEFORE_SYMPTOMS 3
#define MIN_SEVERE_SYMPTOMS_START 2
#define MAX_SEVERE_SYMPTOMS_START 4
#define DEATH_OCCURRENCE_MIN 2
#define DEATH_OCCURRENCE_MAX 4
#define ASYMPTOMATIC_PROBABILITY 35 // %
#define DEATH_PROBABILITY 49 // %
#define SEVERITY_PROBABILITY 19 // %

/*
 * Struktura 2D pozicie predstavuje dvojicu suradnic (x, y)
 */
struct position{
    int x;
    int y;
};

/*
 * Struktura dvoch vektorov pre periody symptomov a infekcie
 */
struct periods {
    std::vector<int> num;
    std::vector<std::string> period;
};

/*
 * Funkcia na generovanie pseudonahodnych cisel pomocou casu
 */
int random_int(int, int);

/*
 * Funkcia sluzi na ziskanie period zaciatkov jednotlivych stadii choroby
 */
std::string get_p(const periods& p, char c);

/*
 * Funkcia sluzi na ziskanie numerickych hodnot zaciatkov jednotlivych stadii choroby
 */
int get_n(const periods& p, char c);

using namespace std;

class Worker {
public:

    /*
    * Konstruktor pracovnika inicializuje vsetky potrebne premenne
    */
    explicit Worker(position position, int id);

    /*
    * Funkcia ktora rozhoduje ci sa pracovnik nakazy
    */
    bool get_infected();

    /*
    * Funkcia na progresovanie infekcie u nakazenych pracovnikov
    */
    bool progress_infection();

    /*
    * Funkcia vrati poziciu pracovnika
    */
    position get_position() const;

    /*
    * Funkcia nastavuje polohu pracovnikovi
    */
    void set_position(position p);

    /*
    * Funkcia vracia ci je pracovnik infekcny
    */
    bool is_infectious() const;

    int age;

    position pos{};

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

    std::string current_infection_stage;

    std::string current_symptom_stage;

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

    vector<position> *empty_spots = new vector<position>;

    vector<Worker> *infectious_spots = new vector<Worker>;

    int low_movement_prob;

    bool dead;

    bool susceptible;

    position *last_position;

    int SD;
    int not_SD;
    int not_WM;
    int WM;
    int movement_prob_before_symptoms;
};

#endif //IMS_WORKER_H
