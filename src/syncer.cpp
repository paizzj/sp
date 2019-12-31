#include <thread>
#include "common.h"
#include "syncer.h"

CSyncer g_syncer;

bool exist(const std::vector<std::string> vec, const std::string txid)
{
  for (int i = 0; i < vec.size(); ++i) {
    if (txid == vec.at(i))
      return true;
  }
  return false;
}

void remove(std::vector<std::string>& vec, std::string txid)
{
  std::vector<std::string>::iterator it = vec.begin();
  while (it != vec.end()) {
    if (*it == txid) {
      it = vec.erase(it);
      break;
    }
  }
}

void CSyncer::syncBlocks(uint64_t height, std::vector<std::string>& pools)
{
  Rpc *rpc = new Rpc();
  uint64_t node_height = 0;
  std::string hash = "";
  std::vector<std::string> txids;
  std::vector<Tx> txs;
  std::vector<std::string> updates;

  std::cout << "pools.size = " << pools.size() << std::endl;

  while (true) {
    std::cout << "height = " << height << std::endl;
    std::cout << "node_height = " << node_height << std::endl;

    if (height >= node_height) {
      if (!rpc->getBlockCount(node_height))
        break;

      usleep(3 * 1000 * 1000);

      if (height == node_height) {
        txids.clear();
        if (!rpc->getMempool(txids))
          break;

        if (txids.size() > pools.size()) {
          if (!handlePoolTxs(rpc, pools, txids)) {
            break;
          }
        }
        continue;
      }
    }

    height++;
    if (!rpc->getBlockHash(height, hash))
      break;

    txids.clear();
    if (!rpc->getBlock(hash, txids))
      break;

    // ignore coinbase tx
    if (txids.size() > 1) {
      for (int i = 1; i < txids.size(); ++i) {
        if (pools.size() > 0 && exist(pools, txids.at(i))) {
          updates.push_back(txids.at(i));
        } else {

          Tx tx;
          tx.txid = txids.at(i);
          tx.height = height;
          if (!rpc->getTransaction(txids.at(i), tx))
	    return;
			
	  txs.push_back(tx);
        }
      }		
    }

    if (txs.size() > 19 || updates.size() > 0 || height % 100 == 0 || height >= node_height) {
      if (!flushTxToDB(txs, height))
        break;

      if (!updateDB(updates, height))
        break;

      for (int i = 0; i < updates.size(); ++i) {
        remove(pools, updates.at(i));
      }

      LOG(INFO) << "scan to " << height;
      std::cout << "scan to " << height << std::endl;
      txs.clear();
      updates.clear();
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
      sql = "INSERT INTO tx (sender,recipient,txid,height) VALUES(\"" + tx.sender + "\",\""
          + tx.vouts.at(0).address + "\",\"" + tx.txid  + "\"," + std::to_string(tx.height) + ")";
      sqls.push_back(sql);
    }
   
    for (int j = 0; j < tx.vouts.size(); ++j) {
      Vout vout = tx.vouts.at(j);
      sql = "INSERT INTO utxo (address,txid,vout,amount,script,height) VALUES(\"" 
	  + vout.address + "\",\"" + tx.txid + "\"," + std::to_string(vout.out) + ",\"" + vout.amount + "\",\"" 
          + vout.script + "\"," + std::to_string(tx.height) + ")";
      sqls.push_back(sql);
    }

    for (int k = 0; k < tx.vins.size(); ++k) {
      Vin vin = tx.vins.at(k);
      sql = "DELETE FROM utxo WHERE vout=\"" + std::to_string(vin.n) + "\" AND txid=\"" + vin.txid + "\"";
      sqls.push_back(sql);
    }
  }		

  if (height > 0) {
    sql = "UPDATE block SET blockcount=" + std::to_string(height);
    sqls.push_back(sql);
  }
  if (sqls.size() == 0)
    return true;

  return g_db_mysql->BatchExecSql(sqls);
}

bool CSyncer::updateDB(std::vector<std::string>& txs, uint64_t height)
{
  if (txs.size() == 0)
    return true;

  std::vector<std::string> sqls;
  std::string sql = "";
  for (int i = 0; i < txs.size(); ++i) {
    sql = "update tx set height=" + std::to_string(height) + " where txid=\"" + txs.at(i)  + "\"";
    sqls.push_back(sql);
    sql = "update utxo set height=" + std::to_string(height) + " where txid=\"" + txs.at(i)  + "\"";
    sqls.push_back(sql);
  }

  return g_db_mysql->BatchExecSql(sqls);
}

bool CSyncer::handlePoolTxs(Rpc *rpc, std::vector<std::string>& pools, std::vector<std::string>& txids)
{
  std::vector<Tx> txs;
  for (int i = 0; i < txids.size(); ++i) {
    if (!exist(pools, txids.at(i))) {
      Tx tx;
      tx.txid = txids.at(i);
      tx.height = 0;

      if (!rpc->getTransaction(txids.at(i), tx))
        return false;

      txs.push_back(tx);
      pools.push_back(txids.at(i));
    }
  }

  if (!flushTxToDB(txs, 0))
    return false;

  return true;
}
