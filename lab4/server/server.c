*
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



int main(int argc, char *argv[]){
	
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
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;     
	
	// obtains address info
	int info = getaddrinfo(NULL, port_num, &hints, &res);	
	
	// makes a  TCP socket
	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
	
	char data[1100];
	// address info for the connection being made in order to receieve "ftp"
	struct sockaddr_in connecting_address;
	socklen_t addr_len = sizeof connecting_address;
	
	// detects the incoming packet from the client and error checks
	int received_bytes_temp = recvfrom(sockfd, data, 1100, 0, (struct sockaddr *)&connecting_address, &addr_len);
	if (received_bytes_temp == -1) {
		printf("Error in receiving the packet message\n");
		return 0;
	}
	
	// conversion of the received packet string into the packet struct using strtok and ":" as a delimiter
	char * id = strtok(data, ":"); 
	char * pw = strtok(NULL, ":");
	char * ip = strtok(NULL, ":");
	char * portnum = strtok(NULL, ":");
	
	printf("%s and %s and %s and %s", id, pw, ip, portnum);

}