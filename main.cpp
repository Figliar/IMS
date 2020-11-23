#include <iostream>
#include <getopt.h>
#include "Worker.h"
#include "DataContainer.h"
using namespace std;

// TODO konstanty
// Worker a DATAContainer
#define GRID_WIDTH 75
#define GRID_HEIGHT 75

#define ITERATIONS 50
class CellularAutomat{
public:
    CellularAutomat(){
        bool open_positions[GRID_HEIGHT][GRID_WIDTH];
        for(int x = 0; x < GRID_HEIGHT; x++){
            for(int y = 0; y < GRID_WIDTH; y++){
                open_positions[x][y] = false;
            }
        }
        CellularAutomat::init_grid();
    }

    void init_grid(){

    }

    void create_worker(positions){
        posi
    }
};


int main(int argc, char *argv[]) {

    Worker worker = Worker();
    CellularAutomat cellularAutomat = CellularAutomat();
    return 0;
}
