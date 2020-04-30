# get unspent tx

## 1. getutxo 

## params:

1, ["address","amount"]
2, ["address","amount"]
   ......  
n, ["address","amount"]

## example: 

curl --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method": "getutxo", "params": [["1A9eutkvZoQdNVevWGMbxwYNyQ5xtErjrc",0.000015]] }' -H 'content-type: text/plain;' http://127.0.0.1:8666/

## result:

{"error":null,"id":"curltest","result":{"1A9eutkvZoQdNVevWGMbxwYNyQ5xtErjrc":[["00ee4f1c06a9d9eb96efaa010a686b0fc75cd54dd137b9a4ea146010e58a741c",2,"0.000006"],["2964363a9d0f8c430eef2d8151ed048cdcf3fbb55e1fa37d74bc19b094704088",0,"0.000006"],["8cac9a03de22471e2ad319f575905e2c6f72315938f4f779d3b27d50bdb6d280",1,"0.000005"]]}}




## 2.  gettokenutxo

## params:

1, ["pubkey"]

## example:

curl --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method": "gettokenutxo", "params": ["0e406c10d0315942e442946661a0931ce7181fab"] }' -H 'content-type: text/plain;' http://127.0.0.1:8666/

## result:

{"error":null,"id":"curltest","result":[["01c2b75e7e35ffb41965dfd458115fc2234afd232e5ac00c93d784c9829a87d8",0,"2.8e-05","76a9140e406c10d0315942e442946661a0931ce7181fab88ac6a06534c502b2b0001010453454e44207ee7a38340fd4fa5a14c9f5f3dc47f1e68a9534af5b17d43ba92dc0cdadda2b20800005af3107a4000"]]}
