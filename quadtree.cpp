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
 
     std::cout << "Splitting node (" << min_x << "," << min_y << ") - (" << max_x << "," << max_y << ")\n";
 
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
 
//  std::vector<int> Quadtree::getMultiQuadrant(const Agent &agent) {
//      int midX = (min_x + max_x) / 2;
//      int midY = (min_y + max_y) / 2;
 
//      std::vector<int> possible_quadrants;
 
//      bool top = agent.y_pos <= midY + 2;
//      bool bottom = agent.y_pos >= midY - 1;
//      bool right = agent.x_pos >= midX - 1;
//      bool left = agent.x_pos <= midX + 2;
 
//      if (top && left) { 
//          possible_quadrants.push_back(0);
//      }
//      if (top && right) { 
//          possible_quadrants.push_back(1);
//      }
//      if (bottom && left) { 
//          possible_quadrants.push_back(2);
//      }
//      if (bottom && right) {
//          possible_quadrants.push_back(3);
//      }
//      return possible_quadrants;
//  }

std::vector<int> Quadtree::getMultiQuadrant(const Agent& agent) {
    int midX = (min_x + max_x) / 2;
    int midY = (min_y + max_y) / 2;
    
    std::vector<int> quadrants;

    if (agent.x_pos == midX) { // on vertical midline
        if (agent.y_pos < midY) {
            quadrants.push_back(0);
            quadrants.push_back(1);
        } else if (agent.y_pos > midY) {
            quadrants.push_back(2);
            quadrants.push_back(3);
        } else { // exact center
            quadrants.push_back(0);
            quadrants.push_back(1);
            quadrants.push_back(2);
            quadrants.push_back(3);
        }
    } else if (agent.y_pos == midY) { // on horizontal midline
        if (agent.x_pos < midX) {
            quadrants.push_back(0);
            quadrants.push_back(2);
        } else if (agent.x_pos > midX) {
            quadrants.push_back(1);
            quadrants.push_back(3);
        }
    } else { // normal non-boundary
        bool left = agent.x_pos < midX;
        bool right = agent.x_pos > midX;
        bool top = agent.y_pos < midY;
        bool bottom = agent.y_pos > midY;

        if (top && left) quadrants.push_back(0);
        if (top && right) quadrants.push_back(1);
        if (bottom && left) quadrants.push_back(2);
        if (bottom && right) quadrants.push_back(3);
    }

    return quadrants;
}


 
 void Quadtree::multiInsert(Agent &agent) {
 
     omp_set_lock(&lock);
 
     if (children[0] != nullptr) { // has quadtree children/is not a leaf
         std::vector<int> indices = getMultiQuadrant(agent);
         omp_unset_lock(&lock);
 
         if (!indices.empty()) {
             for (auto index: indices) {
                 // insert node
                 children[index]->multiInsert(agent);
             }
         }
         return;
     }
     // lock is still set
 
     // try to add to node itself (is leaf)
     agents.push_back(agent);
 
     if (agents.size() > max_agents && depth < max_depth){
         if(children[0] == nullptr){
             // splits into quadrants
             split();
         }
         int i = 0;
 
         while (i < agents.size()){
             std::vector<int> indices = getMultiQuadrant(agents[i]);
 
             if (!indices.empty()) {
                 Agent moved = agents[i]; 
                 // remove from parent
     
                 agents.erase(agents.begin() + i);
 
                 omp_unset_lock(&lock);
 
                 for (auto index:indices) {
                     children[index]->multiInsert(moved);
                 }
                 omp_set_lock(&lock);
             }
             else {
                 i+=1;
             }
         }
     }
     omp_unset_lock(&lock);
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
         
 
 std::vector<Agent> Quadtree::collidable_agents() {
     std::vector<Agent> collidable = agents;
     return collidable;
 }
 
 void Quadtree::remove(Agent &agent) {
     omp_set_lock(&lock);
 
     if (children[0] != nullptr) {
         std::vector<int> indices = getMultiQuadrant(agent);
         omp_unset_lock(&lock);
 
         for (int index : indices) {
             if (children[index]) {
                 children[index]->remove(agent);
           
             }
         }
         return;
     }
     // erase the agent
     agents.erase(
         std::remove_if(agents.begin(), agents.end(), [&](const Agent& a) {
             return a.id == agent.id;
         }),
         agents.end()
     );
 
     omp_unset_lock(&lock);
 }
 
 void Quadtree::multiRemove(Agent &agent) {
     omp_set_lock(&lock);
 
     if (children[0] != nullptr) { 
         std::vector<int> indices = getMultiQuadrant(agent);
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
         std::remove_if(agents.begin(), agents.end(), [&](const Agent& a) {
             return a.id == agent.id;
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
 
 void Quadtree::multiInsert_2(const Agent &agent, std::vector<std::vector<int>>&  leaves) {
 
     omp_set_lock(&lock);
 
     if (children[0] != nullptr) { // not a leaf
         std::vector<int> quadrants = getMultiQuadrant(agent);
         omp_unset_lock(&lock);
 
         if (!quadrants.empty()) {
             for (auto quad: quadrants) {
                 children[quad]->multiInsert_2(agent, leaves);
             }
         }
         return;
     }
     // lock is still set
 
     agents.push_back(agent);
     leaves[agent.id].push_back(this->id);
 
     if (agents.size() > max_agents && depth < max_depth){
         if(children[0] == nullptr){
             split();
         }
         std::vector<Agent> agents_to_reinsert = agents;
         agents.clear();
 
         for (const Agent& a : agents_to_reinsert) {
             auto& agent_leaf_ids = leaves[a.id];
             agent_leaf_ids.erase(std::remove(agent_leaf_ids.begin(), agent_leaf_ids.end(), this->id), agent_leaf_ids.end());
         }
 
         omp_unset_lock(&lock); 
 
         for (const Agent& moved : agents_to_reinsert) {
             std::vector<int> quadrants = getMultiQuadrant(moved);
             for (auto quad : quadrants) {
                 children[quad]->multiInsert_2(moved, leaves);
             }
         }
 
         return;
 
     }
     omp_unset_lock(&lock);
 }