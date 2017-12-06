#include <iostream>
#include <cstdio>
#include <vector>
#include <omp.h>
#include <ctime>
#include <cstdlib>
#include <cmath>
using namespace std;

pair<double, double> getPi(long num) {
  double start = omp_get_wtime();
  unsigned int seed;
  long circle = 0;
  seed = time(NULL);
  for(int i=0;i<num;i++) {
    double x = -1 + ((double)2.0*rand_r(&seed))/(RAND_MAX);
    double y = -1 + ((double)2.0*rand_r(&seed))/(RAND_MAX);
    if(sqrt(x*x + y*y)<=1)
      circle++;
  }
  double pi = (4.0*circle)/(num*1.0);
  double end = omp_get_wtime() - start;
  return(make_pair(end,pi));
}

int main() {
  vector<long int> numPoints;
  srand(time(NULL));
  //different number of points
  numPoints.push_back(1000);
  numPoints.push_back(10000);
  numPoints.push_back(100000);
  numPoints.push_back(1000000);
  numPoints.push_back(10000000);
  numPoints.push_back(100000000);
  numPoints.push_back(1000000000);
  //different number of points
  for(int i=0;i<numPoints.size();i++) {
      double pi = 0;
      double time = 0;
      int r = 1;
      //run r times
      for(int j=0;j<r;j++) {
        pair<double, double> tmp = getPi(numPoints[i]);
	pi = tmp.second;
	time += tmp.first;
      }
      time/=r;
      cout<<pi<<" "<<time<<" "<<numPoints[i]<<endl;
  }
}
