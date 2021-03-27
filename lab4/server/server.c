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



int main(int argc, char *argv[]){
	
	// checks for 2 arguments being passed in <server> <port #>
	if (argc != 2) {
		fprintf(stderr, "usage: server <server port number>\n");
		return 0;
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
	printf("before listen\n");
	int l = listen(sockfd, 3);
	if (l == -1) {
		printf("chopped listing");
	}
	printf("after listen\n");
	int clientLen = sizeof(struct sockaddr_in);
	struct sockaddr_in client;
	printf("before accept\n");
	
	
	int sock = accept(sockfd, (struct sockaddr *)&client, (socklen_t*)&clientLen);
	if (sock == -1) {
		printf("Error in accepting\n");
		return 0;
	}
	else {
		printf("gucci");
	}
	
		printf("after accept\n");

	
	
	char data[1000];
	read(sockfd, data, 1000);
	
	/*
	int i = 0;
	while(i != 999) {
		printf("%d", data[i]);
		i++;
	}
	
	*/
	
	
	
	
	
	int count = 1;
	char *portnum, *id, *pw, *ip;
	char *ptr = strtok(data, ":");
	while (ptr != NULL) { 
	
		if (count == 1) { 
			
			id = malloc(sizeof(char) * (strlen(ptr)+1));
			strcpy(id, ptr);
			printf("%s a\n", id);
			ptr = strtok(NULL, ":");
			count++;
		}

		else if (count == 2) { 
			
			pw = malloc(sizeof(char) * (strlen(ptr)+1));
			strcpy(pw, ptr);
			count++;
			ptr = strtok(NULL, ":");
			printf("%s b\n", pw);
		}

		else if (count == 3) { 
			
			ip = malloc(sizeof(char) * (strlen(ptr)+1));
			strcpy(ip, ptr);
			count++;
			ptr = strtok(NULL, ":");
			printf("%s c\n", ip);
		}

		else if (count == 4) { 
			
			portnum = malloc(sizeof(char) * (strlen(ptr)+1));
			strcpy(portnum, ptr);
			printf("%s d\n", portnum);
			break;
		}
	}
	
	printf("%s and %s and %s and %d", &id, &pw, &ip, &portnum);
	
	
	
	
	
	/*
	char data[1100] = {0};
	memset(data, '\0', sizeof data);
	int rec_bytes = recv(sockfd, data, 1100, 0);
	if (rec_bytes == -1) {
		printf("Error in receiving\n");
		return 0;
	}
	else {
		printf("receiving is gucci");
	}
	
	
	*/
	
	
	
	
	
	
	
	
	
	
	/*
	
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
	char ftp_receive[4];
	
	// receives message from connection and error checks
	int rec_bytes = recvfrom(sockfd, ftp_receive, 4, 0, (struct sockaddr *)&connecting_address, &addr_len);
	if (rec_bytes == -1) {
		printf("Error in receiving\n");
		return 0;
	}
	
	printf("here");
	
	// address info for the connection being made in order to receieve "ftp"
	struct sockaddr_in connecting_address;
	socklen_t addr_len = sizeof connecting_address;
	printf("here");
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
	*/
}
