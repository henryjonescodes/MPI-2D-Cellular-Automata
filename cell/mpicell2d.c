#include "2DCellAut.h"
#include "mpi.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

bool GATHER_EVERY_ITER = true;

//to compile: make -f Makefile
//to run: mpirun -np 4 ./mpicell2d

void printRuleset(char *rule, int size){
  int z;
  for(z = 0; z <size; z++){
    printf("%d",rule[z]);
  }
  printf("\n");
}

void printSendBuf(char *cellworld, int start, int end, int ID, char *message){
  printf("(%d): %s \n",ID,message);
  int j;
  for(j = start; j < end; j++){
    printf("%d", cellworld[j]);
  }
  printf("\n");
}

char * expandCellWorld(char* cellworld, int size, char* prev, char *next, int slicesz){
  int newSize = size+(slicesz*2);
  char * expanded = calloc(newSize, sizeof(char));
  int z;
  for(z = 0; z<newSize; z++){
    if(z < slicesz){
      expanded[z] = prev[z];
    } else if (z > size + slicesz){
      expanded[z] = next[z-(size + slicesz)];
    } else{
      expanded[z] = cellworld[z-slicesz];
    }
  }
  return expanded;
}

void assembleWorld(char **out, char *slice, int worldsz, int slicesz, int ID){
  //Task 0 gathers slices and prints
  int count = slicesz*worldsz;
  char *newworld = calloc(worldsz*worldsz,sizeof(char));
  MPI_Gather(slice,count, MPI_CHAR, newworld, count, MPI_CHAR,0,MPI_COMM_WORLD);

  // //print the world
  // if(ID == 0){
  //   print2DWorld(newworld,worldsz,worldsz,ID);
  // }
  *out = newworld;
  // out = newworld;
}

int main(int argc, char *argv[])
{

  int my_rank;               /* Process rank */
  int comm_sz;                /* Number of processes */
  int worldsize;
  int iterations = 4;
  int curriter = 0;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size (MPI_COMM_WORLD, &comm_sz);



  if (argc > 1) {
    worldsize = atoi(argv[1]);
    if(argc > 2){
      iterations = atoi(argv[2]);
    }
  }
  else {
    if(my_rank == 0){
      printf("usage: runcell <worldsize>\n");
      printf("usage: runcell <worldsize> <iterations\n");
    }
    exit(1);
  }

  if(worldsize%comm_sz != 0){
    if(my_rank == 0){
      printf("can't do this on %d processors", comm_sz);
    }
    exit(1);
  }

  //Figure out slice size (number of rows per processor)
  int slicesize = worldsize/comm_sz;
  char *ruleset = calloc(RULESETSIZE,sizeof(char));

  //Root process makes the random rule
  if(my_rank == 0){
      ruleset = MakeRandomRuleSet();
      printf("(%d): Ruleset\n", my_rank);
      printRuleset(ruleset,RULESETSIZE);
      printf("(%d):\n SliceSize: %d\n", my_rank,slicesize);
  }

  //Root node broadcasts rule to all non-root nodes
  MPI_Bcast(ruleset, RULESETSIZE, MPI_CHAR, 0, MPI_COMM_WORLD);

  //Each processor makes their own world slice (complete rows, partial height worlds)
  char *mycellworld = Make2DCellWorld(slicesize,worldsize);
  // print2DWorld(mycellworld,slicesize,worldsize,my_rank);

  // Task 0 gathers slices and prints
  char *testCell = calloc(worldsize*worldsize,sizeof(char));
  assembleWorld(&testCell,mycellworld,worldsize,slicesize,my_rank);

  if(my_rank == 0){
    print2DWorld(testCell,worldsize,worldsize,my_rank);
  }

  //Make room for the values before and after each slice on each node
  char *beforevals = calloc(worldsize,sizeof(char));
  char *aftervals = calloc(worldsize,sizeof(char));

  //Calculate which processes to send and recieve from
  int above = my_rank - 1;
  int below = my_rank + 1;

  //Adjust the root node's target
  if(above < 0){
    above = comm_sz - 1;
  }

  //Adjust last node's target
  if(below >= comm_sz){
    below = 0;
  }

  //Even processes send then recieve, odd's do the opposite
  if(my_rank % 2 == 0){

    // printSendBuf(mycellworld, 0, worldsize,my_rank,"Sending (above)");
    // printSendBuf(mycellworld, (slicesize - 1) * worldsize, slicesize*worldsize,my_rank,"Sending (Below)");

    //Send and recieve above
    MPI_Send(mycellworld, worldsize, MPI_CHAR, above, 0, MPI_COMM_WORLD);
    MPI_Recv(aftervals, worldsize, MPI_CHAR, above, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    //Send and recieve below
    MPI_Send(mycellworld + (slicesize - 1) * worldsize * sizeof(char), worldsize, MPI_CHAR, below, 0, MPI_COMM_WORLD);
    MPI_Recv(beforevals, worldsize, MPI_CHAR, below, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // printSendBuf(beforevals, 0, worldsize,my_rank,"Recieve (Above)");
    // printSendBuf(aftervals, 0, worldsize,my_rank,"Recieve (Below)");

  }
  else {

    // printSendBuf(mycellworld, 0, worldsize,my_rank,"Sending (Above)");
    // printSendBuf(mycellworld, (slicesize - 1) * worldsize, slicesize*worldsize,my_rank,"Sending (Below)");

    //revieve and send below
    MPI_Recv(aftervals, worldsize, MPI_CHAR, below, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(mycellworld, worldsize, MPI_CHAR, below, 0, MPI_COMM_WORLD);

    //revieve and send above
    MPI_Recv(beforevals, worldsize, MPI_CHAR, above, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(mycellworld + (slicesize - 1) * worldsize * sizeof(char), worldsize, MPI_CHAR, above, 0, MPI_COMM_WORLD);

    // printSendBuf(beforevals, 0, worldsize,my_rank,"Recieve (Above)");
    // printSendBuf(aftervals, 0, worldsize,my_rank,"Recieve (Below)");
  }

  // char* expanndedWorld = expandCellWorld(mycellworld,worldsize*slicesize, beforevals, aftervals, worldsize);

  int newSize = worldsize*slicesize+(worldsize*2);
  char* expandedWorld = calloc(newSize, sizeof(char));

  printf("Before");
  print2DWorld(beforevals,1,worldsize,my_rank);

  printf("After");
  print2DWorld(aftervals,1,worldsize,my_rank);

  strcat(expandedWorld, beforevals);
  strcat(expandedWorld, mycellworld);
  strcat(expandedWorld, aftervals);

  if(my_rank == 0){
    printf("Expanded");
    print2DWorld(expandedWorld,slicesize+2,worldsize,my_rank);
  }

  // //Even processes send then recieve, odd's do the opposite
  // if(my_rank % 2 == 0){
  //
  //   printSendBuf(mycellworld, 0, worldsize,my_rank,"Sending (above)");
  //   printSendBuf(mycellworld, (slicesize - 1) * worldsize, slicesize*worldsize,my_rank,"Sending (Below)");
  //
  //   //Send and recieve above
  //   MPI_Send(mycellworld, worldsize, MPI_CHAR, above, 0, MPI_COMM_WORLD);
  //   MPI_Recv(mycellworld + (slicesize - 1) * worldsize * sizeof(char), worldsize, MPI_CHAR, above, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  //
  //   //Send and recieve below
  //   MPI_Send(mycellworld + (slicesize - 1) * worldsize * sizeof(char), worldsize, MPI_CHAR, below, 0, MPI_COMM_WORLD);
  //   MPI_Recv(mycellworld, worldsize, MPI_CHAR, below, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  //
  //   printSendBuf(mycellworld, (slicesize - 1) * worldsize, slicesize*worldsize,my_rank,"Recieve (Below)");
  //   printSendBuf(mycellworld, 0, worldsize,my_rank,"Recieve (Above)");
  //
  // }
  // else {
  //
  //   printSendBuf(mycellworld, 0, worldsize,my_rank,"Sending (Above)");
  //   printSendBuf(mycellworld, (slicesize - 1) * worldsize, slicesize*worldsize,my_rank,"Sending (Below)");
  //
  //   //revieve and send below
  //   MPI_Recv(mycellworld + (slicesize - 1) * worldsize * sizeof(char), worldsize, MPI_CHAR, below, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  //   MPI_Send(mycellworld, worldsize, MPI_CHAR, below, 0, MPI_COMM_WORLD);
  //
  //   //revieve and send above
  //   MPI_Recv(mycellworld, worldsize, MPI_CHAR, above, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  //   MPI_Send(mycellworld + (slicesize - 1) * worldsize * sizeof(char), worldsize, MPI_CHAR, above, 0, MPI_COMM_WORLD);
  //
  //   printSendBuf(mycellworld, (slicesize - 1) * worldsize, slicesize*worldsize,my_rank,"Recieve (Below)");
  //   printSendBuf(mycellworld, 0, worldsize,my_rank,"Recieve (Above)");
  //
  // }

  // // Task 0 gathers slices and prints
  // char *bigcellworld = calloc(worldsize*worldsize,sizeof(char));
  // assembleWorld(&bigcellworld,mycellworld,worldsize,slicesize,my_rank);
  //
  // if(my_rank == 0){
  //   print2DWorld(bigcellworld,worldsize,worldsize,my_rank);
  // }


  //Each processor runs the rule, producing a new slice
  // Run2DCellWorldOnce(mycellworld, slicesize, worldsize, my_rank, ruleset);

  //Make space for the working world
  // char *bigcellworld = calloc(worldsize*worldsize,sizeof(char));


  // //Run the cell world `iterations` amount of times
  // for(curriter = 0; curriter < iterations; curriter ++){
  //   //Apply the rule on each node
  //   Run2DCellWorldOnce(mycellworld, slicesize, worldsize, my_rank, ruleset);
  //
  //   //Assemble the world
  //   assembleWorld(&bigcellworld,mycellworld,worldsize,slicesize,my_rank);
  //
  //   //print the world
  //   if(my_rank == 0){
  //     print2DWorld(bigcellworld,worldsize,worldsize,my_rank);
  //   }
  // }

  // char *bigcellworld = assembleWorld(mycellworld,count,worldsize,my_rank);


  // //Task 0 gathers slices and prints
  // int count = slicesize*worldsize;
  // char *bigcellworld = calloc(worldsize*worldsize,sizeof(char));
  // MPI_Gather(mycellworld,count, MPI_CHAR, bigcellworld, count, MPI_CHAR,0,MPI_COMM_WORLD);
  //
  // if(my_rank == 0){
  //   print2DWorld(bigcellworld,worldsize,worldsize,my_rank);
  // }

  MPI_Finalize();
}
