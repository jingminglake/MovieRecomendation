#include "socialGraph.h"

SocialGraph::~SocialGraph()
{
  std::vector<VertexNode*>::iterator ptr = vextices.begin();
  while (ptr != vextices.end()) {
    delete *ptr;
    ++ptr;
  }
}
/**
 * Create direceted graph using adjacency list
 */
void SocialGraph::createDG( std::tr1::unordered_map<int,std::vector<int> >& socialMap, std::vector<User *>& userList, std::tr1::unordered_map<int,int> userMap)
{
    /* build the adjacency table. one problem is
       that a node need data and also id, another
       problem is the one degree friend may need not be taken into
       consideration( the user may not exits in userList).
    */
    std::vector<User*>::iterator test = userList.begin();
    while (test != userList.end()) {
      //  std::cout << (*test)->userData << std::endl;
      ++test;
    }
    vexNum = userList.size();
    std::cout << vexNum << std::endl;
    ArcNode *temp = NULL;
    for (int i = 0; i < vexNum; i++)
    {
      VertexNode *vn = new VertexNode();
      vn->vexId = userList[i]->userId;
      vn->vexData = userList[i]->userData;
      std::tr1::unordered_map<int,std::vector<int> >::iterator neighborList = socialMap.find(vn->vexData);
      if (neighborList == socialMap.end()) {
        std::cout << "error: neighborList not found" << std::endl;
        exit(-1);
      }
      std::vector<int>::iterator ptr = (neighborList->second).begin();
      while (ptr != (neighborList->second).end()) {
        std::tr1::unordered_map<int, int>::iterator it = userMap.find(*ptr);
        if(it == userMap.end()) {
          //  std::cout << "in userMap not found, continue..." << std::endl;
           ++ptr;
           continue;
        }
        // std::cout << "in userMap found, id=" << it->second << std::endl;
        temp = new ArcNode();
        temp->adjvex = it->second;
        temp->nextArc = vn->firstArc;
        vn->firstArc = temp;
        ++ptr;
        ++arcNum;
      }//end while
      vextices.push_back(vn);
    }//end for
}//end createDG

/**
 * get shorest distance from one user to all his similar users using
 * a modified Dijstra algorithm
 **/
bool SocialGraph::shortestPathDjst(int vexId, std::vector<std::vector<int> >& prePath)
{
  prePath.resize(vexNum);//prePath is the shortest pre path from vexId to others
  //save all the shortest distances from vexId to others
  std::vector<int> distance(vexNum, INT_MAX);// INT_MAX to all others at the beginning
  std::vector<bool> isVisited(vexNum, false);
  //store the current minimal distance to distMap;
  ArcNode *p = vextices[vexId]->firstArc;
  if(!p) {
    std::cout << vexId <<" has no friend in the social traing set!" << std::endl;
    return false;
  }
  while (p != NULL) {
    distance[p->adjvex] = 1;//all edge weight is 1
    prePath[p->adjvex].push_back(vexId);
    p = p->nextArc;
  }
  isVisited[vexId] = true;
  int nextVexId = 0;
  for (int i = 1; i < vexNum; i++) {
    //find minimum
    int min = distance[0];
    for (int j = 0; j < vexNum; j++) {
      if (!isVisited[j] && (distance[j] < min)) {
        min = distance[j];//shorest distance from vexId to j
        nextVexId = j;
      }
    }
    isVisited[nextVexId] = true;
    //update distance[]
    p = vextices[nextVexId]->firstArc;
    while (p != NULL) {
      if(p->adjvex != -1 && !isVisited[p->adjvex]) {
        int tmp = min + 1;// plus 1 because all edge weight is 1, otherwise should plus the weight from  nextVexId to p->adjvex
        if(tmp <= distance[p->adjvex]) {
           if (tmp < distance[p->adjvex]) {
             prePath[p->adjvex].clear();
             distance[p->adjvex] = tmp;
           }
           prePath[p->adjvex].push_back(nextVexId);
        }
      }
        p = p->nextArc;
    }//while
  }//for
  //result
  std::cout << "all similar users: "  << std::endl;
  for (int ii = 0; ii < vexNum; ii++)
  {
    if(distance[ii] != INT_MAX)
      std::cout << "user" << vexId <<" to user"<< ii << " distance: " << distance[ii] << std::endl;

  }
  return true;
}

/**
 *  after our modified dijstra algorithm, we could get all the nodes preNode, and we could get all the paths use this function
 **/
void SocialGraph::findAllPathsForPrePath(std::vector<std::vector<int> >& prePath, int userId, int vexId, Paths* p, std::vector<int>& nodes, int i)
{
  while (vexId != userId) {
    nodes[i] = vexId;
    if (prePath[vexId].size() > 1) {
      for (int j = 0; j < (int)prePath[vexId].size(); j++) {
        vexId = prePath[vexId][j];
        findAllPathsForPrePath(prePath, userId, vexId, p, nodes, i+1);
      }
      return;
    } else if (prePath[vexId].size() == 0) {
        return;
      }
      else if (prePath[vexId].size() == 1) {
        vexId = prePath[vexId][0];
      }
    i++;
    nodes[i] = vexId;
  }//while
  std::vector<int> pathList;
  for (int k = i; k >= 0; k--) {
    pathList.push_back(nodes[k]);
  }
  p->pathMap.insert(std::pair<int,std::vector<int> >(p->pathCount, pathList));
  (p->pathCount)++;
  pathList.clear();
  return;
}
