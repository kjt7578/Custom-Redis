#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  int server_fd;
  socklen_t client_addr_len;
  struct sockaddr_in client_addr;

  setbuf(stdout, NULL);

  server_fd = socket(AF_INET, SOCK_STREAM, 0);	// Create a TCP socket
  if (server_fd == -1) {
    printf("Socket creation failed: %s...\n", strerror(errno));
    return 1;
  }

  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
      0) {
    printf("SO_REUSEADDR failed: %s \n", strerror(errno));
    return 1;
  }

  struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(6379),
      .sin_addr = {htonl(INADDR_ANY)},
  };

  if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
    printf("Bind failed: %s \n", strerror(errno));
    return 1;
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    printf("Listen failed: %s \n", strerror(errno));
    return 1;
  }

  printf("Waiting for a client to connect...\n");
  client_addr_len = sizeof(client_addr);

  int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);	// Accept an incoming client connection
  if (client_fd < 0) {
    printf("Accept failed: %s \n", strerror(errno));
    close(server_fd);
    return 1;
  }
  
  char *response = "+PONG\r\n";
  send(client_fd, response, strlen(response), 0);	// Send the response to the connected client

  printf("Client connected\n");

  close(client_fd);	// Close the client connection
  close(server_fd);	// Close the server socket
  return 0;
}