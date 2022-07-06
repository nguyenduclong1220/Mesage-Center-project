#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <getopt.h>
#include "string.h"

static int verbose_flag;

#define LENGTH 2048

// Global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];
int role;


void catch_ctrl_c_and_exit(int sig) {
    flag = 1;
}

void send_msg_handler() {
  	char message[LENGTH] = {};
  	char buffer[LENGTH + 32] = {};

  	while(1) {
		str_overwrite_stdout();
		
		if (fgets(message, LENGTH, stdin) == NULL) {
			printf("Failed\n");
		}
		str_trim_lf(message, LENGTH);

		if (strcmp(message, "exit") == 0) 
		{
			break;
		}
		else
		{
			sprintf(buffer, "%s: %s", name, message);
			send(sockfd, buffer, strlen(buffer), 0);
		}

		bzero(message, LENGTH);
		bzero(buffer, LENGTH + 32);
  	}

  	catch_ctrl_c_and_exit(2);

}

void recv_msg_handler() {

  	char message[LENGTH] = {};

  	while (1) {

		int receive = recv(sockfd, message, LENGTH, 0);

    if (receive > 0) 
	{
		printf("%s", message);
		str_overwrite_stdout();
    } 
	else if (receive == 0) 
	{
		break;
    } 
	
	memset(message, 0, sizeof(message));

  	}
}

void help() {

	printf("USAGE:\n\t\t./client <IP> <Port>\n");
	printf("Or\n");
	printf("\t\t./client [OPTIONS]\n");
	printf("OPTIONS:\n");
	printf("\t\t-h	--help		showhelp\n");
	printf("If you join the chatroom successfully\n");
	printf("- Please enter your name\n");
	printf("- Then, Select your role:\n");
	printf("\t\t+ Select '1'- Talker if you want to just send messages.\n");
	printf("\t\t+ Select '2'- Listener if you want to just receive messages.\n");
	printf("\t\t+ Select '3'- Both if you want to both send and receive messages.\n");
	printf("- Enter Ctrl + C or exit if you want to leave\n");

}
int main(int argc, char **argv){

	int index;

	while (1)
	{
		static struct option long_options[] = 
		{
			 {"verbose", no_argument,       &verbose_flag, 1},
          	 	 {"brief",   no_argument,       &verbose_flag, 0},

			 {"help",	 no_argument,		0, 'h'},
			 {0, 0, 0, 0}
		};

		int option_index = 0;

		index = getopt_long (argc, argv, "h",
						 long_options, &option_index);

		if (index == -1)
		{
			break;
		}

		switch (index)
		{
			case 0:
			/* If this option set a flag, do nothing else now. */
			if (long_options[option_index].flag != 0)
				break;
			printf ("option %s", long_options[option_index].name);
			if (optarg)
				printf (" with arg %s", optarg);
			printf ("\n");
			break;

			case 'h':
				help();
				break;

			default:
				abort();
				
		}
	}

	if (verbose_flag) 
	{
   	 	puts ("verbose flag is set");
	}

    /* Print any remaining command line arguments (not options). */
     if (optind < argc)
     {
		if(argc <= 2){
			help();
			return EXIT_FAILURE;
		}

		int port = atoi(argv[2]);

		signal(SIGINT, catch_ctrl_c_and_exit);

		struct sockaddr_in server_addr;

		/* Socket settings */
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = inet_addr(argv[1]);
		server_addr.sin_port = htons(port);


		// Connect to Server
		int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
		if (err == -1) {
			printf("ERROR: connect\n");
			help();
			return EXIT_FAILURE;
		}

		printf("\n=== WELCOME TO THE CHATROOM ===\n\n");

		printf("Please enter your name: ");
		
		if (fgets(name, 32, stdin) == NULL) {
			printf("Failed\n");
		}
		str_trim_lf(name, strlen(name));

		if (strlen(name) > 32 || strlen(name) < 2){
			printf("Name must be less than 30 and more than 2 characters.\n");
			return EXIT_FAILURE;
		}

		printf("Please select your role: \n1 - Talker\n2 - Listener\n3 - Both\n");

		if (scanf("%d", &role) == 0) {
			printf("Failed\n");
		}

		// Send name
		send(sockfd, name, 32, 0);

		// Check role
		if (role == 1) 
		{
			printf("You are Talker - You can send messages!\n");
			pthread_t send_msg_thread;
			if(pthread_create(&send_msg_thread, NULL, (void *) send_msg_handler, NULL) != 0){
				printf("ERROR: pthread\n");
				return EXIT_FAILURE;
			}
		}

		else if (role == 2)
		{
			printf("You are Listener - You can receive messages!\n");
			pthread_t recv_msg_thread;
			if(pthread_create(&recv_msg_thread, NULL, (void *) recv_msg_handler, NULL) != 0){
				printf("ERROR: pthread\n");
				return EXIT_FAILURE;
			}
		}

		else
		{
			printf("You are Both - You can send and receive messages!\n");
			pthread_t send_msg_thread;
			if(pthread_create(&send_msg_thread, NULL, (void *) send_msg_handler, NULL) != 0){
				printf("ERROR: pthread\n");
				return EXIT_FAILURE;
			}

			pthread_t recv_msg_thread;
			if(pthread_create(&recv_msg_thread, NULL, (void *) recv_msg_handler, NULL) != 0){
				printf("ERROR: pthread\n");
				return EXIT_FAILURE;
			}
		}
	

		while (1){
			if(flag){
				printf("\nBye\n");
				break;
		}
		}

		close(sockfd);

		return EXIT_SUCCESS;

}

  exit (0);
	
}
