/*
** ECE361 - Computer Networks - Lab 2 - Client File - client.c
** Group 21 - Abdurrafay Khan & Shadman Kaif
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <math.h>


struct message {
	unsigned int type;
	unsigned int size;
	unsigned char source[1000];
	unsigned char data[1000];
};

typedef enum message_type {
	LOGIN,
	LO_ACK,
	LO_NAK,
	EXIT,
	JOIN,
	JN_ACK,
	JN_NAK,
	LEAVE_SESS,
	NEW_SESS,
	NS_ACK,
	NS_NAK,
	MESSAGE,
	QUERY,
	QU_ACK,
	LOGOUT
}Message_type;


int main(int argc, char *argv[]) {
	
	
	// checks for 1 arguments being passed in "client"
	if (argc != 1) {
		fprintf(stderr, "usage: deliver <server address> <server port number>\n");
		return 0;
	}
	
	// initializes variables
	int sockfd= -1;
	
	bool activeUser = false;
	char *currentUser;
	
	// repeats indefinetly until connection closes
	while (1) {
		
		printf("Enter your commands: \n");
		
		// initializes variables and structs for the command
		char input[1000];
		struct message client;
		struct message server;
		char *type;
		int port_num;
		struct sockaddr_in *connecting_address;
		
		// detects command from user and stores to input
		fgets(input, 1000, stdin);
		
		// sets ptr to the first word of the command and stores it to type
        char *ptr = strtok(input, " ");
		type = malloc(sizeof(char) * (strlen(ptr)+1));
		strcpy(type, ptr);
		char *portnum, *id, *pw, *ip;
		// if the type of the command is login, then...
		if (strcmp(ptr, "login") == 0) {
			
			// goes through the rest of the command line and extracts information
			int count = 1;
			
			if (activeUser == true) {
				printf("Cannot sign in anyone else as there is a user already on: \n");
			}
			else {
				while (ptr != NULL) { 
				
					// stores the users id
					if (count == 1) { 
						ptr = strtok(NULL, " ");
						id = malloc(sizeof(char) * (strlen(ptr)+1));
						strcpy(id, ptr);
						count++;
					}
					
					// stores the users password
					else if (count == 2) { 
						ptr = strtok(NULL, " ");
						pw = malloc(sizeof(char) * (strlen(ptr)+1));
						strcpy(pw, ptr);
						count++;
					}
					
					// stores the servers ip address
					else if (count == 3) { 
						ptr = strtok(NULL, " ");
						ip = malloc(sizeof(char) * (strlen(ptr)+1));
						strcpy(ip, ptr);
						count++;
					}
					
					// stores the servers port number
					else if (count == 4) { 
						ptr = strtok(NULL, " ");
						portnum = malloc(sizeof(char) * (strlen(ptr)+1));
						strcpy(portnum, ptr);
						break;
					}
				}
				
				// makes a  TCP socket
				int sockfd = socket(AF_INET, SOCK_STREAM, 0);
				if (sockfd == -1) {
					printf("Error in making socket\n");
					return 0;
				}	
				
				// sets parameters for the connection
				port_num = atoi(portnum);

				connecting_address = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
				memset((char *)connecting_address, 0, sizeof(struct sockaddr_in));
				connecting_address->sin_family = AF_INET;
				connecting_address->sin_addr.s_addr = inet_addr(ip);
				connecting_address->sin_port = htons(port_num);
					
				// connects to the server
				int connecting = connect(sockfd, (struct sockaddr *)connecting_address, sizeof(struct sockaddr_in));
				if (connecting == -1) {
					printf("Error in making connecting\n");
					return 0;
				}	
				
				// calculates size of data (user id,user password)
				int data_string_size = strlen(id) + strlen(pw) + 1;
				
				// creates memory for the string packet that needs to be sent
				char* packet_to_send = malloc(1000*sizeof(char));
			
				// fills out the string to be sent with the first four members, returning index at the end of four members
				int packet_members = sprintf(packet_to_send, "LOGIN:%d:%s:%s,%s", data_string_size, id, id, pw);
				packet_to_send[packet_members+1] = '\0';

				// sets the members for the client message
				client.type = LOGIN;
				client.size = data_string_size;
				strcpy(client.source, id);
				strcpy(client.data, packet_to_send);
				
				// writes the data to the socket
				write(sockfd, packet_to_send, 1000);
				
				// frees the stored data for the next iteration
				free(packet_to_send);
				
				char data[1000];
			
				// stores the data read in from the socket to data
				read(sockfd, data, 1000);
				
				// sets ptr to the first word of the command and stores it to typeS
				char *ptr = strtok(data, ":");
				type = malloc(sizeof(char) * (strlen(ptr)+1));
				strcpy(type, ptr);
				
				server.type = atoi(ptr);
				ptr = strtok(NULL, ":");
				server.size = atoi(ptr);
				ptr = strtok(NULL, ":");
				strcpy(server.source, ptr);
				
				if (strcmp(type, "LO_ACK") == 0) {
					
					ptr = strtok(NULL, "\n");
					strcpy(server.data, ptr);
					printf("%s\n", server.data);
					activeUser = true;
					currentUser = malloc(sizeof(char) * (strlen(id)+1));
					strcpy(currentUser, id);
				}
				else if (strcmp(type, "LO_NAK") == 0) {
					ptr = strtok(NULL, "\n");
					strcpy(server.data, ptr);
					printf("%s\n", server.data);
				}
				else {
					printf("nothing correct came back: \n");
				}
			}
		}
		

		else if (strcmp(ptr, "quit") == 0) {

			// calculates size of data (type, user id, user password)
			int data_string_size = 4;
			
			// creates memory for the string packet that needs to be sent
			char* packet_to_send = malloc(1000*sizeof(char));
			
			// fills out the string to be sent with the first four members, returning index at the end of four members
			int packet_members = sprintf(packet_to_send, "QUIT:%d:%s:quit", data_string_size, id);
			packet_to_send[packet_members+1] = '\0';

			// sets the members for the client message
			client.type = EXIT;
			client.size = data_string_size;
			strcpy(client.source, id);
			strcpy(client.data, packet_to_send);
			
			// connects to socket again to send data
			int sockfd = socket(AF_INET, SOCK_STREAM, 0);
			if (sockfd == -1) {
				printf("Error in making socket\n");
				return 0;
			}	
			int connecting = connect(sockfd, (struct sockaddr *)connecting_address, sizeof(struct sockaddr_in));
			if (connecting == -1) {
				printf("Error in making connecting\n");
				return 0;
			}	
			
			// writes the data to the socket
			write(sockfd, packet_to_send, 1000);
			
			// frees the stored data for the next iteration
			free(packet_to_send);
			
			// closes conenction and terminates program
			close(sockfd);
			printf("Successfully quit\n");
			return 0;
		}
	
	
		else if (strcmp(ptr, "list") == 0) {

			// calculates size of data (type, user id, user password)
			int data_string_size = 4;
			
			// creates memory for the string packet that needs to be sent
			char* packet_to_send = malloc(1000*sizeof(char));
			
			// fills out the string to be sent with the first four members, returning index at the end of four members
			int packet_members = sprintf(packet_to_send, "LIST:%d:%s:list", data_string_size, id);
			packet_to_send[packet_members+1] = '\0';

			// sets the members for the client message
			client.type = QUERY;
			client.size = data_string_size;
			strcpy(client.source, id);
			strcpy(client.data, packet_to_send);
			
			// connects to socket again to send data
			int sockfd = socket(AF_INET, SOCK_STREAM, 0);
			if (sockfd == -1) {
				printf("Error in making socket\n");
				return 0;
			}	
			int connecting = connect(sockfd, (struct sockaddr *)connecting_address, sizeof(struct sockaddr_in));
			if (connecting == -1) {
				printf("Error in making connecting\n");
				return 0;
			}	
			
			// writes the data to the socket
			write(sockfd, packet_to_send, 1000);
			
			// frees the stored data for the next iteration
			free(packet_to_send);
			
			// closes conenction and terminates program
			printf("Here is the list of servers and users:\n");
			
			char data[1000];
		
			// stores the data read in from the socket to data
			read(sockfd, data, 1000);
			
			// sets ptr to the first word of the command and stores it to typeS
			char *ptr = strtok(data, ":");
			type = malloc(sizeof(char) * (strlen(ptr)+1));
			strcpy(type, ptr);
			
			// fills out the information for the server message
			server.type = atoi(ptr);
			ptr = strtok(NULL, ":");
			server.size = atoi(ptr);
			ptr = strtok(NULL, ":");
			strcpy(server.source, ptr);
			
			// if the response is of type QUERY, print out the data (list)
			if (strcmp(type, "QU_ACK") == 0) {
				ptr = strtok(NULL, "\0");
				strcpy(server.data, ptr);
				printf("%s", server.data);
			}
			else {
				printf("nothing correct came back: \n");
			}
		}
		
		else if (strcmp(ptr, "createsession") == 0) {
			
			char *sessionID;
			ptr = strtok(NULL, "\0");
			sessionID = malloc(sizeof(char) * (strlen(ptr)+1));
			strcpy(sessionID, ptr);
			
			// calculates size of data (NEW_SESS)
			int data_string_size = strlen(sessionID) + 1;
			
			// creates memory for the string packet that needs to be sent
			char* packet_to_send = malloc(1000*sizeof(char));
		
			// fills out the string to be sent with the first four members, returning index at the end of four members
			int packet_members = sprintf(packet_to_send, "NEW_SESS:%d:%s:%s", data_string_size, client.source, sessionID);
			packet_to_send[packet_members] = '\0';

			// sets the members for the client message
			client.type = NEW_SESS;
			client.size = data_string_size;
			strcpy(client.source, id);
			strcpy(client.data, packet_to_send);
			
			// connects to socket again to send data
			int sockfd = socket(AF_INET, SOCK_STREAM, 0);
			if (sockfd == -1) {
				printf("Error in making socket\n");
				return 0;
			}	
			int connecting = connect(sockfd, (struct sockaddr *)connecting_address, sizeof(struct sockaddr_in));
			if (connecting == -1) {
				printf("Error in making connecting\n");
				return 0;
			}	
		
			// writes the data to the socket
			write(sockfd, packet_to_send, 1000);
			
			// frees the stored data for the next iteration
			free(packet_to_send);
			
			char data[1000];
		
			// stores the data read in from the socket to data
			read(sockfd, data, 1000);
			
			// sets ptr to the first word of the command and stores it to typeS
			char *ptr = strtok(data, ":");
			type = malloc(sizeof(char) * (strlen(ptr)+1));
			strcpy(type, ptr);
			
			// fills out the information for the server message
			server.type = atoi(ptr);
			ptr = strtok(NULL, ":");
			server.size = atoi(ptr);
			ptr = strtok(NULL, ":");
			strcpy(server.source, ptr);
			
			// if the response is of type QUERY, print out the data (list)
			if (strcmp(type, "NS_ACK") == 0) {
				ptr = strtok(NULL, "\0");
				strcpy(server.data, ptr);
				printf("Created session: %s", sessionID);
			}
			else if (strcmp(type, "NS_NAK") == 0){
				ptr = strtok(NULL, "\0");
				strcpy(server.data, ptr);
				printf("%s\n", server.data);
			}
			else {
				printf("nothing right came back");
			}
		}



		else if (strcmp(ptr, "joinsession") == 0) {
			
			char *sessionID_joined;
			ptr = strtok(NULL, "\0");
			sessionID_joined = malloc(sizeof(char) * (strlen(ptr)+1));
			strcpy(sessionID_joined, ptr);
			
			// calculates size of data (JOIN)
			int data_string_size = strlen(sessionID_joined) + 1;
			
			// creates memory for the string packet that needs to be sent
			char* packet_to_send = malloc(1000*sizeof(char));
		
			// fills out the string to be sent with the first four members, returning index at the end of four members
			int packet_members = sprintf(packet_to_send, "JOIN:%d:%s:%s", data_string_size, id, sessionID_joined);
			packet_to_send[packet_members] = '\0';

			// sets the members for the client message
			client.type = JOIN;
			client.size = data_string_size;
			strcpy(client.source, id);
			strcpy(client.data, packet_to_send);
			
			// connects to socket again to send data
			int sockfd = socket(AF_INET, SOCK_STREAM, 0);
			if (sockfd == -1) {
				printf("Error in making socket\n");
				return 0;
			}	
			int connecting = connect(sockfd, (struct sockaddr *)connecting_address, sizeof(struct sockaddr_in));
			if (connecting == -1) {
				printf("Error in making connecting\n");
				return 0;
			}	
		
			// writes the data to the socket
			write(sockfd, packet_to_send, 1000);
			
			// frees the stored data for the next iteration
			free(packet_to_send);
			
			char data[1000];
		
			// stores the data read in from the socket to data
			read(sockfd, data, 1000);
			
			// sets ptr to the first word of the command and stores it to typeS
			char *ptr = strtok(data, ":");
			type = malloc(sizeof(char) * (strlen(ptr)+1));
			strcpy(type, ptr);
			
			// fills out the information for the server message
			server.type = atoi(ptr);
			ptr = strtok(NULL, ":");
			server.size = atoi(ptr);
			ptr = strtok(NULL, ":");
			strcpy(server.source, ptr);
			
			// if the response is of type QUERY, print out the data (list)
			if (strcmp(type, "JN_ACK") == 0) {
				ptr = strtok(NULL, "\0");
				strcpy(server.data, ptr);
				printf("User joined session: %s", sessionID_joined);
			}
			else if (strcmp(type, "JN_NAK") == 0){
				ptr = strtok(NULL, "\0");
				strcpy(server.data, ptr);
				printf("%s\n", server.data);
			}
			else {
				printf("nothing right came back");
			}
			
		}
		
		else if (strcmp(ptr, "logout") == 0) {
			
			//bool activeUser = false;
			//char *currentUser;
			
			if (activeUser == false) {
				printf("Cannot log anyone out since noone is currently logged in: \n");
			}
			else {
				
				
				// calculates size of data (JOIN)
				int data_string_size = strlen(currentUser) + 1;
				
				// creates memory for the string packet that needs to be sent
				char* packet_to_send = malloc(1000*sizeof(char));
			
				// fills out the string to be sent with the first four members, returning index at the end of four members
				int packet_members = sprintf(packet_to_send, "LOGOUT:%d:%s:%s", data_string_size, currentUser, currentUser);
				packet_to_send[packet_members] = '\0';

				// sets the members for the client message
				client.type = LOGOUT;
				client.size = data_string_size;
				strcpy(client.source, id);
				strcpy(client.data, packet_to_send);
				
				// connects to socket again to send data
				int sockfd = socket(AF_INET, SOCK_STREAM, 0);
				if (sockfd == -1) {
					printf("Error in making socket\n");
					return 0;
				}	
				int connecting = connect(sockfd, (struct sockaddr *)connecting_address, sizeof(struct sockaddr_in));
				if (connecting == -1) {
					printf("Error in making connecting\n");
					return 0;
				}	
			
				// writes the data to the socket
				write(sockfd, packet_to_send, 1000);
				
				// frees the stored data for the next iteration
				free(packet_to_send);
			
				char data[1000];
			
				// stores the data read in from the socket to data
				read(sockfd, data, 1000);
				
				// sets ptr to the first word of the command and stores it to typeS
				char *ptr = strtok(data, ":");
				type = malloc(sizeof(char) * (strlen(ptr)+1));
				strcpy(type, ptr);
				
				// fills out the information for the server message
				server.type = atoi(ptr);
				ptr = strtok(NULL, ":");
				server.size = atoi(ptr);
				ptr = strtok(NULL, ":");
				strcpy(server.source, ptr);
				
				// if the response is of type QUERY, print out the data (list)
				if (strcmp(type, "LOGOUT_ACK") == 0) {
					ptr = strtok(NULL, "\0");
					strcpy(server.data, ptr);
					printf("%s\n", ptr);
					
					activeUser = false;
					currentUser = NULL;
					
				}
				else if (strcmp(type, "LOGOUT_NAK") == 0){
					ptr = strtok(NULL, "\0");
					strcpy(server.data, ptr);
					printf("%s\n", ptr);
				}
				else {
					printf("nothing right came back");
				}
			}
			
		}
		
		
		else if (strcmp(ptr, "leavesession") == 0) {
			
			
			// calculates size of data (JOIN)
			int data_string_size = strlen(currentUser) + 1;
			
			// creates memory for the string packet that needs to be sent
			char* packet_to_send = malloc(1000*sizeof(char));
		
			// fills out the string to be sent with the first four members, returning index at the end of four members
			int packet_members = sprintf(packet_to_send, "LEAVE_SESS:%d:%s:%s", data_string_size, currentUser, currentUser);
			packet_to_send[packet_members] = '\0';

			// sets the members for the client message
			client.type = LEAVE_SESS;
			client.size = data_string_size;
			strcpy(client.source, currentUser);
			strcpy(client.data, packet_to_send);
			
			// connects to socket again to send data
			int sockfd = socket(AF_INET, SOCK_STREAM, 0);
			if (sockfd == -1) {
				printf("Error in making socket\n");
				return 0;
			}	
			int connecting = connect(sockfd, (struct sockaddr *)connecting_address, sizeof(struct sockaddr_in));
			if (connecting == -1) {
				printf("Error in making connecting\n");
				return 0;
			}	
		
			// writes the data to the socket
			write(sockfd, packet_to_send, 1000);
			
			// frees the stored data for the next iteration
			free(packet_to_send);
			
			char data[1000];
			
				// stores the data read in from the socket to data
				read(sockfd, data, 1000);
				
				// sets ptr to the first word of the command and stores it to typeS
				char *ptr = strtok(data, ":");
				type = malloc(sizeof(char) * (strlen(ptr)+1));
				strcpy(type, ptr);
				
				// fills out the information for the server message
				server.type = atoi(ptr);
				ptr = strtok(NULL, ":");
				server.size = atoi(ptr);
				ptr = strtok(NULL, ":");
				strcpy(server.source, ptr);
				
				// if the response is of type QUERY, print out the data (list)
				if (strcmp(type, "LEAVE_SESS") == 0) {
					ptr = strtok(NULL, "\0");
					strcpy(server.data, ptr);
					printf("%s\n", ptr);
					
				}
				else {
					printf("nothing right came back");
				}
			
			
			
		}


		
		else {
			printf("invalid input, try again: \n");
		}
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		/*

			int i = 0;
			while (i != 1000) {
				printf("%c", packet_to_send[i]);
				i++;
			}
			printf("\n");
		
		*/
		
		
		
		
		
		
		
		
		
		
		
		
		
		
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	// LOGIN <client ID, password> ----------------- Login with the server
	// LO_ACK <> ----------------------------------- Acknowledge successful login
	// LO_NAK <reason for failure> ----------------- Negative acknowledgment of login
	// EXIT <> ------------------------------------- Exit from server
	// JOIN <session ID> --------------------------- Join conference session
	// JN_ACK <session ID> ------------------------- Acknowledge successful conference session join
	// JN_NAK <session ID, reason for failure> ----- Acknowledge new conference session
	// LEAVE_SESS <> ------------------------------- Leave conference session
	// NEW_SESS <> --------------------------------- Create new conference session
	// NS_ACK <session ID> ------------------------- Acknowledge new conference session
	// MESSAGE <message data> ---------------------- Send message to session, or display if receieved
	// QEURY <> ------------------------------------ Get list of online users and available sessions
	// QU_ACK <users and sessions> ----------------- Reply followed by a list of users online
	
	
	
	
	return 0;
	
	
	
	
	
}