#include <stdio.h>
#include <time.h>
#include "CellAut.h"

/* runcell.c
 author: John Rieffel
*/


int main(int argc, char *argv[])
{

  int WORLDSIZE = 128;


  if (argc > 1) {
    if (argc > 2 )
    {
        WORLDSIZE = atoi(argv[2]);
    }
    int *mycellworld = MakeCellWorld(WORLDSIZE);
    InitCellWorld(mycellworld,WORLDSIZE);
    RunCellWorld(mycellworld, WORLDSIZE, atoi(argv[1]));
}
else{
  printf("usage: runcell <rulenumber>\n");
  printf("usage: runcell <rulenumber> <worldsize>\n");
}

  //command line arguments are passed in as an array of strings!
  //where argc says how many arguments there are
  //(note the executable itself is the first argument, so argc is always 1)


}
