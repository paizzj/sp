#include <rpc/server.h>
#include <utilstrencodings.h>
#include <rpc/register.h>
#include "db_mysql.h"

std::vector<std::string> vectFileSendTx;

json downloadnote(const JSONRPCRequest& request)
{
	if (request.fHelp || request.params.size() != 1) {
		throw std::runtime_error(
				"\nArguments:\n"
				"1. \"file name\" (string)  The name of file from downloading success\n"
				);
	}

	std::string file_name = request.params[0].get<std::string>();

	json json_result;
	json_result["name"] = file_name;
    json_result["status"] = true;
	vectFileSendTx.push_back(file_name);

    return json_result;
}

json getutxo(const JSONRPCRequest& request)
{
	if (request.fHelp || request.params.size() < 1) {
		throw std::runtime_error(
				"\n Arguments:\n "
				"1. \"address\" (string) \n "
				"2. \"address\" (string) \n "
				"......\n "	
				"n. \"address\" (string) \n "
				);
	}

    json json_db;
    json_db["db"] = "xsvdb";
    json_db["user"] = "root";
    json_db["pass"] = "root890*()";
    json_db["url"] = "127.0.0.1";
    json_db["port"] = 3306;
    g_db_mysql->openDB(json_db);

	
	std::string address;
	uint address_size = request.params.size();
	

	std::map<int, DBMysql::DataType> col_type;
	col_type[0] = DBMysql::STRING;
	col_type[1] = DBMysql::INT;
	col_type[2] = DBMysql::STRING;
	json json_result;
	for (uint i = 0; i < address_size; ++i)
	{
		json json_data;
		address = request.params[i].get<std::string>();
		std::string sql = "SELECT txid, n, value FROM utxo WHERE address = '" + address + "';";
		g_db_mysql->getData(sql, col_type, json_data);
		json_result[address] = json_data;
	}

	g_db_mysql->closeDB();
	return json_result;
}





static const CRPCCommand commands[] =
{ //  category              name                      actor (function)         argNames
    //  --------------------- ------------------------  -----------------------  ----------
    { "relay",            "downloadnote",           &downloadnote,           {} },
    { "relay",            "getutxo",           		&getutxo,           	 {} },
};


void RegisterRelayRPCCommands(CRPCTable &t)
{
    for (unsigned int vcidx = 0; vcidx < ARRAYLEN(commands); vcidx++)
        t.appendCommand(commands[vcidx].name, &commands[vcidx]);
}
