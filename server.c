#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

void printctime();
void handleInput(char buf[], struct sockaddr_in* srcAddr);
int validate(char msg[], int msgSize);
bool SDU_validate(char msg[]);
//bool MSR_validate();
//void SDU_handle();
//void MSR_handle();

int main() {
    const unsigned short servPort = 42283;
    const unsigned int addr = (0 << 24) + (0 << 16) + (0 << 8) + 0;

    int sfd = socket(AF_INET, SOCK_DGRAM, 0);

    if(sfd == -1) {
        perror("Error creating the UDP socket... exiting");
        return EXIT_FAILURE;
    }

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(servPort);
    saddr.sin_addr.s_addr = htonl(addr);

    if(bind(sfd, (struct sockaddr*) &saddr, sizeof(saddr)) == -1) {
        perror("Error binding the UDP socket... exiting");
        return EXIT_FAILURE;
    }

    printctime();
    printf("SDP SERVER INITIALIZED\n");

    char buf[BUFSIZ];
    struct sockaddr_storage srcAddr;
    socklen_t srcLen = sizeof(srcAddr);

    while(recvfrom(sfd, buf, BUFSIZ, 0, (struct sockaddr*) &srcAddr, &srcLen)) {
        if(fork() == 0)
            handleInput(buf, (struct sockaddr_in *) &srcAddr);
    }

    return 0;
}

void printctime() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("[%d:%d:%d] ", tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void handleInput(char buf[], struct sockaddr_in* srcAddr) {
    int msgSize;
    for(msgSize = 0; buf[msgSize] != '\n'; msgSize++);

    char msg[msgSize];

    for(int i = 0; i<msgSize; i++)
        msg[i] = buf[i];
    msg[msgSize] = '\0';

    unsigned char *srcIP = (unsigned char *) &srcAddr->sin_addr.s_addr;
    printctime();
    printf("\"%s\"", msg);
    printf(" from %d.%d.%d.%d", srcIP[0], srcIP[1], srcIP[2], srcIP[3]);

    int validation = validate(msg, msgSize);

    if(validation == 0) {
        printf(" [INVALID] [MESSAGE DROPPED]\n");
    }
    else {
        printf("\n");
    }
}

int validate(char msg[], int msgSize) {
    if(msgSize < 3)
        return 0;

    char type[3];
    strncpy(type, msg, 3);
    type[3] = '\0';

    if(strcmp(type, "SDU") == 0) {
        //validate_SDU
        return 1;
    }
    else if(strcmp(type, "MSR") == 0) {
        //validate_MSR
        return 2;
    }
    else {
        return 0;
    }
}

bool SDU_validate(char msg[]) {
    //char * last_token = strtok(msg, " ");

    //validate all fields

    return true;
}
