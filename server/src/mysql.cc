#include "mysql.h"

#include <cctype>
#include <glog/logging.h>
#include <sstream>
#include <stdio.h>

#define USER_NAME "root"
#define PASS_WORD "123456"
#define SCHEMA_NAME "opcuadata"

MysqlDataSave::MysqlDataSave() {}

bool MysqlDataSave::isCreated(const std::string &tableName) {
  std::stringstream ss;
  ss << "SELECT COUNT(*) FROM INFORMATION_SCHEMA.TABLES ";
  ss << "WHERE TABLE_SCHEMA = '" << SCHEMA_NAME << "' AND TABLE_NAME = '"
     << tableName << "'";

  try {
    sql::Statement *stmt = conn_->createStatement();
    if (stmt == nullptr) {
      LOG(ERROR) << "SQL createStatement return null";
      return false;
    }

    sql::ResultSet *result = stmt->executeQuery(ss.str());
    if (result == nullptr) {
      LOG(ERROR) << "SQL executeQuery " << ss.str() << " result is null";
      delete stmt;
      return false;
    }

    if (!result->next()) {
      LOG(ERROR) << "SQL executeQuery " << ss.str() << " next is false";
      return false;
    }

    bool flag = result->getInt(1) > 0;
    delete stmt;
    delete result;

    return flag;
  } catch (const std::exception &e) {
    LOG(ERROR) << "SQL executeQuery failed, " << e.what();
  }

  return false;
}

bool MysqlDataSave::getColumns(const std::string &tableName,
                               std::vector<ColumnInfo> &columnInfos) {
  std::stringstream ss;
  ss << "SELECT COLUMN_NAME, COLUMN_COMMENT FROM INFORMATION_SCHEMA.COLUMNS ";
  ss << "WHERE TABLE_SCHEMA = '" << SCHEMA_NAME << "' AND TABLE_NAME = '"
     << tableName << "'";

  try {
    sql::Statement *stmt = conn_->createStatement();
    if (stmt == nullptr) {
      LOG(ERROR) << "SQL createStatement return null";
      return false;
    }

    sql::ResultSet *result = stmt->executeQuery(ss.str());
    if (result == nullptr) {
      LOG(ERROR) << "SQL executeQuery " << ss.str() << " result is null";
      delete stmt;
      return false;
    }

    while (result->next()) {
      ColumnInfo column = {result->getString(1), result->getString(2)};

      LOG(INFO) << "Get Columns for " << tableName << " column: " << column.name
                << " comment: " << column.comment;

      columnInfos.push_back(column);
    }

    LOG(INFO) << "Get Columns for " << tableName << " column size "
              << columnInfos.size();
    delete stmt;
    delete result;

    return true;
  } catch (const std::exception &e) {
    LOG(ERROR) << "SQL executeQuery failed, " << e.what();
  }

  return false;
}

bool MysqlDataSave::insertColumns(const std::string &tableName,
                                  const std::vector<ColumnInfo> &insert,
                                  const std::vector<ColumnInfo> &exists) {
  std::vector<ColumnInfo> not_exists;

  for (const auto &column : insert) {
    auto result = std::find(exists.begin(), exists.end(), column);
    if (result == exists.end()) {
      not_exists.push_back(column);
    }
  }

  try {
    if (not_exists.size()) {
      std::stringstream ss;
      ss << "ALTER TABLE " << SCHEMA_NAME << "." << tableName << " ";
      for (auto i = 0; i < not_exists.size(); i++) {
        ss << "ADD " << not_exists[i].name << " TEXT COMMENT '"
           << not_exists[i].comment << "'";
        if (i + 1 == not_exists.size()) {
          ss << "; ";
        } else {
          ss << ", ";
        }
      }
      if (!executeSQL(ss.str())) {
        return false;
      }
    }
    return true;
  } catch (const std::exception &e) {
    LOG(ERROR) << "SQL executeQuery failed, " << e.what();
  }

  return false;
}

std::string MysqlDataSave::escapeString(const char *input) {
  std::string output;
  const std::vector<char> characters = {
      '-',  '?', '!', ':', ';', '&', '^', '(',  ')',  '#',  '@', '/', '\\', '"',
      '\'', '<', '>', '{', '}', '*', ' ', '\r', '\n', '\t', '|', '=', '+'};
  while (*input != '\0') {
    if (*input == '.') {
      output.push_back('$');
    } else {
      if (std::find(characters.begin(), characters.end(), *input) ==
          characters.end()) {
        output.push_back(*input);
      } else {
        output.push_back('_');
      }
    }
    input++;
  }
  return output;
}

std::string MysqlDataSave::columnString(const std::string &orginName) {
  if (orginName.size() < 60) {
    return orginName;
  }
  return "_" + orginName.substr(orginName.size() - 60);
}

bool MysqlDataSave::attachTable(const NodeInfo &dataParam) {
  TableInfo info;
  info.tableName = escapeString(dataParam.nodeName.c_str());

  //   for (size_t i = 0; i < dataParam._nodeNum; i++) {
  //     std::string comment = escapeString(dataParam._nodes[i].name);

  //     ColumnInfo column;
  //     column.name = columnString(comment);
  //     column.comment = comment;

  //     info.columnInfos.push_back(column);
  //   }

  if (isCreated(info.tableName)) {
    std::vector<ColumnInfo> oldColumnInfos;
    if (!getColumns(info.tableName, oldColumnInfos)) {
      return false;
    }

    if (!insertColumns(info.tableName, info.columnInfos, oldColumnInfos)) {
      return false;
    }

  } else {
    if (!createTable(info.tableName, info.columnInfos)) {
      return false;
    }
  }

  LOG(INFO) << "Attach Table " << info.tableName << " successful";

  tables_[info.tableName] = info;
  return true;
}

bool MysqlDataSave::createTable(const std::string &tableName,
                                const std::vector<ColumnInfo> &columnInfos) {

  std::stringstream ss;
  ss << "CREATE TABLE " << SCHEMA_NAME << "." << tableName << " (";
  ss << " id INT PRIMARY KEY AUTO_INCREMENT, ";
  ss << " timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, ";

  for (size_t i = 0; i < columnInfos.size(); i++) {
    ss << columnInfos[i].name << " TEXT COMMENT '" << columnInfos[i].comment
       << "'";
    if (i + 1 != columnInfos.size()) {
      ss << ", ";
    }
  }
  ss << " )";

  if (!executeSQL(ss.str())) {
    return false;
  }

  LOG(INFO) << "Create Table " << tableName << " successful";

  return true;
}

bool MysqlDataSave::executeSQL(const std::string &sql) {
  if (!conn_) {
    LOG(ERROR) << "SQL executeSQL disconnnect";
    return false;
  }

  try {
    sql::Statement *stmt = conn_->createStatement();
    if (stmt == nullptr) {
      LOG(ERROR) << "SQL createStatement return null";
      return false;
    }

    int result = stmt->executeUpdate(sql);

    if (result < 0) {
      LOG(ERROR) << "SQL executeUpdate SQL " << sql << " result is null";
      delete stmt;

      return false;
    }

    delete stmt;

    return true;
  } catch (const std::exception &e) {
    LOG(ERROR) << "SQL executeSQL failed, " << e.what();
    return false;
  }
}

bool MysqlDataSave::connection() {
  try {
    sql::ConnectOptionsMap options;
    options["hostName"] = "localhost";
    options["port"] = 3306;
    options["userName"] = USER_NAME;
    options["password"] = PASS_WORD;

    driver_ = sql::mysql::get_driver_instance();
    if (driver_ == nullptr) {
      LOG(ERROR) << "SQL get driver instance is null";
      return false;
    }

    conn_ = driver_->connect(options);
    if (conn_ == nullptr) {
      LOG(ERROR) << "SQL connection return is null";
      return false;
    }

    return executeSQL("CREATE DATABASE IF NOT EXISTS " SCHEMA_NAME);
  } catch (const std::exception &e) {
    LOG(ERROR) << "SQL connection failed, " << e.what();
    return false;
  }
}

bool MysqlDataSave::saveValues(const NodeInfo &dataParam,
                               const std::vector<std::string> &nodeValues) {
  try {
    std::string tableName = escapeString(dataParam.nodeName.c_str());
    auto find = tables_.find(tableName);
    if (find == tables_.end()) {
      LOG(ERROR) << "SQL saveValues failed, " << tableName << " not existed";
      return false;
    }

    if (find->second.columnInfos.size() != nodeValues.size()) {
      LOG(ERROR) << "SQL saveValues failed, " << tableName
                 << ", ColumnSize not match";
      return false;
    }

    std::stringstream ss;
    ss << "INSERT INTO " << SCHEMA_NAME << "." << find->second.tableName
       << " ( ";
    for (size_t i = 0; i < find->second.columnInfos.size(); i++) {
      ss << find->second.columnInfos[i].name;
      if (i + 1 != find->second.columnInfos.size()) {
        ss << ", ";
      }
    }
    ss << " ) VALUES ( ";

    for (size_t i = 0; i < nodeValues.size(); i++) {
      if (nodeValues[i].empty()) {
        ss << "NULL";
      } else {
        ss << "'" << nodeValues[i] << "'";
      }
      if (i + 1 != nodeValues.size()) {
        ss << ", ";
      }
    }
    ss << " )";

    if (!executeSQL(ss.str())) {
      return false;
    }

    return true;
  } catch (const std::exception &e) {
    LOG(ERROR) << "SQL execute insert failed, " << e.what();
    return false;
  }
}

bool MysqlDataSave::setDataExpired() {
  try {
    if (!executeSQL("SET GLOBAL event_scheduler = ON;")) {
      return false;
    }

    if (!executeSQL("USE opcuadata;")) {
      return false;
    }

    if (!executeSQL("DROP EVENT IF EXISTS "
                    "opcuadata_data_expired_event;")) {
      return false;
    }

    std::stringstream ss;

    ss << "CREATE EVENT opcuadata_data_expired_event ON SCHEDULE EVERY 1 HOUR "
          "STARTS "
          "CURRENT_TIMESTAMP ON COMPLETION PRESERVE DO BEGIN "
       << std::endl;

    for (const auto &table : tables_) {
      ss << "DELETE FROM opcuadata." << table.second.tableName
         << " WHERE timestamp < DATE_SUB(NOW(), INTERVAL 30 DAY); "
         << std::endl;
    }

    ss << " END;";

    if (!executeSQL(ss.str())) {
      return false;
    }

    LOG(INFO) << "SQL set data expired event success";

    return true;
  } catch (const std::exception &e) {
    LOG(ERROR) << "SQL set data expired event failed, " << e.what();
    return false;
  }
}

MysqlDataSave::~MysqlDataSave() {
  if (conn_) {
    executeSQL("USE opcuadata;");
    executeSQL("DROP EVENT IF EXISTS "
               "opcuadata_data_expired_event;");
    conn_->close();
  }
}