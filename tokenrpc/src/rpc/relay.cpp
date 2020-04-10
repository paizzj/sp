#include <rpc/server.h>
#include <utilstrencodings.h>
#include <rpc/register.h>
#include "db_mysql.h"
#include "token.h"
#include "streams.h"
#include <iostream>
std::vector<std::string> vectFileSendTx;

json creategenesistoken(const JSONRPCRequest& request)
{
	if (request.fHelp || request.params.size() != 7) {
        throw std::runtime_error(
                "\n Arguments:\n "
                "1. \"shortname\" (string) \n "
				"2, \"fullname\" (string) \n "
				"3, \"tokenurl\" (string) \n "
				"4, \"whitePaperHash\" (string) \n "
				"5, \"precsion\" (int) \n "
				"6, \"batonvout\" (int) \n "
				"7, \"total\" (int) \n "
                );
    }
	
	GenesisToken  token;
	token.strProtocol = "SLP";
    token.strTokenType = "GENESISE";
	token.chTokenType = 00;
    token.strShortName = request.params[0].get<std::string>();
    token.strFullName = request.params[1].get<std::string>();
    token.strTokenUrl = request.params[2].get<std::string>();
    token.strWhitePaperHash = request.params[3].get<std::string>();
   	token.chPrecision  = request.params[4].get<int>();
    token.chBatonVout = request.params[5].get<int>();
    token.nTotal = request.params[6].get<uint64_t>();

	CDataStream ssToken(SER_DISK, PROTOCOL_VERSION);
	ssToken << token;
	std::string strHex = HexStr(ssToken.begin(), ssToken.end());
	std::cout << strHex << std::endl;
	json json_result;
	json_result["hex"] = strHex;
    return json_result;
}

json decodetoken(const JSONRPCRequest& request)
{
	if (request.fHelp || request.params.size() != 2) {
        throw std::runtime_error(
                "\n Arguments:\n "
                "1. \"tokentype\" (string) \n "
				"2, \"tokenhex\" (string) \n "
                );
    }
	
 	std::string tokenType = request.params[0].get<std::string>();
	std::string tokenHex = request.params[1].get<std::string>();
	json json_result;
	
	if(tokenType == "genesis")
	{
		GenesisToken token;
		std::vector<unsigned char> tokenData(ParseHex(tokenHex));
    	CDataStream ssToken(tokenData, SER_DISK, PROTOCOL_VERSION);
        ssToken >> token;
		json_result["protocol"] = token.strProtocol;
		json_result["type"] = token.strTokenType;
		json_result["short_name"] = token.strShortName;
		json_result["full_name"] = token.strFullName;
		json_result["token_url"] = token.strTokenUrl;
		json_result["hash"] = token.strWhitePaperHash;
		json_result["precision"] = token.chPrecision;
		json_result["baton_vout"] = token.chBatonVout;
		json_result["total"] = token.nTotal;

	}
	else if(tokenType == "mint")
	{
		MintToken token;
		std::vector<unsigned char> tokenData(ParseHex(tokenHex));
    	CDataStream ssToken(tokenData, SER_DISK, PROTOCOL_VERSION);
        ssToken >> token;
		json_result["protocol"] = token.strProtocol;
		json_result["type"] = token.strTokenType;
		json_result["token_id"] = token.strTokenId;
		json_result["baton_vout"] = token.chBatonVout;
		json_result["mint_total"] = token.nMintTotal;
	
	}
	else if(tokenType == "send")
	{
		SendToken token;
		std::vector<unsigned char> tokenData(ParseHex(tokenHex));
    	CDataStream ssToken(tokenData, SER_DISK, PROTOCOL_VERSION);
        ssToken >> token;
		json_result["protocol"] = token.strProtocol;
		json_result["type"] = token.strTokenType;
		json_result["token_id"] = token.strTokenId;
		json  json_transfers;

		for (uint i = 0; i < token.vectPrecition.size(); i++)
		{
			json json_tmp;
			json_tmp.push_back(token.vectPrecition[i]);
			json_tmp.push_back(token.vectTokenOutput[i]);
			json_tmp.push_back(token.vectTokenAmount[i]);
			
			json_transfers.push_back(json_tmp);
			
		}
/*
		json_result["precition"] = token.vectPrecition;
		json_result["token_output"] = token.vectTokenOutput;
		json_result["token_amount"] = token.vectTokenAmount;*/

		json_result["transfers"] = json_transfers;

	}

	return json_result;
	
}

json sendtoken(const JSONRPCRequest& request)
{
	if (request.fHelp || request.params.size() != 2) {
        throw std::runtime_error(
                "\n Arguments:\n "
                "1. \"tokenId\" (string) \n "
				"2, \"tokenSendInfo\" (array) \n "
                );
    }
	
 	std::string tokenId = request.params[0].get<std::string>();
	json tokenSendInfo = request.params[1];
	SendToken token;
	token.strProtocol = "SLP";
	token.chTokenType = 1;
	token.strTokenType = "send";
	token.strTokenId = tokenId;

	for(uint i =0; i < tokenSendInfo.size(); i++)
	{
		json sendInfo = tokenSendInfo[i];
		token.vectPrecition.push_back(sendInfo[0].get<uint8_t>());
		token.vectTokenOutput.push_back(sendInfo[1].get<uint8_t>());
		token.vectTokenAmount.push_back(sendInfo[2].get<uint64_t>());
	}

	CDataStream ssToken(SER_DISK, PROTOCOL_VERSION);
	ssToken << token;
	std::string strHex = HexStr(ssToken.begin(), ssToken.end());
	std::cout << strHex << std::endl;
	json json_result;
	json_result["hex"] = strHex;
    return json_result;

}

json minttoken(const JSONRPCRequest& request)
{
	if (request.fHelp || request.params.size() != 4) {
        throw std::runtime_error(
                "\n Arguments:\n "
                "1, \"tokenId\" (string) \n "
				"2, \"precition\" (int) \n " 
				"3, \"tokenVout\" (int) \n "
				"4, \"tokenMintTotal\" (int) \n "
                );
    }
	
 	std::string tokenId = request.params[0].get<std::string>();
	uint8_t chPrecision = request.params[1].get<uint8_t>();
	uint8_t chBatonVout = request.params[2].get<uint8_t>();
	uint64_t nMintTotal = request.params[3].get<uint64_t>();
	MintToken token;
	token.strProtocol = "SLP";
	token.chTokenType = 2;
	token.strTokenType = "mint";
	token.strTokenId = tokenId;
	token.nMintTotal = nMintTotal;
	token.chBatonVout = chBatonVout;	
	token.chPrecision = chPrecision;	

	CDataStream ssToken(SER_DISK, PROTOCOL_VERSION);
	ssToken << token;
	std::string strHex = HexStr(ssToken.begin(), ssToken.end());
	std::cout << strHex << std::endl;
	json json_result;
	json_result["hex"] = strHex;
    return json_result;

}


static const CRPCCommand commands[] =
{ //  category              name                      actor (function)         argNames
    //  --------------------- ------------------------  -----------------------  ----------
    { "relay",            "creategenesistoken",     &creategenesistoken,           	 {} },
    { "relay",            "decodetoken",     		&decodetoken,           	 {} },
    { "relay",            "sendtoken",     			&sendtoken,           	 {} },
    { "relay",            "minttoken",     			&minttoken,           	 {} },
};


void RegisterRelayRPCCommands(CRPCTable &t)
{
    for (unsigned int vcidx = 0; vcidx < ARRAYLEN(commands); vcidx++)
        t.appendCommand(commands[vcidx].name, &commands[vcidx]);
}
