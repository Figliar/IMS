#include <utility>

#include "Worker.h"
#include "DataCollector.h"

using namespace std;

/*
 * Trieda automatu
 */
class CellularAutomat{
public:

    /*
     * Grid predstavuje "tovaren", workers je zoznam pracovnikov
     */
    Worker *grid[GRID_WIDTH][GRID_HEIGHT]{};
    Worker *workers[WORKERS]{};
    DataCollector dataCollector;

    /*
     * Constructor of Automat
     */
    explicit CellularAutomat(DataCollector dc){
        this->dataCollector = std::move(dc);
        for(auto & open_position : this->grid){
            for(auto & x : open_position){
                x = nullptr;
            }
        }
        this->init_grid();
    }

    /*
     * Inicializacia grid a pracovnikov na nahodne pozicie
     */
    void init_grid(){
        for(int w = 0; w < WORKERS; w++){
            position pos{};
            do {
                pos.x = random_int(0, GRID_WIDTH - 1);
                pos.y = random_int(0, GRID_HEIGHT - 1);
            }while(this->grid[pos.x][pos.y] != nullptr);
            this->create_worker(pos, w);
            workers[w] = get_worker(pos);
        }
    }

    /*
     * Ak je pozicia volna vytvori na nej pracovnika
     */
    void create_worker(position p, int ID){
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
        }
    }

    /*
     * Vymaze pracovnika z gridu a nastavi aby ho v dalsich iteraciach ignorovali
     */
    void kill_worker(Worker *w){
        this->dataCollector.increment_dead();
        w->dead = true;
        this->grid[w->pos.x][w->pos.y] = nullptr;
    }

    /*
     * Ak je pozicia p volna a nie je mimo hranic gridu, premiestni tam pracovnika
     */
    bool move_worker(Worker *w, position p){
        position current_position = w->get_position();
        if(check_point(p)){
            w->last_position = &w->pos;
            this->grid[p.x][p.y] = w;
            w->set_position(p);
            this->grid[current_position.x][current_position.y] = nullptr;
            return true;
        }
        else{
            return false;
        }
    }

    /*
     * Updatuje hodnoty pracovnika
     */
    bool update_worker(Worker* w){
        bool dead = w->progress_infection();
        if(dead){
            this->kill_worker(w);
            return false;
        }
        if (w->social_distance) {
            this->check_neighbors_SD(w);
        } else {
            this->check_neighbors_n_SD(w);
        }
        return true;
    }

    /*
     * Kontroluje okolite pozicie na volne miesta a na infekcnych susedov
     */
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
                if (check_point(temp_p)) {
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

    /*
     * Vola sa ak pracovnik nedodrzuje socialne distancovanie... ak sa pracovnik rozhodne 50/50 moze sa hybat kam chce
     */
    bool check_neighbors_n_SD(Worker *w){
        this->check_neighbors(w);
        this->check_infection(w);
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
        return true;
    }

    /*
     * Vola sa ak pracovnik dodrziava socialny distanc... Pracovnik ma obmedzeni pohyb tak aby okolo seba nemal inych pracovnikov
     */
    bool check_neighbors_SD(Worker *w){
        this->check_neighbors(w);
        this->check_infection(w);

        int move_length_SD = w->empty_spots->size() < 8 ? 1 : MOVE_LENGTH;

        if (w->empty_spots->size() < 8 or (random_int(1, 100) < w->movement_prob)){
            for(int step = 0; step < move_length_SD; step++){
                bool did_move = false;
                if (!w->empty_spots->empty()) {
                    // i < int(w->empty_spots->size())
                    for(int i = 0; i <= 20; i++){
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
        return true;
    }

    /*
     * Kontroluje ci je miesto kam sa chce pohnut bez pracovnikov v okoli
     */
    bool check_if_safe(Worker *w, position i){
        if(&i == w->last_position)
            return false;
        for (int y = -AREA; y <= AREA; y++) {
            for (int x = -AREA; x <= AREA; x++) {
                if (x == 0 and y == 0) {
                    continue;
                }
                if(this->grid[i.x + x][i.y + y])
                    return false;
            }
        }
        return true;
    }

    /*
     * Kontroluje ci sa pracovnik nakazil
     */
    void check_infection(Worker *w){
        bool newly_infected = w->get_infected();
        if(newly_infected){
            w->susceptible = false;
            this->dataCollector.increment_newly_infected();
            for(auto i = w->infectious_spots->begin(); i <= w->infectious_spots->end(); i++){
                i->num_people_infected +=1;
            }
        }
    }

    /*
     * Najprv sa prejdu pracovnici ktori nedodrzuju socialny distanc
     * Potom ti ktori ho dodrzuju aby mohli na konci iteracie byt bez susedov
     */
    void update_grid(){
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
     * Vytlaci reprezentaciu gridu
     */
    void show_grid(){
        for(auto & open_position : this->grid){
            for(auto & x : open_position){
                if(x != nullptr){
                    if(x->infected)
                        cout<<" 2 ";
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
     * Zisti ci je pozicia mimo hranic gridu
     */
    bool oob(position p){
        return p.x < 0 or p.y < 0 or p.x >= GRID_HEIGHT or p.y >= GRID_WIDTH;
    }

    /*
     * Zisti ci je pozicia volna
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

    /*
     * Vracia pracovnika podla pozicie
     */
    Worker* get_worker(position p){
        return this->grid[p.x][p.y];
    }

    /*
     * Spusti simulaciu pre pocet ITERATIONS
     */
    int run(){
        int i;
        for(i = 1; i < ITERATIONS + 1; i++){
            this->dataCollector.reset(i, false);
            this->update_grid();
            cout<<"    <=== Day: "<<i<<" ===> "<<endl;
            cout<<"Initial_susceptible:     "<<dataCollector.initial_S<<endl;
            cout<<"Newly_infected:          "<<dataCollector.newly_infected<<endl;
            cout<<"Initial_infected:        "<<dataCollector.initial_infected<<endl;
            cout<<"Total_infected:          "<<dataCollector.get_total_infected()<<endl;
            cout<<"Total dead:              "<<dataCollector.total_dead<<endl;
            cout<<"Total recovered:         "<<dataCollector.current_data["R"]<<endl;
            cout<<"Now Susceptible:         "<<dataCollector.current_data["S"]<<endl;
            cout<<"Now infected:            "<<dataCollector.current_data["I"]<<endl;
            cout<<"Now wear_mask:           "<<dataCollector.current_data["WM"]<<endl;
            cout<<"Now social_d:            "<<dataCollector.current_data["SD"]<<endl;
            cout<<"Now mild:                "<<dataCollector.current_data["mild"]<<endl;
            cout<<"Now severe:              "<<dataCollector.current_data["severe"]<<endl;
            cout<<"Now asymptomatic:        "<<dataCollector.current_data["asymptomatic"]<<endl;
            cout<<"Now total infected:      "<<dataCollector.adv_infection_data["total"]<<endl;
            cout<<"Now total inf SD:        "<<dataCollector.adv_infection_data["SD"]<<endl;
            cout<<"Now total inf not_SD:    "<<dataCollector.adv_infection_data["not_SD"]<<endl;
        }

        cout << endl << "History of information: " << endl;
        for( auto & data : dataCollector.current_data) {
            cout << data.first << endl;
            for (auto &more : dataCollector.data_history[data.first])
                cout << more << ", ";
            cout << endl;
        }
        cout << endl << "Advanced informations about infected history:" << endl;
        for( auto & data : dataCollector.adv_infection_data){
            cout << data.first << endl;
            for ( auto & more : dataCollector.adv_infection_data_history[data.first])
                cout << more << ", ";
            cout << endl;
        }
        dataCollector.reset(i + 1, true);
        for(auto w : workers){
            free(w->symptoms_periods);
            free(w->infectious_periods);
            free(w->empty_spots);
            free(w->infectious_spots);
        }
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

    free(cellularAutomat);

    return 0;

}
