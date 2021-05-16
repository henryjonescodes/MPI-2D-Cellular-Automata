#include "2DCellAut.h"
#include "mpi.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

//to compile: make -f Makefile
//to run: mpirun -np 4 ./mpicell2d

void printRuleset(char *rule, int size){
  int z;
  for(z = 0; z <size; z++){
    printf("%d",rule[z]);
  }
  printf("\n");
}

char * assembleWorld(char *slice, int numvals, int worldsz, int ID){
  //Task 0 gathers slices
  // char *newworld = calloc(numvals,sizeof(char));
  // MPI_Gather(slice,numvals, MPI_CHAR, newworld, numvals, MPI_CHAR,0,MPI_COMM_WORLD);

  char *newworld = calloc(numvals,sizeof(char));
  MPI_Gather(slice,numvals, MPI_CHAR, newworld, numvals, MPI_CHAR,0,MPI_COMM_WORLD);
  return newworld;
}

void newAssemble(char *out, char *slice, int worldsz, int slicesz, int ID){
  //Task 0 gathers slices and prints
  int count = slicesz*worldsz;
  char *newworld = calloc(worldsz*worldsz,sizeof(char));
  MPI_Gather(slice,count, MPI_CHAR, newworld, count, MPI_CHAR,0,MPI_COMM_WORLD);

  if(ID == 0){
    print2DWorld(newworld,worldsz,worldsz,ID);
  }
  out = newworld;
}

int main(int argc, char *argv[])
{

  int my_rank;               /* Process rank */
  int comm_sz;                /* Number of processes */
  int worldsize;
  int iterations = 24;
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
  else{
    printf("usage: runcell <worldsize>\n");
    printf("usage: runcell <worldsize> <iterations\n");
    exit(1);
  }

  if(worldsize%comm_sz != 0){
    printf("can't do this on %d processors", comm_sz);
    exit(1);
  }

  //Figure out slice size (number of rows per processor)
  int slicesize = worldsize/comm_sz;
  char *ruleset = calloc(RULESETSIZE,sizeof(char));
  // ruleset = MakeRandomRuleSet();

  //Root process makes the random rule, broadcasts
  if(my_rank == 0){
      ruleset = MakeRandomRuleSet();
      printf("(%d): Ruleset", my_rank);
      printRuleset(ruleset,RULESETSIZE);
  }

  MPI_Bcast(ruleset, RULESETSIZE, MPI_CHAR, 0, MPI_COMM_WORLD);

  // printf("(%d): ", my_rank);
  // printRuleset(ruleset,RULESETSIZE);
  // printf("\n");

  //Each processor makes their own world slice (complete rows, partial height worlds)

  char *mycellworld = Make2DCellWorld(slicesize,worldsize);
  // print2DWorld(mycellworld,slicesize,worldsize,my_rank);

  //Each processor sends its top and bottom rows to the appropriate places

  //Each processor runs the rule, producing a new slice
  Run2DCellWorldOnce(mycellworld, slicesize, worldsize, my_rank, ruleset);
  // print2DWorld(mycellworld,slicesize,worldsize,my_rank);

  // for(curriter = 0; curriter < iterations; curriter ++){
  //   Run2DCellWorldOnce(mycellworld, slicesize, worldsize, my_rank, ruleset);
  // }

  // char *bigcellworld = assembleWorld(mycellworld,count,worldsize,my_rank);
  char *bigcellworld = calloc(worldsize*worldsize,sizeof(char));
  newAssemble(bigcellworld,mycellworld,worldsize,slicesize,my_rank);

  if(my_rank == 0){
    print2DWorld(bigcellworld,worldsize,worldsize,my_rank);
  }

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
