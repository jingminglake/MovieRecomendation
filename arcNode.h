#ifndef ARCNODE__H
#define ARCNODE__H
#include <cstddef>
class ArcNode{
 public:
  ArcNode(){
    adjvex = -1;
    nextArc = NULL;
  }
  int adjvex; // id of adjacency neighbor
  ArcNode *nextArc;
};

#endif
