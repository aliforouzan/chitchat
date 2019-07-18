#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "server.h"
#include "shared_header.h"

#define DEFAULT_PORT    2019
#define DEFAULT_ADDRESS INADDR_LOOPBACK
#define BUFFER_SIZE     IPPORT_MAX

#define SUCCESS     0
#define FAILURE     1

#define LOG_INFO    1
#define LOG_NOTIC   2
#define LOG_DEBUG   3


u_int32_t debug_level;
bool h_flag = 0, v_flag = 0, client_mode = 0;

int
main(int argc, char *argv[])
{
    memset(&server_socket, 0, sizeof server_socket);
    if(option_resolver(argc, argv) == FAILURE)
        return FAILURE;
    if(v_flag) {
        /*print_info*/
        exit(EXIT_SUCCESS);
    }
    if(h_flag) {
        /*print_help*/
        exit(EXIT_SUCCESS);
    }
    if(!client_mode) {
        server_init();
        return(server_lisining());
    }
    else
        return(client_init());
}

void
server_init()
{
    if(server_socket.address.sin_port == 0)
            server_socket.address.sin_port = DEFAULT_PORT;
    if(server_socket.address.sin_addr.s_addr == 0)
        server_socket.address.sin_addr.s_addr = DEFAULT_ADDRESS;
    SLIST_INIT(&inet_queue_head);
}

int
option_resolver(int argc, char *argv[])
{
    int c = 0;
    while ((c = getopt (argc, argv, "-:hvdp:i:cs")) != -1) {
        switch (c) {
            case 'h':
                h_flag = 1;
                break;
            case 'v':
                v_flag = 1;
                break;
            case 'd':
                if(debug_level >= 3)
                    debug_level = 1;
                else
                    debug_level++;
                break;
            case 'p':
                server_socket.address.sin_port = (in_port_t)htons(atoi(optarg));
                break;
            case 'i':
                server_socket.address.sin_addr.s_addr = htonl(atoi(optarg));
                break;
            case 'c':
                client_mode = 1;
                break;
            case 's':
                client_mode = 0;
                break;
            case '?':
                fprintf(stdout,"argument %c not registered.", c);
                return FAILURE;
                break;
            case ':':
                fprintf(stdout,"argument %c need value.", c);
                return FAILURE;
                break;
        }
    }
    return SUCCESS;
}

int
server_lisining()
{
	debug(LOG_INFO, "start for create socket.\n");
	if (!(server_socket.listener_d = socket(AF_INET, SOCK_STREAM, 0))) {
		debug(LOG_NOTIC, "socket failed\n");
		exit(EXIT_FAILURE);
	}
	server_socket.address.sin_family = AF_INET;
	if (listen(server_socket.listener_d, 10) == -1) {
        debug(LOG_NOTIC, "listen failed\n");
        exit(EXIT_FAILURE);
    }
	if (bind(server_socket.listener_d, (struct sockaddr *)&server_socket.address, sizeof(server_socket.address)) == -1) {
		debug(LOG_NOTIC, "bind failed\n");
		exit(EXIT_FAILURE);
	}
    while (true){
        debug(LOG_INFO, "ready for new message.\n");
        struct sockaddr_storage client_addr;
        u_int32_t address_size = sizeof(client_addr);
        int connect_d = 0;
		if ((connect_d = accept(server_socket.listener_d, (struct sockaddr *)&client_addr,
		                         (socklen_t *)&address_size)) == -1) {
			debug(LOG_NOTIC, "accept failed\n");
			exit(EXIT_FAILURE);
		}
		if(!fork()) {
            close(server_socket.listener_d);

            /*call function that responsible for handling message passing.*/

            close(connect_d);
            exit(0);
		}
		close(connect_d);
		debug(LOG_INFO, "new request recieve.\n");
	}
}

void
debug(int level, char message[])
{
	switch (level) {
	case LOG_INFO:
		if (debug_level >= LOG_INFO)
			fprintf(stdout,"DEBUG %d:%s", level, message);
		break;
	case LOG_NOTIC:
		if (debug_level >= LOG_NOTIC)
			fprintf(stdout,"DEBUG %d:%s", level, message);
		break;
	case LOG_DEBUG:
		if (debug_level >= LOG_DEBUG)
			fprintf(stdout,"DEBUG %d:%s", level, message);
		break;
	default:
		break;
	}
}
