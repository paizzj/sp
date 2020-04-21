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
				"1. [\"address\",\"amount\"] \n "
				"2. [\"address\",\"amount\"] \n "
				"......\n "	
				"n. [\"address\", \"amount\"] \n "
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

		json json_address_amount = request.params[i];
		std::string address = json_address_amount[0].get<std::string>();
		double amount = json_address_amount[1].get<double>();
		std::string sql = "SELECT txid, n, value FROM utxo WHERE address = '" + address + "';";
		g_db_mysql->getData(sql, col_type, json_data);
		
		json json_amount;
		double tmp = 0.0;
		if (amount < 0)
		{
			json_result[address] = json_data;
			continue;
		}

		for (uint j = 0; j < json_data.size(); j++)
		{
			tmp += std::atof(json_data[j][2].get<std::string>().c_str());
			json_amount.push_back(json_data[j]);
			if (tmp >= amount)
			{
				json_result[address] = json_amount;
				break;
			}

		}
	}

	g_db_mysql->closeDB();
	return json_result;
}

json gettxidutxo(const JSONRPCRequest& request)
{
	if (request.fHelp || request.params.size() < 1) {
		throw std::runtime_error(
				"\n Arguments:\n "
				"1. [\"address\",\"txid\"] \n "
				"2. [\"address\",\"txid\"] \n "
				"......\n "	
				"n. [\"address\", \"txid\"] \n "
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

		json json_address_txid = request.params[i];
		std::string address = json_address_txid[0].get<std::string>();
		std::string txid = json_address_txid[1].get<std::string>();
		std::string sql = "SELECT txid, n, value FROM utxo WHERE address = '" + address + "',txid= '" + txid + "';";
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
    { "relay",            "gettxidutxo",          &gettxidutxo,          {} },
};


void RegisterRelayRPCCommands(CRPCTable &t)
{
    for (unsigned int vcidx = 0; vcidx < ARRAYLEN(commands); vcidx++)
        t.appendCommand(commands[vcidx].name, &commands[vcidx]);
}
