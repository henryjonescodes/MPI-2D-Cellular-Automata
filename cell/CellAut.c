#include "CellAut.h"

//Note to future self: 
// this would be much better if we operated on arrays of char
// (4x less memory)

//allocate integer array of size size
int * MakeCellWorld(int size)
{
  int *outworld;
  if ((outworld = calloc(size,sizeof(int))) == NULL) {
    printf("Memory allocation error!");
    return NULL;
  }
  return outworld;
}

//put a 1 in the middle
void InitCellWorld(int *inworld,int insize)
{
  int i = 0;
  for (i = 0; i < insize; i++)
  {
    inworld[i] = 0;
  }
  inworld[insize/2] = 1;
}

void ApplyRuleAtLoc(int *oldworld, int *newworld, int loc,int worldsize, int rule)
{
  //with a neighborhood of 3, there are 8 possible cases we need to cover
  // 000 through 111, where the digits correspond to the left neighbor,
  // the current cell, and the right neighbor.
  // Rule 30, for instance, is
  // 111  110  101  100  011  010  001  000
  //  0    0    0    1    1    1    1    0  <- binary 30
  int ruleset[8]; //our rules
  int i;
  //this turns an integer into an array of bits
  for (i = 0; i < 8; i++)
    ruleset[i] = EXTRACT_BIT(rule,i);


  int hood[3];

  hood[1] = oldworld[loc];
  //check boundary conditions
  hood[0] = ((loc - 1) >= 0) ? oldworld[loc-1] : oldworld[worldsize-1];
  hood[2] = ((loc + 1) < worldsize) ? oldworld[loc+1] : oldworld[0];

  // better to use the same binary voodoo as extract_bit, neh?
  // note to future self
  // this converts the 3-bit neighborhood into an integer
  // that we can use as an index into the ruleset array above
  int ruleindex = hood[0] + 2*hood[1] + 4*hood[2];

  //printf("hood is [%d%d%d], applying ruleindex %d: %d%d%d->%d\n", hood[2],hood[1],hood[0], ruleindex, hood[2],hood[1],hood[0], ruleset[ruleindex]);
  newworld[loc] = ruleset[ruleindex];

}

//print, taking into account node id
void printWorld(int *world, int worldsize, int myid)
{
  int i;

  printf("(%d) :",myid);
  for (i = 0; i < worldsize; i++)
    {
      //if (world[i] != Nil){
    //    if (world[i] == 1) printf("#");
  	printf("%d",world[i]);
     //   else printf(" ");
       //}
    }
  printf("\n");
}

//serial version of run cell world
void RunCellWorld(int *world, int worldsize, int rule, int iters,int threads)
{

  int *newworld= MakeCellWorld(worldsize);
  printWorld(world, worldsize,-1);

 int iter = 0;
  while((iter < iters) || (iters == -1))
    {
      int loc;
      for (loc = 0; loc < worldsize; loc++)
      {
	     ApplyRuleAtLoc(world,newworld,loc,worldsize,rule);
     }

      // the world becomes the new world
      // and new world becomes the old world
      // (this way we only have to allocate the array once)
      int *oldworld = world;
      world = newworld;
      newworld = oldworld;
      iter++;
    }
  printWorld(world, worldsize,-1);
  free(newworld);
}
