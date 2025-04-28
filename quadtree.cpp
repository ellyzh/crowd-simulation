/**
 * Quadtree Structure
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
 #include <memory>  
    
 #include <unistd.h>
 #include <omp.h>
 #include "quadtree.h"
 
 int Quadtree::next_id = 0;
 
 void Quadtree::split() {
     int midX = (min_x + max_x) / 2;
     int midY = (min_y + max_y) / 2;
  
     #pragma omp parallel sections 
     {
         #pragma omp section 
         {
             children[0] = std::make_unique<Quadtree>(min_x,min_y, midX, midY, depth + 1);
         }
 
         #pragma omp section 
         {
             children[1] = std::make_unique<Quadtree>(midX, min_y, max_x, midY, depth + 1);
        }
 
         #pragma omp section 
         {
             children[2] = std::make_unique<Quadtree>(min_x, midY, midX, max_y, depth + 1);
        }
         
         #pragma omp section 
         {
             children[3] = std::make_unique<Quadtree>(midX, midY, max_x, max_y, depth + 1);
         }
    }
     children[0]->id = Quadtree::next_id++;
     children[1]->id = Quadtree::next_id++;
     children[2]->id = Quadtree::next_id++;
     children[3]->id = Quadtree::next_id++;
 }
 
   
 int Quadtree::getQuadrant(Agent &agent) {
     int midX = (min_x + max_x) / 2;
     int midY = (min_y + max_y) / 2;
             
     bool top = agent.y_pos <= midY;
     bool bottom = agent.y_pos >= midY;
     bool right = agent.x_pos >= midX;
     bool left = agent.x_pos <= midX;
 
     if (top && left) {
         return 0;
     }
     else if (top && right) {
         return 1;
     }
     else if (bottom && left) { 
         return 2;
     }
     else if (bottom && right) { 
         return 3;
     }
     return -1;
 }
 
 std::vector<int> Quadtree::getMultiQuadrant(const Agent &agent) {
     int midX = (min_x + max_x) / 2;
     int midY = (min_y + max_y) / 2;
 
     std::vector<int> possible_quadrants;
 
     bool top = agent.y_pos <= midY + 2;
     bool bottom = agent.y_pos >= midY - 1;
     bool right = agent.x_pos >= midX - 1;
     bool left = agent.x_pos <= midX + 2;
 
     if (top && left) { 
         possible_quadrants.push_back(0);
     }
     if (top && right) { 
         possible_quadrants.push_back(1);
     }
     if (bottom && left) { 
         possible_quadrants.push_back(2);
     }
     if (bottom && right) {
         possible_quadrants.push_back(3);
     }
     return possible_quadrants;
 }
 
 void Quadtree::reset() {
     agents.clear();
     #pragma omp parallel for
     for(int i = 0; i < 4; i++){
         if (children[i] != nullptr) {
             children[i]->reset();  
             children[i].reset(); 
         }
     }
 }
 
 
 Quadtree* Quadtree::get_leaf(Agent &agent) {
     int index = -1;
     Quadtree *curr = this;
 
     while (curr->children[0] != nullptr){
         index = curr->getQuadrant(agent);
         curr = curr->children[index].get();
     }
     return curr;  
 }
         
 
 std::vector<Agent*> Quadtree::collidable_agents() {
     std::vector<Agent*> collidable = agents;
     return collidable;
 }
 
 
 void Quadtree::multiRemove(Agent *agent) {
     omp_set_lock(&lock);
 
     if (children[0] != nullptr) { 
         std::vector<int> indices = getMultiQuadrant(*agent);
         omp_unset_lock(&lock);
 
         if (!indices.empty()) {
             for (auto index: indices) {
                 if (children[index]) {
                     children[index]->multiRemove(agent);
                 }
             }
         }
         return;
     }
 
     agents.erase(
         std::remove_if(agents.begin(), agents.end(), [&](Agent *a) {
             return a->id == agent->id;
         }),
         agents.end()
     );
 
     omp_unset_lock(&lock);
 }
 
 void Quadtree::get_leaf_nodes(Agent& agent, std::vector<int>& leaves) {
     if (children[0] == nullptr) { // this is a leaf
         leaves.push_back(this->id);
         return;
     }
 
     std::vector<int> indices = getMultiQuadrant(agent);
 
     for (int index : indices) {
         if (children[index]) {
             children[index]->get_leaf_nodes(agent, leaves);
         }
     }
 }
 
 void Quadtree::multiInsert(Agent *agent, std::vector<std::vector<int>>&  leaves) {
 
     omp_set_lock(&lock);
 
     if (children[0] != nullptr) { // not a leaf
         std::vector<int> quadrants = getMultiQuadrant(*agent);
 
         if (!quadrants.empty()) {
             for (auto quad: quadrants) {
                 children[quad]->multiInsert(agent, leaves);
             }
         }
         omp_unset_lock(&lock);
         return;
     }
     // lock is still set
 
     agents.push_back(agent);
     leaves[agent->id].push_back(this->id);
 
     if (agents.size() > max_agents && depth < max_depth){
         if(children[0] == nullptr){
             split();
         }
         std::vector<Agent*> agents_to_reinsert = agents;
         agents.clear();
 
         for (Agent* a : agents_to_reinsert) {
             auto& agent_leaf_ids = leaves[a->id];
             agent_leaf_ids.erase(std::remove(agent_leaf_ids.begin(), agent_leaf_ids.end(), this->id), agent_leaf_ids.end());
         }
 
         for (Agent* moved : agents_to_reinsert) {
             std::vector<int> quadrants = getMultiQuadrant(*moved);
             for (auto quad : quadrants) {
                 children[quad]->multiInsert(moved, leaves);
             }
         }
         omp_unset_lock(&lock); 
 
         return;
     }
     omp_unset_lock(&lock);
 }