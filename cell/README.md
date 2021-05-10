# 2D Cellular Automata Code

## Overview

* The `run2dcell.c` file contains a serial implementation of a 2D cellular automata.  You can build it by typing `make -f Makefile-serial`
* `2DCellAut.h` and `.c` are relatively well documented


## Tips for parallelizing

* this is a relatively straightforward scale-up of your successful 1D CA, and so you can begin by copying `mpicell.c` into a file called `mpicell2d.c`.   You'll need to modify your `Makefile` to add a rule for  `mpicell2d`, but that's a straightforward copy/paste of the pattern for `mpicell`
* Each task will be responsible for its own "slice" through respective rows of the 2D world.  You can save yourself some trouble by *not* creating the whole world in the root node and then scattering, but instead having each task create its own local world.  You can initialize the world to random 0s and 1s, although it's better if there's a bias towards more 0s than 1s
* Each task will then need to send its first and last *row* to its neighors above and below.   Task 0's top neighbor should be the last task.
* Task 0 should gather the full world in order to print it out.