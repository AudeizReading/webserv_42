#!/bin/sh

cd "$(dirname "$0")"

pkill webserv
if [[ $? == 0 ]]; then
	echo "webserv was killed."
fi

make re -C ..
if [[ $? == 0 ]]; then
	echo "se compile :)"
	make run -C .. &
else
	echo "error compilation :("
	exit 1
fi

if [[ $? == 0 ]]; then
	echo "s'execute :)"
else
	echo "error execution :("
	exit 1
fi

sleep 1

echo ">> req GET http://127.0.0.1:5000/"
curl -s http://127.0.0.1:5000/ > output.log
cat output.log

echo ""
echo "default_request_homepage"
diff output.log diff/default_request_homepage.txt
if [[ $? == 0 ]]; then
	echo "output ok"
else
	echo "error output different :("
	exit 1
fi

exit 0
