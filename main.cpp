#include "Worker.h"
#include "DataCollector.h"

using namespace std;



class CellularAutomat{
public:

    Worker *grid[GRID_WIDTH][GRID_HEIGHT];
    Worker *workers[WORKERS];

    vector<int> *ids_social_distance = new vector<int>;
    vector<int> *ids_not_social_distance = new vector<int>;

    DataCollector dataCollector;
    /*
     * Constructor of Automat
     */
    CellularAutomat(DataCollector dc){
        this->dataCollector = dc;
        for(auto & open_position : this->grid){
            for(auto & x : open_position){
                x = nullptr;
            }
        }
        this->init_grid();
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
            if(worker->infected){
                this->dataCollector.increment_initial_infected();
            }
            else if(worker->susceptible){
                this->dataCollector.increment_initial_S();
            }
            this->dataCollector.update_data(worker);
            if(worker->social_distance)
                this->ids_social_distance->push_back(worker->id);
            else
                this->ids_not_social_distance->push_back(worker->id);
        }
//        cout<<"create_worker(end)"<<endl;
    }

    void kill_worker(Worker *w){
        this->dataCollector.increment_dead();
        w->dead = true;
        this->grid[w->pos.x][w->pos.y] = nullptr;
    }

    /*
     * If the position p is free and not out of boundaries function moves worker to the position p
     */
    bool move_worker(Worker *w, position p){
        position current_position = w->get_position();
        if(check_point(p)){
//            w->last_position = &w->pos;
            this->grid[p.x][p.y] = w;
            w->set_position(p);
            this->grid[current_position.x][current_position.y] = nullptr;
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
        if(w->ali){
//            this->dataCollector.add_lifetime_infected(w);
        }
        if (w->social_distance) {
            this->check_neighbors_SD(w);
//            this->check_neighbors_n_SD(w);
        } else {
            this->check_neighbors_n_SD(w);
//            this->check_neighbors_SD(w);
        }

//        cout<<"update_grid(end)"<<endl;
        return true;
    }

    void check_neighbors(Worker *w){
        w->empty_spots->clear();
        w->infectious_spots->clear();
        for (int y = -AREA; y <= AREA; y++) {
            for (int x = -AREA; x <= AREA; x++) {
                if (x == 0 and y == 0) {
                    continue;
                }
                position temp_p = w->pos;
                temp_p.x = temp_p.x + x;
                temp_p.y = temp_p.y + y;
                if (check_point(temp_p) and w->last_position != &temp_p) {
                    w->empty_spots->push_back(temp_p);
                } else {
                    if(!oob(temp_p)) {
                        if (this->grid[temp_p.x][temp_p.y]->is_infectious()) {
                            w->infectious_spots->push_back(*this->grid[temp_p.x][temp_p.y]);
                        }
                    }
                }
            }
        }
    }

    bool check_neighbors_n_SD(Worker *w){
        this->check_neighbors(w);
        this->check_infection(w);
//        w->last_position = &w->pos;
        if(random_int(1, 100) < w->movement_prob){
            for(int step = 0; step < MOVE_LENGTH; step++){
                if (!w->empty_spots->empty()) {
                    position to = w->empty_spots->at(random_int(0, int(w->empty_spots->size() - 1)));
                    this->move_worker(w, to);
                    this->check_neighbors(w);
                    this->check_infection(w);
                }
                else{
                    break;
                }
            }
        }
        w->last_position = nullptr;
        return true;
    }


    bool check_neighbors_SD(Worker *w){
        this->check_neighbors(w);
        this->check_infection(w);

        int move_length_SD = w->empty_spots->size() < 8 ? 1 : MOVE_LENGTH;

        if (w->empty_spots->size() < 8 or (random_int(1, 100) < w->movement_prob)){
            for(int step = 0; step < move_length_SD; step++){
//                w->last_position = &w->pos;
                bool did_move = false;
                if (!w->empty_spots->empty()) {
                    for(int i = 0; i < int(w->empty_spots->size()); i++){
                        unsigned int which = random_int(0, int(w->empty_spots->size() - 1));
                        if(this->check_if_safe(w, w->empty_spots->at(which))){
                            this->move_worker(w, w->empty_spots->at(which));
                            this->check_neighbors(w);
                            this->check_infection(w);
                            did_move = true;
                            break;
                        }
                    }
                    if(!did_move)
                        break;
                }
                else{
                    break;
                }
            }
        }
        w->last_position = nullptr;
        return true;
    }

    bool check_if_safe(Worker *w, position i){
        for (int y = -AREA; y <= AREA; y++) {
            for (int x = -AREA; x <= AREA; x++) {
                if (x == 0 and y == 0) {
                    continue;
                }
                if(&i == w->last_position)
                if(this->grid[i.x + x][i.y + y])
                    return false;
            }
        }
        return true;
    }

    void check_infection(Worker *w){
        bool newly_infected = w->get_infected();
        if(newly_infected){
            this->dataCollector.increment_newly_infected();
            for(auto i = w->infectious_spots->begin(); i <= w->infectious_spots->end(); i++){
                i->num_people_infected +=1;
            }
        }
    }

    void update_grid(){
//        cout<<"update_grid(start)"<<endl;
//        vector<int> new_SD_list;
//        vector<int> new_not_SD_list;
//        for (auto w = *this->ids_not_social_distance->begin(); w != *this->ids_not_social_distance->end(); w++){
//            if(!workers[w]->dead){
//                this->update_worker(workers[w]);
//                bool new_SD = workers[w]->social_distance;
//                if(workers[w]->dead) continue;
//                if(new_SD) new_SD_list.push_back(workers[w]->id);
//                this->dataCollector.update_data(workers[w]);
//            }
//        }
//        for (auto w = *this->ids_social_distance->begin(); w != *this->ids_social_distance->end(); w++){
//            if(!workers[w]->dead){
//                this->update_worker(workers[w]);
//                bool new_SD = workers[w]->social_distance;
//                if(workers[w]->dead) continue;
//                if(!new_SD) new_not_SD_list.push_back(workers[w]->id);
//                this->dataCollector.update_data(workers[w]);
//            }
//        }
//
//        for(auto &id : new_SD_list){
////            this->ids_not_social_distance->erase(this->ids_not_social_distance->begin() + id);
//            this->ids_social_distance->push_back(id);
//            for(auto &i : *this->ids_not_social_distance) {
//                if(i == id){
//                    this->ids_not_social_distance->erase(this->ids_not_social_distance->begin() + i);
//                }
//            }
//        }
//        for(auto &id : new_not_SD_list){
////            this->ids_social_distance->erase(this->ids_social_distance->begin() + id);
//            this->ids_not_social_distance->push_back(id);
//            for(auto &i : *this->ids_social_distance) {
//                if(i == id){
//                    this->ids_social_distance->erase(this->ids_social_distance->begin() + i);
//                }
//            }
//        }
//        for(auto i: *ids_not_social_distance) {
//            cout << i << endl;
//        }
//        for(auto i: *ids_social_distance) {
//            cout << i << endl;
//        }
        for(auto & worker : this->workers){
            if(!worker->dead and !worker->social_distance) {
                this->update_worker(worker);
                if (worker->dead) continue;
                this->dataCollector.update_data(worker);
            }
        }
        for(auto & worker : this->workers){
            if(!worker->dead and worker->social_distance) {
                this->update_worker(worker);
                if (worker->dead) continue;
                this->dataCollector.update_data(worker);
            }
        }
    }

    /*
     * Prints present state of grid
     */
    void show_grid(){
        for(auto & open_position : this->grid){
            for(auto & x : open_position){
                if(x != nullptr){
                    if(x->infected)
                        cout<<" 2 ";
//                    cout<<"<"<<x->age<<">";
                    else if(x->recovered)
                        cout<<" 1 ";
                    else
                        cout<<" 0 ";
                }
                else{
                    cout<<" - ";
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
        int i;
        for(i = 0; i < ITERATIONS; i++){
            this->dataCollector.reset(i, false);
            this->update_grid();
            cout<<i<<"====================="<<endl;
            cout<<"Initial_susceptible:     "<<dataCollector.initial_S<<endl;
            cout<<"Newly_infected:          "<<dataCollector.newly_infected<<endl;
            cout<<"Initial_infected:        "<<dataCollector.initial_infected<<endl;
            cout<<"Total_infected:          "<<dataCollector.get_total_infected()<<endl;
            cout<<"End dead:                "<<dataCollector.total_dead<<endl;
            cout<<"End Susceptible:         "<<dataCollector.current_data["S"]<<endl;
            cout<<"End infected:            "<<dataCollector.current_data["I"]<<endl;
            cout<<"All recovered:           "<<dataCollector.current_data["R"]<<endl;
            cout<<"All wear_mask:   "<<dataCollector.current_data["WM"]<<endl;
            cout<<"All social_d:    "<<dataCollector.current_data["SD"]<<endl;
            cout<<"All mild:        "<<dataCollector.current_data["mild"]<<endl;
            cout<<"All severe:      "<<dataCollector.current_data["severe"]<<endl;
            cout<<"All asymptomatic:"<<dataCollector.current_data["asymptomatic"]<<endl;
        }
        this->show_grid();
        for( auto & data : dataCollector.current_data){
            cout<<data.first<<endl;
            for( auto & more : dataCollector.data_history[data.first])
                cout<<more<<", ";
            cout<<endl;
        }
        cout<<dataCollector.current_data["death"]<<endl;
        dataCollector.reset(i + 1, true);
        for(auto w : workers){
            free(w->symptoms_periods);
            free(w->infectious_periods);
            free(w->empty_spots);
            free(w->infectious_spots);
        }
        free(ids_not_social_distance);
        free(ids_social_distance);
        return 1;
    }

};

/*                                                                               */
/* ------------------------------------ MAIN ----------------------------------- */
/*                                                                               */

int main(int argc, char *argv[]) {

    DataCollector data_collector = DataCollector();
    auto *cellularAutomat = new CellularAutomat(data_collector);
    cellularAutomat->show_grid();
    cellularAutomat->run();
    return 0;

}
