while true
do
	cat /tmp/psplash.txt | awk '{print $2}'
	sleep 0.1
done
