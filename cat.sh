while true
do
	cat psplash.txt | awk '{print $2}'
	sleep 0.1
done
