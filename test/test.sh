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
	if [[ $4 ]]; then
		curl -s "$1" ${4} > "$2.log"
	else
		curl -s "$1" > "$2.log"
	fi
	# cat "$2.log"

	sleep 0.1
	echo ""
	echo "$2"
	if [[ $4 ]]; then
		diff -I 'Date: ' "$2.log" $3 # TODO: Improve this one
	else
		diff "$2.log" $3
	fi
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

test_diff "http://localhost:8080/"	"server_blue"		"../demo/servers/blue/index.html"
test_diff "http://127.0.0.1:8080/"	"server_green"		"../demo/servers/green/index.html"
test_diff "http://0.0.0.0:8080/"	"server_red"		"../demo/servers/red/index.html"
test_diff "http://127.0.0.1:8080/"	"server_default"	"../demo/servers/green/index.html"
test_diff "http://127.0.0.1:8080/"	"server_black"		"../demo/servers/black/index.html" "-H Host:"

test_diff "http://127.0.0.1:4242/cgi-bin/test/try-error-500.pl" "cgi_try-error-500" "../res/error/500.html"

test_diff "http://127.0.0.1:4242/cgi-bin/test/no-header.pl" "cgi_no-header" "./diff/cgi_no-header.txt"
# TODO: Improve this two test, check also content
test_diff "http://127.0.0.1:4242/cgi-bin/test/full-header.pl" "cgi_full-header" "./diff/cgi_full-header.txt" "-I"
test_diff "http://127.0.0.1:4242/cgi-bin/test/partial-header.pl" "cgi_partial-header" "./diff/cgi_partial-header.txt" "-I"

test_diff "http://localhost:8081/fdshfjkds" "custom_err_page"	"test_page.html"

pkill -2 webserv

exit 0
