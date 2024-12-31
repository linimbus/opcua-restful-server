#pragma once

#include <stdint.h>
#include <string>
#include <vector>

typedef struct nodeInfo {
  uint32_t nsIndex;
  uint32_t signCode;
  std::string nodeID;
  std::string nodeName;
} NodeInfo;

typedef struct nodeTree {
  uint32_t level;
  NodeInfo nodeInfo;
  struct nodeTree *parent;
  std::vector<NodeInfo> subNodes;
} NodeTree;

typedef struct deviceInfo {
  std::string name;
  std::string followName;
  std::string address;
  std::vector<NodeInfo> nodes;
} DeviceInfo;