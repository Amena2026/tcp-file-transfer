#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXLINE 128
#define RCVBUFSIZE 84  // 2 bytes count + 2 bytes seq + 80 bytes max data
#define DATALEN 80

void DieWithError(char *errorMessage);

// Helper: Receive exactly `len` bytes from the socket
int recv_all(int sock, void *buffer, int len) {
    int total = 0;
    int bytesLeft = len;
    int n;

    while (total < len) {
        n = recv(sock, buffer + total, bytesLeft, 0);
        if (n <= 0) return -1;
        total += n;
        bytesLeft -= n;
    }

    return total;
}

int main() {
    int sock;
    struct sockaddr_in servAddr;
    unsigned short servPort = 12345; // hardcoded port #
    char *servIP = "127.0.0.1";      // hardcoded address
    char filePrompt[MAXLINE];
    char buffer[RCVBUFSIZE];
    uint16_t count, seqNum;
    FILE *fp = NULL;
    int firstPacket = 1;
    int packetCount = 0;
    int totalBytes = 0;

    while (1) {
        // Prompt for file name
        printf("Enter file name: ");
        fgets(filePrompt, MAXLINE, stdin);
        filePrompt[strcspn(filePrompt, "\n")] = '\0'; // null terminator for string
        uint16_t filenameLen = strlen(filePrompt);

        // Create socket
        if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
            DieWithError("socket() failed");

        // Set up server address
        memset(&servAddr, 0, sizeof(servAddr));
        servAddr.sin_family = AF_INET;
        servAddr.sin_addr.s_addr = inet_addr(servIP);
        servAddr.sin_port = htons(servPort);

        // Connect
        if (connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
            DieWithError("connect() failed");

        // Send filename packet
        uint16_t netCount = htons(filenameLen);
        uint16_t netSeq = htons(0);
        memcpy(buffer, &netCount, 2);
        memcpy(buffer + 2, &netSeq, 2);
        memcpy(buffer + 4, filePrompt, filenameLen);
        send(sock, buffer, 4 + filenameLen, 0);

        firstPacket = 1;
        fp = NULL;
        packetCount = 0;
        totalBytes = 0;

        while (1) {
            // Step 1: Receive 4-byte header
            if (recv_all(sock, buffer, 4) != 4) {
                DieWithError("recv() failed or incomplete header");
            }

            memcpy(&count, buffer, 2);
            memcpy(&seqNum, buffer + 2, 2);
            count = ntohs(count);
            seqNum = ntohs(seqNum);

            // Handle EOT as first packet = file doesn't exist
            if (count == 0 && seqNum == 1 && firstPacket) {
                printf("%s does not exist on server. Try again.\n", filePrompt);
                close(sock);
                break; // back to prompt
            }

            firstPacket = 0;

            // Open output file after first valid packet
            if (fp == NULL) {
                fp = fopen("out.txt", "w");
                if (fp == NULL)
                    DieWithError("Failed to open out.txt");

                printf("%s exists.\n", filePrompt);
            }

            // EOT = done
            if (count == 0) {
                fclose(fp);
                close(sock);
                printf("%s received.\n", filePrompt);
                printf("Number of packets received: %d\n", packetCount);
                printf("Total bytes received: %d\n", totalBytes);
                return 0;
            }

            // Step 2: Receive data of `count` bytes
            if (recv_all(sock, buffer + 4, count) != count) {
                DieWithError("recv() failed on data");
            }

            // Step 3: Write to output file
            fwrite(buffer + 4, 1, count, fp);

            // Track stats
            packetCount++;
            totalBytes += count;
        }
    }
}
