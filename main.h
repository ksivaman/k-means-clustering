#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "common.h"
#include "centroid.h"
#include "datapoint.h"

void cleanup(FILE * fpin);
bool readData(FILE * fpin, DataPoint * * dp, int nval, int dim);
void writeCentroids(const char* filename, Centroid * * centroids, int kval);
long long int distance(const DataPoint * datapoint, const Centroid * centroid);
int closestCentroid(int kval, DataPoint * datapoint, Centroid * * centroids);
void kmean(int kval, int nval, DataPoint * * datapoints, Centroid * * centroids);
