### TODO
* Softbody struct stores mass, angular rotation, angular velocity, size, elasticity
* Define shapes with enum as lots of slimes will have the same shape.
    Softbody just stores enum reference which maps to an array.
    Shapes also store eye positions and max eye counts
* Multiple softbody simulation (spatial partitioning for fast search grid is best here)
* Render slimes to target at max opacity then set opacity when rendering to the screen
    this will stop the overlapping low opacity artifacts
* Softbody editor (separate script) place points on grid, export as C array
