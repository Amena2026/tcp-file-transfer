#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXLINE 128
#define DATALEN 80

void DieWithError(char *errorMessage);

void HandleTCPClient(int clntSocket)
{
    char buffer[4 + DATALEN]; // header + data
    uint16_t count, seqNum;
    int recvLen;

    // --- Receive the filename packet ---
    if ((recvLen = recv(clntSocket, buffer, sizeof(buffer), 0)) <= 0)
        DieWithError("recv() failed");

    if (recvLen < 4) DieWithError("Filename packet too short");

    memcpy(&count, buffer, 2);
    memcpy(&seqNum, buffer + 2, 2);
    count = ntohs(count);
    seqNum = ntohs(seqNum);

    // --- give error if count is invalid ---
    if (count == 0 || count >= MAXLINE)
        DieWithError("Invalid filename length");

    char filename[MAXLINE];
    memcpy(filename, buffer + 4, count);
    filename[count] = '\0';

    printf("Requested file: %s\n", filename);

    // --- Open the file ---
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("File not found. Sending EOT...\n");

        // Send EOT packet with seqNum = 1
        count = htons(0);
        seqNum = htons(1);
        memcpy(buffer, &count, 2);
        memcpy(buffer + 2, &seqNum, 2);
        send(clntSocket, buffer, 4, 0);
        close(clntSocket);
        return;
    }

    // --- Send file line-by-line ---
    char line[DATALEN + 1];
    seqNum = 1;

    while (fgets(line, sizeof(line), fp) != NULL) {
        size_t lineLen = strlen(line);
        if (lineLen > DATALEN) lineLen = DATALEN;

        // Build packet
        uint16_t netCount = htons(lineLen);
        uint16_t netSeq = htons(seqNum);
        memcpy(buffer, &netCount, 2);
        memcpy(buffer + 2, &netSeq, 2);
        memcpy(buffer + 4, line, lineLen);

        // Send full packet
        if (send(clntSocket, buffer, 4 + lineLen, 0) < 0)
            DieWithError("send() failed");

        seqNum++;
    }

    fclose(fp);

    // --- Send EOT ---
    uint16_t netCount = htons(0);
    uint16_t netSeq = htons(seqNum);
    memcpy(buffer, &netCount, 2);
    memcpy(buffer + 2, &netSeq, 2);
    send(clntSocket, buffer, 4, 0);

    close(clntSocket);
}
