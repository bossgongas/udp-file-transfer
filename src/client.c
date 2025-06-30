#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 9876
#define MAX_BUFFER_SIZE 500

int main() {
    	int sock;
    	struct sockaddr_in server_addr;
    	FILE *file1;
    	char buffer[MAX_BUFFER_SIZE];
    	int bytes_r, bytes_s;
    	unsigned long int sequence_number = 0;
    	unsigned long int expected_ack_number = 0;

    	// Create socket for sending packets UDP
    	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    	    perror("socket");
    	    exit(EXIT_FAILURE);
    	}

    	// Setup the server address to send packets to
    	memset(&server_addr, 0, sizeof(server_addr));
    	server_addr.sin_family = AF_INET;
    	server_addr.sin_port = htons(PORT);
    	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
    	// Open the source file of the content
   	if ((file1 = fopen("file1.txt", "rb")) == NULL) {
        	perror("Error opening file");
        	exit(EXIT_FAILURE);
    	}

    	// Read the file and send packets of data to the socket 
    	while (1) {
        	// Read data from file into buffer
        	int bytes_read_from_file = fread(buffer + sizeof(sequence_number), 1, MAX_BUFFER_SIZE - sizeof(sequence_number), file1);
        	if (bytes_read_from_file == 0) {
            		break;
        	}

        	// Add sequence number to the beginning of the buffer
        	memcpy(buffer, &sequence_number, sizeof(sequence_number));
        	//This sequence number is used by the server to ensure that the data is received in the correct order.

        	// Send packet to the server
        	if ((bytes_s = sendto(sock, buffer, bytes_read_from_file + sizeof(sequence_number), 0, (struct sockaddr *) &server_addr, sizeof(server_addr))) == -1) {
            	perror("Error sending packet");
           	exit(EXIT_FAILURE);
        }

        // Wait for acknowledgement from server
        while (1) {
            // Clear buffer
            memset(buffer, 0, sizeof(buffer));

            // Blocks until data is received from socket or an error occurs
            printf("[Waiting for aknowledgment]...");
            int recv_len = recvfrom(sock, buffer, sizeof(expected_ack_number), 0, NULL, NULL);
            if (recv_len == -1) {
                perror("Error in recvfrom");
                exit(EXIT_FAILURE);
            }

            // Verify acknowledgement number
            memcpy(&expected_ack_number, buffer, sizeof(expected_ack_number));
            if (expected_ack_number == sequence_number) {
                // Acknowledgement received, break out of inner loop
                break;
            } else {
                // Acknowledgement not received, resend packet
                if ((bytes_s = sendto(sock, buffer, bytes_read_from_file + sizeof(sequence_number), 0, (struct sockaddr *) &server_addr, sizeof(server_addr))) == -1) {
                    perror("Error sending packet");
                    exit(EXIT_FAILURE);
                }
            }
        }

        // Increment sequence number
        sequence_number++;
    }

    // Close the file and socket
    fclose(file1);
    close(sock);
    return 0;
}
