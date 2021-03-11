/*
** ECE361 - Computer Networks - Lab 1 - Client File - deliver.c
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

// struct for each individual packet to be sent
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
	
	// initialize the start time before sending the file and records time before sending
	clock_t start_time, end_time, start_op, end_op;
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
	
	// receives message from server 
	int rec_bytes = recvfrom(sockfd, received_message, 75 , 0, (struct sockaddr *)&connecting_address, &addr_len);
	
	// records time when receiving confirmation from server
	end_time = clock();
	
	// initializes and stores the amount of time the total transfer took
	float total_time, initial_rtt, total_op;
	total_time = (float)(end_time - start_time)/CLOCKS_PER_SEC;
	initial_rtt = total_time;
	printf("Round trip time for connection: %f \n", total_time);
	
	// error checks the message receieved from server and verifies "yes" has been receieved from the server
	if (rec_bytes == -1) {
		printf("Error in receiving message from server\n");
	}
    if(strcmp(received_message, "yes") == 0){
        printf("A file transfer can start.\n");
    }
	else {
		printf("Did not receieve 'yes' from server\n");
	}
	
	
	//*************************************************************************************
	//                                     PART 2 / 3
	//*************************************************************************************
	
	
	// reads the file in binary mode using the filename passed in
	FILE* file = fopen(filename, "rb");
	
	// goes to the end of the file to calculate size of file
	fseek(file, 0, SEEK_END);
	int f_size = ftell(file);
	
	// goes back to beginning of file to begin breaking up into packets
	fseek(file, 0, SEEK_SET);
	
	// using 1000 as max packet size, find out how many packets are needed for the file, and create data structure for it
	int num_of_fragments = (f_size/1000) + 1;
	char data_for_packet[1000];

	// creates pointer to store previous packet, store head packet, all to connect linked_list of packets
	struct packet *prev_packet, *head_packet, *curr_packet;
	
	// creates linked list adding packets with every iteration
	for (int i = 1; i <= num_of_fragments; i++) {
		
		// creates a new packet with respective memory allocation
		struct packet *new_packet = malloc(sizeof(struct packet));
		
		// sets first packet to head, otherwise connect previous packet to this new one
		if (i == 1) {
			head_packet = new_packet;
		}
		else {
			prev_packet->next = new_packet;
		}
		
		// stores values for this given packet, reads the first 1000 bytes from file as the data
		new_packet->total_frag = num_of_fragments;
		new_packet->frag_no = i;
		int n = fread(data_for_packet, 1, 1000, file);
		new_packet->size = n;
		new_packet->filename = filename;
		memcpy(new_packet->filedata, data_for_packet, n);
		new_packet->next = NULL;
		
		// Changes previous packet pointer to new packet
		prev_packet = new_packet;
	}
	
	// close file that was being broken into packets
	fclose(file);
	
	// start at head of the linked list to send to server, initialize a string to read ACK messages from server
	curr_packet = head_packet;
	char received_message2[1000];
	int sent_count;

	// sets up initial timeout value
	float devRTT = total_time;
	float sampleRTT = total_time;
	float timeout_value = sampleRTT + 4*devRTT;
	
	// goes through linked list of all packets in order to send each, and receieve corresponding ACK messages
	while (curr_packet != NULL) {
		
		// calculates size of each individual member of the packet to find total length of packet
		int s1 = snprintf(NULL, 0, "%d", curr_packet->total_frag);
		int s2 = snprintf(NULL, 0, "%d", curr_packet->frag_no);
		int s3 = snprintf(NULL, 0, "%d", curr_packet->size);
		int s4 = strlen(curr_packet->filename);
		int s5 = curr_packet->size;
		
		// account for the four ":"'s when calculating total size for packet
		int packet_string_size = s1 + s2 + s3 + s4 + s5 + 4;
		
		// creates memory for the string packet that needs to be sent
		char* packet_to_send = malloc(packet_string_size*sizeof(char));
		
		// fills out the string to be sent with the first four members, returning index at the end of four members
		int four_members = sprintf(packet_to_send, "%d:%d:%d:%s:", curr_packet->total_frag, curr_packet->frag_no, curr_packet->size, curr_packet->filename);
		
		// copies the data into the string starting from the index calculated above
		memcpy(&packet_to_send[four_members], curr_packet->filedata, curr_packet->size);
		
		start_time = clock();
		if (curr_packet->frag_no == 1) {
			start_op = clock();
		}

		// sends packet to server
		int sent_packet = sendto(sockfd, packet_to_send, packet_string_size, 0, res->ai_addr, res->ai_addrlen);
		if (sent_packet == -1) {
			printf("Error in sending packet'\n");
			return 0;
		}
			
		// sets up counter, flags and variables for the loop to send the packet
		sent_count = 1;
		bool timeout_flag = false;
		bool sent = false;
		fd_set readfd;
		FD_ZERO(&readfd);
		struct timeval timeout;
		
		
		
		// repeatedly attempt to send the packet until an ACK has been receieved
		while (!sent) {
			
			printf("Packet #%d: ", curr_packet->frag_no);
			
			// sets timeout value and reads from the socket for an ACK message
			timeout.tv_sec = timeout_value/1;
			timeout.tv_usec = (timeout_value - timeout.tv_sec)*1000000;
			FD_SET(sockfd, &readfd);
			select(sockfd+1, &readfd, NULL, NULL, &timeout);
			
			// if the timeout limit is reaches, then display it to the user and attempt to send again if the max limit has not been reaches
			if (!FD_ISSET(sockfd, &readfd)) {
				
				// update the user on the timeout and increase the time out value for the subsequent retransmission
				printf("Timeout occured. ");
				timeout_value = timeout_value * 2;

				// if attempted to retransmit 18 times, abort program, otherwise attempt to send again
				if (sent_count == 20) {
					end_op = clock();
					total_op = (float)(end_op - start_op)/CLOCKS_PER_SEC;
					printf("\nAttempted to resend packet #%d, 20 times\n Aborting file transfer at a time of %f seconds.\n", curr_packet->frag_no, total_op);
					sent_count = 0;
					return 0;
				}
				else {
					
					// sends packet again to the socket, error  cheks and incrememnts counter for num of times sent
					int sent_packet_again = sendto(sockfd, packet_to_send, packet_string_size, 0, res->ai_addr, res->ai_addrlen);
					if (sent_packet_again == -1) {
						printf("Error in sending packet'\n");
						return 0;
					}
					sent_count = sent_count + 1;
					printf("Resent packet #%d, %d times.\n", curr_packet->frag_no, sent_count);
				}
			}
			
			// if time out did not occur, then check what was receieved
			else {
				
				// receieves message from the socket
				rec_bytes = recvfrom(sockfd, received_message, 999 , 0, (struct sockaddr *)&connecting_address, &addr_len);
				end_time = clock();
				if (rec_bytes == -1) {
					printf("Error in receiving ACK message from server\n");
					sent = false;
				}
				
				// if we receieve ack, move on to next packet
				if(strcmp(received_message, "ACK") == 0){
					printf("ACK receieved for packet #%d.\n",curr_packet->frag_no);
					sent = true;
				}
			}
		}
		
		// updating to new sample time for the next packet
		sampleRTT = (1 - 0.125) * sampleRTT + (0.125) * (float)(end_time - start_time)/CLOCKS_PER_SEC;
		devRTT = (0.75) * devRTT + (0.25) * fabs(sampleRTT - (float)(end_time - start_time)/CLOCKS_PER_SEC);
		timeout_value = sampleRTT + 4 * devRTT;
		
		// moves onto sending next packet, and frees the string for the next iteration
		curr_packet = curr_packet->next;
		free(packet_to_send);
	}
	
	// calculates time for the entire transmission
	end_op = clock();
	total_op = (float)(end_op - start_op)/CLOCKS_PER_SEC;
	printf("Initial round trip time for connection: %f \n", initial_rtt);
	//printf("Total transfer of %d packets took: %f seconds.\n",num_of_fragments, total_op);

	// frees memory and closes connection
    freeaddrinfo(res);
    close(sockfd);
	
	return 0;
}