#!/bin/sh

cd "$(dirname "$0")"

pkill -2 webserv
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
echo "=============================="
echo "============ TEST ============"
echo "=============================="

test_diff () {
	sleep 0.1
	echo ">> req GET $1"
	curl -s "$1" > "$2.log"
	# cat "$2.log"

	sleep 0.1
	echo ""
	echo "$2"
	diff "$2.log" $3
	if [[ $? == 0 ]]; then
		echo "output ok"
	else
		echo "error output different :("
		exit 1
	fi
	echo "=============================="
}

test_diff "http://127.0.0.1:4242/?je_suis_inutile=1&mais=je_debug&bien=1" \
	"default_request_homepage" "../demo/www/index.html"

test_diff "http://127.0.0.1:4242/.password" \
	"forbidden_access_on_hidden_files" "../res/error/403.html"

test_diff 'http://127.0.0.1:5000/' \
	"default_to_server2" "../demo/www2/index.html"

test_diff "http://localhost:8080/" "server_blue" "../demo/servers/blue/index.html"
test_diff "http://127.0.0.1:8080/" "server_green" "../demo/servers/green/index.html"
test_diff "http://0.0.0.0:8080/" "server_red" "../demo/servers/red/index.html"

pkill -2 webserv

exit 0
