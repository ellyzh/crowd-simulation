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


// Quadtree::Quadtree(int min_x, int min_y, int max_x, int max_y, int depth):
//     min_x(min_x), min_y(min_y), max_x(max_x), max_y(max_y), depth(depth) {
//     for (int i = 0; i < 4; i++) {
//         children[i] = nullptr;
//         }
// }


void Quadtree::split() {
    int midX = (min_x + max_x) / 2;
    int midY = (min_y + max_y) / 2;

    children[0] = std::make_unique<Quadtree>(min_x, min_y, midX, midY, depth + 1);
    children[1] = std::make_unique<Quadtree>(midX, min_y, max_x, midY, depth + 1);
    children[2] = std::make_unique<Quadtree>(min_x, midY, midX, max_y, depth + 1);
    children[3] = std::make_unique<Quadtree>(midX, midY, max_x, max_y, depth + 1);

            // children[0] = new Quadtree(min_x, min_y, midX, midY, depth + 1); // top left
            // children[1] = new Quadtree(midX, min_y, max_x, midY, depth + 1); // top right
            // children[2] = new Quadtree(min_x, midY, midX, max_y, depth + 1); // bottom left
            // children[3] = new Quadtree(midX, midY, max_x, max_y, depth + 1); // bottom right
}

  
int Quadtree::getQuadrant(Agent &agent) {
    int midX = (min_x + max_x) / 2;
    int midY = (min_y + max_y) / 2;
            
    bool top = agent.y_pos < midY;
    bool bottom = agent.y_pos >= midY;
    bool right = agent.x_pos >= midX;
    bool left = agent.x_pos < midX;

    if (top && left) { // top left = 0
        return 0;
    }
    else if (top && right) { // top right = 1
        return 1;
    }
    else if (bottom && left) { // bottom left = 2
        return 2;
    }
    else if (bottom && right) { // bottom right = 3
        return 3;
    }
    return -1;
}


void Quadtree::insert(Agent &agent) {
    if (children[0] != nullptr) { // has quadtree children/is not a leaf
        int index = getQuadrant(agent);
        if (index != -1) {
            // insert node
            children[index]->insert(agent);
            return;
            }
        }

    // try to add to node itself (is leaf)
    agents.push_back(agent);

    if (agents.size() > max_agents && depth < max_depth){
        if(children[0] == nullptr){
            split();
            // splits into quadrants
        }
        int i = 0;
        while (i < agents.size()){
            int index = getQuadrant(agents[i]);
            if (index != -1) {
                children[index]->insert(agents[i]);
                agents.erase(agents.begin() + i);
                // remove from parent
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
    return agents;

    // case on edges w/bounding boxes
}