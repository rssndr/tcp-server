// tcp-server.c
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define PORT 8181
#define BUF_SIZE 512

int main() {
    int s, c;
    socklen_t addrlen;
    struct sockaddr_in srv, cli;
    char buf[BUF_SIZE];
    const char *data = "httpd v1.0\n";

    // Create socket
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("socket");
        return 1;
    }

    // Allow port reuse
    int opt = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(s);
        return 1;
    }

    // Configure server address
    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(PORT);

    // Bind socket
    if (bind(s, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
        perror("bind");
        close(s);
        return 1;
    }

    // Listen for connections
    if (listen(s, 5) < 0) {
        perror("listen");
        close(s);
        return 1;
    }
    printf("Listening on 0.0.0.0:%d\n", PORT);

    // Main loop to handle clients
    while (1) {
        addrlen = sizeof(cli);
        c = accept(s, (struct sockaddr *)&cli, &addrlen);
        if (c < 0) {
            perror("accept");
            continue;
        }
        printf("Client connected from %s:%d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));

        // Read from client
        ssize_t n = read(c, buf, BUF_SIZE - 1);
        if (n < 0) {
            perror("read");
            close(c);
            continue;
        }
        buf[n] = '\0';
        printf("Received: %s\n", buf);

        // Send response
        if (write(c, data, strlen(data)) != strlen(data)) {
            perror("write");
        }

        close(c);
    }

    close(s);
    return 0;
}

