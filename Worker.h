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

#define INFECTED 2
#define RECOVERED 1
#define SUSCEPTIBLE 0

class Worker {
public:
    Worker();

    void progress_infection();

    void get_infected();

    int mask;

    int state;
};


#endif //IMS_WORKER_H
