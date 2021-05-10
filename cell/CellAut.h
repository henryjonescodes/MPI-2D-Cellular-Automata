#include <stdio.h>
#include <stdlib.h>
#define EXTRACT_BIT(n,i) ((n&(1<<i))?1:0)

//allocate a buffer of ints of size size
int *MakeCellWorld(int size);

//initialize by zeroing out array and putting a 1 in the middle
void InitCellWorld(int *inworld,int insize);

//given a world, apply a rule at a particular location
void ApplyRuleAtLoc(int *oldworld, int *newworld, int loc,int worldsize, int rule);

//MPI-capable version of ApplyRule
//given a world, apply a rule at a particular location, where leftval and rightval come from neighbors (i.e. they are only used if loc == 0 or loc == size-1)
//UNUSED
//void ApplyRuleAtLocWithNeighbors(int *oldworld, int *newworld, int loc,int worldsize, int rule, int leftval, int rightval);

//serial version of RunCellWorld
void RunCellWorld(int *world, int worldsize, int rule, int iters, int threads);

//MPI-capable version of printWorld, takes id as input
void printWorld(int *world, int worldsize, int myid);
