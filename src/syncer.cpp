#include <thread>
#include "common.h"
#include "syncer.h"

CSyncer g_syncer;

void CSyncer::syncBlocks(uint64_t height)
{
  Rpc *rpc = new Rpc();
  uint64_t node_height = 0;
  std::string hash = "";
  std::vector<std::string> txids;
  std::vector<Tx> txs;

  while (true) {
    if (height >= node_height) {
      if (!rpc->getBlockCount(node_height))
        break;

      std::cout << "node_height = " << node_height << std::endl;
      usleep(30 * 1000 * 1000);
      continue;	
    }

    height++;
    if (!rpc->getBlockHash(height, hash))
      break;

    txids.clear();
    if (!rpc->getBlock(hash, txids))
      break;

    if (txids.size() > 1) {
      // ignore coinbase tx
      for (int i = 1; i < txids.size(); ++i) {
        Tx tx;
        tx.txid = txids.at(i);
        tx.height = height;

        if (!rpc->getTransaction(txids.at(i), tx))
	    break;
			
	txs.push_back(tx);
      }		
    }

    if (txs.size() > 19 || height % 100 == 0 || height == node_height) {
      if (!flushTxToDB(txs, height)) 
        break;

	LOG(INFO) << "scan to " << height; 
	txs.clear();
      }
    }
}

bool CSyncer::flushTxToDB(std::vector<Tx>& txs, uint64_t height)
{
  std::vector<std::string> sqls;
  std::string sql = "";
  for (int i = 0; i < txs.size(); ++i) {
    Tx tx = txs.at(i);
    if (tx.data) {
      sql = "INSERT INTO tx (address,txid,height) VALUES(\"" + tx.vouts.at(0).address + "\",\"" + tx.txid  + "\"," + std::to_string(tx.height) + ")";
      LOG(INFO) << sql;
      sqls.push_back(sql);
    }
   
    for (int j = 0; j < tx.vouts.size(); ++j) {
      Vout vout = tx.vouts.at(j);
      sql = "INSERT INTO utxo (address,txid,vout,amount,script,height) VALUES(\"" 
	  + vout.address + "\",\"" + tx.txid + "\"," + std::to_string(vout.out) + ",\"" + vout.amount + "\",\"" 
          + vout.script + "\"," + std::to_string(tx.height) + ")";
      LOG(INFO) << sql;
      sqls.push_back(sql);
    }

    for (int k = 0; k < tx.vins.size(); ++k) {
      Vin vin = tx.vins.at(k);
      sql = "DELETE FROM utxo WHERE vout=\"" + std::to_string(vin.n) + "\" AND txid=\"" + vin.txid + "\"";
      LOG(INFO) << sql;
      sqls.push_back(sql);
    }
  }		

  sql = "UPDATE block SET blockcount=" + std::to_string(height);
  sqls.push_back(sql);
  return g_db_mysql->BatchExecSql(sqls);
}

