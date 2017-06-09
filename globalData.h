#ifndef GLOBALDATA__H
#define GLOBALDATA__H

#include <string>
#include <map>
#include <tr1/unordered_map>
#include "user.h"
#include "movie.h"
#include "readFileUtil.h"
#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <set>
#include <sstream>
#include <map>
#include <tr1/unordered_map>

class GlobalData
{
public:
  static GlobalData& getInstance();
  ~GlobalData();
  std::vector<User*> userList;
  std::vector<Movie*> movieList;
  std::tr1::unordered_map<int,int> userMap;
  std::tr1::unordered_map<int,int> movieMap;
  std::vector<std::vector<unsigned char> > rating;
  std::tr1::unordered_map<int,std::vector<int> > socialMap;
  std::vector<float> avgRate;// average rating of all users in matrix
  std::tr1::unordered_map<int,std::vector<int> > predictPairs;
  int findUserIdByUserData(int userData);
  int findMovieIdByMovieData(int movieData);
private:
  GlobalData();
  GlobalData(const GlobalData&);
  GlobalData& operator=(const GlobalData&);
  void getUMList(const std::string& userHistory, const std::string& userSocial);
  void getUMMap();
  void getRatingMatrix(const std::string& userHistory, const std::string& userSocial, const std::string& trainingSet);
//  void userTraining();
  void getSocialMap(const std::string& userSocial);
  void calAvgRating();
  void getPredictPairs(const std::string& predictSource);
};


#endif
