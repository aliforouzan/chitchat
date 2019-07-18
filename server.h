#include <stdbool.h>
#include <sys/types.h>
#include <sys/queue.h>

struct server{
    int listener_d;
    struct sockaddr_in address;
}server_socket;

typedef struct message {
	int status;
	int reply_length;
	char *message;
} message;

SLIST_HEAD(slisthead, interface) inet_queue_head = SLIST_HEAD_INITIALIZER(
                                                   inet_queue_head);
void
server_init();
int
server_lisining();
int
option_resolver(int argc, char *argv[]);
void
debug(int level, char message[]);

