#pragma once

#include "types.h"

#include <map>
#include <vector>

struct DataCacheHeader {
  uint32_t m_magic;
  uint32_t m_version;
  uint32_t m_dataSize;

  DataCacheHeader() {
    m_magic = 0x1F19AB51;
    m_version = 0x00000001;
    m_dataSize = 0;
  }
};

using FlowerBasketName = std::string;
using FlowerBasketID = std::string;
using FlowerBasketKey = std::string; // FlowerBasketName+FlowerBasketID

using SiliconName = std::string;
using SiliconList = std::string;
using SiliconMap = std::map<SiliconName, SiliconList>;

struct DataCacheInfo {
  std::string itemName;
  std::string followName;

  FlowerBasketName inFlowerBasketName;
  FlowerBasketName outFlowerBasketName;

  std::map<FlowerBasketName, FlowerBasketID> outFlowerBasketIDList;
  std::map<FlowerBasketKey, SiliconMap *> outSiliconMapList;

  DataCacheInfo(const DeviceInfo *device) {
    itemName = device->name;
    followName = device->followName;

    for (auto &node : device->nodes) {
      if (node.signCode == 6) {
        inFlowerBasketName = node.nodeName;
      } else if (node.signCode == 7) {
        outFlowerBasketName = node.nodeName;
      }
    }
  }
};

class DataCache {
public:
  DataCache(const std::string &filename);
  ~DataCache();

public:
  bool readOpcuaData(const DeviceInfo *DeviceInfo,
                     const std::vector<std::string> &nodeValue,
                     std::vector<std::string> &opcuaData);

  bool saveOpcuaData(const DeviceInfo *DeviceInfo,
                     const std::vector<std::string> &nodeValue);

private:
  std::string getNodeValue(const std::string &nodeName,
                           const DeviceInfo *DeviceInfo,
                           const std::vector<std::string> &nodeValue);

  void getSiliconList(const DeviceInfo *DeviceInfo,
                      const std::vector<std::string> &nodeValue,
                      SiliconMap *silicons);

  bool getSiliconList(const std::string &itemName,
                      const FlowerBasketName &outFlowerName,
                      const FlowerBasketID &flowerID, SiliconMap **silicons);

  DataCacheInfo *getCache(const DeviceInfo *DeviceInfo);

  std::map<std::string, DataCacheInfo *> cache_;
};