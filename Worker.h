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

#define INFECTED 2
#define RECOVERED 1
#define SUSCEPTIBLE 0

#define INITIAL_INFECTION_PROBABILITY 1000 // 100 / 1000 = 0.1 * 5 = 0.5
#define SICK 14
#define MAX_AGE 80
#define MIN_AGE 18
#define MAX_INCUBATION_DURATION 6
#define MIN_INCUBATION_DURATION 4
#define MAX_INFECTIOUS_DURATION 9
#define MIN_INFECTIOUS_DURATION 8

int random_int(int, int);
unsigned short * current_time_nanoseconds();

struct position{
    int x;
    int y;
};

class Worker {
public:

    Worker(position position);

    void progress_infection();

    void get_infected();

    int state;

    int sick;

    int age;

    position pos{};

    position get_position();

    void set_position(position p);
};


#endif //IMS_WORKER_H
