# BUILD

mkdir bin && cd bin  
cmake .. && make  

# RUN
init db [sql](https://github.com/SimbaBlock/sp/blob/master/xsvdb/doc/xsvdb.sql)  
configure [file](https://github.com/SimbaBlock/sp/blob/master/xsvdb/conf/conf.json)  
./xsvd -c ../conf/conf.json   


