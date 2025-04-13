/**
 * Quadtree Structure (Header file)
 * Elly Zheng (ellyz), Rose Liu (roseliu)
 */

 #ifndef QUADTREE_H
 #define QUADTREE_H

 #include <array>
 #include <vector>
 
 const int max_agents = 4;
 const int max_depth = 10;

 struct Agent {
    /* Define the data structure for agent here. */ 
    int x_pos, y_pos, dir, next_x, next_y;
 };
  
 class Quadtree {
     private:
         // bounds
         int min_x, min_y, max_x, max_y;
         int depth;
 
         std::vector<Agent> agents();
 
         std::array<Quadtree, 4> children;
 
     public:
         Quadtree(int min_x, int min_y, int max_x, int max_y, int depth);
          
         void split();
         void getQuadrant(Agent &agent);
         void insert(Agent &agent);
         void reset();
 };
 
 #endif