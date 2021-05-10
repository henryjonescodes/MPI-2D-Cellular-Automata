#include "2DCellAut.h"


char *MakeRandomRuleSet()
  {
      char *outset;
      if ((outset = calloc(RULESETSIZE,sizeof(char))) == NULL) {
        printf("Memory allocation error!");
        return NULL;
      }
      int i;
      for (i = 0; i < RULESETSIZE; i++){
        //either a 0 or a 1
        outset[i]  = rand()%2;
        printf("%d",outset[i]);
      }
      printf("\n");
      return outset;

  }
// Makes a 2D cell world of size ROW*COLS, and randomly fills it with 0s and 1s
char * Make2DCellWorld(int rows,int cols)
{
  char *outworld;
  if ((outworld = calloc(rows*cols,sizeof(char))) == NULL) {
    printf("Memory allocation error!");
    return NULL;
  }
//int i;

 // for (i = 0; i < rows*cols; i++){
    //either a 0 or a 1
  //  outworld[i]  = rand()%2;
//  }
  return outworld;
}

//put a 1 in the middle
void Init2DCellWorld(char* inworld,int numrows,int numcols)
{
//  inworld[(int)(numcols*numrows/2) + (int)(numcols/2)] = 1;
int i;
for (i = 0; i < numrows*numcols; i++){
    //either a 0 or a 1
    inworld[i]  = rand()%2;
  }
}

//given a location in a 2D cell world
//find the neighbhord composed of that cell and its immediate neighbors
//and converts that neighbhoord into a binary number
// for instance, if your hood looks like this:
//   000
//   000   <- middle 0 is loc
//   000
//
// then this becomes index 0
// whereas if your hood looks like this:
//
// 000
// 001
// 111
//
// that becomes  000001111, which is index 15
//etc.

int MakeIndexFromHood(char *world, int loc, int rows, int cols)
{
  char NW,NN,NE,WW,CC,EE,SW,SS,SE;

  int rownum = (int)loc/cols; //this truncates to the floor
  int colnum = loc%cols;

  int northrow = ((rownum - 1) >= 0) ? rownum-1 : rows-1;
  int southrow = ((rownum + 1) < rows) ? rownum+1 : 0;
  int eastcol = ((colnum + 1) < cols) ? colnum+1 : 0;
  int westcol = ((colnum - 1) >= 0) ? colnum-1 : cols-1;
  NW = world[northrow*cols + westcol];
  NN = world[northrow*cols + colnum];
  NE = world[northrow*cols + eastcol];
  EE = world[rownum*cols + eastcol];
  CC = world[loc];
  WW = world[rownum*cols + westcol];
  SW = world[southrow*cols + westcol];
  SS = world[southrow*cols + colnum];
  SE = world[southrow*cols + eastcol];


//0 0110 0110
  //int index = NW<<8 | NN << 7 | NE < 6 | WW << 5 | CC << 4 | EE << 3 | SW << 2 | SS << 1 | SE;
  int index = (int)(ALIGN_BIT(NW,8) |
              ALIGN_BIT(NN,7) |
              ALIGN_BIT(NE,6) |
              ALIGN_BIT(WW,5) |
              ALIGN_BIT(CC,4) |
              ALIGN_BIT(EE,3) |
              ALIGN_BIT(SW,2) |
              ALIGN_BIT(SS,1) |
              ALIGN_BIT(SE,0)) ;
  //for testing
  //printf("hood at %d\n[%d%d%d\n %d%d%d\n %d%d%d] --> %x\n", loc,NW,NN,NE,WW,CC,EE,SW,SS,SE,index);

  return index;

}
//
 void Apply2DRuleAtLoc(char *world, char *newworld,int loc, int rows, int cols, char *ruleset)
 {
   int curIndex = MakeIndexFromHood(world, loc, rows, cols);
   //do some other stuff here
   //CSC333 - PROJECT - CHANGE THIS line
   // so that newworld[loc] actually becomes
   // whathever the ruleset tells you.
   newworld[loc] = world[loc];

   return;

 }

void print2DWorld(char *world, int rows, int cols, int myid)
{
  int rownum, colnum;

  printf("(%d):\n",myid);
  for (rownum = 0; rownum < rows ; rownum++) {
      for (colnum= 0; colnum < cols ; colnum++){

  	printf("%d",world[rownum*cols + colnum]) ;
      }
  	printf("\n");

      //if (world[i] != Nil){
    //    if (world[i] == 1) printf("#");
     //   else printf(" ");
       //}
    }
  printf("\n");
}
//
// //serial version of run 2d cell world

/* "rules" no longer fit into integers.
 We need a 512-bit RULESET, because there are 2^9 possible neighborhoods, and
 our ruleset needs to specify what to do in each case.  We can use the fingerprint
 of the neighborhood as an "index" into the ruleset.  For instance:

 The neighborhood

 000
 000      (the center is zero and surrounded by zeroes)
 000

 would have index 000000000 and the rule to apply would be found at index 0 of the ruleset.

 whereas the neighborhood:

 000
 000 (the center is zero and surrounded by zeroes
 001  except for the southeast neighbor, which is 1)

would have index  000000001, and the rule to apply would be found at index 1 of the ruleset.

and the neighborhood

101
010
101

would have index 101010101, and the rule would be found at index 341 (0x155) of the ruleset

 */
void Run2DCellWorld(char *world, int rows, int cols, int myid, char *ruleset)
{

  char *newworld= Make2DCellWorld(rows,cols);

  while(1)
    {
      int loc;
      print2DWorld(world, rows,cols,0);
      for (loc = 0; loc < rows*cols ; loc++){
	        Apply2DRuleAtLoc(world,newworld,loc,rows,cols,ruleset);
      }

      // the world becomes the new world
      // and new world becomes the old world
      // (this way we only have to allocate the array once)
      char *oldworld = world;
      world = newworld;
      newworld = oldworld;
    }
  free(newworld);
}
