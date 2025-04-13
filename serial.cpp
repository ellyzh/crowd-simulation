/**
 * Large-scale Crowd Simulations (Sequential)
 * Elly Zheng (ellyz), Rose Liu (roseliu)
 */

struct Agent {
  /* Define the data structure for agent here. */ 
  int x_pos, y_pos, dir, next_x, next_y;
};

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

bool is_in_range(std::vector<Agent> agents, int num_agents, int dim_x, int dim_y){

    for(int i = 0; i < num_agents; i++) {
        int x = agents[i].x_pos; 
        int y = agents[i].y_pos; 

        if(x > dim_x-1 || x < 0 || y > dim_y-1|| y < 0){
            return false;
        } 
    }
    return true;
}

void update_positions(std::vector<Agent>& agents, int num_agents) {
    for(int i = 0; i < num_agents; i++) {
        agents[i].x_pos = agents[i].next_x;
        agents[i].y_pos = agents[i].next_y;
    }
}

void check_collisions(std::vector<Agent>& agents, int num_agents) {

    for(int i = 0; i < num_agents; i++) {
        for(int j = i; j < num_agents; j++) {
            // collision detected
            if (i != j && agents[i].next_x == agents[j].next_x && agents[i].next_y == agents[j].next_y) {

                printf("Collision at %d %d\n", agents[i].next_x, agents[i].next_y);

                // agents[i]
                if(agents[i].dir == 0){
                    agents[i].dir = 2;
                }
                else if(agents[i].dir == 1){
                    agents[i].dir = 3;
                }
                else if(agents[i].dir == 2){
                    agents[i].dir = 0;
                }
                else{
                    agents[i].dir = 1;
                }
                
                //  agents[j]
                if(agents[j].dir == 0){
                    agents[j].dir = 2;
                }
                else if(agents[j].dir == 1){
                    agents[j].dir = 3;
                    
                }
                else if(agents[j].dir == 2){
                    agents[j].dir = 0;

                }
                else{
                    agents[j].dir = 1;

                }
                // directions have been reset
                agents[i].next_x = agents[i].x_pos;
                agents[i].next_y = agents[i].y_pos;
                agents[j].next_x = agents[j].x_pos;
                agents[j].next_y = agents[j].y_pos;
            }
        }
    }
}


// assuming no collisions
void move_agent(int agent_id, Agent &agent, int dimX, int dimY, std::mt19937 generator) { 
    int currX = agent.x_pos;
    int currY = agent.y_pos;

    agent.next_x = agent.x_pos;
    agent.next_y = agent.y_pos;
    
    int direction = -1;

    // N E S W
    // 0 1 2 3
    std::uniform_int_distribution<int> dist_2(0, 1);
    std::uniform_int_distribution<int> dist_3(0, 2);

    if(currX == 0 && currY == 0){ // top left
        // only options are E and S
        int next_dir = dist_2(generator); 
        if (next_dir == 0){
            direction = 1; // E
        }
        else{
            direction = 2; // S
        }
    } 
    else if (currX == dimX-1 && currY == 0){ // top right
        // only options are S and W
        int next_dir = dist_2(generator); 
        if (next_dir == 0) {
            direction = 2; // S
        }
        else {
            direction = 3; // W
        }
    }
    else if (currX == 0 && currY == dimY-1){ // bottom left
         // only options are N and E
        int next_dir = dist_2(generator); 
        if (next_dir == 0) {
            direction = 0; // N
        }
        else {
            direction = 1; // E
        }
    }
    else if (currX == dimX-1 && currY == dimY-1 ){ // bottom right
        // only options are N and W
        int next_dir = dist_2(generator);
        if (next_dir == 0) {
            direction = 0; // N
        }
        else {
            direction = 3; // W
        }
    }
    else if ((currX == dimX-1 && agent.dir == 1) || (currX == 0 && agent.dir == 3) || (currY == dimY-1 && agent.dir == 2) || (currY == 0 && agent.dir == 0)){
        // agent is on edge
        if (currX == dimX-1) { // right
            int next_dir = dist_3(generator); 
            if (next_dir == 0){
                direction = 0;    
            }
            else if(next_dir == 1){
                direction = 2;
            }
            else{
                direction = 3;
            }
        }
        else if (currX == 0) { // left
            int next_dir = dist_3(generator); 
            if (next_dir == 0) { // N
                direction = 0;
            }
            else if (next_dir == 1) { // E
                direction = 1;
            }
            else { // S
                direction = 2;
            }
        }
        else if(currY == dimY-1){ // bottom
            int next_dir = dist_3(generator); 
            if (next_dir == 0){ // W
                direction = 3;
            }
            else if(next_dir == 1){ // N
                direction = 0;
            }
            else{ // E
                direction = 1;
            }
        }
        else { // top
            int next_dir = dist_3(generator); 
            if (next_dir == 0){ // E
                direction = 1; 
            }
            else if(next_dir == 1){ // S
                direction = 2;
 
            }
            else{ // W
                direction = 3;
            }
        }
    }
    else { // free space
        direction = agent.dir;
    }

    if (direction == 0) {      
        agent.next_y = currY - 1;
    }
    else if (direction == 1) {
        agent.next_x = currX + 1;
    }
    else if (direction == 2) {
        agent.next_y = currY + 1;
    }
    else if (direction == 3) {
        agent.next_x = currX - 1;
    }

    if(agent.next_x > dimX -1 || agent.next_x < 0 || agent.next_y > dimY -1|| agent.next_y < 0){
        printf("OUT OF RANGE NEXT X Y, %d %d", agent.next_x, agent.next_y);
    }

    return;
 }


int main(int argc, char *argv[]) {
    const auto init_start = std::chrono::steady_clock::now();
 
    std::string input_filename;
    int num_iterations = 0;

    int opt;
    while ((opt = getopt(argc, argv, "f:i:")) != -1) {
        switch (opt) {
        case 'f':
            input_filename = optarg;
            break;
        case 'i':
            num_iterations = atoi(optarg);
            break;

        default:
            std::cerr << "Usage: " << argv[0] << " -f input_filename\n";
            exit(EXIT_FAILURE);
        }
    }
 
    // Check if required options are provided
    if (empty(input_filename) ||  num_iterations <= 0) {
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

    for (auto& agent : agents) {
        fin >> agent.x_pos >> agent.y_pos >> agent.dir;
        agent.next_x = agent.x_pos;
        agent.next_y = agent.y_pos;
    }
 
    const double init_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - init_start).count();
    std::cout << "Initialization time (sec): " << std::fixed << std::setprecision(10) << init_time << '\n';
 
    const auto compute_start = std::chrono::steady_clock::now();

    int iteration_count = 0;

    while (iteration_count < num_iterations) {

        for (int i = 0; i < num_agents; i++) {
            std::random_device rd;  
            std::mt19937 generator(rd()); 
            move_agent(i, agents[i], dim_x, dim_y, generator);
        }

        check_collisions(agents, num_agents);
        update_positions(agents, num_agents);
        iteration_count += 1;

        if(!is_in_range(agents, num_agents, dim_x, dim_y)){
            printf("AGENT NOT IN RANGE\n");
        }
    }
  
    const double compute_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - compute_start).count();

    std::cout << "Computation time (sec): " << compute_time << '\n';
}
 