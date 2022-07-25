# GET
echo "\033[36m"
echo "method: get"
echo "url: http://localhost"
echo "port: 9090"
echo "\033[0m"
curl -X GET http://localhost:9090

# 405, Method Not Allowed
echo "\033[36m"
echo "method: post"
echo "url: http://localhost"
echo "port: 9090"
echo "\033[0m"
curl -X POST http://localhost:9090

# POST
echo "\033[36m"
echo "method: post"
echo "url: http://localhost/post_body"
echo "port: 9090"
echo "body: 01234567890"
echo "\033[0m"
curl -X POST http://localhost:9090/post_body -d "1234567890"

# 413, Payload Too Large
#echo "\033[36m"
#echo "method: post"
#echo "url: http://localhost/post_body"
#echo "port: 9090"
#echo "body: 1234567890"
#echo "\033[0m"
#curl -X POST http://localhost:9090/post_body -d "1234567890 ..."

# HEAD
curl -I HEAD http://localhost:9090/

#header
curl -X POST http://localhost:9090/post_body -H "Content-Length=5&Content-Type=text/html"





