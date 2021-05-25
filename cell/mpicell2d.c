#include "2DCellAut.h"
#include "mpi.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

bool PRINT_EVERY_ITER = true;
bool PRINT_AT_END = true;

//to compile: make -f Makefile
//to run: mpirun -np 4 ./mpicell2d

void printRuleset(char *rule, int size){
  int z;
  for(z = 0; z <size; z++){
    printf("%d",rule[z]);
  }
  printf("\n");
}

char * expandCellWorld(char *cellworld, char *prev, char *next, int size, int slicesz){
  int count = 0;
  int newSize = (size*slicesz) + (size * 2);
  char * expanded = calloc(newSize, sizeof(char));

  int i;
  for(i = 0; i < size; i++){
    expanded[count] = prev[i];
    count += 1;
  }
  for(i = 0; i < size*slicesz; i++){
    expanded[count] = cellworld[i];
    count += 1;
  }
  for(i = 0; i < size; i++){
    expanded[count] = next[i];
    count += 1;
  }
  return expanded;
}

char * reduceCellWorld(char *cellworld, int size, int slicesz){
  char * out = calloc(size*slicesz,sizeof(char));
  int i;
  int j = 0;
  int end = size + (size*slicesz);
  for(i = size; i < end; i++){
    out[j] = cellworld[i];
    j++;
  }
  return out;
}

void assembleWorld(char **out, char *slice, int worldsz, int slicesz, int ID){
  //Task 0 gathers slices and prints
  int count = slicesz*worldsz;
  char *newworld = calloc(worldsz*worldsz,sizeof(char));
  MPI_Gather(slice,count, MPI_CHAR, newworld, count, MPI_CHAR,0,MPI_COMM_WORLD);

  *out = newworld;
}

void distrubuteValues(char* wholeworld, char* before, char* after, int size, int slicesz, int ID, int numProcs){
  //Calculate which processes to send and recieve from
  int above = ID - 1;
  int below = ID + 1;

  //Adjust the root node's target
  if(above < 0){
    above = numProcs - 1;
  }

  //Adjust last node's target
  if(below >= numProcs){
    below = 0;
  }

  //Even processes send then recieve, odd's do the opposite
  if(ID % 2 == 0){
    //Send and recieve above
    MPI_Send(wholeworld, size, MPI_CHAR, above, 0, MPI_COMM_WORLD);
    MPI_Recv(after, size, MPI_CHAR, above, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    //Send and recieve below
    MPI_Send(wholeworld + (slicesz - 1) * size * sizeof(char), size, MPI_CHAR, below, 0, MPI_COMM_WORLD);
    MPI_Recv(before, size, MPI_CHAR, below, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  else {
    //revieve and send below
    MPI_Recv(after, size, MPI_CHAR, below, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(wholeworld, size, MPI_CHAR, below, 0, MPI_COMM_WORLD);

    //revieve and send above
    MPI_Recv(before, size, MPI_CHAR, above, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(wholeworld + (slicesz - 1) * size * sizeof(char), size, MPI_CHAR, above, 0, MPI_COMM_WORLD);
  }
}

int main(int argc, char *argv[])
{

  int my_rank;               /* Process rank */
  int comm_sz;                /* Number of processes */
  int worldsize;
  int iterations = 4;
  int curriter = 0;
  int t1, t2;

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
      printf("(%d): Ruleset: ", my_rank);
      printRuleset(ruleset,RULESETSIZE);
      printf("(%d): SliceSize: %d\n", my_rank,slicesize);
  }
  t1 = MPI_Wtime();

  //Root node broadcasts rule to all non-root nodes
  MPI_Bcast(ruleset, RULESETSIZE, MPI_CHAR, 0, MPI_COMM_WORLD);

  //Each processor makes their own world slice on the first iteration
  char *mycellworld = Make2DCellWorld(slicesize,worldsize);

  //Run the cell world `iterations` amount of times
  for(curriter = 0; curriter < iterations; curriter ++){
      //Make room for the values before and after each slice on each node
      char *beforevals = calloc(worldsize,sizeof(char));
      char *aftervals = calloc(worldsize,sizeof(char));

      //Distribute values before and after each node's block
      distrubuteValues(mycellworld,beforevals,aftervals,worldsize,slicesize,my_rank, comm_sz);

      //Make an expanded world 2 rows larger containing the recieved values
      char * expandedWorld = expandCellWorld(mycellworld,beforevals,aftervals,worldsize,slicesize);

      //Apply the rule once in paralell
      Run2DCellWorldOnce(&expandedWorld, slicesize+2, worldsize, my_rank, ruleset);

      //Reduce the world back to slicesize for the next iteration
      mycellworld = reduceCellWorld(expandedWorld,worldsize,slicesize);
      
      if (PRINT_EVERY_ITER){
        // Task 0 gathers slices and prints
        //Gather permuted world back to root node
        char * permutedWorld = calloc(worldsize*worldsize,sizeof(char));
        MPI_Gather(expandedWorld + (worldsize*sizeof(char)), worldsize*slicesize, MPI_CHAR, permutedWorld, worldsize*slicesize, MPI_CHAR, 0, MPI_COMM_WORLD);
        if(my_rank == 0){
          print2DWorld(permutedWorld,worldsize,worldsize,my_rank);
        }
      }
  }
  t2 = MPI_Wtime();

  if (PRINT_AT_END){
    // Task 0 gathers slices and prints
    //Gather permuted world back to root node
    char * permutedWorld = calloc(worldsize*worldsize,sizeof(char));
    MPI_Gather(mycellworld, worldsize*slicesize, MPI_CHAR, permutedWorld, worldsize*slicesize, MPI_CHAR, 0, MPI_COMM_WORLD);
    
    if(my_rank == 0){
      print2DWorld(permutedWorld,worldsize,worldsize,my_rank);
      printf("duration: %d seconds \n", t2 -t1);
      printf("t1 %d \n", t1);
      printf("t2 %d \n", t2);


    }
  }
  
  MPI_Finalize();
}
