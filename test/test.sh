#!/bin/sh

cd "$(dirname "$0")"

if [[ $? == 0 ]]; then
	echo "webserv was killed."
fi

make -C ..
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

sleep 0.1

echo ">> req GET http://127.0.0.1:4242/"
curl -s http://127.0.0.1:4242/ > output.log
cat output.log

echo ""
echo "default_request_homepage"
diff output.log ../demo/www/index.html
if [[ $? == 0 ]]; then
	echo "output ok"
else
	echo "error output different :("
	exit 1
fi

pkill -2 webserv

exit 0
