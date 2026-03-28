#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <float.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#define EPSILON 1e-9

typedef struct { int id; double x, y; } Vertex;
typedef struct { int u, v; double weight; } Edge;

// Prototypy
void calculate_smacof(Vertex* vertices, int num_v, Edge* edges, int num_e, int iterations);
void calculate_fruchterman(Vertex* vertices, int num_v, Edge* edges, int num_e, int iterations);

#endif