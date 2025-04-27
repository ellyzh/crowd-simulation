/**
 * Quadtree Structure (Header file)
 * Elly Zheng (ellyz), Rose Liu (roseliu)
 */

 #ifndef QUADTREE_H
 #define QUADTREE_H

 #include <array>
 #include <vector>
 #include <memory> 
 #include <omp.h>


 const int max_agents = 4;
 const int max_depth = 5;

 struct Agent {
    int x_pos, y_pos, dir, next_x, next_y;
    int id;
 };
  
 class Quadtree {
     public:
         // bounds
         int min_x, min_y, max_x, max_y;
         int depth;
         static int next_id;
         int id;
         
         omp_lock_t lock;
 
         //std::vector<Agent> agents;
         std::vector<Agent*> agents;

         std::array<std::unique_ptr<Quadtree>, 4> children;
 
        Quadtree(int min_x, int min_y, int max_x, int max_y, int depth):
        min_x(min_x), min_y(min_y), max_x(max_x), max_y(max_y), depth(depth){
            for (int i = 0; i < 4; i++) {
                children[i] = nullptr;
            }
            omp_init_lock(&lock);

            #pragma omp atomic capture
            id = next_id++;
        }

        ~Quadtree() {
            omp_destroy_lock(&lock);
        }
        
        void split();
        int getQuadrant(Agent &agent);
        std::vector<int> getMultiQuadrant(const Agent &agent);
        void insert(Agent &agent);
        void multiInsert(Agent *agent);
        void reset();
        Quadtree *get_leaf(Agent &agent);
        std::vector<Agent*> collidable_agents();
        //void remove(Agent &agent);
        void multiRemove(Agent *agent);
        void get_leaf_nodes(Agent& agent, std::vector<int>& leaves);
        void multiInsert_2(Agent *agent, std::vector<std::vector<int>>&  leaves);
 };
 
 #endif