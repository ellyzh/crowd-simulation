/**
 * Quadtree Structure (Header file)
 * Elly Zheng (ellyz), Rose Liu (roseliu)
 */

 #ifndef QUADTREE_H
 #define QUADTREE_H

 #include <array>
 #include <vector>
 #include <memory> 

 const int max_agents = 4;
 const int max_depth = 10;

 struct Agent {
    /* Define the data structure for agent here. */ 
    int x_pos, y_pos, dir, next_x, next_y;
 };
  
 class Quadtree {
     public:
         // bounds
         int min_x, min_y, max_x, max_y;
         int depth;
 
         std::vector<Agent> agents;
         std::array<std::unique_ptr<Quadtree>, 4> children;

         //std::array<Quadtree, 4> children;
 
    //  public:
        Quadtree(int min_x, int min_y, int max_x, int max_y, int depth):
        min_x(min_x), min_y(min_y), max_x(max_x), max_y(max_y), depth(depth){
            for (int i = 0; i < 4; i++) {
                children[i] = nullptr;
            }
        }
        
          
        void split();
        int getQuadrant(Agent &agent);
        void insert(Agent &agent);
        void reset();
        Quadtree *get_leaf(Agent &agent);
        std::vector<Agent> collidable_agents();
 };
 
 #endif