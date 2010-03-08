#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "defs.h"
#include "server.h"

void mst_server_sigchld_handler(int s) {
  while (waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *mst_server_get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *) sa)->sin_addr);
  }
  return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

mst_server_t* mst_server_create(int port, int backlog, mst_server_cb_t cb){
  mst_server_t* server = malloc(sizeof(mst_server_t));
  server -> port = port;
  server -> backlog = backlog;
  server -> callback = cb;
  
  struct addrinfo hints, *servinfo, *p;
  struct sigaction sa;
  int yes = 1;
  int rv;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  char port_s[10];
  sprintf(port_s, "%d", server -> port);

  if ((rv = getaddrinfo(NULL, port_s, &hints, &servinfo)) != 0) {
    ERROR("Getaddrinfo: %s\n", gai_strerror(rv));
    return NULL;
  }
    
  // loop through all the results and bind to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((server -> fd = socket(p->ai_family, p->ai_socktype,
     p->ai_protocol)) == -1) {
      ERROR("Socket");
      continue;
    }

    CHECK_UNLESS(setsockopt(server -> fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)

    if (bind(server -> fd, p->ai_addr, p->ai_addrlen) == -1) {
      close(server -> fd);
      continue;
    }
    break;
  }

  if (p == NULL) {
    ERROR("Bind failed. Port may be busy.");
    return NULL;
  }
  
  freeaddrinfo(servinfo);

  CHECK_UNLESS(listen(server -> fd, server -> backlog) == -1);

  sa.sa_handler = mst_server_sigchld_handler;	// reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  CHECK_UNLESS(sigaction(SIGCHLD, &sa, NULL) == -1);

  INFO("Server has been created.");

  return server;
}

int mst_server_dispatch(mst_server_t* mst_server) {
  int client_fd = 0;
  struct sockaddr_storage their_addr;
  socklen_t sin_size;
  char s[INET6_ADDRSTRLEN];  
  
  INFO("Start listening...");    
  LOOP {
   sin_size = sizeof(their_addr);
    client_fd = accept(mst_server -> fd, (struct sockaddr*) &their_addr, &sin_size);
    if (client_fd == -1) {
      ERROR("Accept");
      continue;
    }
    
    inet_ntop(
      their_addr.ss_family, mst_server_get_in_addr((struct sockaddr *)&their_addr), s, sizeof(s));
    INFO("Got connection from %s", s);

    if (!fork()) {		            
      close(mst_server -> fd);	  
      mst_server -> callback(client_fd);
      INFO("Connection with %s closed", s);
      close(client_fd);      
      exit(EXIT_SUCCESS);
    }
    close(client_fd);    
  }
}

void mst_server_destroy(mst_server_t* mst_server) {
  close(mst_server -> fd);
  free(mst_server);
}

