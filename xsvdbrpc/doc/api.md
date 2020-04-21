# get unspent tx

## api:

 getutxo 

## params:

1, ["address","amount"]
2, ["address","amount"]
   ......  
n, ["address","amount"]

## example: 

curl --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method": "getutxo", "params": [["1A9eutkvZoQdNVevWGMbxwYNyQ5xtErjrc",0.000015]] }' -H 'content-type: text/plain;' http://127.0.0.1:8666/

## result:

{"error":null,"id":"curltest","result":{"1A9eutkvZoQdNVevWGMbxwYNyQ5xtErjrc":[["00ee4f1c06a9d9eb96efaa010a686b0fc75cd54dd137b9a4ea146010e58a741c",2,"0.000006"],["2964363a9d0f8c430eef2d8151ed048cdcf3fbb55e1fa37d74bc19b094704088",0,"0.000006"],["8cac9a03de22471e2ad319f575905e2c6f72315938f4f779d3b27d50bdb6d280",1,"0.000005"]]}}


## api:

 gettxidutxo 

## params:

1, ["address","txid"]
2, ["address","txid"]
   ......  
n, ["address","txid"]

## example: 

curl --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method": "gettxidutxo", "params": [["1A9eutkvZoQdNVevWGMbxwYNyQ5xtErjrc","00ee4f1c06a9d9eb96efaa010a686b0fc75cd54dd137b9a4ea146010e58a741c"]] }' -H 'content-type: text/plain;' http://127.0.0.1:8666/

## result:

{"error":null,"id":"curltest","result":{"1A9eutkvZoQdNVevWGMbxwYNyQ5xtErjrc":[["00ee4f1c06a9d9eb96efaa010a686b0fc75cd54dd137b9a4ea146010e58a741c",2,"0.000006"]]}}



