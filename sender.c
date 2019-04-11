// Client side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
  
#define MAXLINE 1024
#define SYN 1
#define SYNACK 2
#define ACK 3
#define RST 4
#define DATA 5
#define FIN 6
#define FINACK 7
#define PORT 8080
  
void getchecksum(uint16_t checksum, uint8_t type, uint8_t seqnum, char data){
    int sum;
    sum = type + seqnum + (int)data;
    while (sum > 65535){
        sum = sum - 65536 + 1;
    }
    checksum = -sum;
    return;
}  
  
  
int main(int argc, char *argv[]) { 
    int sockfd;
    //int PORT;
    uint8_t type=0, seqnum=0;
    uint16_t checksum=0;
    char packet[14];    // "x,yyy,zzzzz,c" x - type (1-7), yyy - seqnum (0-255), zzzzz - checksum (0-65535), c - data (char)
    char data='\0';
    
    char buffer[MAXLINE]="";
    char *hello = "Hello from client";
    struct sockaddr_in servaddr;
    FILE *fs;
    
    // check if all arguments are input
    if (argc != 5) {
        fprintf(stderr,"Usage: sender <dest_IP> <dest_port> <filename> <proto>\n");
        exit(1);
    }
    
    // parse dest_port
    //PORT = atoi(argv[2]);
    
    // open filename
	if((fs = fopen(argv[3], "r")) == NULL){
		perror("File open failure");
		exit(EXIT_FAILURE);
	}
    
    // Creating socket file descriptor 
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    memset(&servaddr, 0, sizeof(servaddr)); 
      
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(atoi(argv[2])); 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
      
    int n, len; 
    
    //send SYN packet
    type = SYN;
    seqnum = 1;
    data='\0';
    getchecksum(checksum,type,seqnum,data);
    sprintf(packet, "%u,%u,%u,%c", type, seqnum, checksum, data);
    
    
    sendto(sockfd, (const char *)packet, strlen(hello), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    printf("SYN packet sent.\n"); 
          
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
    buffer[n] = '\0'; 
    printf("Server : %s\n", buffer);
    
    fclose(fs);
    close(sockfd); 
    return 0; 
} 
