BUILD

mkdir bin && cd bin
cmake .. && make

RUN
Init db from 'doc/xsvdb.sql'
Edit you configure file in 'conf/conf.json'
cd bin && ./xsvd  or
./xsvd -c <configure file> like this
./xsvd -c ../conf/conf.json


