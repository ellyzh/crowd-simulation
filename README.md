# Large-scale Crowd Collision Simulations with Parallel Quadtree
Rose Liu (roseliu), Elly Zheng (ellyz)

Simulate collisions between agents in a large crowd using a quadtree in parallel.

## Summary
We implemented a crowd collision simulator with a parallel quadtree in OpenMP. The crowd collision simulator consists of a number of agents on a grid that are given a starting cardinal direction (north, east, south, or west), and each agent moves in their specified direction until a collision with either another agent or the grid edge. If two agents are predicted to move onto the same grid space, they bounce off each other and change directions instead of overlapping. We ran our algorithm on the GHC machines and the PSC machines to check scalability. Additionally, we also created a visualization tool with SDL to map out the agents as they move and interact with other agents on the grid to show how the crowd collision simulation works.


## Schedule
### April 2- April 9 [Completed]
1. Revised project goals and implementation **(Rose + Elly)**
   * Redefined project statement from crowd control to crowd collision
2. Complete sequential implementation of crowd collision simulation **(Elly)**
3. Create visualization tool to debug unexpected behavior of collisions **(Rose)**
4. Write small input files (grid size of 128x128) to test the validity of the code **(Rose)**
### April 9 - April 15 [Completed]
1. Integrate quadtree-based spatial partitioning **(Rose)**
2. Parallelize algorithm with OpenMP **(Elly)**
3. Finish project milestone report **(Rose + Elly)**
### April 16 - April 19 [Completed]
1. Add functions to verify that the code is working as expected **(Rose)**
2. Analyse bottlenecks and explore optimization techniques **(Elly)**
    * Parallelize building of quadtree and collision check algorithm
### April 20 - April 22 [Completed]
1. Analyse bottlenecks and explore optimization techniques **(Rose)**
    * Find ways to reduce the number of collision checks
2. Test code with larger input files (ex. grid size of 8192x8192) **(Elly)**
### April 23 - April 26 [Completed]
1. Experiment with PSC to test scalability **(Rose + Elly)**
2. Gather results and examine key metrics **(Rose + Elly)**
### April 27 - April 29 [Completed]
1. Finish our final report and prepare for presentation **(Rose + Elly)**
2. Finalize Github repository code and documentation **(Rose + Elly)**
