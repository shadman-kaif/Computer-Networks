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
#include <time.h>



// Struct for each individual packet to be sent
struct packet {
	unsigned int total_frag;
	unsigned int frag_no;	
	unsigned int size;
	char* filename;
	char filedata[1000];
	struct packet * next;
};



int main(int argc, char *argv[])
{
	
	//*************************************************************************************
	//                                     PART 1
	//*************************************************************************************
	
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
	
	// Initialize the start time before sending the file and records time before sending
	clock_t start_time, end_time;
	start_time = clock();
	
	
	// sends a "ftp" message to server and checks if it sent
	int sent_bytes = sendto(sockfd, "ftp", 3, 0, res->ai_addr, res->ai_addrlen);
	if (sent_bytes == -1) {
		printf("Error in sending 'ftp'\n");
		return 0;
	}
	
	// address info for the connection being made in order to receieve message from server
	struct sockaddr_storage connecting_address;
	socklen_t addr_len = sizeof connecting_address;
	char received_message[1000];
	
	// receives message from server and error checks
	int rec_bytes = recvfrom(sockfd, received_message, 74 , 0, (struct sockaddr *)&connecting_address, &addr_len);
	
	// Records time when receiving confirmation from server
	end_time = clock();
	
	// Initializes and stores the amount of time the total transfer took
	float total_time;
	total_time = (float)(end_time - start_time)/CLOCKS_PER_SEC;
	printf("Round trip time for connection: %f \n", total_time);
	
	if (rec_bytes == -1) {
		printf("Error in receiving message from server\n");
	}
	
	// verifies "yes" has been receieved from the server
    if(strcmp(received_message, "yes") == 0){
        printf("A file transfer can start.\n");
    }
	else {
		printf("Did not receieve 'yes' from server\n");
	}
	
	received_message[rec_bytes] = '\0';
	
	
	//*************************************************************************************
	//                                     PART 2
	//*************************************************************************************
	
	
	
	// Go through every packet in linked list
	// For every packet
	//      Convert into string
	//      Send to server
	//      Receive ACK
	//      Proceed to next packet
	
	struct packet *linked_list_packet(char *filename_test) {
		// reads the file in binary mode using the filename passed in
		FILE *file = fopen(filename_test, "rb");
		
		// Goes to the end of the file to calculate size of file
		fseek(file, 0, SEEK_END);
		int f_size = ftell(file);
		
		// Goes back to beginning of file to begin breaking up into packets
		fseek(file, 0, SEEK_SET);
		
		// Using 1000 as max packet size, find out how many packets are needed for the file, and create data structure for it
		int num_of_fragments = (f_size/1000) + 1;
		char data_for_packet[1000];

		// Creates pointer to store previous packet, store head packet, all to connect linked_list of packets
		struct packet * prev_packet, * head_packet, * curr_packet;
		
		// Creates linked list adding packets with every iteration
		for (int i = 1; i <= num_of_fragments; i++) {
			
			// Creates a new packet
			struct packet * new_packet = malloc(sizeof(struct packet));
			
			// If this is the first packet, set the first packet as head, otherwise connect previous packet to this new one
			if (i == 1) {
				head_packet = new_packet;
			}
			else {
				prev_packet->next = new_packet;
			}
			
			// Stores values for this given packet
			new_packet->total_frag = num_of_fragments;
			new_packet->frag_no = i;
			new_packet->size = fread(data_for_packet, 1, 1000, file);
			new_packet->filename = filename_test;
			memcpy(new_packet->filedata, data_for_packet, fread(data_for_packet, 1, 1000, file));
			new_packet->next = NULL;
			
			// Changes previous packet pointer to new packet
			prev_packet = new_packet;
		}
		
		// Close file that was being
		fclose(file);
		return head_packet;
	}
	
	
	// total_frag : frag_no : size : filename : filedata
	struct packet *curr_packet = linked_list_packet(filename);
	
	char *structtostring(struct packet *p, int *length) {
				
		// Calculates size of each individual member of the packet to find total length of packet
		int s1 = snprintf(NULL, 0, "%d", curr_packet->total_frag);
		int s2 = snprintf(NULL, 0, "%d", curr_packet->frag_no);
		int s3 = snprintf(NULL, 0, "%d", curr_packet->size);
		int s4 = strlen(curr_packet->filename);
		int s5 = curr_packet->size;
		
		// Account for the four ":" when calculating total size for packet
		int packet_string_size = s1 + s2 + s3 + s4 + s5 + 4;
		
		// Creates memory for the string packet that needs to be sent
		char *packet_to_send = malloc(packet_string_size*sizeof(char));
		
		int four_members = sprintf(packet_to_send, "%d:%d:%d:%s:", curr_packet->total_frag, curr_packet->frag_no, curr_packet->size, curr_packet->filename);
		
		memcpy(&packet_to_send[four_members], curr_packet->filedata, curr_packet->size);
		*length = four_members + curr_packet->size;
		return packet_to_send;
	}

	int length;
	
	while (curr_packet != NULL) {
		
		//Converting packet from struct to string format
        char *packet_to_send = structtostring(curr_packet, &length);
		
		// sends packet to server
		rec_bytes = sendto(sockfd, packet_to_send, length, 0, res->ai_addr, res->ai_addrlen);
		/*if (sent_packet == -1) {
			printf("Error in sending packet'\n");
			return 0;
		}*/
		
		
		// receives message from server and error checks
		rec_bytes = recvfrom(sockfd, received_message, 999 , 0, (struct sockaddr *)&connecting_address, &addr_len);
		received_message[rec_bytes] = '\0';

/*
		received_message[rec_bytes] = '\0';

		if (strcmp(received_message, "ACK") != 0)
			continue;
			
		curr_packet = curr_packet->next;
		free(packet_to_send);
		
	
		*/
		
		
		
		/*if (rec_bytes == -1) {
			printf("Error in receiving ACK message from server\n");
			return 0;
		}*/
		
		// verifies "yes" has been receieved from the server
		if(strcmp(received_message, "ACK") != 0){
			continue;
		}
		
		curr_packet = curr_packet->next;
		free(packet_to_send);
		
	}

	
	// fees memory and closes connection
    freeaddrinfo(res);
    close(sockfd);
	
	return 0;
}

