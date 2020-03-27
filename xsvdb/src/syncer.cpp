#include "syncer.h"
#include <glog/logging.h>
#include "db_mysql.h"

static void SetTimeout(const std::string& name, int second)
{
    struct timeval timeout ;
    timeout.tv_sec = second;
    timeout.tv_usec = 0;
    evtimer_add(Job::map_name_event_[name], &timeout);
}

static void ScanChain(int fd, short kind, void *ctx)
{
	LOG(INFO) << "scan block begin ";
    Syncer::instance().scanBlockChain(); 
    SetTimeout("ScanChain", 120);
}

static void ScanMempool(int fd, short kind, void *ctx)
{
	LOG(INFO) << "scan mempool";
	Syncer::instance().scanMempool();
    SetTimeout("ScanMempool", 10);
}

void Syncer::appendBlockSql(const json& json_block, const uint64_t& height, std::vector<std::string>& vect_txid)
{
	uint64_t timestamps = json_block["result"]["time"].get<uint64_t>();

	for(int i = 0; i < json_block["result"]["tx"].size(); i++)
	{
		vect_txid.push_back(json_block["result"]["tx"][i].get<std::string>());
	}
	std::string hash = json_block["result"]["hash"].get<std::string>();
	//INSERT INTO `xsvdb`.`block` (`height`, `timestamps`) VALUES ('23', '123123');
	std::string sql = "INSERT INTO `block` (`hash`, `height`, `timestamps`) VALUES ('" + hash +
	   				  "','" + std::to_string(height) + "','" + std::to_string(timestamps) + "');";

	vect_sql_.push_back(sql);
}

void Syncer::appendTxVinVoutSql(const json& json_tx, const std::string& txid)
{
	json json_vins = json_tx["result"]["vin"];
	json json_vouts = json_tx["result"]["vout"];
	std::string sql_vin  = "INSERT INTO `vin` (`txid`, `prevtxid`, `n`) VALUES ";
	bool vin = false;
	json json_vin;
	std::string prev_txid;
	int n = 0;
	for (int i = 0; i < json_vins.size(); i ++)
	{
		json_vin = json_vins[i];
		if (json_vin.find("txid") != json_vin.end())
		{
			prev_txid = json_vin["txid"].get<std::string>();
			n = json_vin["vout"].get<int>();
			vin = true;	 	
		}
		else
		{
			continue;
		}
		
		std::string sql = sql_vin + "('" + txid + "','" + prev_txid +"','" + std::to_string(n) +"');";
		vect_sql_.push_back(sql);
	}

	std::string address;
	double value = 0.0;
	json json_vout;
	for (int i = 0; i < json_vouts.size(); i++)
	{
		json_vout = json_vouts[i];
		if (json_vout["scriptPubKey"].find("addresses") != json_vout["scriptPubKey"].end())
		{
			std::string sql_prefix = "INSERT INTO `voutaddress` (`txid`, `n`, `address`, `addresspos`, `value`) VALUES ";
			n = json_vout["n"].get<int>();
			value = json_vout["value"].get<double>();
			for(int j = 0; j < json_vout["scriptPubKey"]["addresses"].size(); j++)
			{
				std::string sql = sql_prefix + "('" + txid + "','" + std::to_string(n) + "','" + 
								  json_vout["scriptPubKey"]["addresses"][j].get<std::string>() + "','" + std::to_string(j) + "','" + std::to_string(value) + "');";
				vect_sql_.push_back(sql);
			}	
		}
		else
		{
			n = json_vout["n"].get<int>();
			std::string ret_data = json_vout["scriptPubKey"]["asm"].get<std::string>();
			std::string sql = "INSERT INTO `voutret` (`txid`, `n`, `data`) VALUES ('" + 
							  txid + "','" + std::to_string(n) + "','" + ret_data +"');";

			vect_sql_.push_back(sql);
		}
	}
}

void Syncer::refreshDB()
{
	g_db_mysql->batchRefreshDB(vect_sql_);
	vect_sql_.clear();
}

void Syncer::scanBlockChain()
{
	std::string sql = "select height from block order by height desc limit 1;";
	std::map<int,DBMysql::DataType> map_col_type;
	map_col_type[0] = DBMysql::INT;

	json json_data;
	g_db_mysql->getData(sql, map_col_type, json_data);
	uint64_t pre_height  = 0;
	if (json_data.size() > 0)
	{
		pre_height = json_data[0][0].get<uint64_t>();
	}

	uint64_t cur_height  = 0;
	rpc_.getBlockCount(cur_height);
	
	if (cur_height <= pre_height)
	{
		return ;
	}
	json json_block;
	json json_tx;
	std::vector<std::string> vect_txid;

	for (int i = pre_height + 1; i < cur_height; i++)
	{
		json_block.clear();
		vect_txid.clear();
		rpc_.getBlock(i, json_block);
		appendBlockSql(json_block, i, vect_txid);

		for (uint j = 0; j < vect_txid.size(); j++)
		{
			json_tx.clear();

			std::string sql = "INSERT INTO `chaintx` (`txid`, `height`) VALUES ('" + vect_txid[j] + "','" + std::to_string(i) + "');";
			vect_sql_.push_back(sql);
			rpc_.getRawTransaction(vect_txid[j], json_tx);
			appendTxVinVoutSql(json_tx, vect_txid[j]);
		}
		refreshDB();
	}
}


void Syncer::scanMempool()
{
	uint64_t cur_height  = 0;
    rpc_.getBlockCount(cur_height);
	json json_rawmempool;
	rpc_.getRawMempool(json_rawmempool);
	json json_tx;
	std::vector<std::string> vect_txid = json_rawmempool["result"];
	for (uint i = 0; i < vect_txid.size(); i++)
	{
		std::string sql = "INSERT INTO `mempooltx` (`txid`, `height`) VALUES ('" + vect_txid[i] + "','" + std::to_string(cur_height) + ");";
		vect_sql_.push_back(sql);
		rpc_.getRawTransaction(vect_txid[i],json_tx);
		appendTxVinVoutSql(json_tx, vect_txid[i]);	
	}

	refreshDB();
}

Syncer Syncer::single_;
void Syncer::registerTask(map_event_t& name_events, map_job_t& name_tasks)
{
    REFLEX_TASK(ScanChain);
	REFLEX_TASK(ScanMempool);
}


