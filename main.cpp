#include <iostream>
#include <getopt.h>
#include "Worker.h"
#include "DataContainer.h"
using namespace std;

#define PROBABILITY
#define GRID_WIDTH 75
#define GRID_HEIGHT 75
#define GRID_
#define GRID_
class CellularAutomat{

};



int main(int argc, char *argv[]) {

    Worker worker = Worker();
    static struct option long_options[] = {
            {"help", optional_argument, nullptr, 'h'},
            {"bus", optional_argument, nullptr, 'b'},
            {nullptr, 0, nullptr, 0}
    };
    int arg;
    while((arg = getopt_long(argc, argv, "hb:t", long_options, nullptr)) != EOF){
        switch(arg){
            case 'h': {
                printf("For this help type argument: -h or --help");
                break;
            }
            case 'b': {
                const string typ = (optarg);
                printf("%s\n", typ.c_str());
                break;
            }
            default : {
                printf("No arguments");
                exit(-1);
            }
        }
    }
    return 0;
}
