/*
** ECE361 - Computer Networks - Lab 1 - Server File - server.c
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



typedef struct user_info {
	char id[1000];
	char pass[1000];
	bool signedIn;
	char *sessionID;
}USER_INFO;

USER_INFO listOfUsers[5];



int main(int argc, char *argv[]){
	
	bool loggedIn = false;
	
	// checks for 2 arguments being passed in <server> <port #>
	if (argc != 2) {
		fprintf(stderr, "usage: server <server port number>\n");
		return 0;
	}
	
	// creation of the user list
	strcpy(listOfUsers[0].id, "User0");
	strcpy(listOfUsers[1].id, "User1");
	strcpy(listOfUsers[2].id, "User2");
	strcpy(listOfUsers[3].id, "User3");
	strcpy(listOfUsers[4].id, "User4");

	strcpy(listOfUsers[0].pass, "0000");
	strcpy(listOfUsers[1].pass, "1111");
	strcpy(listOfUsers[2].pass, "2222");
	strcpy(listOfUsers[3].pass, "3333");
	strcpy(listOfUsers[4].pass, "4444");
	
	listOfUsers[0].signedIn = false;
	listOfUsers[1].signedIn = false;
	listOfUsers[2].signedIn = false;
	listOfUsers[3].signedIn = false;
	listOfUsers[4].signedIn = false;
	
	listOfUsers[0].sessionID = NULL;
	listOfUsers[1].sessionID = NULL;
	listOfUsers[2].sessionID = NULL;
	listOfUsers[3].sessionID = NULL;
	listOfUsers[4].sessionID = NULL;
	
	char *sessionsList[5];
	for (int i = 0; i < 5; i++) {
		sessionsList[i] = NULL;
	}
	
	
	
	
	
	// stores arguments passed in to variables
	char* server_string = argv[0];
	char* port_num = argv[1];
	
	// makes a  TCP socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("Error in making socket\n");
		return 0;
	}
	
	// sets parameters for the connection
	struct sockaddr_in connecting_address;
	connecting_address.sin_family = AF_INET;
	connecting_address.sin_addr.s_addr = INADDR_ANY;
	connecting_address.sin_port = htons(atoi(port_num));
	
	// binds the socket with the respective IP address and port #
	int bind_info = bind(sockfd, (struct sockaddr *)&connecting_address, sizeof(connecting_address));
	if (bind_info == -1) {
		printf("Error in binding\n");
		return 0;
	}
	
	// listens repeatedly as long as server is open
	while (1) {
		
		// listens for packet from client
		printf("listening: \n");
		int l = listen(sockfd, 3);
		if (l == -1) {
			printf("chopped listing");
		}
		
		// accepts the packet from the client
		int clientLen = sizeof(struct sockaddr_in);
		struct sockaddr_in cl;		
		int sock = accept(sockfd, (struct sockaddr *)&cl, (socklen_t*)&clientLen);
		if (sock == -1) {
			printf("Error in accepting\n");
			return 0;
		}
		printf("accepted packet, proceeding: \n");
		
		// initializes structs and variables for the messages
		struct message client;
		struct message server;
		char *type;
		char data[1000];
		
		// stores the data read in from the socket to data
		read(sock, data, 1000);
		
		
		// sets ptr to the first word of the command and stores it to typeS
		char *ptr = strtok(data, ":");
		type = malloc(sizeof(char) * (strlen(ptr)+1));
		strcpy(type, ptr);
		
		// fills the rest of the client message struct excluding data
		client.type = atoi(ptr);
		ptr = strtok(NULL, ":");
		client.size = atoi(ptr);
		ptr = strtok(NULL, ":");
		strcpy(client.source, ptr);
		
		// if the client message is a LOGIN message, then...
		if (strcmp(type, "LOGIN") == 0) {
			
			if (loggedIn == true) {
				printf("We already have a logged in user\n");
			}
			
			// goes through the rest of the command line and extracts information
			int count = 1;
			char *id, *pw;
			
			// goes to the first word after login
			ptr = strtok(NULL, ",");

			// traverses through the rest of the message and extracts information
			while (ptr != NULL) { 
				
				// extracts the id of the user
				if (count == 1) { 
					id = malloc(sizeof(char) * (strlen(ptr)+1));
					strcpy(id, ptr);
					ptr = strtok(NULL, "\0");
					count++;
				}
				
				// extracts password of the user
				else if (count == 2) { 
					pw = malloc(sizeof(char) * (strlen(ptr)+1));
					strcpy(pw, ptr);
					break;
				}
			}
			
			// sets default flags
			bool foundUser = false;
			bool correctPass = false;
			bool newLogin = false;
			
			// checks to see if given user and pw can sign in or not
			for (int i = 0; i < 5; i++) {
				if (strcmp(id, listOfUsers[i].id) == 0) {
					foundUser = true;
					if (strcmp(pw, listOfUsers[i].pass) == 0) {
						correctPass = true;
						if (listOfUsers[i].signedIn == false) {
							listOfUsers[i].signedIn = true;
							newLogin = true;
						}
					}
				}
			}
			
			// creates memory for the string packet that needs to be sent
			char* packet_to_send = malloc(1000*sizeof(char));
		
			// creates packet with ACK or NAK depending on if the right information was given
			if (foundUser == true) {
				if (correctPass == true) {
					
					// send ACK for a successful log in
					if (newLogin == true) {
						server.type = LO_ACK;
						strcpy(server.data, "Logged In");
						strcpy(server.source, client.source);
						int data_string_size = strlen(server.data);
						int packet_members = sprintf(packet_to_send, "LO_ACK:%d:%s:%s", data_string_size, server.source, server.data);
						packet_to_send[packet_members+1] = '\0';
						loggedIn = true;
					}
					// sends NAK if the user has already signed in previously
					else {
						server.type = LO_NAK;
						strcpy(server.data, "Already signed in:");
						strcpy(server.source, client.source);
						int data_string_size = strlen(server.data);
						int packet_members = sprintf(packet_to_send, "LO_NAK:%d:%s:%s", data_string_size, server.source, server.data);
						packet_to_send[packet_members+1] = '\0';
					}
				}
				
				// sends NAK if valid user inputted incorrect password
				else {
					server.type = LO_NAK;
					strcpy(server.data, "User found. Incorrect Password. Try again: ");
					strcpy(server.source, client.source);
					int data_string_size = strlen(server.data);
					int packet_members = sprintf(packet_to_send, "LO_NAK:%d:%s:%s", data_string_size, server.source, server.data);
					packet_to_send[packet_members+1] = '\0';
				}
			}
			
			// sends NAK if given username does not exist
			else {
				server.type = LO_NAK;
				strcpy(server.data, "User not found. Try again: ");
				strcpy(server.source, client.source);
				int data_string_size = strlen(server.data);
				int packet_members = sprintf(packet_to_send, "LO_NAK:%d:%s:%s", data_string_size, server.source, server.data);
				packet_to_send[packet_members+1] = '\0';
			}
			
			// sets size of the server message
			server.size = 1000;
			
			// writes the data to the socket for the client to read
			write(sock, packet_to_send, 1000);
			
			// frees the stored data for the next iteration
			free(packet_to_send);
		}
		
		// if the client message is a QUIT message, then...
		else if (strcmp(type, "QUIT") == 0) {
			
			// detects the rest of the message to get the data
			ptr = strtok(NULL, "\0");
			strcpy(client.data, ptr);
			
			// attemps to close the server
			printf("Attempting to quit: \n");
			close(sockfd);
			printf("Successfully closed\n");
			return 0;
		}
		
		else if (strcmp(type, "NEW_SESS") == 0) {
			
			// detects the rest of the message to get the data
			char *sessionID;
			ptr = strtok(NULL, "\0");
			sessionID = malloc(sizeof(char) * (strlen(ptr)+1));
			strcpy(sessionID, ptr);
			
			// checks if the session exists already
			bool found = false;
			bool created = false;
			for (int i = 0; i < 5; i++) {
				if (sessionsList[i] != NULL) {
					if (strcmp(sessionsList[i], sessionID) == 0)
					found = true;
				}
			}
			
			// create new session if not found and if there is space for a new session
			if (found == false) {
				for (int i = 0; i < 5; i++) {
					if (sessionsList[i] == NULL) {
						sessionsList[i] = malloc(sizeof(char) * (strlen(ptr)+1));
						strcpy(sessionsList[i], sessionID);
						created = true;
						break;
					}
				}
			}
			
			// creates memory for the string packet that needs to be sent
			char* packet_to_send = malloc(1000*sizeof(char));
			
			if (found == false) {
				if (created == true) {
					server.type = NS_ACK;
					strcpy(server.data, sessionID);
					strcpy(server.source, client.source);
					int data_string_size = strlen(server.data);
					int packet_members = sprintf(packet_to_send, "NS_ACK:%d:%s:%s", data_string_size, server.source, sessionID);
					packet_to_send[packet_members+1] = '\0';
				}
				else {
					server.type = NS_NAK;
					strcpy(server.data, "There are not enough rooms available for the new session. Please try again:");
					strcpy(server.source, client.source);
					int data_string_size = strlen(server.data);
					int packet_members = sprintf(packet_to_send, "NS_NAK:%d:%s:%s", data_string_size, server.source, server.data);
					packet_to_send[packet_members+1] = '\0';
				}
			}
			else {
				server.type = NS_NAK;
				strcpy(server.data, "A session already exists with the same name. Please try again: ");
				strcpy(server.source, client.source);
				int data_string_size = strlen(server.data);
				int packet_members = sprintf(packet_to_send, "NS_NAK:%d:%s:%s", data_string_size, server.source, server.data);
				packet_to_send[packet_members+1] = '\0';
			}
			
			// sets size of the server message
			server.size = 1000;
			
			// writes the data to the socket for the client to read
			write(sock, packet_to_send, 1000);
			
			// frees the stored data for the next iteration
			free(packet_to_send);
		}
		
		
		else if (strcmp(type, "JOIN") == 0) {
			
			// detects the rest of the message to get the data
			char *sessionID_joined;
			ptr = strtok(NULL, "\0");
			sessionID_joined = malloc(sizeof(char) * (strlen(ptr)+1));
			strcpy(sessionID_joined, ptr);
			
			// Create boolean to check if a session is joinable or not
			bool joinable = false;

			// Go through list of sessions and check if the joined session exists
			for (int i = 0; i < 5; i++) {
				if (sessionsList[i] != NULL) {
					if (strcmp(sessionsList[i], sessionID_joined) == 0) {
						joinable = true;
					
						for (int j = 0; j < 5; j++) {
							if (listOfUsers[j].id != NULL) {
								if (strcmp(listOfUsers[j].id, client.source) == 0) {
									listOfUsers[j].sessionID = malloc(sizeof(char) * (strlen(sessionID_joined)+1));
									strcpy(listOfUsers[j].sessionID, sessionID_joined);
									printf("stores %s into: %s\n", listOfUsers[j].id, listOfUsers[i].sessionID);
									break;
								}
							}
						}
					}
				}
			}
						
			// creates memory for the string packet that needs to be sent
			char* packet_to_send = malloc(1000*sizeof(char));
			
			// If the session can be joined
			if (joinable == true) {
				server.type = JN_ACK;
				strcpy(server.source, client.source);
				int data_string_size = strlen(server.data);
				int packet_members = sprintf(packet_to_send, "JN_ACK:%d:%s:%s", data_string_size, server.source, sessionID_joined);
				packet_to_send[packet_members+1] = '\0';
			}
			
			// If the session cannot be joined
			else {
				server.type = JN_NAK;
				strcpy(server.data, "The session has not been created yet. Please try again: ");
				strcpy(server.source, client.source);
				int data_string_size = strlen(server.data);
				int packet_members = sprintf(packet_to_send, "JN_NAK:%d:%s:%s", data_string_size, server.source, server.data);
				packet_to_send[packet_members+1] = '\0';
			}
		
			
			// sets size of the server message
			server.size = 1000;
			
			// writes the data to the socket for the client to read
			write(sock, packet_to_send, 1000);
			
			// frees the stored data for the next iteration
			free(packet_to_send);
		}
		
		// if the client message is a LIST message, then...
		else if (strcmp(type, "LIST") == 0) {
		
			// creates memory for the string packet that needs to be sent and temp list string
			char* packet_to_send = malloc(1000*sizeof(char));
			char* creation_of_list = malloc(1000 *sizeof(char));
			
			strcat(creation_of_list, "List of available sessions: \n");
			
			for (int i = 0; i < 5; i++) {
				if (sessionsList[i] != NULL) {
					
					char info[100];
					
					sprintf(info, "%s", sessionsList[i]);
					
					// attach to the list string
					strcat(creation_of_list, info);
				}
			}
			
			strcat(creation_of_list, "Here are the users online: \n");
			
			// goes through list of users to see who has logged in
			for (int i = 0; i < 5; i++) {
				if (listOfUsers[i].signedIn == true) {
					
					char info[100];
					// display session number for each logged in user, unless they are not in any sessions
					if (listOfUsers[i].sessionID == NULL) {
						sprintf(info, "%s: Not in any session\n", listOfUsers[i].id);
					}
					else {
						sprintf(info, "%s: In session %s\n", listOfUsers[i].id, listOfUsers[i].sessionID);
					}
					
					// attach to the list string
					strcat(creation_of_list, info);
				}
			}
			
			// fills out the packet to be sent with the necessary information
			int packet_members = sprintf(packet_to_send, "QU_ACK:%d:%s:%s", strlen(creation_of_list), client.source, creation_of_list);
			packet_to_send[packet_members+1] = '\0';
			
			// updates server message information
			server.type = QU_ACK;
			server.size = strlen(creation_of_list);
			strcpy(server.source, client.source);
			strcpy(server.data, packet_to_send);
			
			// writes the data to the socket for the client to read
			write(sock, packet_to_send, 1000);
			
			// frees the stored data for the next iteration
			free(packet_to_send);
			free(creation_of_list);
		}
		
		
		else if (strcmp(type, "LOGOUT") == 0) {
			
			// detects the rest of the message to get the data
			char *currentUser;
			ptr = strtok(NULL, "\0");
			currentUser = malloc(sizeof(char) * (strlen(ptr)+1));
			strcpy(currentUser, ptr);
			
			bool signedOut = false;
			for (int i = 0; i < 5; i++) {
				if (listOfUsers[i].id != NULL) {
					if (strcmp(listOfUsers[i].id, currentUser) == 0) {
						listOfUsers[i].signedIn = false;
						listOfUsers[i].sessionID = NULL;
						signedOut = true;
					}
				}
			}
			
			// creates memory for the string packet that needs to be sent and temp list string
			char* packet_to_send = malloc(1000*sizeof(char));
			
			if (signedOut == true) {
				server.type = LOGOUT;
				strcpy(server.data, "Succesfully logged out: ");
				strcpy(server.source, client.source);
				int data_string_size = strlen(server.data);
				int packet_members = sprintf(packet_to_send, "LOGOUT_ACK:%d:%s:%s", data_string_size, server.source, server.data);
				packet_to_send[packet_members+1] = '\0';
			}
			else {
				server.type = LOGOUT;
				strcpy(server.data, "Did not log you out. Please try again: ");
				strcpy(server.source, client.source);
				int data_string_size = strlen(server.data);
				int packet_members = sprintf(packet_to_send, "LOGOUT_NAK:%d:%s:%s", data_string_size, server.source, server.data);
				packet_to_send[packet_members+1] = '\0';
			}
			
			// sets size of the server message
			server.size = 1000;
			
			// writes the data to the socket for the client to read
			write(sock, packet_to_send, 1000);
			
			// frees the stored data for the next iteration
			free(packet_to_send);
			
		}
		
		
		
		else if (strcmp(type, "LEAVE_SESS") == 0) {
			
			// detects the rest of the message to get the data
			char *currentUser;
			ptr = strtok(NULL, "\0");
			currentUser = malloc(sizeof(char) * (strlen(ptr)+1));
			strcpy(currentUser, ptr);
			
			bool leftSuccessfully = false;
			bool inSession;
			
			
			// creates memory for the string packet that needs to be sent and temp list string
			char* packet_to_send = malloc(1000*sizeof(char));
			
			
			
			for (int i = 0; i < 5; i++) {
				if (strcmp(listOfUsers[i].id, client.source) == 0) {
					if (listOfUsers[i].sessionID == NULL) {
						
						
						server.type = LEAVE_SESS;
						strcpy(server.data, "User is not in any session yet, so cannot leave:");
						strcpy(server.source, client.source);
						int data_string_size = strlen(server.data);
						int packet_members = sprintf(packet_to_send, "LEAVE_SESS:%d:%s:%s", data_string_size, server.source, server.data);
						packet_to_send[packet_members+1] = '\0';
						
						
					}
					else {
						listOfUsers[i].sessionID = NULL;
						
						server.type = LEAVE_SESS;
						strcpy(server.data, "Removed from session:");
						strcpy(server.source, client.source);
						int data_string_size = strlen(server.data);
						int packet_members = sprintf(packet_to_send, "LEAVE_SESS:%d:%s:%s", data_string_size, server.source, server.data);
						packet_to_send[packet_members+1] = '\0';
					}
				}
			}
			
			// sets size of the server message
			server.size = 1000;
			
			// writes the data to the socket for the client to read
			write(sock, packet_to_send, 1000);
			
			// frees the stored data for the next iteration
			free(packet_to_send);
			
			
			
			
		}
	

		
		else {
			printf("rando shit");
		}
	}
}