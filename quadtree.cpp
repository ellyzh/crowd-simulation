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

std::vector<int> Quadtree::getMultiQuadrant(Agent &agent) {
    int midX = (min_x + max_x) / 2;
    int midY = (min_y + max_y) / 2;

    std::vector<int> possible_quadrants;

    bool top = agent.y_pos <= midY + 2;
    bool bottom = agent.y_pos >= midY - 1;
    bool right = agent.x_pos >= midX - 1;
    bool left = agent.x_pos <= midX + 2;

    if (top && left) { // top left = 0
        possible_quadrants.push_back(0);
    }
    if (top && right) { // top right = 1
        possible_quadrants.push_back(1);
    }
    if (bottom && left) { // bottom left = 2
        possible_quadrants.push_back(2);
    }
    if (bottom && right) { // bottom right = 3
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
                //printf("AGENT %d %d explores quadrant %d of (%d, %d), (%d, %d)\n", agent.x_pos, agent.y_pos, index, this->min_x, this->min_y, this->max_x, this->max_y);
                children[index]->multiInsert(agent);
            }
            return;
        }
    }

    //printf("AGENT %d %d adds self to quadrant (%d, %d), (%d, %d)\n", agent.x_pos, agent.y_pos, this->min_x, this->min_y, this->max_x, this->max_y);

    // try to add to node itself (is leaf)
    agents.push_back(agent);

    if (agents.size() > max_agents && depth < max_depth){
        if(children[0] == nullptr){
            //printf("QUADRANT (%d, %d), (%d, %d) splits\n", this->min_x, this->min_y, this->max_x, this->max_y);
            split();
            // splits into quadrants
        }
        int i = 0;

        while (i < agents.size()){
            std::vector<int> indices = getMultiQuadrant(agents[i]);

            for(auto index:indices){
                //printf("AFTER SPLIT: AGENT %d %d to be placed in quadrant %d of (%d, %d), (%d, %d)\n", agents[i].x_pos, agents[i].y_pos, index, this->min_x, this->min_y, this->max_x, this->max_y);
            }

            if (!indices.empty()) {
                for (auto index:indices) {
                    //printf("AFTER SPLIT: AGENT %d %d explores quadrant %d of (%d, %d), (%d, %d)\n", agents[i].x_pos, agents[i].y_pos, index, this->min_x, this->min_y, this->max_x, this->max_y);

                    children[index]->multiInsert(agents[i]);
                    // remove from parent
                }
                agents.erase(agents.begin() + i);
            }
            else {
                i+=1;
            }
        }
    }
}


void Quadtree::insert(Agent &agent) {

    if (children[0] != nullptr) { // has quadtree children/is not a leaf
        int index = getQuadrant(agent);
        if (index != -1) {
            // insert node
            //printf("AGENT %d %d explores quadrant %d of (%d, %d), (%d, %d)\n", agent.x_pos, agent.y_pos, index, this->min_x, this->min_y, this->max_x, this->max_y);

            children[index]->insert(agent);
            return;
        }
    }

    //printf("AGENT %d %d adds self to quadrant (%d, %d), (%d, %d)\n", agent.x_pos, agent.y_pos, this->min_x, this->min_y, this->max_x, this->max_y);

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
    std::vector<Agent> collidable = agents;
    return collidable;
    // case on edges w/bounding boxes
}