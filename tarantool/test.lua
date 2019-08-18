require'strict'.on()

local http_client = require('http.client')
local tap = require('tap')

local port = os.getenv('PORT')
if port == nil then
    port = 8080
end

local URI = string.format("localhost:%d", port)

test_get = tap.test("GET")
test_get:plan(2)
test_get:test("THE GET SHOULD BE SUCCESS", function(test)
    test:plan(1)
    http_client.post(URI .. "/kv", '{"key":"test", "value":"1"}')
    local res = http_client.get(URI .. "/kv/test")
    http_client.delete(URI .. "/kv/test")
    test:is(res.status, 200)
end)

test_get:test("THE GET SHOULD BE WRONG", function(test)
    test:plan(1)
    http_client.delete(URI .. "/kv/test")
    local res = http_client.get(URI .. "/kv/test")
    test:is(res.status, 404)
end)

test_get:check()

test_post = tap.test("POST")
test_post:plan(3)
test_post:test("THE POST SHOULD BE SUCCESS", function(test)
    test:plan(1)
    local res = http_client.post(URI .. "/kv", '{"key":"test", "value":"1"}')
    http_client.delete(URI .. "/kv/test")
    test:is(res.status, 200)
end)

test_post:test("THE POST SHOULD BE WRONG (BODY)", function(test)
    test:plan(4)
    local resp1 = http_client.post(URI .. "/kv", '{"key":"1"')
    test:is(resp1.status, 400)
    local resp2 = 
         http_client.post(URI .. "/kv",'{"key":"test", "value":"1", "extra":1}')
    test:is(resp2.status, 400)
    local resp_no_key = 
        http_client.post(URI .. "/kv",'{"value":"1"}')
    test:is(resp_no_key.status, 400)
    local resp3 =
        http_client.post(URI .. "/kv",'{"key":"test"}')
    test:is(resp_no_key.status, 400)
end)

test_post:test("THE POST SHOULD BE WRONG (KEY)", function(test)
    test:plan(1)
    http_client.post(URI .. "/kv", '{"key":"test", "value":"1"}')
    local resp = http_client.post(URI .. "/kv", '{"key":"test", "value":"1"}')
    test:is(resp.status, 409)
    http_client.delete(URI .. "/kv/test")
end)

test_post:check()

test_delete = tap.test("DELETE")
test_delete:plan(2)
test_delete:test("THE DELETE SHOULD BE SUCCESS", function(test)
    test:plan(1)
    http_client.post(URI .. "/kv", '{"key":"test", "value":"1"}')
    local resp = http_client.delete(URI .. "/kv/test")
    test:is(resp.status, 200)
end)

test_delete:test("THE DELETE SHOULD BE WRONG (KEY)", function(test)
    test:plan(1)
    http_client.delete(URI .. "/kv/test")
    local resp = http_client.delete(URI .. "/kv/test")
    test:is(resp.status, 404)
end)

test_delete:check()

test_put = tap.test("PUT")
test_put:plan(3)
test_put:test("THE PUT SHOULD BE SUCCESS", function(test)
    test:plan(1)
    http_client.post(URI .. "/kv", '{"key":"test", "value":"1"}')
    local resp = http_client.put(URI .. "/kv/test", '{"value":"2"}')
    http_client.delete(URI .. "/kv/test")
    test:is(resp.status, 200)
end)

test_put:test("THE PUT SHOULD BE WRONG (KEY)", function(test)
    test:plan(1)
    http_client.delete(URI .. "/kv/test")
    local resp = http_client.put(URI .. "/kv/test", '{"value":2}')
    test:is(resp.status, 404)
end)

test_put:test("THE PUT SHOULD BE WRONG (BODY)", function(test)
    test:plan(3)
    http_client.post(URI .. "/kv", '{"key":"test", "value":"1"}')
    local resp1 = http_client.put(URI .. "/kv/test", '{"value":"1"')
    test:is(resp1.status, 400)
    local resp2 = 
         http_client.put(URI .. "/kv/test",'{"value":"1", "extra":1}')
    test:is(resp2.status, 400)
    local resp3 =
        http_client.put(URI .. "/kv/test",'{"key":"test"}')
    test:is(resp3.status, 400)
    http_client.delete(URI .. "/kv/test")
end)

test_put:check()

os.exit(0)
