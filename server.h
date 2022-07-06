#ifndef LIST
#define LIST

static int uid = 0;

// client structure 
typedef struct client{
	struct sockaddr_in address;
	int sockfd;
	int uid;
	char name[32];
	struct client* prev;
    struct client* link;
} client_t;

// create a new client
client_t *newClient(int sockfd, struct sockaddr_in addr) {
	client_t *temp = (client_t *)malloc(sizeof(client_t));
	temp->address = addr;
	temp->sockfd = sockfd;
	temp->uid = uid++;
	strncpy(np->name, "NULL", 5);
	temp->prev = NULL;
    temp->link = NULL;

	return temp;
}

#endif // LIST