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

    void syncBlocks(uint64_t height);

    bool flushTxToDB(std::vector<Tx>& txs, uint64_t height);
};

extern CSyncer g_syncer;

#endif

