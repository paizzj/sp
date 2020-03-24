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
    it++;
  }
}

void CSyncer::syncBlocks(uint64_t height, std::vector<std::string>& pools)
{
  Rpc *rpc = new Rpc();
  uint64_t node_height = 0;
  std::string hash = "";
  std::vector<std::string> txids;
  std::vector<Tx> txs;

  while (true) {
    if (height >= node_height) {
      if (!rpc->getBlockCount(node_height)) {
        goto loop;
      }

    std::cout << "height = " << height << std::endl;
    std::cout << "node_height = " << node_height << std::endl;
LOG(INFO) << "height = " << height;
LOG(INFO) << "node_height = " << node_height;

      usleep(3 * 1000 * 1000);
      continue;
    }

    height++;
    if (!rpc->getBlockHash(height, hash)) {
      height--;
      goto loop;
    }

    txids.clear();
    if (!rpc->getBlock(hash, txids)) {
      goto loop;
    }

    for (int i = 0; i < txids.size(); ++i) {
      Tx tx;
      tx.txid = txids.at(i);
      tx.height = height;
      tx.data = false;
      tx.index = i;
      if (!rpc->getTransaction(txids.at(i), tx)) {
        goto loop;
      }
      txs.push_back(tx);
    }

    std::cout << "height = " << height << std::endl;
    std::cout << "node_height = " << node_height << std::endl;
LOG(INFO) << "height = " << height;
LOG(INFO) << "node_height = " << node_height;

    if (txs.size() > 9 || height >= node_height) {
      if (!flushTxToDB(txs, height)) {
	goto loop;
      }

LOG(INFO) << "scan to " << height;
std::cout << "scan to " << height << std::endl;
      txs.clear();
    }

    continue;

loop:
    usleep(3 * 1000 * 1000);
  }
}

bool CSyncer::flushTxToDB(std::vector<Tx>& txs, uint64_t height)
{
  std::vector<std::string> sqls;
  std::string sql = "";

  for (int i = 0; i < txs.size(); ++i) {
    Tx tx = txs.at(i);
    sql = "INSERT INTO history (txid,height,idx,tx_size,time) VALUES (\"" + tx.txid + "\"," + std::to_string(tx.height)
	+ "," + std::to_string(tx.index) + "," + std::to_string(tx.size) + "," + std::to_string(tx.time) + ")";
    sqls.push_back(sql);

    if (tx.data) {
      sql = "INSERT INTO tx (address,txid,height,data) VALUES (\"" + tx.sender + "\",\""
          + tx.txid  + "\"," + std::to_string(tx.height) + ",\"" + tx.text + "\")";
      sqls.push_back(sql);
    }
   
    for (int j = 0; j < tx.vouts.size(); ++j) {
      Vout vout = tx.vouts.at(j);
      int cb = tx.index > 0 ? 0 : 1;
      if (vout.data.empty() && vout.amount != "0") {
          sql = "INSERT INTO utxo (address,txid,vout,amount,height,coinbase,spent) VALUES (\""
	      + vout.address + "\",\"" + tx.txid + "\"," + std::to_string(vout.n) + ",\"" + vout.amount + "\","
              + std::to_string(tx.height) + "," + std::to_string(cb) + ",0)";
          sqls.push_back(sql);
      }

      sql = "INSERT INTO vouts (txid,address,n,value,data) VALUES (\"" + tx.txid + "\",\"" + vout.address + "\","
          + std::to_string(vout.n) + ",\"" + vout.amount + "\",\""+ vout.data + "\")";
      sqls.push_back(sql);
    }

    for (int k = 0; k < tx.vins.size(); ++k) {
      Vin vin = tx.vins.at(k);
//      sql = "UPDATE utxo SET spent=1 WHERE vout=" + std::to_string(vin.n) + " AND txid=\"" + vin.txid + "\"";
      sql = "DELETE from utxo WHERE txid=\"" + vin.txid + "\" AND vout=" + std::to_string(vin.vout);
      sqls.push_back(sql);

      sql = "INSERT INTO vins (txid,i,hash,vout) VALUES (\"" + tx.txid + "\"," + std::to_string(vin.i) + ",\""
	  + vin.txid + "\"," + std::to_string(vin.vout) + ")";
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
