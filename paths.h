#ifndef PATHS__H
#define PATHS__H

#include <tr1/unordered_map>
#include <vector>

class Paths
{
public:
  Paths(){
    pathCount = 0;
  }
  ~Paths() {
    std::tr1::unordered_map<int, std::vector<int> >::iterator ptr = pathMap.begin();
      while(ptr != pathMap.end()) {
        ptr->second.clear();
        ++ptr;
      }
      pathMap.clear();
   }

  int pathCount;
  std::tr1::unordered_map<int,std::vector<int> > pathMap;
};

#endif
