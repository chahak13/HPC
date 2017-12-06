#include <iostream>
#include <omp.h>
#include <vector>

using namespace std;

pair<long long int, double> dotProduct(int n) {
  vector<int> a(n,2);
  vector<int> b(n,3);
  long long int sum = 0;

  double startTime = omp_get_wtime();
  #pragma omp parallel for reduction(+: sum)
  for(int i=0;i<n;i++) {
    sum += a[i]*b[i];
  }
  double endTime = omp_get_wtime() - startTime;
  pair<long long int, double> ans = make_pair(sum, endTime);
  return(ans);
}

int main() {
  //different problem sizes
  vector<int> sizes;
  sizes.push_back(1000);
  sizes.push_back(10000);
  sizes.push_back(100000);
  sizes.push_back(1000000);
  sizes.push_back(10000000);
  sizes.push_back(100000000);
  sizes.push_back(1000000000);
  //sizes.push_back(1000000000);

  //different probem sizes
  for(int i=0;i<sizes.size();i++) {
    double time = 0;
    long long int ans = 0;
    //10 times avg for each
    for(int j=0;j<10; j++) {
    	pair<long long int, double> tmp = dotProduct(sizes[i]);
	time += tmp.second;
	ans = tmp.first;
    }
    time/=10;
    cout<<sizes[i]<<" "<<ans<<" "<<time<<endl;
  }
}
