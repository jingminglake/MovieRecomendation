#include "manager.h"
#include <ostream>
#include <sstream>
Manager::Manager()
{
}

Manager::~Manager()
{
}

void Manager::loadDataAndCreateDG()
{
   std::vector<User*>& userList  = GlobalData::getInstance().userList;
   std::tr1::unordered_map<int,std::vector<int> >& socialMap = GlobalData::getInstance().socialMap;
   std::tr1::unordered_map<int,int>& userMap = GlobalData::getInstance().userMap;

   //create social graph use user_social.txt
   sg.createDG(socialMap, userList, userMap);
}

void Manager::predictUnrated()
{
  loadDataAndCreateDG();
  predictUnratedMovies();
}
/**
 * predict rating about unrated movies and append to predictResult.txt
 *
 **/
void Manager::predictUnratedMovies()
{

   //writeToFile
  std::ofstream fout("data/testData/predictResult.txt");
   std::string predictString;
    // std::cout << "predict-->userId: " << userId << " ,movieId: " << movieId << std::endl;
    //predict begin
    std::tr1::unordered_map<int,std::vector<int> >& predictPairs = GlobalData::getInstance().predictPairs;
  std::tr1::unordered_map<int,std::vector<int> >::iterator ptr = predictPairs.begin();
  while (ptr != predictPairs.end()) {
    std::cout << "predict for userId : " << ptr->first << "--->" << std::endl;
    std::vector<int>::iterator ptr2 = ptr->second.begin();
    while (ptr2 != ptr->second.end()) {
      predictOneMovie(ptr->first, *ptr2, predictString);
      fout << predictString << std::endl;
      ++ptr2;
    }
    std::cout << std::endl;
    ++ptr;
    }
  fout.close();
}


void Manager::predictOneMovie(int userId, int movieId, std::string& predictString)
{
   //calculate shorest path in social graph for userId
   std::vector<std::vector<int> > prePath;
   std::tr1::unordered_map<int, std::vector<std::vector<int> > >::iterator ptr = prePathMap.find(userId);
   if(ptr != prePathMap.end()) {
     prePath = ptr->second;
   }
   else {
     bool success = sg.shortestPathDjst(userId, prePath);
     if(!success) {
       //  std::cout << "warning: shortestPathDjst return false!" << std::endl;
     }
     prePathMap.insert(std::pair<int, std::vector<std::vector<int> > >(userId, prePath));
   }//else

   //find similar users and calculate all simUser similarity for userId
   std::vector<int> simUserIds;
   std::vector<float> sims;

   findAllSimUsers(userId, movieId, simUserIds, sims);

   //calculate adv sim
   float advSim = 0.0f;
   float total1 = 0.0f;
   float total2 = 0.0f;
   float preRating = 0.0f;

   for(int i = 0; i < (int)sims.size(); i++) {
     advSim = sims[i];//equal to regular sims first
     Paths *p = new Paths();
     std::vector<int> nodes(100,-1);//100 is enough!!
       sg.findAllPathsForPrePath(prePath, userId, simUserIds[i], p, nodes, 0);
     //check the paths
     if (p->pathCount > 0) {
       advSim = calAdvSimilarity(p, sims[i]);
     }
     total1 += advSim * (static_cast<int>(GlobalData::getInstance().rating[simUserIds[i]][movieId]) - GlobalData::getInstance().avgRate[simUserIds[i]]);
      total2 += fabs(advSim); //fabs !!! not abs !!
      delete p;
    }//for
   if (total2 > 0) {
     preRating = GlobalData::getInstance().avgRate[userId] + total1 / total2;
   }
   else {
     preRating = GlobalData::getInstance().avgRate[userId];
   }
   prePath.clear();
   simUserIds.clear();
   sims.clear();

   int userData = GlobalData::getInstance().userList[userId]->userData;
   int movieData = GlobalData::getInstance().movieList[movieId]->movieData;

   float point = (preRating > 5) ? 5.00 : preRating;
   std::cout << "user: " << userData << " preRating for movie: " << movieData << " is: "<< std::setprecision(1) << std::fixed <<  point << std::endl;

   std::stringstream s;
   s << userData << "\t" << movieData << std::setprecision(1) << std::fixed << "\t" << point;
   predictString = s.str();

}

/*
 * Calculate similarity between users and find all his similar users, that is:
 * find similar users who rated the same movie and the similarity is postive
 */
void Manager::findAllSimUsers(int userId, int movieId, std::vector<int>& simUserIds, std::vector<float>& sims)
{

   for(int i = 0; i < (int)GlobalData::getInstance().userList.size(); i++) {
    //if rate the same movie
    if(i != userId &&  (static_cast<int>(GlobalData::getInstance().rating[i][movieId]) != 255)){
      // then calculate the similarity bwtween the two users
      float avgRating1 = 0.0f, avgRating2 = 0.0f;
      int rating1 = 0, rating2 = 0;
      float total1 = 0.0f, total2 = 0.0f, total3 = 0.0f;
      avgRating1 = GlobalData::getInstance().avgRate[userId];
      avgRating2 = GlobalData::getInstance().avgRate[i];
      for (int j = 0; j < (int)GlobalData::getInstance().movieList.size(); j++) {
        rating1 = static_cast<int>(GlobalData::getInstance().rating[userId][j]);
        rating2 = static_cast<int>(GlobalData::getInstance().rating[i][j]);
        if(rating1 != 255 && rating2 != 255) {
          total1 += (rating1 - avgRating1)*(rating2 - avgRating2);
          total2 += (rating1 - avgRating1)*(rating1 - avgRating1);
          total3 += (rating2 - avgRating2)*(rating2 - avgRating2);
        }
      }//for
      if(total1 > 0 && total2 > 0 && total3 > 0) {
        simUserIds.push_back(i);
        sims.push_back(total1 / sqrt(total2 * total3));
      }//if
    }//if
  }//for
 }

float Manager::calAdvSimilarity(Paths* p, float sim)
{
  int pathId = getBestPath(p);
  std::tr1::unordered_map<int, std::vector<int> >::iterator ptr = p->pathMap.find(pathId);
  if (ptr == p->pathMap.end()) {
    std::cout << "error: could not find best path!!" << std::endl;
  }
  std::vector<int>& pathList = ptr->second;
  int length = (pathList.size() >= 6) ? 6 : pathList.size();
  float weight = 0.2 * (6 - length);
  float advSim = sim + (1 - sim) * weight;
  return advSim;
}

/**
 * get the best path from the same weight path
 */

int Manager::getBestPath(Paths* p)
{
  int minPathIndex = 0;

  for (int i = 0; i < p->pathCount; i++) {
    float minS = 100.0f;
    float minF = 100.0f;
    float s = 0.0f;
    float f = 0.0f;
    float avg = 0.0f;
    float total1 = 0.0f;
    float total2 = 0.0f;
    float total3 = 0.0f;
    float avgRating1 = 0.0f, avgRating2 = 0.0f;
    int rating1 = 0, rating2 = 0;
    std::tr1::unordered_map<int, std::vector<int> >::iterator ptr1 = p->pathMap.find(i);
    std::vector<int>& pathList = ptr1->second;
    int n = pathList.size();
    std::vector<float> sims(n-1, 0.0f);
    for(int j = 0; j < n - 1; j++) {
      total1 = 0.0f;
      total2 = 0.0f;
      total3 = 0.0f;

      avgRating1 = GlobalData::getInstance().avgRate[pathList[j]];
      avgRating1 = GlobalData::getInstance().avgRate[pathList[j+1]];

      for(int k = 0; k < (int)GlobalData::getInstance().movieList.size(); k++) {
        rating1 = static_cast<int>(GlobalData::getInstance().rating[pathList[j]][k]);
        rating2 = static_cast<int>(GlobalData::getInstance().rating[pathList[j+1]][k]);
        if (rating1 != 0 && rating2 != 0){
          total1 += (rating1 - avgRating1)*(rating2 - avgRating2);
          total2 += (rating1 - avgRating1)*(rating1 - avgRating1);
          total3 += (rating2 - avgRating2)*(rating2 - avgRating2);
        }
      }//for
      if (total2 > 0 && total3 > 0){
        sims[j] = total1 / sqrt(total2 * total3);
        avg += sims[j];
      }
    }//for
    avg = avg / (n - 1);
    for(int m = 0; m < n - 1; m++) {
      s += (sims[m] - avg)*(sims[m] - avg);
      f += sims[m] - avg;
    }
    s = s / (n - 1);
    if(s < minS) {
      minS = s;
      minPathIndex = f;
    }
    if(s == minS && f < minF) {
      minPathIndex = i;
    }
    sims.clear();
  }//for
  return minPathIndex;
}
