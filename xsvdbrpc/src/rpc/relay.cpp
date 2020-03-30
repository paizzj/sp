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
	if (request.fHelp || request.params.size() != 1) {
		throw std::runtime_error(
				"\nArguments:\n"
				"1. \"address\" (string) \n"
				);
	}
	
	std::string address = request.params[0].get<std::string>();
	
	std::map<int, DBMysql::DataType> col_type;
	col_type[0] = DBMysql::STRING;
	col_type[1] = DBMysql::INT;
	col_type[2] = DBMysql::DOUBLE;
	 

	json json_result;
	std::string sql = "SELECT txid, n, value FROM utxo WHERE address = '" + address + "';";
	g_db_mysql->getData(sql, col_type, json_result);

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
