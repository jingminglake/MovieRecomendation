#ifndef SOCIALGRAPH__H
#define SOCIALGRAPH__H

#include "vertexNode.h"
#include "user.h"
#include "paths.h"
#include <sstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <tr1/unordered_map>
#include <climits>

class SocialGraph{
 public:
  //static long serialVersionUID = 1;
   ~SocialGraph();
   int vexNum;
   int arcNum;
   std::vector<VertexNode*> vextices;// all the users vertexnode
   void createDG( std::tr1::unordered_map<int,std::vector<int> >& socialMap, std::vector<User *>& userList, std::tr1::unordered_map<int,int> userMap);
   /* calculate vexId to all other reachable users(distance less than 6) and save all the possible preNode for each node on the shorest path
    */
   bool shortestPathDjst(int vexId, std::vector<std::vector<int> >& prePath);
   /*calculate all possible shorest path for prePath and save in path object
    */
   void findAllPathsForPrePath(std::vector<std::vector<int> >& prePath, int userId, int vexId, Paths* p, std::vector<int>& nodes, int i);
};

#endif
