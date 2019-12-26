#include "rpc.h"

bool Rpc::structRpcMethodParams(const std::string method, const json &params, json &post)
{
    post["jsonrpc"] = "1.0";
    post["id"] = "curltest";
    post["method"] = method;
    post["params"] = params;
    return true;
}

bool Rpc::getBlockCount(uint64_t &height)
{
    json json_post;
    json json_params = json::array();
    structRpcMethodParams("getblockcount", json_params, json_post);
    std::string response;
    if (!CurlPost(json_post.dump(), response))
    {
        LOG(ERROR) << "getBlockCount error";
        return false;
    }

    json json_response = json::parse(response);
    height = json_response["result"].get<uint64_t>();
    return true;
}

bool Rpc::getBlockHash(const uint64_t &height, std::string &hash)
{
    json json_post;
    json json_params = json::array();
    json_params.push_back(height);
    structRpcMethodParams("getblockhash", json_params, json_post);
    std::string response;
    if (!CurlPost(json_post.dump(),response))
    {
        LOG(ERROR) << "getBlockHash error height = " << height;
        return false;
    }

    json json_response = json::parse(response);
    hash = json_response["result"].get<std::string>();
    return true;
}

bool Rpc::getBlock(const std::string &hash, std::vector<std::string> &txs)
{
    json json_post;
    json json_params;
    json_params.push_back(hash);
    structRpcMethodParams("getblock", json_params, json_post);
    std::string response;
    if (!CurlPost(json_post.dump(), response))
    {
        LOG(ERROR) << "getBlock error hash = " << hash;
        return false;
    }

    json json_response = json::parse(response);
    json json_result = json_response["result"];
    json json_txs = json_result["tx"];
    for (uint i = 0; i < json_txs.size(); i++)
    {
        txs.push_back(json_txs.at(i).get<std::string>());
    }
    return true;
}

bool Rpc::getTransaction(const std::string &hash, Tx &tx)
{
    json json_post;
    json json_params;
    json_params.push_back(hash);
    json_params.push_back(true);
    structRpcMethodParams("getrawtransaction", json_params, json_post);
    std::string response;
    if (!CurlPost(json_post.dump(), response))
    {
        LOG(ERROR) << "getTransaction error txid = " << hash;
        return false;
    }

    std::cout << response << std::endl;
    json json_response = json::parse(response);
    json json_result = json_response["result"];
    json json_vout = json_result["vout"];
    json json_vin = json_result["vin"];
    Vin vin;
    for (int i = 0; i < json_vin.size(); ++i) 
    { 
        vin.txid = json_vin.at(i)["txid"].get<std::string>();
        vin.n = json_vin.at(i)["vout"].get<int>();
    }  	
    tx.vins.push_back(vin);

    tx.data = false;
    Vout vout;
    for (int i = 0; i < json_vout.size(); ++i)
    {
        double amount = json_vout.at(i)["value"].get<double>();
	if (amount != 0) {
	    std::string script = json_vout.at(i)["scriptPubKey"]["hex"].get<std::string>();
            std::string address = json_vout.at(i)["scriptPubKey"]["addresses"].at(0).get<std::string>();
 	    vout.out = i;
	    vout.script = script;
	    vout.address = address;
	    vout.amount = std::to_string(amount);
	    tx.vouts.push_back(vout);
	} else {
            if (json_vout.size() == 2) 
                tx.data = true;
	}
    }

    return true;
}

