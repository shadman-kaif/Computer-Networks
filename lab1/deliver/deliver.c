/*
** ECE361 - Computer Networks - Lab 1 - Client File - deliver.c
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
	// checks for 3 arguments being passed in <deliver> <server address> <port#>
	if (argc != 3) {
		fprintf(stderr, "usage: deliver <server address> <server port number>\n");
		return 0;
	}
	
	// stores arguments passed in to variables
	char* server_address = argv[1];
	char* port_num = argv[2];
	
	// asks user for file name to transfer
	printf("Enter file name to transfer:\n");
	
	// takes input and stores to given variables
	char ftp_string[75];
	char filename[75];
	scanf("%s %s", ftp_string, filename);
	
	// verifying the input for ftp
    if(strcmp(ftp_string, "ftp")!= 0){
        printf("Did not start with 'ftp' correctly\n", ftp_string);
        return 0;
    }
	
	// checks to see if filename exists in directory
	if (access(filename, F_OK) == -1) {
		printf("File does not exist\n");
		return 0;
	}

	// sets reference information for which socket type to use
	struct addrinfo hints; 
	
	// pointer to linked list of addrinfo's with addresses that match paramenters set by in hints
	struct addrinfo *res; 
	
	// resets hints and updates with desires parameters (IPv4 or IPv6, datagram, udp connection)
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	
	// fils out res with the given possible addresses to connect to
	int info = getaddrinfo(server_address, port_num, &hints, &res);
	
	// makes a socket connecting to the first address in res and checks for error
	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd == -1) {
		printf("Error in making socket\n");
		return 0;
	}
	
	// sends a "ftp" message to server and checks if it sent
	int sent_bytes = sendto(sockfd, "ftp", 3, 0, res->ai_addr, res->ai_addrlen);
	if (sent_bytes == -1) {
		printf("Error in sending 'ftp'\n");
		return 0;
	}
	
	// address info for the connection being made in order to receieve message from server
	struct sockaddr_storage connecting_address;
	socklen_t addr_len = sizeof connecting_address;
	char received_message[75];
	
	// receives message from server and error checks
	int rec_bytes = recvfrom(sockfd, received_message, 74 , 0, (struct sockaddr *)&connecting_address, &addr_len);
	if (rec_bytes == -1) {
		printf("Error in receiving message from server\n");
		return 0;
	}
	
	// verifies "yes" has been receieved from the server
    if(strcmp(received_message, "yes") == 0){
        printf("A file transfer can start.\n");
        return 0;
    }
	else {
		printf("Did not receieve 'yes' from server\n");
		return 0;
	}
	
	// frees memory and closes connection
    freeaddrinfo(res);
    close(sockfd);
	
	return 0;
}