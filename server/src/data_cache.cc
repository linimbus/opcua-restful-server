
#include "data_cache.h"
#include <glog/logging.h>

DataCache::DataCache(const std::string &filename) {}

DataCacheInfo *DataCache::getCache(const DeviceInfo *device) {
  std::string itemName = device->name;
  auto find = cache_.find(itemName);
  if (find == cache_.end()) {
    cache_[itemName] = new DataCacheInfo(device);
  }
  return cache_[itemName];
}

void DataCache::getSiliconList(const DeviceInfo *device,
                               const std::vector<std::string> &nodeValue,
                               SiliconMap *silicons) {
  for (size_t i = 0; i < device->nodes.size(); i++) {
    if (device->nodes[i].signCode == 5) {
      SiliconName siliconName = device->nodes[i].nodeName;
      SiliconList siliconList = nodeValue[i];
      silicons->emplace(siliconName, siliconList);
    }
  }
}

std::string DataCache::getNodeValue(const std::string &nodeName,
                                    const DeviceInfo *device,
                                    const std::vector<std::string> &nodeValue) {

  for (size_t i = 0; i < device->nodes.size(); i++) {
    if (device->nodes[i].nodeName == nodeName) {
      return nodeValue[i];
    }
  }
  return "";
}

bool DataCache::getSiliconList(const std::string &itemName,
                               const FlowerBasketName &outFlowerName,
                               const FlowerBasketID &flowerID,
                               SiliconMap **silicons) {
  auto find = cache_.find(itemName);
  if (find == cache_.end()) {
    return false;
  }
  DataCacheInfo *cache = find->second;

  if (cache->outFlowerBasketName != outFlowerName) {
    return false;
  }

  FlowerBasketKey key = outFlowerName + "_" + flowerID;

  auto find2 = cache->outSiliconMapList.find(key);
  if (find == cache_.end()) {
    return false;
  }
  *silicons = find2->second;

  return true;
}

bool DataCache::readOpcuaData(const DeviceInfo *device,
                              const std::vector<std::string> &nodeValue,
                              std::vector<std::string> &opcuaData) {
  DataCacheInfo *cache = getCache(device);
  FlowerBasketID flowerID =
      getNodeValue(cache->inFlowerBasketName, device, nodeValue);
  if (flowerID.empty()) {
    return false;
  }

  SiliconMap *silicons = nullptr;
  if (getSiliconList(cache->followName, cache->inFlowerBasketName, flowerID,
                     &silicons) == false) {
    return false;
  }

  for (auto &node : device->nodes) {
    if (node.signCode == 5) {
      SiliconName siliconName = node.nodeName;
      auto find = silicons->find(siliconName);
      if (find != silicons->end()) {
        opcuaData.push_back(find->second);
        continue;
      }
    }
    opcuaData.push_back("");
  }

  return true;
}

bool DataCache::saveOpcuaData(const DeviceInfo *DeviceInfo,
                              const std::vector<std::string> &nodeValue) {
  DataCacheInfo *cache = getCache(DeviceInfo);
  FlowerBasketID flowerID =
      getNodeValue(cache->outFlowerBasketName, DeviceInfo, nodeValue);
  if (flowerID.empty()) {
    return false;
  }

  SiliconMap *silicons = new SiliconMap();
  getSiliconList(DeviceInfo, nodeValue, silicons);

  FlowerBasketKey key = cache->outFlowerBasketName + "_" + flowerID;
  cache->outFlowerBasketIDList[cache->outFlowerBasketName] = flowerID;

  if (cache->outSiliconMapList[key]) {
    delete cache->outSiliconMapList[key];
  }

  cache->outSiliconMapList[key] = silicons;
  return true;
}
