# GET - root : OK
echo "\033[36m GET - root : OK \033[0m"
curl -X GET http://localhost:9090

# POST - post_body : OK
echo "\033[36m POST - post_body : OK \033[0m"
curl -X POST http://localhost:9090/post_body -d "1234567890"

# POST/PUT - root : 405, Method Not Allowed
echo "\033[36m POST - root : 405, Method Not Allowed \033[0m"
curl -X POST http://localhost:9090

# POST/PUT - post_body : 413, Payload Too Large
echo "\033[36m POST - post_body : 413, Payload Too Large \033[0m"
curl -X POST http://localhost:9090/post_body -d "12345678901"

# HEAD
# curl -I HEAD http://localhost:9090/

# DELETE
echo "\033[36m DELETE - root : 405, Method Not Allowed \033[0m"
curl -X DELETE http://localhost:9090/

# PUT
curl -X PUT http://localhost:9090/

#header
curl -X POST http://localhost:9090/post_body -H "Content-Length=5&Content-Type=text/html"

# GET, session
curl -X GET http://localhost:9090/session -H "Secret-Key=hellowebserv&Cookie=session_id=2&Name=yeju"
# POST, session
# DELETE, session




