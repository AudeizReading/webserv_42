#!/bin/sh

cd "$(dirname "$0")"

make -C ..
if [[ $? == 0 ]]; then
	echo "se compile :)"
	make run -C ..
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
