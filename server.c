#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

typedef enum msg_type {SDU, MSR, REP, IRW, INV} msg_type;

typedef struct message {
    msg_type type;
    char* raw_msg;
    char** fields;
} message;

void printctime();
void handleInput(char* buf, struct sockaddr_in* src_addr);
message* parse_msg(char* msg, int msg_size);
void free_msg(message* msg);

int main() {
    const unsigned short serv_port = 42283;
    const unsigned int addr = (0 << 24) + (0 << 16) + (0 << 8) + 0;

    int sfd = socket(AF_INET, SOCK_DGRAM, 0);

    if(sfd == -1) {
        perror("Error creating the UDP socket... exiting");
        return EXIT_FAILURE;
    }

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(serv_port);
    saddr.sin_addr.s_addr = htonl(addr);

    if(bind(sfd, (struct sockaddr*) &saddr, sizeof(saddr)) == -1) {
        perror("Error binding the UDP socket... exiting");
        return EXIT_FAILURE;
    }

    printctime();
    printf("SDP SERVER INITIALIZED\n");

    char buf[BUFSIZ];
    struct sockaddr_storage src_addr;
    socklen_t srcLen = sizeof(src_addr);

    while(recvfrom(sfd, buf, BUFSIZ, 0, (struct sockaddr*) &src_addr, &srcLen)) {
        if(fork() == 0)
            handleInput(buf, (struct sockaddr_in *) &src_addr);
    }

    return 0;
}

void printctime() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("[%d:%d:%d] ", tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void handleInput(char* buf, struct sockaddr_in* src_addr) {
    int msg_size;
    for(msg_size = 0; buf[msg_size] != '\n'; msg_size++);

    char msg[msg_size];

    for(int i = 0; i<msg_size; i++)
        msg[i] = buf[i];
    msg[msg_size] = '\0';

    unsigned char *srcIP = (unsigned char *) &src_addr->sin_addr.s_addr;
    printctime();
    printf("\"%s\"", msg);
    printf(" from %d.%d.%d.%d", srcIP[0], srcIP[1], srcIP[2], srcIP[3]);

    message* inc_msg = parse_msg(msg, msg_size);

    if(inc_msg->type == INV) {
        printf(" [INVALID] [MESSAGE DROPPED]\n");
    }
    else {
        printf("\n");
    }

    free_msg(inc_msg);
}

message* parse_msg(char* const msg, const int msg_size) {
    char* raw_msg = malloc(msg_size * sizeof(char));
    *raw_msg = *msg;
    message* res = malloc(sizeof(message));
    *res = (message) {INV, raw_msg, NULL};

    if(msg_size < 3)
        return res;

    char type[4];
    strncpy(type, msg, 4);

    char* _str = strdup(msg);

    if(strcmp(type, "SDU ") == 0) {
        res->type = SDU;
        res->fields = malloc(5 * sizeof(char*));

        for(int i=0; i<5; i++)
            res->fields[i] = strsep(&_str, " ");
    }

    else if(strcmp(type, "MSR ") == 0) {
        res->type = MSR;
        res->fields = malloc(7 * sizeof(char*));

        for(int i=0; i<7; i++)
            res->fields[i] = strsep(&_str, " ");
    }

    free(_str);

    return res;
}

void free_msg(message* msg) {
    free(msg->raw_msg);
    free(msg->fields);
    free(msg);
}
