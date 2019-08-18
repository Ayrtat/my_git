box.cfg{
    listen = 3310,
}

require('strict').on()

local json = require('json')
local log = require('log')

--error handling
local function error_create (message, err_num, req)
	local result = req:render {text = string.format(message)}
	result.status = err_num
	return result
end


--create simple data base key-value
local my_base = function ()
    box.once('init', function()
        box.schema.create_space('key_value')
        box.space.key_value:format({
            {name = 'key', type = 'string'},
            {name = 'value', type = 'string'}
        })
        box.space.key_value:create_index('primary', {parts = {'key'}})
        box.schema.user.grant('guest', 'read,write,execute', 'universe',
            nil, {if_not_exists = true})
        end
    )
    log.info('Base configured')
end

--HTTP:GET
local function get(req)
	local key = req:stash('id')
	log.info("key = %s", key)
	local result = box.space.key_value:count(key)

	if result == 0 then 
	        log.info("The key doesn't exist")
		return error_create("invalid key\n", 404, req) 
	end
	
	local kr = box.space.key_value:get{key}[1]
	local rr = box.space.key_value:get{key}[2]

	log.info("key = %s, value = %s", kr, rr)
	local resp = req:render {
		text = string.format("key = %s, value = %s\n", kr, rr)
	}
	resp.status = 200
	return resp
end

--HTTP:POST
local function post(req)
	--get body
	local ok, body = pcall(function() return req:json() end)

	if ok == false then 
		log.info("Not supported format of data")
		return error_create("wrong body\n", 400, req)
	end


	local size_of_body = 0
	local key_loop, val_loop
	
	for key_loop, val_loop in pairs(body) do
		size_of_body = size_of_body + 1
	end
	
	--The body may be right formatted but inappropriate
	if(size_of_body ~= 2 or body['key'] == nil or body['value'] == nil) then
		log.info("Not supported format of data")
		return error_create("wrong body\n", 400, req)
	end

	local key = tostring(body['key'])
   	local value = tostring(json.encode(body['value']))

	local key_exist = box.space.key_value:count(key)
	if key_exist ~= 0 then
		log.info("Wrong key")
		return error_create("wrong key\n", 409, req)
	end

	box.space.key_value:insert{key,value}

	log.info("Pair has been inserted")

	local result = req:render {
		text = string.format("The entry has been inserted\n")
	} result.status = 200
	
	return result
end

--HTTP:DELETE
local function del(req)
	local key = req:stash('id')
	log.info("delete entry with key = %s", key)
	local key_exist = box.space.key_value:count(key)
	if(key_exist == 0) then
		log.info("Wrong key")
		return error_create("The key doesn't exist\n", 404, req)
	end
	box.space.key_value:delete(key)
	
	local resp = req:render {
		text = string.format("Entry has been deleted\n")
		} resp.status = 200
	return resp

end

--HTTP:PUT
local function put(req)
	--:id
	local key = req:stash('id')

	--get body
	local ok, body = pcall(function() return req:json() end)

	if ok == false then 
		log.info("Not supported format of data")
		return error_create("wrong body\n", 400, req)
	end

	local size_of_body = 0
	local key_loop, val_loop
	--Body may be right formatted but inappropriate
	for key_loop, val_loop in pairs(body) do
		size_of_body = size_of_body + 1
	end

	if(size_of_body ~= 1 or body['value'] == nil) then
		log.info("Not supported format of data")
		return error_create("wrong body\n", 400, req)
	end

   	local value = tostring(json.encode(body['value']))


	log.info("update entry with key = %s", key)
	
	local key_exist = box.space.key_value:count(key)
	if(key_exist == 0) then
		log.info("Wrong key")
		return error_create("The key doesn't exist\n", 404, req)
	end

	box.space.key_value:update(key, {{'=',2,value}})
	log.info("Updating")
	local resp = req:render {
		text = string.format("Entry has been updated\n")
		} resp.status = 200
	return resp

end


local function start_server()
    local port = os.getenv('PORT')
    if port == nil then
        port = 8080
    end
    local server = require('http.server').new(nil, port)
    server:route({ path = '/kv/:id', method = 'GET' }, get)
    server:route({ path = '/kv', method = 'POST' }, post)
    server:route({ path = '/kv/:id', method = 'DELETE' }, del)
    server:route({ path = '/kv/:id', method = 'PUT' }, put)
    server:start()
    log.info('Server started on port = %d', port)
end



my_base()
start_server()


