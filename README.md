How it configs?

1. start SLIP on PC
	sudo slattach -p slip -s 115200 /dev/ttyUSB0&

2. config SLIP interface
	sudo ifconfig sl0 192.168.240.2 pointopoint 192.168.240.1 up mtu 1500
	sudo ip route add 192.168.240.0 via 192.168.240.1 dev sl0

3. telnet and config
	telnet 192.168.240.1 1023

