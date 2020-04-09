#include "db_mysql.h"
#include <glog/logging.h>
#include <utility>

DBMysql* g_db_mysql = new DBMysql();

DBMysql::DBMysql()
{
    connect_ = nullptr;
}

DBMysql::~DBMysql()
{
    delete connect_;
}


bool DBMysql::OpenDB()
{
    if (!connect_)
        return false;

    char value = 1;

    if (mysql_init(&mysql_) == NULL)
    {
        return false;
    }

    mysql_options(&mysql_, MYSQL_OPT_RECONNECT, (char *)&value);

    if (!mysql_real_connect(&mysql_,
                            connect_->url.c_str(),
                            connect_->user_name.c_str(),
                            connect_->user_pass.c_str(),
                            connect_->use_db.c_str(),
                            connect_->port, NULL, 0))
    {
        std::string error= mysql_error(&mysql_);
        LOG(ERROR) << "openDB : " << "数据库连接失败:"<<error;
        return false;
    }
    return true;
}

void DBMysql::SetConnect(MysqlConnect*connect)
{	
    connect_ = connect;
}

void DBMysql::GetConnect(MysqlConnect*connect)
{
    connect = connect_;
}

bool DBMysql::ExecuteSql(const std::string& sql)
{
    mtx_.lock();
    LOG(INFO) << sql << std::endl;
    int ret = mysql_real_query(&mysql_, sql.c_str(), strlen(sql.c_str()));
    if (ret != 0 && mysql_errno(&mysql_) != 1062)
    {
        LOG(ERROR) << "ExecuteSql: " << sql << " FAIL." ;
        LOG(ERROR) << mysql_errno(&mysql_) << " " << mysql_error(&mysql_);
        mtx_.unlock();
        CloseDB();
        OpenDB();
        ExecuteSql(sql);
        return false;
    }
    mtx_.unlock();
    return true;
}

bool DBMysql::BatchExecSql(const std::vector<std::string> &vect_sql)
{
    mtx_.lock();
    mysql_query(&mysql_,"START TRANSACTION");
    std::string sql_query;
    int ret = 0;
    for(uint32_t i = 0; i < vect_sql.size(); i ++)
    {
        sql_query = vect_sql.at(i);
        ret = mysql_real_query(&mysql_, sql_query.c_str(), strlen(sql_query.c_str()));
        if (ret != 0 && mysql_errno(&mysql_) != 1062)
        {
            LOG(ERROR) << "batch exec sql failed: " << sql_query ;
        }
    }
    mysql_query(&mysql_,"COMMIT");
    mtx_.unlock();
    return true;
}

bool DBMysql::GetDataAsJson(const std::string& select_sql, JsonDataFormat* json_data_format ,json& json_data)
{
    if (json_data_format->column_size != json_data_format->map_column_type.size())
    {
        LOG(ERROR) << "json data format size error!";
        return false;
    }
    mtx_.lock();
    int ret = mysql_real_query(&mysql_, select_sql.c_str(), strlen(select_sql.c_str()));
    if (ret != 0)
    {
        LOG(ERROR) << "exec sql: " << select_sql << " fail: ";
        LOG(ERROR) << "reason: " << mysql_errno(&mysql_) << " " << mysql_error(&mysql_);
        mtx_.unlock();
        return false;
    }

    MYSQL_RES *result = mysql_store_result(&mysql_);
    size_t num_rows = mysql_num_rows(result);

    LOG(INFO) << "select  size:  " << num_rows ;

    for (size_t i = 0; i < num_rows; ++i)
    {
        MYSQL_ROW row = mysql_fetch_row(result);
        json row_data = json::array();

        for (size_t j = 0; j < json_data_format->column_size; j++)
        {
            std::string sql_data = row[j];
            JsonDataType type  = json_data_format->map_column_type[j];
            if ( INT == type )
            {
                int real_data = std::stoi(sql_data);
                row_data.push_back(real_data);
            }
            else if ( DOUBLE == type )
            {
                double real_data = std::stof(sql_data);
                row_data.push_back(real_data);
            }
            else if ( STRING == type )
            {
                row_data.push_back(sql_data);
            }
        }
        json_data.push_back(row_data);
    }
    mysql_free_result(result);
    mtx_.unlock();
    return true;
}


void DBMysql::CloseDB()
{
    mysql_close(&mysql_);
}

