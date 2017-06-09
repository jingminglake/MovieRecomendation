#ifndef VERTEXNODE__H
#define VERTEXNODE__H

#include "arcNode.h"
class VertexNode{
 public:
  // static long serialVersionUID = 1;
  VertexNode() {
    vexId = 0;
    vexData = 0;
    firstArc = NULL;
  }
  ~VertexNode() {
     ArcNode *temp;
     while (firstArc != NULL) {
       temp = firstArc;
       firstArc = firstArc->nextArc;
       delete temp;
     }
  }
  int vexId;
  int vexData;
  ArcNode* firstArc;
};

#endif
