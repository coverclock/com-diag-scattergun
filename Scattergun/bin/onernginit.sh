if [ -c /dev/${1} ]; then
	/usr/bin/logger -p local0.notice -t OneRNG -- ${0} "${@}" begin
	/bin/stty -F /dev/${1} raw -echo clocal -crtscts
	/bin/echo "cmd0" > /dev/${1}
	/bin/echo "cmdO" > /dev/${1}
	/usr/bin/logger -p local0.notice -t OneRNG -- ${0} "${@}" end
fi
