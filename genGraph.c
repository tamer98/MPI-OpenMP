#include <stdio.h>
#include <stdlib.h>

/*  Generate file with a description of a random graph
    in the format of exercise 2  summer of 2023.
    
    The file is written to the standard output. 

    command line arguments (all of them are numbers):
      number of vertices 
      maximum weight of an edge
      seed for generating the weights for the random graph

      last two arguments are optional.

      example: genGraph 10 20 7
      A graph with 10 vertices will be generated.  Weights
      will not exceed 20.  Use 7 as the seed for the random sequence
      of weights.

      Note: no edges with INFINITY weight are generated (except from
            each node to itself). This may be considered a bug.
      
*/
int main(int argc, char **argv) 
{

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <number of vertices> [max-weight] [seed]\n", argv[0]);
        return 1;
    }

    int NV = atoi(argv[1]);

    int max_weight;
    if (argc >= 3)
        max_weight = atoi(argv[2]);
    else
        max_weight = 10;


    int seed;
    if (argc >= 4)
        seed = atoi(argv[3]);
    else
        seed = 1;

    unsigned int *edges = (unsigned int *) malloc(NV*NV*sizeof(unsigned int));

    unsigned int INFINITY = 1000000; /* a large number.
        if edge has this weight it means the edge does not exist.  */
    
    // generate a random graph
    srand(seed);
    for (int i = 0; i < NV; i++)  
        for (int j = 0; j < NV; j++)  {
             if (j == i) edges[i*NV+i] = INFINITY;
             else  {
                 int w = rand() % (max_weight+1);
                 if (w == 0) w = 1; // weight should be positive
                 edges[i*NV+j] = w;
             }
        }
    
    //   write to the output

    // first write number of vertices
    printf("%d\n", NV);

    //  then write the weights.
    //  each row in the edges table  will appear in a separate line

    for (int i = 0; i < NV; i++) {
         for (int j = 0; j < NV; j++) {
              unsigned int w = edges[i*NV+j];
              if (w == INFINITY)
                  printf("*  ");
              else 
                  printf("%d  ", w);
         }
         putchar('\n');
    }

    return 0;
}
   


