#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char ** argv) {
    if(argc != 5) {
        printf("Error : bad arguments\nUsage : SDP_Client <server IP> <clientID> <dataType> <value>\n");
    }

    return 0;
}
