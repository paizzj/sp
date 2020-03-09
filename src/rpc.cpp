#include "rpc.h"

void structRpcMethodParams(const std::string method, const json &params, json &post)
{
    post["jsonrpc"] = "1.0";
    post["id"] = "curltest";
    post["method"] = method;
    post["params"] = params;
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

//LOG(INFO) << "getBlockCount: " << response;
    json json_response = json::parse(response);
    if (json_response["result"].is_null()) {
        LOG(ERROR) << "getBlockCount: " << response;
        return false;
    }

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

//LOG(INFO) << "getBlockHash: " << response;
    json json_response = json::parse(response);
    if (json_response["result"].is_null()) {
        LOG(ERROR) << "getBlockHash: " << response;
        return false;
    }

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

//LOG(INFO) << "getBlock: " << response;
    json json_response = json::parse(response);
    if (json_response["result"].is_null()) {
        LOG(ERROR) << "getBlock: " << response;
        return false;
    }

    json json_result = json_response["result"];
    json json_txs = json_result["tx"];
    for (uint i = 0; i < json_txs.size(); i++)
    {
        txs.push_back(json_txs.at(i).get<std::string>());
    }
    return true;
}

std::string getSenderAddress(const Vin vin)
{
    std::string res = "";
    json json_post;
    json json_params;
    json_params.push_back(vin.txid);
    json_params.push_back(true);
    structRpcMethodParams("getrawtransaction", json_params, json_post);
    std::string response;
    if (!CurlPost(json_post.dump(), response))
    {
        LOG(ERROR) << "getSenderAddress error txid = " << vin.txid;
        return res;
    }

//LOG(INFO) << "getSenderAddress: " << response;
    json json_response = json::parse(response);
    json json_result = json_response["result"];
    json vout = json_result["vout"].at(vin.n);
    res = vout["scriptPubKey"]["addresses"].at(0).get<std::string>();
    return res;
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

//LOG(INFO) << "getTransaction: " << response;
    json json_response = json::parse(response);
    if (json_response["result"].is_null()) {
        LOG(ERROR) << "getTransaction: " << response;
        return false;
    }

    json json_result = json_response["result"];
    json json_vout = json_result["vout"];

    if (!tx.coinbase) {
        json json_vin = json_result["vin"];
        Vin vin;
        for (int i = 0; i < json_vin.size(); ++i) {
            vin.txid = json_vin.at(i)["txid"].get<std::string>();
            vin.n = json_vin.at(i)["vout"].get<int>();
            tx.vins.push_back(vin);
        }
    }

    Vout vout;
    for (int i = 0; i < json_vout.size(); ++i)
    {
        double a = json_vout.at(i)["value"].get<double>();
        std::stringstream ss;
	ss << std::setprecision(8) << a;
	std::string amount = ss.str();
	if (a != 0) {
            std::string address = json_vout.at(i)["scriptPubKey"]["addresses"].at(0).get<std::string>();
 	    vout.out = i;
	    vout.address = address;
	    vout.amount = amount;
	    tx.vouts.push_back(vout);
	} else {
	    if (!tx.coinbase) {
                tx.data = true;
                tx.sender = getSenderAddress(tx.vins.at(0));
	        std::string text = json_vout.at(i)["scriptPubKey"]["hex"].get<std::string>();
	        tx.text = text;
	    }
	}
    }
    return true;
}

bool Rpc::getMempool(std::vector<std::string>& txs)
{
    json json_post;
    json json_params = json::array();
    structRpcMethodParams("getrawmempool", json_params, json_post);
    std::string response;
    if (!CurlPost(json_post.dump(), response))
    {
        LOG(ERROR) << "getMempool error";
        return false;
    }

//LOG(INFO) << "getMempool: " << response;
    json json_response = json::parse(response);
    if (json_response["result"].is_null()) {
        LOG(ERROR) << "getMempool: " << response;
        return false;
    }

    json res = json_response["result"];
    for (int i = 0; i < res.size(); ++i) {
        txs.push_back(res.at(i));
    }
    return true;
}

