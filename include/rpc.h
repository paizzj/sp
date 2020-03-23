#ifndef RPC_H
#define RPC_H

#include "common.h"

struct Vin
{
    std::string txid;
    int n;
};
struct Vout
{
    std::string address;
    std::string script;
    int out;
    std::string amount;
};
struct Tx
{
    std::string txid;
    uint64_t height;
    int index;
    int size;
    long time;
    bool data;
    std::string sender;
    std::string text;
    std::vector<Vin> vins;
    std::vector<Vout> vouts;
    std::string vins_json;
    std::string vouts_json;
};

class Rpc
{
public:
    Rpc() {}
    ~Rpc() {}

    bool getBlockCount(uint64_t& height);
    bool getBlockHash(const uint64_t& height, std::string& hash);
    bool getBlock(const std::string& hash, std::vector<std::string>& txs);
    bool getTransaction(const std::string& hash, Tx& tx);
    bool getMempool(std::vector<std::string>& txs);
};

void structRpcMethodParams(const std::string method, const json& params, json& post);

#endif // RPC_H

