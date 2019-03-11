#include "main.h"
  
void cleanup (FILE * fpin)
{
  fclose (fpin);
}  

bool readData (FILE * fpin, DataPoint * *dp, int nval, int dim)
{
  int niter, diter;
  for (niter = 0; niter < nval; niter++)
    {
      for (diter = 0; diter < dim; diter++)
	{
	  if (fscanf (fpin, "%d", &dp[niter]->data[diter]) == 0)
	    {
	      return false;
	    }
	}
    }
  return true;
}

void writeCentroids (const char *filename, Centroid * *centroids, int kval)
{
  FILE * fpout = fopen (filename, "w");
  if (fpout == NULL)
    {
      fprintf (stderr, "File %s, cannot be open\n", filename);
      exit (1);
    }
  int kiter;
 
  qsort (centroids, kval, sizeof (Centroid *), Centroid_cmp);
  for (kiter = 0; kiter < kval; kiter++)
    {
      Centroid_print (centroids[kiter], fpout);
    }
  fclose (fpout);
}

#ifdef TEST_DIST
// distance - funtion to get the distance between present centroid and datapoint
// @param DataPoint * - pointer to DataPoint structure from which distance needs to be calculated
// @param Centroid * - pointer to Centroid struct from which distance is being calculated
long long int
distance (const DataPoint * datapoint, const Centroid * centroid)
{
  long long int sum = 0;	// must initialize to zero
  int lcv; 
  for (lcv=0;lcv< datapoint->dimension; lcv++)
  {
    sum+=pow((datapoint->data[lcv])-(centroid->data[lcv]),2);
  }  
	return sum;
}

int closestCentroid (int kval, DataPoint * datapoint, Centroid * *centroids)
{
  int mindex = 0; //index of the closest centroid
  long long int minDist = distance(datapoint, centroids[0]);
  int lcv9;
  for (lcv9 = 1; lcv9 < kval; lcv9++)
  {
    long long int Dist = distance(datapoint, centroids[lcv9]);
    if (Dist < minDist)
    {
      minDist = Dist;
      mindex = lcv9;
    }
  } 

  return mindex;
}

void kmean (int kval, int nval, DataPoint * *datapoints, Centroid * *centroids)
{
  // reset all centroids
     int lcv1;
     for (lcv1 = 0; lcv1 < kval; lcv1++)
       Centroid_reset(centroids[lcv1]);
  // initialize each data point to a cluster between 0 and kval - 1
     int lcv2;
     for (lcv2=0; lcv2 < nval; lcv2++)
       datapoints[lcv2]-> cluster = rand() % kval;
  // find the centroid for initial random assignment of datapoints
     int lcv3;
     for (lcv3 = 0; lcv3 < nval; lcv3++)
     {
       int clust = datapoints[lcv3]->cluster;
       Centroid_addPoint(centroids[clust], datapoints[lcv3]);
     } 		
     int lcv4;
     for (lcv4 = 0; lcv4 < kval; lcv4++)
     {
       Centroid_findCenter(centroids[lcv4]);
     }

     int flag;
     while (flag != 0)
     {
       flag = 0;
       int lcv5;
       for (lcv5 = 0; lcv5<nval; lcv5++)
       { 
         int temp = datapoints[lcv5]->cluster;
         datapoints[lcv5]->cluster = closestCentroid(kval, datapoints[lcv5], centroids);
         if(temp != datapoints[lcv5]->cluster)
         {
           flag = 1;
         }
       }
       int lcv6;
       for (lcv6 = 0; lcv6 < kval; lcv6++)
       {
         Centroid_reset(centroids[lcv6]);
       }
       int lcv7;
       for (lcv7 = 0; lcv7 < nval; lcv7++)
       {
         int clust = datapoints[lcv7]->cluster;
         Centroid_addPoint(centroids[clust], datapoints[lcv7]);
       }
       int lcv8;
       for (lcv8 = 0; lcv8 < kval; lcv8++)
       {
         Centroid_findCenter(centroids[lcv8]);
       }
     } 
}	

int main (int argc, char * *argv)
{
  // argv[1]: name of input file
  // argv[2]: value of k/ number of centroids
  // argv[3]: name of output file
  if (argc < 4)
    {
      fprintf (stderr, "argc is %d, not 4\n", argc);
      return EXIT_FAILURE;
    }
   
  // opening file to read the data points from
  FILE * fpin = fopen (argv[1], "r");
  if (fpin == NULL)
    {
      fprintf (stderr, "fopen %s fail\n", argv[1]);
      return EXIT_FAILURE;
    }
   
  // convert long to int
  int kval = (int) strtol (argv[2], NULL, 10);
  if (kval <= 0)
    {
      fprintf (stderr, "kval is %d, must be positive\n", kval);
      return EXIT_FAILURE;
    }
  // control the random number sequence
  int randseed = 1729;
  srand (randseed);
    
  // getting number of datapoints
  int nval;
  fscanf (fpin, "%d", &nval);
  if (nval < kval)
    {
      fprintf (stderr, "nval= %d must be greater than kval = %d\n", nval,
	       kval);
      cleanup (fpin);
      return EXIT_FAILURE;
    }
   
  // getting dimensions of the data from the file
  int dim;
  fscanf (fpin, "%d", &dim);
  if (dim < 2)
    {
      fprintf (stderr, "nval= %d must be greater than kval = %d\n", nval,
	       kval);
      cleanup (fpin);
      return EXIT_FAILURE;
    }
    
  // allocate memory for the data points
  DataPoint * *datapoint_array = DataPoint_createArray (nval, dim);
  if (datapoint_array == NULL)
    {
      printf ("Error in creating datapoint array\n");
      cleanup (fpin);
      return EXIT_FAILURE;
    }
  
  // allocate memory for array of centroids
  Centroid * *centroids = Centroid_createArray (kval, dim);	
  if (centroids == NULL)
    {
      printf ("Error in creating centroids array\n");
      DataPoint_freeArray (datapoint_array, nval);
      cleanup (fpin);
      return EXIT_FAILURE;
    }
   
  // read the data from the file
  if (readData (fpin, datapoint_array, nval, dim) == false)
    {
      printf ("Error in reading data array\n");
      cleanup (fpin);
      DataPoint_freeArray (datapoint_array, nval);
      return EXIT_FAILURE;
    }
     
  // calling kmean function to find the cetroids
  kmean (kval, nval, datapoint_array, centroids);

	//writing those centroids to the file
  writeCentroids (argv[3], centroids, kval);
  // free all the allocated spaces
  DataPoint_freeArray (datapoint_array, nval);
  Centroid_freeArray (centroids, kval);
  cleanup (fpin);
  return EXIT_SUCCESS;
}