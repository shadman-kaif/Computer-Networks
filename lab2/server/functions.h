#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>


#define MAXFRAGLEN 1000

struct packet {
    unsigned int total_frag;        
    unsigned int frag_no;        
    unsigned int size;          
    char* filename;                  
    char filedata[MAXFRAGLEN];       
    struct packet * next;         
};

//Converting the packet to its individual elements from string format
struct packet * string_to_struct(char * packet_string){

    struct packet * separated_packet;
    char *total_frag_string, *frag_no_string, *size_string, *file_name, *data;
    int total_fragments, frag_num, frag_size;
    
    //splitting the string
    total_frag_string = strtok(packet_string, ":");
    frag_no_string = strtok(NULL, ":");
    size_string = strtok(NULL, ":");
    file_name = strtok(NULL, ":");
    
    //Converting the strings to int
    total_fragments = atoi(total_frag_string);
    frag_num = atoi(frag_no_string);
    frag_size = atoi(size_string);
    
    //index for data
    int index = strlen(total_frag_string) + strlen(frag_no_string) +
    strlen(size_string) + strlen(file_name) + 4;
    
    //copy data
    data = malloc(frag_size*sizeof(char));
    memcpy(data, &packet_string[index], frag_size);
    
    //asign values
    separated_packet = malloc(sizeof(struct packet));
    separated_packet->total_frag = total_fragments;
    separated_packet->frag_no = frag_num;
    separated_packet->size = frag_size;
    separated_packet->filename = file_name;
    memcpy(separated_packet->filedata, data, frag_size);
    
    
    if(frag_size < MAXFRAGLEN){
        separated_packet->filedata[frag_size] = '\0';//adding null character
    }
    return separated_packet;
}

#endif
