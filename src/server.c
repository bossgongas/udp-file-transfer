#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 9876
#define MAX_BUFFER_SIZE 500

int main() {
	int sock, recv_len;
   	struct sockaddr_in server_addr, client_addr;
   	socklen_t slen = sizeof(client_addr);
	char buffer[MAX_BUFFER_SIZE];
	FILE *file2;
	int bytes_w;
	unsigned long int expected_sequence_number = 0;

   	// Create socket for receiving packets UDP
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
		perror("socket");
        	exit(EXIT_FAILURE);
    	}
    	memset(&server_addr, 0, sizeof(server_addr));
    	server_addr.sin_family = AF_INET;
    	server_addr.sin_port = htons(PORT);
    	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    	// Open destine file of the content
    	if ((file2 = fopen("file2.txt", "w")) == NULL) {
        	perror("Error opening file");
        	exit(EXIT_FAILURE);
    	}

    	// Associates a socket to the address data, binds a port number
    	if (bind(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        	perror("Error in bind");
        	exit(EXIT_FAILURE);
    	}

    	// Wait for incoming connection
    	while (1) {
        	// Clear buffer
        	memset(buffer, 0, sizeof(buffer));
        	// Blocks until data is received from socket or an error occurs
        	if ((recv_len = recvfrom(sock, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr *) &client_addr, &slen)) == -1) {
        	    perror("Error in recvfrom");
        	    exit(EXIT_FAILURE);
        	}

        	// Verify the sequence number of the received packet
        	unsigned long int sequence_number;
        	memcpy(&sequence_number, buffer, sizeof(sequence_number));

        	if (sequence_number == expected_sequence_number) {
        	    // Write the received data to the file
        	    int data_len = recv_len - sizeof(sequence_number);//to not copy the sequence number to the file
        	    if (fwrite(buffer + sizeof(sequence_number), 1, data_len, file2) != data_len) {
        	        perror("Error writing to file");
        	        exit(EXIT_FAILURE);
        	    }
        	    if (fflush(file2) == 0){
    			printf("\nDATA RECEIVED!!");
    	            };

            	// Update sequence number by Increment
            	expected_sequence_number++;
            	
        	}

        	// Send an acknowledgement packet with the number of bytes already received
        	unsigned long int ack_number = expected_sequence_number - 1;
        	if (sendto(sock, &ack_number, sizeof(ack_number), 0, (struct sockaddr *) &client_addr, slen) == -1) {
            		perror("Error sending acknowledgement");
            		exit(EXIT_FAILURE);
	 	}
    	}

    	// Close the file and socket
    	fclose(file2);
    	close(sock);
    	return 0;
}
