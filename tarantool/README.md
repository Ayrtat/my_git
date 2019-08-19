# API:
 - POST /kv body: {key: "test", "value": {SOME ARBITRARY JSON}} 
 - PUT kv/{id} body: {"value": {SOME ARBITRARY JSON}}
 - GET kv/{id} 
 - DELETE kv/{id}

 - POST  возвращает 409 если ключ уже существует, 
 - POST, PUT возвращают 400 если боди некорректное
 - PUT, GET, DELETE возвращает 404 если такого ключа нет
 - все операции логируются
 
 RUN SERVER LOCALLY (based on binary package):
 ```/usr/bin/tarantool my.lua```
 
 RUN CLIENT LOCCALY FOR TEST
```/usr/bin/tarantool test.lua```
 
 All results will be stored in ```logger.log``` file
 
###FOR REMOTE CONNECTION ```mcs.mail.ru``` WAS USED
 
A virtual machine has been created with ```tarantool package```. After a server has been run:

```tarantoolctl start /etc/tarantool/instances.enabled/my.lua```

#To connect to server:

```http://85.192.33.85:8080/```
