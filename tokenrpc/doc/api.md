# token create 

## api:

 creategenesistoken 

## params:

"1, "shortname" (string) "
"2, "fullname" (string) "
"3, "tokenurl" (string) " 
"4, "whitePaperHash" (string) " 
"5, "precision" (int) "
 "6, "batonvout" (int) "
 "7, "total" (int) "

## example: 

curl --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method":  "creategenesistoken", "params": ["USDT","usdt into", "www baidu.com",  "d3dab4264e640b268bf746b7c4912236c84c9b9809129969b96765df638a1b10",8,1,1000000000] }' -H 'content-type: text/plain;' http://127.0.0.1:6666/ 

## result:

 {"error":null,"id":"curltest","result":{"hex":"03534c50000847454e45534953450455534454097573647420696e746f0d7777772062616964752e636f6d4064336461623432363465363430623236386266373436623763343931323233366338346339623938303931323939363962393637363564663633386131623130080100ca9a3b00000000"}}

# token mint

##  api:

 minttoken 

## params: 

"1, "tokenId" (string) " 
"2, "precision" (int) " 
"3, "tokenVout" (int) " 
"4, "tokenMintTotal" (int) "

## example: 

curl --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method":  "minttoken", "params":  ["d3dab4264e640b268bf746b7c4912236c84c9b9809129969b96765df638a1b10",8,2,1000000] }' -H 'content-type: text/plain;' http://127.0.0.1:6666/ 

## result:

 {"error":null,"id":"curltest","result":{"hex":"03534c5002046d696e744064336461623432363465363430623236386266373436623763343931323233366338346339623938303931323939363962393637363564663633386131623130080240420f0000000000"}}

# token decode 

## api: 

decodetoken 

## params: 

"1. "tokentype" (string) " 
"2, "tokenhex" (string) "

## example: 

curl --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method":  "decodetoken", "params":  ["genesis","03534c50000847454e45534953450455534454097573647420696e746f0d7777772062616964752e636f6d4064336461623432363465363430623236386266373436623763343931323233366338346339623938303931323939363962393637363564663633386131623130080100ca9a3b00000000"] }' -H 'content-type: text/plain;' http://127.0.0.1:6666/ 

## result:

 {"error":null,"id":"curltest","result":{"baton_vout":1,"full_name":"usdt  into","hash":"d3dab4264e640b268bf746b7c4912236c84c9b9809129969b96765df638a1b10","precision":8,"protocol":"SLP","short_name":"USDT","token_url":"www baidu.com","total":1000000000,"type":"GENESISE"}}

# token send 

## api: 

sendtoken 

## params: 

"1. "tokenId" (string) " 
"2, "tokenSendInfo" (array of array [precision, baton_vout, token_amount]) "

## example: 

curl --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method":  "sendtoken", "params":  ["d3dab4264e640b268bf746b7c4912236c84c9b9809129969b96765df638a1b10",[[8,0,100]]] }' -H 'content-type: text/plain;' http://127.0.0.1:6666/ 

## result: 

{"error":null,"id":"curltest","result":{"hex":"03534c50010473656e64406433646162343236346536343062323638626637343662376334393132323336633834633962393830393132393936396239363736356466363338613162313001080100016400000000000000"}}