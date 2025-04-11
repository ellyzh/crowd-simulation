/**
 * Large-scale Crowd Simulations (Parallel with Open MP)
 * Elly Zheng (ellyz), Rose Liu (roseliu)
 */

struct Agent {
    /* Define the data structure for agent here. */ 
    int start_x, start_y, end_x, end_y, curr_x, curr_y, occu_val;
};
  
#define GRID_VALUE_LIMIT 6
#define PROBABILITY 0.1
  
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <string>
#include <vector>
   
#include <random>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <climits>
   
#include <unistd.h>
#include <omp.h>

bool is_finished(const std::vector<Agent>& agents, int num_agents) {
    for(int i = 0; i < num_agents; i++){
        if(agents[i].curr_x != agents[i].end_x || agents[i].curr_y != agents[i].end_y) {
            return false;
        }
    }
    return true;
}


bool verify_occupancy(std::vector<std::vector<int>>& occupancy, int dim_x, int dim_y) {
    for (int i = 0; i < dim_y; i++) {
        for (int j = 0; j < dim_x; j++) {
            if(occupancy[i][j] > GRID_VALUE_LIMIT) {
                return false;
            }
        }
    }
    return true;
}


void write_to_occupancy(Agent &agent, std::vector<std::vector<int>>& occupancy, bool add) {
    int currX = agent.curr_x;
    int currY = agent.curr_y;
   
    int agent_val = agent.occu_val;
 
    if(add){ 
        #pragma omp atomic
        occupancy[currY][currX] += agent_val;
    }
    else {
        #pragma omp atomic
        occupancy[currY][currX] -= agent_val;
    }
}

// assuming direction is valid
// brings you closer to end goal
bool brings_closer(Agent &agent, int direction){
    int currX = agent.curr_x;
    int currY = agent.curr_y;

    int endX = agent.end_x;
    int endY = agent.end_y;

    if(direction == 0 && (currY - endY) > 0){ // NORTH 
        return true;
    }
    else if (direction == 1 && (currX - endX) < 0){ // EAST
        return true;
    }
    else if(direction == 2 && (currY - endY) < 0){ // SOUTH
        return true;
    }
    else if (direction == 3 && (currX - endX) > 0){ // WEST
        return true;
    }
    else {
        return false;
    }
 }
 
 
void move_agent(int agent_id, Agent &agent, const std::vector<std::vector<int>>& occupancy, int dimX, int dimY, std::mt19937 generator) { 
    int currX = agent.curr_x;
    int currY = agent.curr_y;

    // check which ones are feasible
    std::vector<int> feasible;

    // N E S W
    if(currY-1 >= 0 && (occupancy[currY-1][currX] + agent.occu_val) <= GRID_VALUE_LIMIT) { // check N
        feasible.push_back(0);
    }
    if (currX+1 < dimX && (occupancy[currY][currX+1] + agent.occu_val) <= GRID_VALUE_LIMIT) { // check E
        feasible.push_back(1);
    }
    if (currY+1 < dimY && (occupancy[currY+1][currX] + agent.occu_val) <= GRID_VALUE_LIMIT) { // check S
        feasible.push_back(2);
    }
    if (currX-1 >= 0 && (occupancy[currY][currX-1] + agent.occu_val) <= GRID_VALUE_LIMIT) { // check W
        feasible.push_back(3);
    }

    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    double P = distribution(generator); // percentage
    int result = (P < 1 - PROBABILITY) ? 1 : 0;

    int direction = -1;

    std::shuffle(feasible.begin(), feasible.end(), generator);

    if (result == 1) { // less than 0.9, choose best move
        for (int index : feasible) {
            if (brings_closer(agent, index)) {
                direction = index;
                break;
            }
        }
    }
    else { // choose random move
        if (feasible.size() != 0) {
            direction = feasible.at(0);
        }
    }

    bool is_trapped = feasible.size() == 0 ? true : false;

    if (is_trapped){
        printf("Agent is trapped at %d %d\n", currX, currY);
        if(currX == dimX-1 || currX == 0 || currY == dimY-1 || currY == 0){
            printf("Agent is on edge");
        }
    }

    if (direction == 0) {
        agent.curr_y -= 1;
    }
    else if (direction == 1) {
        agent.curr_x += 1;
    }
    else if (direction == 2) {
        agent.curr_y += 1;
    }
    else if (direction == 3) {
        agent.curr_x -= 1;
    }

    return;
 }
  
 
int main(int argc, char *argv[]) {
    const auto init_start = std::chrono::steady_clock::now();
 
    std::string input_filename;
    int num_threads = 0;
 
    int opt;
    while ((opt = getopt(argc, argv, "f:n:")) != -1) {
        switch (opt) {
        case 'f':
            input_filename = optarg;
            break;
        case 'n':
            num_threads = atoi(optarg);
            break;

        default:
            std::cerr << "Usage: " << argv[0] << " -f input_filename\n";
            exit(EXIT_FAILURE);
        }
    }
 
    // Check if required options are provided
    if (empty(input_filename) ||  num_threads <= 0) {
        std::cerr << "Usage: " << argv[0] << " -f input_filename\n";
        exit(EXIT_FAILURE);
    }
    std::cout << "Number of threads: " << num_threads << '\n';

    std::ifstream fin(input_filename);
 
    if (!fin) {
        std::cerr << "Unable to open file: " << input_filename << ".\n";
        exit(EXIT_FAILURE);
    }
 
    int dim_x, dim_y;
    int num_agents;
 
    /* Read the grid dimension and agent information from file */
    fin >> dim_x >> dim_y >> num_agents;
 
    std::vector<Agent> agents(num_agents);
   
    // Occupancy grid is created
    std::vector occupancy(dim_y, std::vector<int>(dim_x));
 
    for (auto& agent : agents) {
        fin >> agent.start_x >> agent.start_y >> agent.end_x >> agent.end_y >> agent.occu_val;
        agent.curr_x = agent.start_x;
        agent.curr_y = agent.start_y;
    }
 
    /* Initialize any additional data structures needed in the algorithm */
 
    const double init_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - init_start).count();
    std::cout << "Initialization time (sec): " << std::fixed << std::setprecision(10) << init_time << '\n';
 
    const auto compute_start = std::chrono::steady_clock::now();

    int iteration_count = 0;
    std::random_device rd; 
    
    omp_set_num_threads(num_threads);

    while (!is_finished(agents, num_agents)) {
        
       //  #pragma omp parallel 
        for (int i = 0; i < num_agents; i++){
            if (agents[i].curr_x == agents[i].end_x && agents[i].curr_y == agents[i].end_y){
                // printf("Agent %d has already completed it's mission\n", i);
                continue;
            }
            else {
                // printf("Agent %d has not completed it's mission, %d %d\n", i, agents[i].curr_x, agents[i].curr_y);    

                if(iteration_count != 0){
                    write_to_occupancy(agents[i], occupancy, false);
                }
        
                std::mt19937 generator(rd()); 
                
                move_agent(i, agents[i], occupancy, dim_x, dim_y, generator);

                // if(agents[i].curr_x == agents[i].end_x && agents[i].curr_y == agents[i].end_y){
                //     printf("Agent %d has completed it's mission\n", i);
                // }
                write_to_occupancy(agents[i], occupancy, true);
            }
      }

      iteration_count += 1;

      if(!verify_occupancy(occupancy, dim_x, dim_y)){
        printf("Occupancy did not verify");
        break;
      }
    }
    printf("Iteration Count: %d\n",iteration_count);
  
    const double compute_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - compute_start).count();
    std::cout << "Computation time (sec): " << compute_time << '\n';
}
 