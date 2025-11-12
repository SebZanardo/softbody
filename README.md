# softbody
Softbody physics written in C making use of a simple arena allocator
  
The simulation makes use of technique called Shape Matching  
This makes the simultion more stable than traditional softbody techniques, but less physically accurate  
Massive credit to this fantastic video: https://www.youtube.com/watch?v=3OmkehAJoyo&t=655s

<img width="395" height="395" alt="cover" src="https://github.com/user-attachments/assets/f9ccda61-82d5-4f60-a4d6-6538e3b7f828" />
  
__FUTURE TASKS:__
* Coloured wall particle effects like SUPERMEAT BOY (using a ring buffer)
* Collisions between softbodies
  * Takes into account relative mass
  * Spatial partitioning scheme to optimise collisions (simple grid is good enough)
  * (DONE) Double buffer for point positions and velocity
* Static shape map obstacles that integrate into spatial partitioning scheme
