/bin/stty -F /dev/${1} raw -echo clocal -crtscts
/bin/echo "cmd0" > /dev/${1}
/bin/echo "cmdO" > /dev/${1}
