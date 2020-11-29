#include <iostream>
#include <getopt.h>
#include "Worker.h"
#include "DataContainer.h"
#include <cstdlib>

using namespace std;

class CellularAutomat{
public:

    Worker *grid[GRID_HEIGHT][GRID_WIDTH]{};

    Worker *workers[WORKERS];

    vector<int> ids_social_distance;
    vector<int> ids_not_social_distance;
    /*
     * Constructor of Automat
     */
    CellularAutomat(){
        //TODO datacollector

        for(auto & open_position : this->grid){
            for(auto & x : open_position){
                x = nullptr;
            }
        }
//        cout<<"CellularAutomat"<<endl;
        this->init_grid();
//        this->show_grid();
    }

    /*
     * Inicialization of grid with workers assigned random positions
     */
    void init_grid(){
//        cout<<"init_grid(start)"<<endl;
        for(int w = 0; w < WORKERS; w++){
            position pos{};
            do {
                pos.x = random_int(0, GRID_WIDTH - 1);
                pos.y = random_int(0, GRID_HEIGHT - 1);
            }while(this->grid[pos.x][pos.y] != nullptr);
            this->create_worker(pos, w);
            workers[w] = get_worker(pos);
        }
//        cout<<"init_grid(end)"<<endl;
    }

    /*
     * Creates worker if the position is free
     */
    void create_worker(position p, int ID){
//        cout<<"create_worker(start)"<<endl;
        if(check_point(p)){
            auto *worker = new Worker(p, ID);
            this->grid[p.x][p.y] = worker;
            if(!worker->infected){
                // TODO datacollector
            }
            // TODO datacollector
            if(worker->social_distance)
                ids_social_distance.push_back(worker->id);
            else
                ids_not_social_distance.push_back(worker->id);
        }
//        cout<<"create_worker(end)"<<endl;

    }

    void kill_worker(Worker *w){
        w->dead = true;
        this->grid[w->pos.x][w->pos.y] = nullptr;
    }

    /*
     * If the position p is free and not out of boundaries function moves worker to the position p
     */
    bool move_worker(Worker *w, position p){
        position current_position = w->get_position();
        if(check_point(p)){
            w->set_position(p);
            this->grid[current_position.x][current_position.y] = nullptr;
            this->grid[p.x][p.y] = w;
//            this->show_grid();
            return true;
        }
        else{
            return false;
        }
    }

    bool update_worker(Worker* w){
//        cout<<"update_worker(start)"<<endl;
        bool dead = w->progress_infection();
        if(dead){
            this->kill_worker(w);
            return false;
        }
        this->check_neighbors_n_SD(w);
//        cout<<"update_grid(end)"<<endl;
        return true;
    }

    void check_neighbors(Worker *w){
        w->empty_spots.clear();
        w->infectious_spots.clear();
        for (int y = -AREA; y <= AREA; y++) {
            for (int x = -AREA; x <= AREA; x++) {
                if (x == 0 and y == 0) {
                    continue;
                }
                position temp_p = w->pos;
                temp_p.x = temp_p.x + x;
                temp_p.y = temp_p.y + y;
                if (check_point(temp_p)) {
                    w->empty_spots.push_back(temp_p);
                } else {
                    if(!oob(temp_p)) {
                        if (this->grid[temp_p.x][temp_p.y]->is_infectious()) {
                            w->infectious_spots.push_back(*this->grid[temp_p.x][temp_p.y]);
                        }
                    }
                }
            }
        }
    }

    bool check_neighbors_n_SD(Worker *w){
        this->check_neighbors(w);
        this->check_infection(w);
//        cout<<endl<<"worker_ID: "<<w->id<<endl;
//        cout<<"movement_prob: "<<w->movement_prob<<endl;
//        cout<<"current_symptoms_stage: "<<w->current_symptom_stage<<endl;
        if(random_int(1, 100) < w->movement_prob){
            for(int step = 0; step < MOVE_LENGTH; step++){
                if (!w->empty_spots.empty()) {
                    position to = w->empty_spots.at(random_int(0, int(w->empty_spots.size() - 1)));
                    this->move_worker(w, to);
                    this->check_neighbors(w);
                    this->check_infection(w);
                }
                else{
                    break;
                }
            }
        }
        return true;
    }

    void check_infection(Worker *w){
        bool newly_infected = w->get_infected();
        if(newly_infected){
            for(auto i = w->infectious_spots.begin(); i <= w->infectious_spots.end(); i++){
                i->num_people_infected +=1;
            }
        }
    }

    void update_grid(){
//        cout<<"update_grid(start)"<<endl;
        for(auto & worker : this->workers){
            if(!worker->dead)
                this->update_worker(worker);
        }
//        cout<<"update_grid(end)"<<endl;
    }

    /*
     * Prints present state of grid
     */
    void show_grid(){
        for(auto & open_position : this->grid){
            for(auto & x : open_position){
                if(x != nullptr){
                    if(x->infected)
                        cout<<"2";
//                    cout<<"<"<<x->age<<">";
                    else if(x->recovered)
                        cout<<"1";
                    else
                        cout<<"0";
                }
                else{
                    cout<<"-";
                }
            }
            cout<<endl;
        }
        cout<<"2: INFECTED      ← y →\n"
              "1: RECOVERED     ↑ x ↓\n"
              "0: SUSCEPTIBLE        \n"<<endl;
    }

    /*
     * Checks if point is not out of bounds
     */
    bool oob(position p){
        return p.x < 0 or p.y < 0 or p.x >= GRID_HEIGHT or p.y >= GRID_WIDTH;
    }

    /*
     * Checks if point is free
     */
    bool check_point(position p) {
        if(!oob(p)) {
            if (this->grid[p.x][p.y] == nullptr) {
                return true;
            }
            return false;
        }
        return false;
    }

    Worker* get_worker(position p){
        return this->grid[p.x][p.y];
    }

    int run(){
        for(int i = 0; i < ITERATIONS; i++){
            this->update_grid();
            this->show_grid();
        }
        return 1;
    }
};

/*                                                                               */
/* ------------------------------------ MAIN ----------------------------------- */
/*                                                                               */

int main(int argc, char *argv[]) {

    CellularAutomat cellularAutomat = CellularAutomat();
    cellularAutomat.show_grid();
    cellularAutomat.run();
    return 0;

}
