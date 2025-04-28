/**
 * Crowd Collision Simulation (Parallel)
 * Elly Zheng (ellyz), Rose Liu (roseliu)
 */

 struct Agent {
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
  #include <omp.h>
  /* Uncomment the following line to use the simulation
  #include <SDL2/SDL.h> */
  
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
  
  void update_positions(std::vector<Agent>& agents, int num_agents) {
    #pragma omp parallel for
      for(int i = 0; i < num_agents; i++) {
          agents[i].x_pos = agents[i].next_x;
          agents[i].y_pos = agents[i].next_y;
      }
  }
  
  
  void check_collisions(std::vector<Agent>& agents, int num_agents, int dimX, int dimY, std::vector<omp_lock_t>& agent_locks) {
      #pragma omp parallel for schedule(dynamic)
      for(int i = 0; i < num_agents; i++) {
          for(int j = i+1; j < num_agents; j++) {
              // collision detected
              if (agents[i].next_x == agents[j].next_x && agents[i].next_y == agents[j].next_y) {
                    if (i < j) {
                        omp_set_lock(&agent_locks[i]);
                        omp_set_lock(&agent_locks[j]);
                    } else {
                        omp_set_lock(&agent_locks[j]);
                        omp_set_lock(&agent_locks[i]);
                    }
        
                  // agents[i]
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
                  
                  //  agents[j]
                  if(agents[j].dir == 0){
                      agents[j].dir = 2;
                      agents[j].next_y = agents[j].y_pos + 1; 
                      if(agents[j].next_y > dimY -1 || agents[j].next_y < 0){
                          agents[j].next_y = agents[j].y_pos;
                      }
                  }
                  else if(agents[j].dir == 1){
                      agents[j].dir = 3;
                      agents[j].next_x = agents[j].x_pos - 1;
                      if(agents[j].next_x > dimX -1 || agents[j].next_x < 0){
                          agents[j].next_x = agents[j].x_pos;
                      }
                      
                  }
                  else if(agents[j].dir == 2){
                      agents[j].dir = 0;
                      agents[j].next_y = agents[j].y_pos - 1;
                      if(agents[j].next_y > dimY -1 || agents[j].next_y < 0){
                          agents[j].next_y = agents[j].y_pos;
                      }
                  }
                  else{
                      agents[j].dir = 1;
                      agents[j].next_x = agents[j].x_pos + 1;
                      if(agents[j].next_x > dimX -1 || agents[j].next_x < 0){
                          agents[j].next_x = agents[j].x_pos;
                      }
                  }

                  omp_unset_lock(&agent_locks[j]);
                  omp_unset_lock(&agent_locks[i]);
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
  
  /* Uncomment the following section to use the simulation 

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
  
  
   void visualize_simulation(std::vector<Agent>& agents, int dim_x, int dim_y, int num_agents, int num_iterations,  const std::vector<std::tuple<int, int, int>>& agent_colors, std::vector<omp_lock_t>& agent_locks) {
      SDL_Init(SDL_INIT_VIDEO);
      SDL_Window* window = SDL_CreateWindow("Crowd Simulation",
                                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                            WINDOW_WIDTH, WINDOW_HEIGHT,
                                            SDL_WINDOW_SHOWN);
      SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  
      CELL_WIDTH = WINDOW_WIDTH / dim_x;
      CELL_HEIGHT = WINDOW_HEIGHT / dim_y;
  
      bool quit = false;
      SDL_Event event;
  
      int iteration_count = 0;
  
      while (!quit && iteration_count < num_iterations) {
          while (SDL_PollEvent(&event)) {
              if (event.type == SDL_QUIT)
                  quit = true;
          }
  
          #pragma omp parallel
        {
            std::mt19937 generator(std::random_device{}() + omp_get_thread_num());
            #pragma omp for
            for (int i = 0; i < num_agents; i++) {
                move_agent(i, agents[i], dim_x, dim_y, generator);
            }
        }
  
          check_collisions(agents, num_agents, dim_x, dim_y, agent_locks);
          update_positions(agents, num_agents);
  
          render_agents(renderer, agents, dim_x, dim_y, agent_colors);
  
          SDL_Delay(500);  
          if(!is_in_range(agents, num_agents, dim_x, dim_y)){
              printf("AGENT NOT IN RANGE\n");
          }
  
          iteration_count++;
      }
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();
  }
  */
  
  
  int main(int argc, char *argv[]) {
      const auto init_start = std::chrono::steady_clock::now();
   
      std::string input_filename;
      int num_iterations = 0;

      int num_threads = 0;
  
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
  
      for (auto& agent : agents) {
          fin >> agent.x_pos >> agent.y_pos >> agent.dir;
          agent.next_x = agent.x_pos;
          agent.next_y = agent.y_pos;
      }
  
      std::vector<std::tuple<int, int, int>> agent_colors;
  
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<> color_dist(0, 255);

      /* Uncomment the following section to use the simulation
      for (int i = 0; i < num_agents; i++) {
          agent_colors.push_back({
              color_dist(gen), color_dist(gen), color_dist(gen)
          });
      }
      */

      std::vector<omp_lock_t> agent_locks(num_agents);

        for (int i = 0; i < num_agents; ++i) {
            omp_init_lock(&agent_locks[i]);
        }
  
      const double init_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - init_start).count();
      std::cout << "Initialization time (sec): " << std::fixed << std::setprecision(10) << init_time << '\n';
   
      const auto compute_start = std::chrono::steady_clock::now();
  
      int iteration_count = 0;
  
      /* Uncomment the following line and comment the while loop to use the simulation 
      visualize_simulation(agents, dim_x, dim_y, num_agents, num_iterations, agent_colors, agent_locks); */
  
      while (iteration_count < num_iterations) {
            #pragma omp parallel
            {
                std::mt19937 generator(std::random_device{}() + omp_get_thread_num());
                #pragma omp for
                for (int i = 0; i < num_agents; i++) {
                    move_agent(i, agents[i], dim_x, dim_y, generator);
                }
            }
  
          check_collisions(agents, num_agents, dim_x, dim_y, agent_locks);
          update_positions(agents, num_agents);
          iteration_count += 1;
  
          if(!is_in_range(agents, num_agents, dim_x, dim_y)){
              printf("AGENT NOT IN RANGE\n");
          }
      }
    
      const double compute_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - compute_start).count();

      for (int i = 0; i < num_agents; ++i) {
        omp_destroy_lock(&agent_locks[i]);
     }
  
      std::cout << "Computation time (sec): " << compute_time << '\n';
  }
   