#ifndef MANAGER__H
#define MANAGER__H

#include "user.h"
#include "movie.h"
#include <cstdlib>
#include <set>
#include <sstream>
#include <string>
#include <map>
#include <tr1/unordered_map>
#include <iostream>
#include "readFileUtil.h"
#include "socialGraph.h"
#include "globalData.h"
#include "paths.h"
#include <vector>
#include <cmath>
#include <iomanip>

class Manager
{
public:
  Manager();
  ~Manager();
  void predictUnrated();
  void loadDataAndCreateDG();
  void predictUnratedMovies();
  void predictOneMovie(int userId, int movieId, std::string& predictString);
private:
  void findAllSimUsers(int userId, int movieId, std::vector<int>& simUserIds, std::vector<float>& sims);
  float calAdvSimilarity(Paths* p, float sim);
  int getBestPath(Paths* p);
  SocialGraph sg;
  std::tr1::unordered_map<int, std::vector<std::vector<int> > > prePathMap;

};


#endif
