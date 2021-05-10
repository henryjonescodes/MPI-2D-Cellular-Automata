#include <stdio.h>
#include <time.h>
#include "2DCellAut.h"

/* run2dcell.c
 author: John Rieffel
 description: starter code for 2D CA (game of life)
 to compile: make -f Makefile-serial run2dcell
*/


int main(int argc, char *argv[])
{

  	srand(time(NULL));

  int WORLDSIZE = 5;
  int WORLD_COLS = WORLDSIZE;
  int WORLD_ROWS = WORLDSIZE;

  char *ruleset = MakeRandomRuleSet();

//this makes a random world and fills it with 0s and 1s
  char *big2Dworld = Make2DCellWorld(WORLD_ROWS, WORLD_COLS);
  Init2DCellWorld(big2Dworld,WORLD_ROWS,WORLD_COLS);
  //this should run the world
    Run2DCellWorld(big2Dworld,WORLD_ROWS, WORLD_COLS,0,ruleset);
    //this should print the world
    print2DWorld(big2Dworld,WORLD_ROWS,WORLD_COLS,0);

  //command line arguments are passed in as an array of strings!
  //where argc says how many arguments there are
  //(note the executable itself is the first argument, so argc is always 1)


}
