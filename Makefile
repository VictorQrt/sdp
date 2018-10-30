all: sdp_server sdp_client

sdp_server: server.c
	gcc -Wall -O2 -o sdp_server server.c

sdp_client: client.c
	gcc -Wall -O2 -o sdp_client client.c

clean:
	rm -f sdp_server sdp_client
