/**
 * Large-scale Crowd Simulations
 * Elly Zheng (ellyz), Rose Liu (roseliu)
 */

struct Agent {
  /* Define the data structure for agent here. */ 
  int start_x, start_y, end_x, end_y, curr_x, curr_y, occu_val;
};

#define GRID_VALUE_LIMIT 4

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
 

bool is_finished(const std::vector<Agent>& agents, int num_agents){
    for(int i = 0; i < num_agents;i++){
        if(agents[i].curr_x != agents[i].end_x) {
            return false;
        }
        if(agents[i].curr_y != agents[i].end_y) {
            return false;
        }
    }
    return true;
}


void write_to_occupancy(Agent &agent, std::vector<std::vector<int>>& occupancy, bool add) {
    int startX = agent.start_x;
    int startY = agent.start_y;
    int endX = agent.end_x;
    int endY = agent.end_y;
 
    int currX = agent.curr_x;
    int currY = agent.curr_y;
   
    int agent_val = agent.occu_val;
 
    if(add){ 
        occupancy[currY][currX] += agent_val;
    }
    else {
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
    else{
        return false;
    }
 }
 
void move_agent(Agent &agent, const std::vector<std::vector<int>>& occupancy, int dimX, int dimY) { 
    int startX = agent.start_x;
    int startY = agent.start_y;
    
    int currX = agent.curr_x;
    int currY = agent.curr_y;

    int endX = agent.end_x;
    int endY = agent.end_y;
    
    // check which ones are feasible
    int feasible[4]; // set all to zero

    // N E S W
    if(currY-1 >= 0 && occupancy[currY-1][currX] + agent.occu_val > GRID_VALUE_LIMIT) { // check N
        feasible[0] = 1;
    }
    if (currX+1 < dimX && occupancy[currY][currX+1] + agent.occu_val > GRID_VALUE_LIMIT) { // check E
        feasible[1] = 1;
    }
    if (currY+1 < dimY && occupancy[currY+1][currX] + agent.occu_val > GRID_VALUE_LIMIT) { // check S
        feasible[2] = 1;
    }
    if (currX-1 >= 0 && occupancy[currY][currX-1] + agent.occu_val > GRID_VALUE_LIMIT) { // check W
        feasible[3] = 1;
    }

    int direction = -1;

    for (int i = 0; i < 3;i++){
        if(feasible[i] == 0){
            continue;
        }
        if(brings_closer(agent, i)){
            direction = i;
        }
    } // only create positive results for the agent

    if (direction == 0) {
        agent.curr_y -= 1;
    }
    else if (direction == 1) {
        agent.curr_x +=1;
    }
    else if (direction == 2) {
        agent.curr_y +=1;
    }
    else if (direction == 3) {
        agent.curr_x -= 1;
    }
    return;
 }
  
 
int main(int argc, char *argv[]) {
    const auto init_start = std::chrono::steady_clock::now();
 
    std::string input_filename;
 
    int opt;
    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
        case 'f':
            input_filename = optarg;
            break;

        default:
            std::cerr << "Usage: " << argv[0] << " -f input_filename\n";
            exit(EXIT_FAILURE);
        }
    }
 
    // Check if required options are provided
    if (empty(input_filename)) {
        std::cerr << "Usage: " << argv[0] << " -f input_filename\n";
        exit(EXIT_FAILURE);
    }
 
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
        fin >> agent.start_x >> agent.start_y >> agent.end_x >> agent.end_y;
        agent.curr_x = agent.start_x;
        agent.curr_y = agent.start_y;
    }
 
    /* Initialize any additional data structures needed in the algorithm */
 
    const double init_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - init_start).count();
    std::cout << "Initialization time (sec): " << std::fixed << std::setprecision(10) << init_time << '\n';
 
    const auto compute_start = std::chrono::steady_clock::now();

    while (!is_finished(agents, num_agents)) {
      for(int i = 0; i< num_agents;i++){
      
            write_to_occupancy(agents[i], occupancy, false);

            move_agent(agents[i], occupancy, dim_x, dim_y);

            write_to_occupancy(agents[i], occupancy, true);
      }
    }
  
    const double compute_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - compute_start).count();
    std::cout << "Computation time (sec): " << compute_time << '\n';
 
}
 