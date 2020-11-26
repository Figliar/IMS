//
// Created by rene on 23. 11. 2020.
//

#include "Worker.h"

using namespace std;

unsigned short * current_time_nanoseconds(){
    struct timespec tm;
    clock_gettime(CLOCK_REALTIME, &tm);
    return reinterpret_cast<unsigned short *>(tm.tv_nsec);
}

int random_int(int min, int max){
    std::random_device os_seed;
    const uint_least32_t seed = os_seed();
    std::mt19937 generator(seed);
    std::uniform_int_distribution< uint_least32_t > distribute(min, max);
    return distribute(generator);
    return min + rand() / (RAND_MAX / (max - min  + 1) +1);
}

Worker::Worker(position position) {
    int random = rand() % INITIAL_INFECTION_PROBABILITY;
    if(random <= 5) {
        Worker::state = INFECTED;
        Worker::sick = SICK;
    }else if(5 < random and random <= 900){
        Worker::state = SUSCEPTIBLE;
    }else{
        Worker::state = RECOVERED;
    }
    Worker::pos = position;
    Worker::age = rand() % (MAX_AGE - MIN_AGE) + MIN_AGE;
}

position Worker::get_position(){
    return this->pos;
}

void Worker::set_position(position p){
    this->pos = p;
}

void Worker::get_infected() {

}

void Worker::progress_infection() {

}


