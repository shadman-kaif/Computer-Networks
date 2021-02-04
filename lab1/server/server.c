/*
** ECE361 - Computer Networks - Lab 1 - Server File - server.c
** Group 21 - Abdurrafay Khan & Shadman Kaif
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
	// checks for 2 arguments being passed in <server> <port #>
	if (argc != 2) {
		fprintf(stderr, "usage: server <server port number>\n");
		return 0;
	}
	
	// stores arguments passed in to variables
	char* server_string = argv[0];
	char* port_num = argv[1];

	// initializes structs to be used in opening socket
	struct addrinfo hints; 
	struct addrinfo *res; 
	
	// resets hints and updates with desires parameters (IPv4 or IPv6, datagram, udp connection) and autofilled IP
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;     
	
	// obtains address info
	int info = getaddrinfo(NULL, port_num, &hints, &res);	
	
	// makes a UDP socket
	int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sockfd == -1) {
		printf("Error in making socket\n");
		return 0;
	}
	
	// binds the socket with the respective IP address and port #
	int bind_info = bind(sockfd, res->ai_addr, res->ai_addrlen);
	if (bind_info == -1) {
		printf("Error in binding\n");
		return 0;
	}
	
	// address info for the connection being made in order to receieve "ftp"
	struct sockaddr_in connecting_address;
	socklen_t addr_len = sizeof connecting_address;
	char received_message[75];
	
	// receives message from connection and error checks
	int rec_bytes = recvfrom(sockfd, received_message, 74 , 0, (struct sockaddr *)&connecting_address, &addr_len);
	if (rec_bytes == -1) {
		printf("Error in receiving\n");
		return 0;
	}
	
	// verifies "ftp" has been receieved from the client
    if(strcmp(received_message, "ftp") == 0){
		
		// sends back a "yes" message to client and checks if it sent
		int sent_bytes = sendto(sockfd, "yes", 75, 0, (struct sockaddr *)&connecting_address, addr_len);
		if (sent_bytes == -1) {
			printf("Error in sending back the 'yes' message\n");
			return 0;
		}
    }
	else {
		
		// if "yes" wasn't receieved, sends back a "no" message to client and checks if it sent
		int sent_bytes = sendto(sockfd, "no", 75, 0, (struct sockaddr *)&connecting_address, addr_len);
		if (sent_bytes == -1) {
			printf("Error in sending back the 'no' messsage\n");
			return 0;
		}
	}
	
	// closes connection
    close(sockfd);
	
    return 0;
}