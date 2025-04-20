/**
 * Crowd Collision Simulation (Parallel with Open MP)
 * Elly Zheng (ellyz), Rose Liu (roseliu)
 */
  
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
#include "quadtree.h"

#include <omp.h>
#include <SDL2/SDL.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

int CELL_WIDTH;
int CELL_HEIGHT;

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
  
void check_collisions(Agent &agent, std::vector<Agent>& agents, int dimX, int dimY) {

    // N E S W
    // 0 1 2 3

    for(int i = 0; i < agents.size(); i++) {
        if (agents[i].id != agent.id && (agent.next_x == agents[i].next_x && agent.next_y == agents[i].next_y)) {
            //  || (agents[i].next_x == agent.x_pos && agents[i].next_y == agent.y_pos)
            if(agents[i].dir == 0){ 
                agents[i].dir = 2;
                agents[i].next_y = agents[i].y_pos + 1; 

                if(agents[i].next_y > dimY -1 || agents[i].next_y < 0){
                    agents[i].next_y = agents[i].y_pos;
                }
            }
            else if(agents[i].dir == 1){
                agents[i].dir = 3;
                agents[i].next_x = agents[i].x_pos - 1;

                if(agents[i].next_x > dimX -1 || agents[i].next_x < 0){
                    agents[i].next_x = agents[i].x_pos;
                }
            }
            else if(agents[i].dir == 2){
                agents[i].dir = 0;
                agents[i].next_y = agents[i].y_pos - 1;
                if(agents[i].next_y > dimY -1 || agents[i].next_y < 0){
                    agents[i].next_y = agents[i].y_pos;
                }
            }
            else{
                agents[i].dir = 1;
                agents[i].next_x = agents[i].x_pos + 1;
                if(agents[i].next_x > dimX -1 || agents[i].next_x < 0){
                    agents[i].next_x = agents[i].x_pos;
                }
            }
            
            if(agent.dir == 0){ 
                agent.dir = 2;
                agent.next_y = agent.y_pos + 1; 

                if(agent.next_y > dimY -1 || agent.next_y < 0){
                    agent.next_y = agent.y_pos;
                }
            }
            else if(agent.dir == 1){
                agent.dir = 3;
                agent.next_x = agent.x_pos - 1;

                if(agent.next_x > dimX -1 || agent.next_x < 0){
                    agent.next_x = agent.x_pos;
                }
            }
            else if(agent.dir == 2){
                agent.dir = 0;
                agent.next_y = agent.y_pos - 1;
                if(agent.next_y > dimY -1 || agent.next_y < 0){
                    agent.next_y = agent.y_pos;
                }
            }
            else{
                agent.dir = 1;
                agent.next_x = agent.x_pos + 1;
                if(agent.next_x > dimX -1 || agent.next_x < 0){
                    agent.next_x = agent.x_pos;
                }
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
            direction = 1; 
        }
        else{
            direction = 2;
        }
    } 
    else if (currX == dimX-1 && currY == 0){ // top right
        // only options are S and W
        int next_dir = dist_2(generator); 
        if (next_dir == 0) {
            direction = 2;
        }
        else {
            direction = 3;
        }
    }
    else if (currX == 0 && currY == dimY-1){ // bottom left
        // only options are N and E
        int next_dir = dist_2(generator); 
        if (next_dir == 0) {
            direction = 0;
        }
        else {
            direction = 1;
        }
    }
    else if (currX == dimX-1 && currY == dimY-1 ){ // bottom right
        // only options are N and W
        int next_dir = dist_2(generator);
        if (next_dir == 0) {
            direction = 0; 
        }
        else {
            direction = 3;
        }
    }

    else if ((currX == dimX-1 && agent.dir == 1) || (currX == 0 && agent.dir == 3) || (currY == dimY-1 && agent.dir == 2) || (currY == 0 && agent.dir == 0)){
        // agent is on edge
        if (currX == dimX-1) { // right
            direction = 3;
        }
        else if (currX == 0) { // left
            direction = 1;
        }
        else if(currY == dimY-1){ // bottom
            direction = 0;
        }
        else { // top
            direction = 2;
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
    agent.dir = direction;
    
    return;
}


void printQuadtree(const Quadtree &node, int level = 0) {
    std::string indent(level * 2, ' ');

    std::cout << indent << "Node: [" << node.min_x << ", " << node.min_y << "] to ["
              << node.max_x << ", " << node.max_y << "], Agents: " << node.agents.size() << "\n";
    
    for (int i = 0; i < 4; i++) {
        if (node.children[i] != nullptr) {
            std::cout << indent << "Child " << i << ":\n";
            printQuadtree(*(node.children[i]), level + 1);
        }
    }
}

void render_agents(SDL_Renderer* renderer, const std::vector<Agent>& agents, int dim_x, int dim_y, const std::vector<std::tuple<int, int, int>>& agent_colors) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white background
    SDL_RenderClear(renderer);

    for (size_t i = 0; i < agents.size(); ++i) {
        const auto& agent = agents[i];
        const auto& [r, g, b] = agent_colors[i];
    
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_Rect rect;
        rect.x = agent.x_pos * CELL_WIDTH;
        rect.y = agent.y_pos * CELL_HEIGHT;
        rect.w = CELL_WIDTH;
        rect.h = CELL_HEIGHT;
        SDL_RenderFillRect(renderer, &rect);
    }
    SDL_RenderPresent(renderer);
}


 void visualize_simulation(std::vector<Agent>& agents, int dim_x, int dim_y, int num_agents, int num_iterations, const std::vector<std::tuple<int, int, int>>& agent_colors) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Crowd Simulation",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // CELL_WIDTH = std::max(1, WINDOW_WIDTH / dim_x);
    // CELL_HEIGHT = std::max(1, WINDOW_HEIGHT / dim_y);
    CELL_WIDTH = WINDOW_WIDTH / dim_x;
    CELL_HEIGHT = WINDOW_HEIGHT / dim_y;

    bool quit = false;
    SDL_Event event;

    int iteration_count = 0;

    Quadtree qt(0, 0, dim_x-1, dim_y-1, 0);

    while (!quit && iteration_count < num_iterations) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                quit = true;
        }
        // clear quadtree
        qt.reset();

        #pragma omp parallel
        {
            std::mt19937 generator(std::random_device{}() + omp_get_thread_num());
            #pragma omp for
            for (int i = 0; i < num_agents; i++) {
                move_agent(i, agents[i], dim_x, dim_y, generator);
            }
        }
   
        // build quadtree by inserting elements
        for (int i = 0; i < num_agents; i++) {
            qt.multiInsert(agents[i]); // if needed: make this thread-safe
        }

        for (int i = 0; i < num_agents; i++){
            Quadtree *leaf = qt.get_leaf(agents[i]);
            // query quadtree to find which nodes could possibly collide
            std::vector<Agent> to_compare = leaf->collidable_agents();
            check_collisions(agents[i], to_compare, dim_x, dim_y);
        }

        #pragma omp parallel for
        for(int i = 0; i < num_agents; i++) {
            agents[i].x_pos = agents[i].next_x;
            agents[i].y_pos = agents[i].next_y;
        }

        render_agents(renderer, agents, dim_x, dim_y, agent_colors);

        SDL_Delay(100);  

        if(!is_in_range(agents, num_agents, dim_x, dim_y)){
            printf("AGENT NOT IN RANGE\n");
        }
        iteration_count += 1;

    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

  
int main(int argc, char *argv[]) {
    const auto init_start = std::chrono::steady_clock::now();
   
    std::string input_filename;
    int num_threads = 0;
    int num_iterations = 0;
  
    int opt;
    while ((opt = getopt(argc, argv, "f:i:n:")) != -1) {
        switch (opt) {
        case 'f':
            input_filename = optarg;
            break;
        case 'i':
            num_iterations = atoi(optarg);
            break;
        case 'n':
            num_threads = atoi(optarg);
            break;
        default:
            std::cerr << "Usage: " << argv[0] << " -f input_filename\n";
            exit(EXIT_FAILURE);
        }
    }

    // check if required options are provided
    if (empty(input_filename) ||  num_iterations <= 0 || num_threads <= 0) {
        std::cerr << "Usage: " << argv[0] << " -f input_filename\n";
        exit(EXIT_FAILURE);
    }

    omp_set_num_threads(num_threads);
   
    std::ifstream fin(input_filename);
   
    if (!fin) {
        std::cerr << "Unable to open file: " << input_filename << ".\n";
        exit(EXIT_FAILURE);
    }
   
    int dim_x, dim_y;
    int num_agents;
   
    // read the grid dimension and agent information from file
    fin >> dim_x >> dim_y >> num_agents;
   
    std::vector<Agent> agents(num_agents);

    int id_counter = 0;
  
    for (auto& agent : agents) {
        fin >> agent.x_pos >> agent.y_pos >> agent.dir;
        agent.next_x = agent.x_pos;
        agent.next_y = agent.y_pos;
        agent.id = id_counter;
        id_counter++;
    }
   
    const double init_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - init_start).count();
    std::cout << "Initialization time (sec): " << std::fixed << std::setprecision(10) << init_time << '\n';

    std::vector<std::tuple<int, int, int>> agent_colors;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> color_dist(0, 255);

    for (int i = 0; i < num_agents; i++) {
        agent_colors.push_back({
            color_dist(gen), color_dist(gen), color_dist(gen)
        });
    }
   
    const auto compute_start = std::chrono::steady_clock::now();
  
    //visualize_simulation(agents, dim_x, dim_y, num_agents, num_iterations, agent_colors);

    
    int iteration_count = 0;

    Quadtree qt(0, 0, dim_x-1, dim_y-1, 0);

    while (iteration_count < num_iterations) {
        // clear quadtree
        qt.reset();

        #pragma omp parallel
        {
            std::mt19937 generator(std::random_device{}() + omp_get_thread_num());
            #pragma omp for
            for (int i = 0; i < num_agents; i++) {
                move_agent(i, agents[i], dim_x, dim_y, generator);
            }
        }
            
        // build quadtree by inserting elements
        for (int i = 0; i < num_agents; i++) {
            qt.multiInsert(agents[i]); // if needed: make this thread-safe
        }

        for (int i = 0; i < num_agents; i++){
            Quadtree *leaf = qt.get_leaf(agents[i]);
            // query quadtree to find which nodes could possibly collide
            std::vector<Agent> to_compare = leaf->collidable_agents();
            check_collisions(agents[i], to_compare, dim_x, dim_y);
        }

        #pragma omp parallel for
        for(int i = 0; i < num_agents; i++) {
            agents[i].x_pos = agents[i].next_x;
            agents[i].y_pos = agents[i].next_y;
        }

        iteration_count += 1;
  
        if(!is_in_range(agents, num_agents, dim_x, dim_y)){
            printf("AGENT NOT IN RANGE\n");
        }
    }
    
    
    const double compute_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - compute_start).count();
  
    std::cout << "Computation time (sec): " << compute_time << '\n';
  }
   