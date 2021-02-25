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

// Struct for each individual packet to be sent
struct packet {
	unsigned int total_frag;
	unsigned int frag_no;	
	unsigned int size;
	char* filename;
	char filedata[1000];
	struct packet* next;
};

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
			printf("Error in sending back the 'no' message\n");
			return 0;
		}
	}

	FILE * file;
	char data[1100] = {0};
	bool flag = true;
	
	while (flag) {
		int received_bytes_temp = recvfrom(sockfd, data, 1100, 0, (struct sockaddr *)&connecting_address, &addr_len);
		
		if (received_bytes_temp == -1) {
			printf("Error in receiving the packet message\n");
			return 0;
		}

		int sending_ack = sendto(sockfd, "ACK", 75, 0, (struct sockaddr *)&connecting_address, addr_len);
		if (sending_ack == -1) {
			printf("Error in sending the 'ACK' message\n");
			return 0;
		}
		
		// Conversion from string back to struct
		char * total_frags = strtok(data, ":"); 
		char * frag_nos = strtok(NULL, ":");
		char * sizes = strtok(NULL, ":");
		char * file_name = strtok(NULL, ":");
		
		int total_frag_final = atoi(total_frags), frag_num_final = atoi(frag_nos), size_final = atoi(sizes);
		
		int index_before_fd = 4 + strlen(total_frags) + strlen(frag_nos) + strlen(sizes) + strlen(file_name);
		
		char * temp = malloc(size_final * sizeof(char));
		
		memcpy(temp, &data[index_before_fd], size_final);
		
		struct packet * curr_packet = malloc(sizeof(struct packet));
		curr_packet->total_frag = total_frag_final;
		curr_packet->frag_no = frag_num_final;
		curr_packet->size = size_final;
		curr_packet->filename = file_name;
		
		memcpy(curr_packet->filedata, temp, size_final);
		
		//if (size_final < 1000) {
			//curr_packet->filedata[size_final] = "\0";
		//}
		
		// Check if frag no. == total frag no. --> break inf loop
		if (frag_num_final == total_frag_final) {
			flag = false;
		}
		
		if (frag_num_final == 1) {
			file = fopen(file_name, "wb");
		}
		
		char* tempdata = curr_packet->filedata;
		
		// Write to file after opening file
		fwrite(tempdata, 1, size_final, file);
		
		
		free(curr_packet);
		
	}
	
	
	fclose(file);
	
	
	// closes connection
    close(sockfd);
	
    return 0;
}