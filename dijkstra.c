/* 
  Dijkstra's algorithm (the sequential version).
  Find the distance from a source vertex (vertex 0) to all other vertices
  in a directed graph. Edges in the graph have positive weights.
  (Based on example taken from  Norm Matloff's book "Programming on Parallel Machines".)
  
  An optional command line argument (a number) may specify the 
  destination vertex.
  In this case, the distance to this destination will be written
  to the standard output.  If there is no command line argument then the distances
  to all vertices will be written to the output.  
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef unsigned int VERTEX; //  vertices are numbered 0, 1, 2 ... (NV-1)

struct vertex {
     VERTEX vertex; 
     unsigned int distance; // distance of this vertex from vertex 0
};

const unsigned int INFINITY = 1000000; // a large integer

// globals
int NV;   // number of vertices
int *done; /*  done[v] == 1 means we are done with vertex v. done[v] == 0 means we are not done yet. */

unsigned int *edges;  /* weights of edges between vertices;
                  'edges' is (logically) a two dimensional array:  it has NV rows (one for each vertex)
                  and NV columns (one for each vertex). The entry in row i and column j
                  is the weight of the edge i -> j. 
                  'edges' is accessed as if it was a one dimensional array: 
                  The weight of the edge i -> j is stored in 
                  'edges[i*NV+j]'.  This is the entry in the i'th row and the j'th column. */
                                     
int  *distance;  /* distance[v] is the minumum distance of vertex v from the source 
                   (vertex 0) (as found so far) */

enum goal { FIND_ONE_DISTANCE, /* find distance from source to one 
                   vertex given as a command line argument */
            FIND_ALL_DISTANCES /* find distance from source 
                             			to all vertices */
} goal;
		  
VERTEX destination;  /* when goal == FIND_ONE_DISTANCE,
                        we want to find the distance from
                        the source vertex to 'destination' */
						
void init(int argc, char **argv);
void doWork();
struct vertex find_vertex_with_minimum_distance();
void update_distances(struct vertex current);

void printGraph();
void printDistances(char *s);
void readGraph(void);

int main(int argc, char **argv)
{  
    init(argc,argv);
    doWork();  

    // printGraph(); // for debugging  
    
	if (goal == FIND_ALL_DISTANCES)
        printDistances(NULL);
	else // goal == FIND_ONE_DISTANCE
     	if (distance[destination] == INFINITY)
            printf("no path to vertex %u\n", destination);			
		else printf("distance from 0 to %u is %u\n", destination, 
	            distance[destination]);
}

void init(int argc, char **argv)
{ 
    readGraph(); // initialize NV and 'edges'

    if (argc > 1) {
        goal = FIND_ONE_DISTANCE;
        destination = atoi(argv[1]);
		if (destination >= NV) {
			fprintf(stderr, "illegal destination vertex\n");
			exit(4);
		}
    } else
        goal = FIND_ALL_DISTANCES;		

    distance = malloc(NV*sizeof(int));
    done = malloc(NV*sizeof(int)); 
    if (distance == NULL || done == NULL) { perror("malloc"); exit(1);}

    for (VERTEX v = 0; v < NV; v++)  {
        done[v] = 0;
        distance[v] = INFINITY;
    }
    distance[0] = 0;
}

void doWork()
{  
   struct vertex current; // current vertex and its distance from vertex 0

   for (int step = 0; step < NV; step++)  {  // step < (NV-1) should also work (see note at end of this function) 
      if (step == 0) {
         current.vertex = 0;
         current.distance = 0;
      }
      else
          current = find_vertex_with_minimum_distance();

#ifdef DEBUG
      printf("current is %u, distance is %u\n", current.vertex,
                                current.distance);
#endif

      /*  check: if no path exists from vertex 0 to 'current' then
          we can stop. distance to 'current' and all other vertices which are not
          'done' yet will remain INFINITY.
      */ 
      if (current.distance >= INFINITY) 
          break;
	  
	  /* if all we need is to find distance to 'destination' vertex
	     (not to all vertices) and we found it now then we can stop 
	  */
	  if (goal == FIND_ONE_DISTANCE && current.vertex == destination)
		  break;

      // mark current vertex as done 
      done[current.vertex] = 1;  
      update_distances(current);
   } // for

   /* note: final iteration of the for loop  (step == NV-1) actually does nothing useful because all final distances
         have already been found */
} // doWork

// finds vertex closest to vertex 0 among the vertices not done.
struct vertex
find_vertex_with_minimum_distance()
{  
   struct vertex vmin;
   vmin.distance = INFINITY; 

   for (VERTEX v = 0; v < NV; v++) {
#ifdef DEBUG
      printf("finding min: v=%u, done[v]=%d distance[v]= %d  vmin.distance=%d\n",
                    v, done[v], distance[v], vmin.distance); 
#endif
      if (!done[v] && distance[v] < vmin.distance)  {
         vmin.distance = distance[v];
         vmin.vertex = v;
      }
   }
   return vmin; // note: when vmin.distance is INFINITY, vmin.vertex is meaningless
}


/* Update distances for  vertices.
   For each vertex v (which is not 'done' yet), ask whether a shorter path to v 
   exists, through vertex 'current'. 
*/ 
void update_distances(struct vertex current)
{
   for (VERTEX v = 1; v < NV; v++) 
       if (!done[v]) {
           unsigned int alternative = current.distance + edges[current.vertex*NV+v];
           // printf("alternative: %d\n", alternative);
           if (alternative < distance[v])
               distance[v] = alternative; 
       }
   // print_distances("distances:");
}

/*  Read the standard input  containing the description of a graph
    and initialize 'edges' and 'NV'. 
    The input contains a sequence of integers.
    The first integer (call it 'nv') is the number of vertices
    in the graph. The following integers are weights (separated by white space) used to 
    initialize the entries in 'edges'. There should be nv*nv weights.
    A weight appears in the input as a positive integer or as a '*'  character.
    If a '*' appears in the input then the corresponding entry in 'edges' is initialized to
    INFINITY.
*/
int lineno = 1; // current input line number

void skip_white_space();

void readGraph() {
    
    int c;
    unsigned int w;
    int count_w = 0; // number of entries read in so far

    /* First number in the input is the number of vertices. Use it to initialize 'NV' */
        
    if (scanf("%d", &NV) == 1) {
         edges = (unsigned int *)malloc(NV * NV * sizeof(unsigned int));
         if (edges == NULL) { perror("malloc"); exit(1); }
    } else {
        fprintf(stderr, 
                "line %d: first item in the input should be the number of vertices in the graph\n",
                lineno);
        exit(1);
    }

    unsigned int *next_entry = edges;

    while (1) {
        skip_white_space();
        c = getchar();
        if (c == EOF) 
            break;
        if (count_w >= NV*NV) {
             fprintf(stderr, "line %d: too many weights (expecting %d*%d weights)\n",
                              lineno, NV, NV);
            exit(5);
        }
        if (c == '*') {
             *next_entry++ = INFINITY;
             count_w++;
        } else {
             ungetc(c, stdin);
             int r = scanf("%u", &w);
             if (r == 1) { // a number (weight) was read
                *next_entry++ = w;
                count_w++;
             } else {
                  fprintf(stderr, "line %d: error in input\n", lineno);
                  exit(2);
             }
        }
        
    }
    if (count_w != NV*NV) {
        fprintf(stderr, "%d weights appear in the input (expected\
 %d weights because number of vertices is %d)\n", 
         count_w, NV*NV, NV);
         exit(6);
    }
}
    
void skip_white_space() {
   int c;
   while(1) {
       if ((c = getchar()) == '\n')
           lineno++;
       else if (isspace(c))
           continue;
       else if (c == EOF)
           break;
       else {
         ungetc(c, stdin); // push non space character back onto input stream
         break;
       }
   }
}

void printDistances(char *s) 
{  
   if (s) printf("%s\n", s);

   for (VERTEX v = 0; v < NV; v++)
       if (distance[v] >= INFINITY)
           printf("%u:*\n", v);
       else
           printf("%u:%u\n", v, distance[v]);
}

// can be used for debugging
void printGraph() {

    printf("graph weights:\n");
    for (int i = 0; i < NV; i++)  {
        for (int j = 0; j < NV; j++)
            if (edges[NV*i+j] >= INFINITY)
                printf("*  ");
            else 
                printf("%u  ", edges[NV*i+j]);
         putchar('\n');
     }
}

   
      
        