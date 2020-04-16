#include <rpc/server.h>
#include <utilstrencodings.h>
#include <rpc/register.h>
#include "db_mysql.h"
#include <iostream>
std::vector<std::string> vectFileSendTx;

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
    g_db_mysql->openDB(g_json_db);
	
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
    { "relay",            "getutxo",           		&getutxo,           	 {} },
};


void RegisterRelayRPCCommands(CRPCTable &t)
{
    for (unsigned int vcidx = 0; vcidx < ARRAYLEN(commands); vcidx++)
        t.appendCommand(commands[vcidx].name, &commands[vcidx]);
}
