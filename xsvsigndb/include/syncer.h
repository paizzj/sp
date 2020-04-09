#ifndef SYNCER_H
#define SYNCER_H

#include <vector>
#include <map>
#include <mutex>
#include "rpc.h"

class CSyncer
{
public:
    CSyncer() {}
    ~CSyncer() {}

    void syncBlocks(uint64_t height, std::vector<std::string>& pools);
    bool handlePoolTxs(Rpc *rpc, std::vector<std::string>& pools, std::vector<std::string>& txids);
    bool flushTxToDB(std::vector<Tx>& txs, uint64_t height);
    bool updateDB(std::vector<std::string>& txs, uint64_t height);
};

extern CSyncer g_syncer;

#endif

