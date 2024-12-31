#pragma once

#include <stdint.h>
#include <string>
#include <vector>

typedef struct nodeInfo {
  uint32_t namespaceIndex;
  std::string name;
  std::string nodeID;
} NodeInfo;

typedef struct nodeTree {
  uint32_t level;
  NodeInfo nodeInfo;
  struct nodeTree *parent;
  std::vector<NodeInfo> subNodes;
} NodeTree;
