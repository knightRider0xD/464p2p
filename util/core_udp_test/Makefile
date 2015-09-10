all: client4 client6 server4 server6

fresh: clean all

client4: udp-client4.c
	gcc udp-client4.c -o client4
client6: udp-client6.c
	gcc udp-client6.c -o client6
server4: udp-server4.c
	gcc udp-server4.c -o server4
server6: udp-server6.c
	gcc udp-server6.c -o server6
	
clean:
	rm -f client4 client6 server4 server6
