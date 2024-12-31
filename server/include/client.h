#pragma once

#include "types.h"

#include <open62541/client.h>
#include <vector>

class Client {
public:
  Client(/* args */);
  ~Client();

  void setEnable(bool enable);
  bool getEnable();
  bool connnectServer(const char *serverAddress);
  void disconnectServer();
  bool isOnline();
  bool reconnect();
  bool readNode(NodeInfo np, std::string &nodeValue);
  bool readNode(NodeInfo np, UA_Variant &variant);
  bool readNodes(const NodeInfo *nps, int nNum,
                 std::vector<std::string> &nodeValues);
  bool writeNode(NodeInfo nodeParam, int nValue);
  bool writeNode(NodeInfo nodeParam, UA_Variant variant);
  bool writeNodes(const NodeInfo *nps, int nNum);
  void browseNodes(std::vector<NodeTree> &nodes, int level);

private:
  UA_Client *m_client;
  std::string m_serverAddress;
  bool m_enable;
};
