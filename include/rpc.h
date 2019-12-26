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
    std::vector<Vin> vins;
    std::vector<Vout> vouts;
    uint64_t height;
    bool data;
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
    bool structRpcMethodParams(const std::string method, const json& params, json& post);
};

#endif // RPC_H

