#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include "string.h"
#include "server.h"

#define BUFFER_SZ 2048

client_t *root, *now;

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Send message to all clients except sender */
void send_message(char *s, client_t *list){
	pthread_mutex_lock(&clients_mutex);

	client_t *tmp = root->link;
    while (tmp != NULL) {
        if (list->uid != tmp->uid) { 
            send(tmp->sockfd, s, strlen(s), 0);
        }
        tmp = tmp->link;
    }

	pthread_mutex_unlock(&clients_mutex);
}

/* Handle all communication with the client */
void *handle_client(void *arg){
	char buff_out[BUFFER_SZ];
	char name[32];
	int leave_flag = 0;

	client_t *cli = (client_t *)arg;

	// Check name
	if(recv(cli->sockfd, name, 32, 0) <= 0 || strlen(name) <  2 || strlen(name) >= 32-1){
		printf("Didn't enter the name.\n");
		leave_flag = 1;
	} else{
		strcpy(cli->name, name);
		sprintf(buff_out, "%s[%d] has joined\n", cli->name, cli->uid);
		printf("%s", buff_out);
		send_message(buff_out, cli);
	}

	bzero(buff_out, BUFFER_SZ);

	while(1){
		if (leave_flag) {
			break;
		}

		int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
		if (receive > 0){
			if(strlen(buff_out) > 0){
				sprintf(buff_out, "%s -> %s[%d]\n", buff_out, cli->name, cli->uid);
				send_message(buff_out, cli);

				str_trim_lf(buff_out, strlen(buff_out));
				printf("%s\n", buff_out);
			}
		} else if (receive == 0 || strcmp(buff_out, "exit") == 0){
			sprintf(buff_out, "%s[%d] has left\n", cli->name, cli->uid);
			printf("%s", buff_out);
			send_message(buff_out, cli);
			leave_flag = 1;
		} else {
			printf("ERROR: -1\n");
			leave_flag = 1;
		}

		bzero(buff_out, BUFFER_SZ);
	}

  	/* Delete client from queue and yield thread */
	close(cli->sockfd);

  	if (cli == now) { 
		// remove last node
        now = cli->prev;
        now->link = NULL;
    } else { 
		// remove a middle node
        cli->prev->link = cli->link;
        cli->link->prev = cli->prev;
    }

  	free(cli);
  	pthread_detach(pthread_self());
	return NULL;
}

int main(int argc, char **argv){

	if(argc != 2){
		printf("Usage: %s <port>\n", argv[0]);
		return EXIT_FAILURE;
	}

	int port = atoi(argv[1]);
	int option = 1;
	int listenfd = 0, connfd = 0;

	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	
	pthread_t tid;

	/* Socket settings */
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

  	/* Ignore pipe signals */
	signal(SIGPIPE, SIG_IGN);

	if(setsockopt(listenfd, SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0){
		perror("ERROR: setsockopt failed");
    	return EXIT_FAILURE;
	}

	/* Bind */
  	if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    	perror("ERROR: Socket binding failed");
    	return EXIT_FAILURE;
  	}

  	/* Listen */
  	if (listen(listenfd, 10) < 0) {
    	perror("ERROR: Socket listening failed");
    	return EXIT_FAILURE;
	}

	printf("\n=== WELCOME TO THE CHATROOM ===\n\n");

	root = newClient(listenfd, serv_addr) ;
	now = root;

	while(1){
		socklen_t clilen = sizeof(cli_addr);
		connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen);

		/* Add client to the queue and fork thread */
		client_t *cli = newClient(connfd, cli_addr);
		cli->prev = now;
        now->link = cli;
        now = cli;

		pthread_create(&tid, NULL, &handle_client, (void*)cli);

		/* Reduce CPU usage */
		sleep(1);
	}

	return EXIT_SUCCESS;
}
