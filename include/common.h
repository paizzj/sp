#ifndef __COMMON_H__
#define __COMMON_H__

#include <string>
#include <map>
#include <iostream>
#include <fstream>

#include "db_mysql.h"
#include "json.hpp"

#include <curl/curl.h>
#include <glog/logging.h>
#include <boost/program_options.hpp>

using json = nlohmann::json;

class ConfManager
{
public:
    ConfManager(std::string path = "../conf/server_main.conf"):path_(path)
    {}
    ~ConfManager(){}
    std::string getArgs(const std::string& strArg, const std::string& strDefault);
    void setConfPath(std::string path)
    {
        path_ = path;
    }
    void printArg();
    void readConfigFile();

private:
    void readConfJsonObj(const json js);
private:
    std::string path_;
    json conf_;
    std::map<std::string,std::string> mapArgs_;
};

extern std::unique_ptr<ConfManager>confptr;

bool ParseCmd(int argc, char*argv[]);
bool InitDB();
bool GetLastHeight(uint64_t &height);
bool CurlPost(const std::string& data, std::string& response);

#endif

