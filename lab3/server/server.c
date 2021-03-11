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
	
	//*************************************************************************************
	//                                     PART 1
	//*************************************************************************************
	
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
	char ftp_receive[4];
	
	// receives message from connection and error checks
	int rec_bytes = recvfrom(sockfd, ftp_receive, 4, 0, (struct sockaddr *)&connecting_address, &addr_len);
	if (rec_bytes == -1) {
		printf("Error in receiving\n");
		return 0;
	}
	
	// verifies "ftp" has been receieved from the client
    if(strcmp(ftp_receive, "ftp") == 0){
		
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


	//*************************************************************************************
	//                                     PART 2 / 3
	//*************************************************************************************
	
	// initializes file structure and necessary variables to detect all packets and write to file
	FILE * file;
	char data[1100];
	bool flag = true;
	int count = 1;
	
	// repeats continuously until all packets have been receieved
	while (flag) {
		
		// detects the incoming packet from the client and error checks
		int received_bytes_temp = recvfrom(sockfd, data, 1100, 0, (struct sockaddr *)&connecting_address, &addr_len);
		if (received_bytes_temp == -1) {
			printf("Error in receiving the packet message\n");
			return 0;
		}
		
		// conversion of the received packet string into the packet struct using strtok and ":" as a delimiter
		char * total_frags = strtok(data, ":"); 
		char * frag_nos = strtok(NULL, ":");
		char * sizes = strtok(NULL, ":");
		char * file_name = strtok(NULL, ":");
		
		// converts the string values detected into their corresponding integer values
		int total_frag_final = atoi(total_frags), frag_num_final = atoi(frag_nos), size_final = atoi(sizes);
		
		// calculates at what index the data string starts at and copies the data string into its respective variable
		int index_before_fd = 4 + strlen(total_frags) + strlen(frag_nos) + strlen(sizes) + strlen(file_name);
		char * data_string = malloc(size_final * sizeof(char));
		memcpy(data_string, &data[index_before_fd], size_final);
	
		// creates a packet and fills it with its corresponding members
		struct packet * curr_packet = malloc(sizeof(struct packet));
		curr_packet->total_frag = total_frag_final;
		curr_packet->frag_no = frag_num_final;
		curr_packet->size = size_final;
		curr_packet->filename = file_name;
		memcpy(curr_packet->filedata, data_string, size_final);
		char* data_to_write = curr_packet->filedata;
		
		// if the last packet has been receieved, exit the loop
		if (frag_num_final == total_frag_final) {
			flag = false;
		}
		
		// if this is the first packet, then open a file so that the incoming data can be written into it
		if (count == 1) {
			file = fopen(file_name, "wb");
		}
		
		// only writes to file if the correct packet number is receieved
		if (count == frag_num_final) {
			
			// writes data to file
			fwrite(data_to_write, 1, size_final, file);
			
			// sends back an "ACK" message to acknowledge packet has been receieved, and error checks
			int sending_ack = sendto(sockfd, "ACK", 75, 0, (struct sockaddr *)&connecting_address, addr_len);
			if (sending_ack == -1) {
				printf("Error in sending the 'ACK' message\n");
				return 0;
			}
			
			// increments counter to only write subsequent packet the next iteration
			count = count + 1;
		}
		
		// frees the packet struct before moving onto the next one
		free(curr_packet);
	}
	
	// closes file, connection
	fclose(file);
    close(sockfd);
	
    return 0;
}