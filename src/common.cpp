#include "common.h"

std::unique_ptr<ConfManager> confptr(new ConfManager);

std::string ConfManager::getArgs(const std::string& strArg,const std::string& strDefault)
{
    if(mapArgs_.count(strArg))
    {
        return mapArgs_[strArg];
    }
    return strDefault;
}

void ConfManager::readConfigFile()
{
    std::ifstream jfile(path_);

    if (!jfile)
    {
        LOG(ERROR) << "No such config file!";
        return ;
    }

    jfile >> conf_;
    if(!conf_.is_object())
    {
        conf_.clear();
        return ;
    }
    readConfJsonObj(conf_);
    conf_.clear();
}

void ConfManager::readConfJsonObj(const json js)
{
    try
    {
        auto it = js.begin();
        auto end = js.end();
        for (it; it !=end; ++it)
        {
            std::string key = it.key();
            auto val = it.value();
            if (val.is_object())
            {
                readConfJsonObj(val);
            }
            else if (val.is_string())
            {
                mapArgs_[it.key()] = val;
            }
        }
    }
    catch(...)
    {
        printArg();
        conf_.clear();
    }
}

void ConfManager::printArg()
{
    for (auto &it: mapArgs_)
        std::cout << it.first << ": " << it.second << std::endl;
}

static ConfManager s_conf;
static bool InitLog(const std::string& log_path)
{
    FLAGS_alsologtostderr = false;
    FLAGS_colorlogtostderr = true;
    FLAGS_max_log_size = 100;
    FLAGS_stop_logging_if_full_disk  = true;
    std::string log_exec = "log";

    FLAGS_logbufsecs = 0;

    google::InitGoogleLogging(log_exec.c_str());
    FLAGS_log_dir = log_path;

    std::string log_dest = log_path+"/info_";
    google::SetLogDestination(google::GLOG_INFO,log_dest.c_str());
    log_dest = log_path+"/warn_";

    google::SetLogDestination(google::GLOG_WARNING,log_dest.c_str());
    log_dest = log_path+"/error_";
    google::SetLogDestination(google::GLOG_ERROR,log_dest.c_str());
    log_dest = log_path+"/fatal_";
    google::SetLogDestination(google::GLOG_FATAL,log_dest.c_str());
    google::SetStderrLogging(google::GLOG_ERROR);

    return true;
}

static size_t ReplyCallback(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::string *str = (std::string*)stream;
    (*str).append((char*)ptr, size*nmemb);
    return size * nmemb;
}

bool CurlPost(const std::string& data, std::string& response)
{
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    CURLcode res;
    std::string error_str;
    if (curl)
    {
        headers = curl_slist_append(headers, "content-type:application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, s_conf.getArgs("node", "127.0.0.1").c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)data.size());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ReplyCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);

        curl_easy_setopt(curl, CURLOPT_USERPWD, s_conf.getArgs("auth", "dev:a").c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);

        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
        res = curl_easy_perform(curl);
    }
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        error_str = curl_easy_strerror(res);
        LOG(ERROR) << "CurlPost error: " << error_str ;
        return false;
    }

    if (response.empty() || response == "")
    {
        LOG(ERROR) << "CurlPost response empty";
        return false;
    }
    return true;
}

bool ParseCmd(int argc,char*argv[])
{
    using namespace boost::program_options;
    std::string path_configure ;
    std::string path_log;
    boost::program_options::options_description opts_desc("All options");
    opts_desc.add_options()
            ("help,h", "help info")
            ("log,l", value<std::string>(&path_log)->default_value("../log"), "log configure ")
            ("configure,c", value<std::string>(&path_configure)->default_value("../conf/server_main.conf"), "path configure ");

    variables_map cmd_param_map;
    try
    {
        store(parse_command_line(argc, argv, opts_desc), cmd_param_map);
    }
    catch(boost::program_options::error_with_no_option_name &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    notify(cmd_param_map);

    if (cmd_param_map.count("help"))
    {
        std::cout << opts_desc << std::endl;
        return false;
    }

    InitLog(path_log);

    s_conf.setConfPath(path_configure);
    s_conf.readConfigFile();
    s_conf.printArg();
    return true;
}

bool InitDB()
{
    DBMysql::MysqlConnect* connect = new DBMysql::MysqlConnect();
    connect->url = s_conf.getArgs("mysqlserver", "127.0.0.1");
    connect->user_name = s_conf.getArgs("mysqluser", "root");
    connect->user_pass = s_conf.getArgs("mysqlpass", "a");
    connect->port = std::atoi(s_conf.getArgs("mysqlport", "3306").c_str());
    connect->use_db = s_conf.getArgs("mysqldb", "fch");
    g_db_mysql->SetConnect(connect);
    return g_db_mysql->OpenDB();
}

bool GetLastHeight(uint64_t &height)
{
    std::string sql = "select blockcount from block";

    DBMysql::JsonDataFormat json_format;
    json_format.column_size = 1;
    json_format.map_column_type[0] = DBMysql::INT;

    json json_data;
    bool ret = g_db_mysql->GetDataAsJson(sql, &json_format, json_data);

    if (!ret)
    {
        LOG(ERROR) << "GetLastHeight error";
        return false;
    }

    if (json_data.size() == 0) {
        height = 0;
    } else {
        height = json_data.at(0).at(0).get<uint64_t>();
    }
    return true;
}

bool GetPools(std::vector<std::string>& pools)
{
    std::string sql = "select distinct txid from utxo where height=0";

    DBMysql::JsonDataFormat json_format;
    json_format.column_size = 1;
    json_format.map_column_type[0] = DBMysql::STRING;

    json json_data;
    bool ret = g_db_mysql->GetDataAsJson(sql, &json_format, json_data);

    if (!ret)
    {
        LOG(ERROR) << "GetPools error";
        return false;
    }

    std::string txid = "";
    for (int i = 0; i < json_data.size(); ++i) {
        json json_value = json_data.at(i);
        txid = json_value.at(0).get<std::string>();
        pools.push_back(txid);
    }
    return true;
}
