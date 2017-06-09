#include "globalData.h"
#include <algorithm>

GlobalData& GlobalData::getInstance() {
  static GlobalData globalData;
  return globalData;
}

GlobalData::~GlobalData(){
  std::vector<User *>::iterator ptr = userList.begin();
  while (ptr != userList.end()) {
    delete *ptr;
    ++ptr;
  }
  userList.clear();
  std::vector<Movie *>::iterator ptr1 = movieList.begin();
  while (ptr1 != movieList.end()) {
    delete *ptr1;
    ++ptr1;
  }
  movieList.clear();
  userMap.clear();
  movieMap.clear();
  rating.clear();
}

GlobalData::GlobalData()
{
  getRatingMatrix(std::string("data/testData/user_history.txt"),
                  std::string("data/orginData/user_social.txt"),
                  std::string("data/testData/training_set.txt"));
  getSocialMap(std::string("data/orginData/user_social.txt"));
  calAvgRating();
  getPredictPairs(std::string("data/testData/predictSource.txt"));
}

/**
 * Get user list and movie list from user_history.txt
 * only keep the user who has social relation in user list by combining user in "user_social.txt"
 */
void GlobalData::getUMList(const std::string& userHistory, const std::string& userSocial)
{
  std::cout << "get userList and movieList..." << std::endl;
  std::vector<std::string> infoList;
  try {
    ReadFileUtil::readFileByLine(userHistory, infoList);
  }catch(const std::string& msg) {
    std::cout << "error:" << msg << std::endl;
    exit(-1);
  }

  std::set<int> uSet1;
  std::set<int> mSet1;

  std::vector<std::string>::const_iterator ptr = infoList.begin();
  while(ptr != infoList.end())
    {
      std::istringstream f(*ptr);
      std::string s;
      getline(f, s, '\t');
      uSet1.insert(atoi(s.c_str()));
      getline(f, s, '\t');
      mSet1.insert(atoi(s.c_str()));
      ptr++;
    }
  std::cout << "uSet_1 size:" << uSet1.size() << std::endl;
  std::cout << "mSet_1 size:" << mSet1.size() << std::endl;
  infoList.clear();
  //merge users in user_social.txt and user in user_history.txt
  try {
    ReadFileUtil::readFileByLine(userSocial, infoList);
  }catch(const std::string& msg) {
    std::cout << "error:" << msg << std::endl;
  }
  std::cout << userSocial <<" size: " << infoList.size() << std::endl;

  std::set<int> uSet2;
  ptr = infoList.begin();
  while(ptr != infoList.end())
    {
      std::istringstream f(*ptr);
      std::string s;
      getline(f, s, '\t');
      uSet2.insert(atoi(s.c_str()));
      //  std::cout << s << std::endl;
      ++ptr;
    }
  infoList.clear();
  std::cout << "uSet_2 size:" << uSet2.size() << std::endl;
  std::set<int> uSet;
  set_intersection(uSet1.begin(), uSet1.end(), uSet2.begin(), uSet2.end(), std::inserter(uSet, uSet.begin()));

  std::cout << "after merge, uSet size:" << uSet.size() << std::endl;

  //copy uSet to userList
  int uId = 0;
  User *user;
  std::set<int>::iterator it = uSet.begin();
  while (it != uSet.end()) {
    user = new User;
    user->userId = uId++;
    user->userData = *it;
    userList.push_back(user);
    ++it;
  }
  //copy mSet to movieList
  int mId = 0;
  Movie *movie;
  std::set<int>::iterator it_m = mSet1.begin();
  while (it_m != mSet1.end()) {
    movie = new Movie;
    movie->movieId = mId++;
    movie->movieData = *it_m;
    movieList.push_back(movie);
    ++it_m;
  }
  std::cout << "after copy, userList size:" << userList.size() << std::endl;
  std::cout << "after copy, movieList size:" << movieList.size() << std::endl;
  std::cout << "get userList and movieList done" << std::endl;
  getUMMap();// copy userList and movieList to hashmap for find
}

// copy userList and movieList to hashmap for find
void GlobalData::getUMMap()
{
  std::cout << "copy userList and movieList to hashmap for find..." << std::endl;
  std::vector<User *>::iterator user_ptr = userList.begin();
  while (user_ptr != userList.end())
  {
    userMap.insert(std::pair<int,int>((*user_ptr)->userData, (*user_ptr)->userId));
    ++user_ptr;
  }
  std::vector<Movie *>::iterator movie_ptr = movieList.begin();
  while (movie_ptr != movieList.end())
  {
    movieMap.insert(std::pair<int,int>((*movie_ptr)->movieData, (*movie_ptr)->movieId));
    ++movie_ptr;
  }
  std::cout << "done" << std::endl;
}

/**
 * Get the training set and save in rating matrix
 */
void GlobalData::getRatingMatrix(const std::string& userHistory, const std::string& userSocial,  const std::string& trainingSet)
{
  getUMList(userHistory, userSocial);
  std::vector<std::string> infoList;
  try {
    ReadFileUtil::readFileByLine(trainingSet, infoList);
  }catch(const std::string& msg) {
    std::cout << "error:" << msg << std::endl;
    exit(-1);
  }
  //reserve memory for 2-dimensional std::vector
  rating.resize(userList.size());
  std::vector<std::vector<unsigned char> >::iterator it = rating.begin();
  while (it != rating.end()) {
    it->resize(movieList.size(), -1);// == 255
     ++it;
  }
  std::vector<std::string>::const_iterator ptr = infoList.begin();
  int rowIndex = 0, colIndex = 0;
  while(ptr != infoList.end())
  {
    std::istringstream f(*ptr);
    std::string s;
    getline(f, s, '\t');
    std::tr1::unordered_map<int,int>::iterator u = userMap.find(atoi(s.c_str())) ;
    if (u != userMap.end()) {
      rowIndex =  u->second;
    }
    else {
      ++ptr;
      continue;
    }
    getline(f, s, '\t');
    std::tr1::unordered_map<int,int>::iterator m = movieMap.find(atoi(s.c_str())) ;
    if (m != movieMap.end()) {
      colIndex =  m->second;
    }
    else {
      ++ptr;
      continue;
    }
    getline(f, s, '\t');
    rating[rowIndex][colIndex] = static_cast<unsigned char>(atoi(s.c_str()));
    ++ptr;
  }
  infoList.clear();
}
// store the one degree friend for each user in map
void GlobalData::getSocialMap(const std::string& userSocial)
{
    std::vector<std::string> infoList;
    try {
      ReadFileUtil::readFileByLine(userSocial, infoList);
    }catch(const std::string& msg) {
      std::cout << "error: " << msg << std::endl;
    }

    std::vector<int> socialList;
    std::vector<std::string>::iterator ptr = infoList.begin();
    std::string firstUser;
    std::string s;
    while(ptr != infoList.end()) {
      std::istringstream f(*ptr);
      getline(f, firstUser, '\t');
      std::string sec;
      getline(f,sec,'\t');
      std::istringstream secPartUser(sec);
      while (getline(secPartUser, s, ',')) {
        socialList.push_back(atoi(s.c_str()));
      }
      socialMap.insert(std::pair<int, std::vector<int> >(atoi(firstUser.c_str()), socialList));
      socialList.clear();
      ++ptr;
    }
    infoList.clear();
}

void GlobalData::calAvgRating()
{
  //std::cout << "rating size" << rating.size() << std::endl;
  avgRate.resize(userList.size());
  int rowIndex = 0, colIndex = 0, rowSum = 0;
  int realNum = 0;
  std::vector<std::vector<unsigned char> >::iterator ptr =  rating.begin();
  while (ptr != rating.end()) {
    std::vector<unsigned char>::iterator ptr1 = ptr->begin();
    colIndex = 0;
    rowSum = 0;
    realNum = 0;
    while (ptr1 != ptr->end()) {
      if(static_cast<int>(*ptr1) != 255) {
        rowSum += static_cast<int>(*ptr1);
        realNum++;
      }
      ++colIndex;
      ++ptr1;
    }
    if(realNum != 0)
      avgRate[rowIndex] = (rowSum * 1.0)/realNum;
    else {
      avgRate[rowIndex] = 0;
    }
    ++rowIndex;
    ++ptr;
  }

}


void GlobalData::getPredictPairs(const std::string& predictSource)
{
  std::vector<std::string> infoList;
  try {
    ReadFileUtil::readFileByLine((predictSource), infoList);
  }catch(const std::string& msg)
  {
    std::cout << "error:" << msg << std::endl;
    exit(-1);
  }
  //for each info pair predict
  int userData =  0, movieData = 0;
  int userId = 0, movieId = 0;
  int oldUserId = -1;
  std::vector<int> tempList;
  for(int i = 0; i < (int)infoList.size(); i++) {
    //split to userData and movieData
    std::istringstream f(infoList[i]);
    std::string s;
    getline(f, s, '\t');
    userData = atoi(s.c_str());
    getline(f, s, '\t');
    movieData = atoi(s.c_str());

    //find userId by userData
    userId = findUserIdByUserData(userData);
    if(userId == -1){
      // std::cout << "warning: could not find user in usermap, so, we could not predit this pair" << std::endl;
       continue;
    }
    //find movieData by movieId
    movieId = findMovieIdByMovieData(movieData);
    if(movieId == -1){
      // std::cout << "warning: could not find movie in moviemap, so, we could not predit this pair" << std::endl;
       continue;
    }
    if(oldUserId == -1) {
      oldUserId = userId;
      tempList.push_back(movieId);
      continue;
    }
    if(oldUserId != userId) {
      //insert to map
      predictPairs.insert(std::pair<int, std::vector<int> >(oldUserId, tempList));
      //create a new list
      tempList.clear();
      oldUserId = userId;
    }
    tempList.push_back(movieId);

  }//for
  infoList.clear();
  tempList.clear();
}

int GlobalData::findUserIdByUserData(int userData)
{
  std::tr1::unordered_map<int, int>::iterator u = userMap.find(userData);
  if(u != userMap.end())
    return  u->second;
  else
    return -1;
}

int GlobalData::findMovieIdByMovieData(int movieData)
{
  std::tr1::unordered_map<int, int>::iterator m = movieMap.find(movieData);
  if(m != movieMap.end())
    return  m->second;
  else
    return -1;
}
