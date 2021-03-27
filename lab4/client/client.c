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





int main(int argc, char *argv[]) {
	
	
	// checks for 1 arguments being passed in "client"
	if (argc != 1) {
		fprintf(stderr, "usage: deliver <server address> <server port number>\n");
		return 0;
	}
	
	
	
	
	
	while (1) {
		
		printf("Enter your commands: \n");
		char input[1000];
		fgets(input, 1000, stdin);
        char *ptr = strtok(input, " ");
		if (strcmp(ptr, "login") == 0) {//login command
			
			printf("%s", ptr);
			int count = 1;
			char *portnum, *id, *pw, *ip;
			while (ptr != NULL) { 
			
				if (count == 1) { 
					ptr = strtok(NULL, " ");
					id = malloc(sizeof(char) * (strlen(ptr)+1));
					strcpy(id, ptr);
					printf("%s", id);
					count++;
				}

				else if (count == 2) { 
					ptr = strtok(NULL, " ");
					pw = malloc(sizeof(char) * (strlen(ptr)+1));
					strcpy(pw, ptr);
					count++;
					printf("%s", pw);
				}

				else if (count == 3) { 
					ptr = strtok(NULL, " ");
					ip = malloc(sizeof(char) * (strlen(ptr)+1));
					strcpy(ip, ptr);
					count++;
					printf("%s", ip);
				}

				else if (count == 4) { 
					ptr = strtok(NULL, " ");
					portnum = malloc(sizeof(char) * (strlen(ptr)+1));
					strcpy(portnum, ptr);
					printf("%s", portnum);
					break;
				}
			}
			
			// makes a  TCP socket
			int sockfd = socket(AF_INET, SOCK_STREAM, 0);
			if (sockfd == -1) {
				printf("Error in making socket\n");
				return 0;
			}	
			
			int port_num = atoi(portnum);
			struct sockaddr_in *connecting_address;
			connecting_address = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
			memset((char *)connecting_address, 0, sizeof(struct sockaddr_in));
			connecting_address->sin_family = AF_INET;
			connecting_address->sin_addr.s_addr = inet_addr(ip);
			connecting_address->sin_port = htons(port_num);
				
			
			
			int connecting = connect(sockfd, (struct sockaddr *)connecting_address, sizeof(struct sockaddr_in));
			if (connecting == -1) {
				printf("Error in making connecting\n");
				return 0;
			}	
			
			// account for the four ":"'s when calculating total size for packet
			int packet_string_size = strlen(id) + strlen(pw) + strlen(ip) + strlen(portnum) + 4 ;
			
			// creates memory for the string packet that needs to be sent
			char* packet_to_send = malloc(1000*sizeof(char));
		
			
			// fills out the string to be sent with the first four members, returning index at the end of four members
			
			int four_members = sprintf(packet_to_send, "%s:%s:%s:%s", id, pw, ip, portnum);
			packet_to_send[four_members+1] = '\0';
			
			int i = 0;
			
			while (i != 1000) {
				printf("%c", packet_to_send[i]);
				i++;
			}
			
			
			write(sockfd, packet_to_send, 1000);
			/*
			// sends packet to server
			int sent_packet = send(sockfd, packet_to_send, packet_string_size, 0);
			if (sent_packet == -1) {
				printf("Error in sending packet'\n");
				return 0;
			}
			else {
				printf("sending is gucci");
			}
*/
		}
		

		
		else {
			printf("invalid input, try again: \n");
		}
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
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
