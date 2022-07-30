echo "\033[31m[ MAIN TEST ]\033[0m"

# GET - root : OK
echo "\033[36m GET - root : OK \033[0m"
curl -X GET http://localhost:9090

# GET - root , Host:"" : 400 Bad Request
echo "\n\033[36m GET - Host: \"\" : 400, Bad Request \033[0m"
curl -X GET http://localhost:9090/ -H "Host: "

# GET - root , check file permission : 200 / 403
echo "\033[36m GET - check file permission : 200 / 403 \033[0m"
curl -X GET http://localhost:9090/index.html

# GET - root , wrong file : 404 not found
echo "\n\033[36m GET - wrong path : 404, not found \033[0m"
curl -X GET http://localhost:9090/aa

# POST/PUT - root : 405, Method Not Allowed
echo "\033[36m POST - root : 405, Method Not Allowed \033[0m"
curl -X POST http://localhost:9090

# HEAD - root
echo "\033[36m HEAD - root \033[0m"
curl -I HEAD http://localhost:9090


# DELETE
echo "\033[36m DELETE - root : 405, Method Not Allowed \033[0m"
curl -X DELETE http://localhost:9090/

# POST - post_body : OK
echo "\033[36m POST - post_body \"1234567890\" : OK \033[0m"
curl -X POST http://localhost:9090/post_body -d "1234567890"

# POST/PUT - post_body : 413, Payload Too Large
echo "\033[36m POST - post_body \"12345678901\" : 413, Payload Too Large \033[0m"
curl -X POST http://localhost:9090/post_body -d "12345678901"

# POST - post_body : 415, Unsupported Media Type
echo "\033[36m POST - post_body \"Content-Type: asd\" : 415, Unsupported Media Type \033[0m"
curl -X POST http://localhost:9090/post_body -H "Content-Type: asd"

echo "\033[31m[ REDIRECT TEST ]\033[0m"

echo "\033[36m GET location: /naver \033[0m"
curl -X GET http://localhost:9090/naver

echo "\033[31m[ SESSION TEST ]\033[0m"

# GET
echo "\033[36m GET - session 1 : 401 \033[0m"
curl -X GET http://localhost:9090/session -H "Cookie: session_id=1"

# POST
echo "\033[36m POST - session 1 : 200 \033[0m"
curl -X POST http://localhost:9090/session -H "Secret-Key: hellowebserv" -H "Name: yeju"

# GET
echo "\033[36m GET - session 1 : 200 \033[0m"
curl -X GET http://localhost:9090/session -H "Cookie: session_id=1"

# GET
echo "\033[36m GET - session 1 : 200, hit point++ \033[0m"
curl -X GET http://localhost:9090/session -H "Cookie: session_id=1"

# POST
echo "\033[36m POST - session 2 , wrong secret-key : 401 \033[0m"
curl -X POST http://localhost:9090/session -H "Secret-Key: hello" -H "Name: yeju"

# GET
echo "\033[36m GET - session 2 : 401 \033[0m"
curl -X GET http://localhost:9090/session -H "Cookie: session_id=2"

# DELETE
echo "\033[36m DELETE - session 1 : 200 \033[0m"
curl -X DELETE http://localhost:9090/session -H "Secret-Key: hello" -H "Cookie: session_id=1"

# GET
echo "\033[36m GET - session 1 : 401 \033[0m"
curl -X GET http://localhost:9090/session -H "Cookie: session_id=1"

echo "\033[31m[ PORT TEST ]\033[0m"

echo "\033[36m POST - post_body : 405, Method Not Allowed \033[0m"
curl -X POST http://localhost:9191/post_body

echo "\033[36m POST - session off 1 \033[0m"
curl -X POST http://localhost:9191/session -H "Secret-Key: hellowebserv" -H "Name: yeju" -d "0987654321"

echo "\033[36m GET - session off 1 \033[0m"
curl -X GET http://localhost:9191/session -H "Cookie: session_id=1"

echo "\n\033[36m GET location: /naver \033[0m"
curl -X GET http://localhost:9191/naver

# autoindex
echo "\033[31m[ AUTOINDEX TEST ]\033[0m"

echo "\033[36m GET location: /autoindex \033[0m"
curl -X GET http://localhost:9191/autoindex

echo "\033[36m GET location: /none-autoindex \033[0m"
curl -X GET http://localhost:9191/none-autoindex

echo "\n\033[31m[ TEST ]\033[0m"

echo "\033[36m server name \033[0m"
curl --resolve example.com:80:127.0.0.1 http://example.com/

echo "\n\033[31m[ LISTEN ]\033[0m"
lsof -Pni4 | grep LISTEN


#DELETE

curl -X DELETE localhost:9090/crab.jpeg