#include <iostream>
#include <getopt.h>
#include "Worker.h"
#include "DataContainer.h"
#include <cstdlib>

using namespace std;

// TODO konstanty, move_to,...
// Worker a DATAContainer
#define GRID_WIDTH 10
#define GRID_HEIGHT 10

#define WORKERS 20


#define ITERATIONS 50
class CellularAutomat{
public:

    Worker *open_positions[GRID_HEIGHT][GRID_WIDTH]{};

    /*
     * Constructor of Automat
     */
    CellularAutomat(){
        for(auto & open_position : this->open_positions){
            for(auto & x : open_position){
                x = nullptr;
            }
        }
        this->init_grid();
        this->show_grid();
        this->move();
        this->show_grid();

    }

    /*
     * Inicialization of grid with workers assigned random positions
     */
    void init_grid(){
        for(int w = 0; w < WORKERS; w++){
            position pos{};
            pos.x = random_int(0, GRID_WIDTH - 1);
            pos.y = random_int(0, GRID_HEIGHT - 1);
            CellularAutomat::create_worker(pos);
        }
    }

    /*
     * Creates worker if the position is free
     */
    void create_worker(position p){
        if(check_point(p)){
            auto *worker = new Worker(p);
            this->open_positions[p.x][p.y] = worker;
        }
    }

    // TODO
    void kill_worker(position p){
        open_positions[p.x][p.y] = nullptr;
    }

    /*
     * If the position p is free and not out of boundaries function moves worker to the position p
     */
    bool move_worker(Worker *w, position p){
        position current_position = w->get_position();
        if(check_point(p)){
            w->set_position(p);
            this->open_positions[current_position.x][current_position.y] = nullptr;
            this->open_positions[p.x][p.y] = w;
            return true;
        }
        else{
            return false;
        }
    }

    /*
     * Helping function to test move_worker() function
     */
    void move(){
        for(auto & open_position : this->open_positions){
            for(auto & x : open_position){
                if(x != nullptr){
                    position p = x->get_position();
                    p.y = p.y + 1;
                    move_worker(x, p);
                }
            }
        }
    }

    /*
     * Prints present state of grid
     */
    void show_grid(){
        for(auto & open_position : this->open_positions){
            for(auto & x : open_position){
                if(x != nullptr){
                    cout<<x->state;
//                    cout<<"<"<<x->age<<">";
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
            if (this->open_positions[p.x][p.y] == nullptr) {
                return true;
            }
            return false;
        }
    }
};

/*                                                                               */
/* ------------------------------------ MAIN ----------------------------------- */
/*                                                                               */

int main(int argc, char *argv[]) {

    CellularAutomat cellularAutomat = CellularAutomat();

    return 0;

}
