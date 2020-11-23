#include <iostream>
#include <getopt.h>
#include "Worker.h"
#include "DataContainer.h"
using namespace std;

// TODO konstanty
// Worker a DATAContainer
#define GRID_WIDTH 75
#define GRID_HEIGHT 75

#define WORKERS 100

struct position{
    int x;
    int y;
};
#define ITERATIONS 50
class CellularAutomat{
public:
    CellularAutomat(){
        for(int x = 0; x < GRID_HEIGHT; x++){
            for(int y = 0; y < GRID_WIDTH; y++){
                CellularAutomat::open_positions[x][y] = false;
            }
        }
        CellularAutomat::init_grid();
    }

    void init_grid(){
        for(int w = 0; w <= WORKERS; w++){
            position pos;
            pos.x = rand() % GRID_WIDTH;
            pos.y = rand() % GRID_HEIGHT;
            CellularAutomat::create_worker(pos);
        }
    }

    void create_worker(position p){
        if(!open_positions[p.x][p.y]){
            open_positions[p.x][p.y] = true;
        }
    }

    void show_grid(){

        for(int x = 0; x < GRID_HEIGHT; x++){
            for(int y = 0; y < GRID_WIDTH; y++){
                cout<<CellularAutomat::open_positions[x][y];
            }
            cout<<endl;
        }
    }

    bool open_positions[GRID_HEIGHT][GRID_WIDTH];
};


int main(int argc, char *argv[]) {

    Worker worker = Worker();
    CellularAutomat cellularAutomat = CellularAutomat();
    cellularAutomat.init_grid();
    cellularAutomat.show_grid();
    return 0;
}
