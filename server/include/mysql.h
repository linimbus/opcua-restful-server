#pragma once

#include "types.h"

#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <mysql_driver.h>

struct ColumnInfo {
  std::string name;
  std::string comment;

  bool operator==(const ColumnInfo &b) const {
    if (name == b.name) {
      return true;
    }
    return comment == b.comment;
  }
};

struct TableInfo {
  std::string tableName;
  std::vector<ColumnInfo> columnInfos;
};

class MysqlDataSave {
public:
  MysqlDataSave();
  ~MysqlDataSave();

  bool connection();

  bool attachTable(const NodeInfo &dataParam);

  bool saveValues(const NodeInfo &dataParam,
                  const std::vector<std::string> &nodeValues);

  bool setDataExpired();

private:
  bool executeSQL(const std::string &sql);

  bool createTable(const std::string &tableName,
                   const std::vector<ColumnInfo> &columnNames);

  bool isCreated(const std::string &tableName);

  bool insertColumns(const std::string &tableName,
                     const std::vector<ColumnInfo> &insert,
                     const std::vector<ColumnInfo> &exists);

  bool getColumns(const std::string &tableName,
                  std::vector<ColumnInfo> &columnInfos);

  std::string escapeString(const char *);
  std::string columnString(const std::string &orginName);

  std::map<std::string, TableInfo> tables_;
  sql::mysql::MySQL_Driver *driver_ = NULL;
  sql::Connection *conn_ = NULL;
};
