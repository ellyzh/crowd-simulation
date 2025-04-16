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
#include "quadtree.h"


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

std::vector<int> Quadtree::getMultiQuadrant(Agent &agent) {
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

void Quadtree::multiInsert(Agent &agent) {

    if (children[0] != nullptr) { // has quadtree children/is not a leaf
        std::vector<int> indices = getMultiQuadrant(agent);
        if (!indices.empty()) {
            for (auto index: indices) {
                // insert node
                children[index]->multiInsert(agent);
            }
            return;
        }
    }

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
                for (auto index:indices) {
                    children[index]->multiInsert(agents[i]);
                }
                // remove from parent
                agents.erase(agents.begin() + i);
            }
            else {
                i+=1;
            }
        }
    }
}


void Quadtree::reset() {
    agents.clear();
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