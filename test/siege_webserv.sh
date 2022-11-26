#!/bin/bash

# Example:
# ./siege_webserv.sh -bc50 127.0.0.1 4242 "cgi-bin/apply-for-iceberg.pl?" 20

function countdown
{
	[[ -n $1 ]] && let time_sl="$1" || let time_sl="10"
	p="."
	for (( i=1; $i <= ${time_sl}; i++ ))
	do
		printf " %5ds $p\r" ${i}
		sleep 1s
		p+="."
	done
	printf "\n\n"
}

function synopsis
{
	echo -e "Usage:\n./siege_webserv [-bc #nb_conc] hostname/ip port path [countdown]"
	echo -e "launch the siege application on the address http://[hostname|ip]:port/path for countdown seconds."
	echo -e "FLAGS:\n\t-b\tuse the -b (--benchmark) siege's flags\n\t-c\tuse the -c (--concurrent) siege's flags with a #nb_conc concurrents\n\t\thas to be between 0 and 255. Better results around 125 concurrents\n"
}

function launch_siege
{
	echo -e "siege $c_flags $b_flags http://$1:$2/$3\n"
	nohup siege $c_flags $b_flags "http://$1:$2/$3" 2>&1 > nohup.out &
	pid_siege=$!


	[[ $# -eq 4 ]] && countdown $4 || countdown # C'est comme ca qu'on fait les ternaires en bash...
	kill -15 $pid_siege
	sleep 1s # without this sleep we can catch with wait the siege before SIGTERM was send -> fail
	wait $pid_siege
}

function stat_siege
{
	grep "Transactions:" nohup.out
	grep "Availability:" nohup.out
	grep "Elapsed time:" nohup.out
	grep "Data transferred:" nohup.out
	grep "Response time:" nohup.out
	grep "Transaction rate:" nohup.out
	grep "Throughput:" nohup.out
	grep "Concurrency:" nohup.out
	grep "Successful transactions:" nohup.out
	grep "Failed transactions:" nohup.out
	grep "Longest transaction:" nohup.out
	grep "Shortest transaction:" nohup.out

	awk '/Availability:/ {
		if ($(NF-1) >= 99.5)
			print "\033[32mYou pass the siege\033[0m"
		else
			print "\033[31mYou fail the siege\033[0m"
		}' nohup.out	
	rm -rf nohup.out
}

if [[ `uname` = "Darwin"  ]]; then
	which brew 2>&1 > /dev/null
	if [[ $? -eq 0 ]]; then
		if [[ -x "${HOME}/.brew/bin/siege" ]]; then
			# Check des flags et des args du script ici...
			b_flags=""
			c_flags=""

			while getopts "bc:" OPTNAME
			do
				case "$OPTNAME" in
					b)
						b_flags="-b"
						;;
					c)
						[[ ${#OPTARG} -gt 4 || ${OPTARG} -gt 255 || ${OPTARG} -lt 1 ]] && echo "${OPTARG} invalid flag." && synopsis && exit 5
						c_flags="-c$OPTARG"
						;;
					*)
						synopsis
						break
						;;
				esac
				shift $(( OPTIND - 1 ))
			done
			if [[ $# -eq 3 || $# -eq 4 ]]; then
				launch_siege $1 $2 $3 $4
				stat_siege

			else
				synopsis
				exit 4;
			fi
		else
			echo -e "siege cannot be executed!\nInstallation...\r"
			brew install siege 2>&1 > /dev/null
			if [[ $? -ne 0 ]]; then
				echo "Pb during the siege's installation. Better to do it manually, sorry..."
				exit 3;
			fi
			./$0 $1 $2 $3 $4
		fi
	else
		echo -e "brew is not installed!\nYou have to install a MacOSX package manager as brew for doing the webserv correction."
		exit 2;
	fi
else
		echo "Webserv correction has to be done on MacOSX! [Nice Campus - 11/2022]"
		exit 1;
fi
