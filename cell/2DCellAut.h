#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#define RULESETSIZE 512 //rulesets have 512 rules

#define EXTRACT_BIT(n,i) ((n&(1<<i))?1:0)
#define ALIGN_BIT(n,i)((n==1)?1<<i:0)

char *MakeRandomRuleSet();
//allocate a buffer of ints of size size
char *Make2DCellWorld(int numrows,int numcols);
//initialize by zeroing out array and putting a 1 in the middle
void Init2DCellWorld(char* inworld,int numrows, int numcols);
//given a world, apply a rule at a particular location
// void ApplyRuleAtLoc(int *oldworld, int *newworld, int loc,int worldsize, int rule);
//serial version of RunCellWorld
 void Run2DCellWorld(char* world, int rows, int cols, int myid, char *ruleset);
//MPI-capable version of printWorld, takes id as input
void print2DWorld(char* world, int rows, int cols, int myid);

//and now 2D versions
