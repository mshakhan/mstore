#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "defs.h"
#include "storage.h"
#include "server.h"
#include "parser.h"

#define MST_BACKLOG           10
#define MST_RECV_BUFFER_LEN   1024

char mst_recv_buffer[MST_RECV_BUFFER_LEN];

static mst_storage_t* storage;

typedef struct mst_options {
  int port;
} mst_options_t;

mst_options_t*  mst_parse_options(int argc, char** argv);
void            mst_print_usage();
int             mst_server_callback(int client);

int main(int argc, char** argv) {
  mst_options_t* options = mst_parse_options(argc, argv);

  if (NULL == options) {
    mst_print_usage();
    return EXIT_FAILURE;
  }
  
  storage = mst_storage_create();

  mst_server_t* server = mst_server_create(
    options -> port, MST_BACKLOG, &mst_server_callback);
  
  if (NULL == server) {
    ERROR("Cannot create server.");
    EXIT;
  }
    
  mst_server_dispatch(server);
  
  mst_storage_destroy(storage);
  mst_server_destroy(server);

  return EXIT_SUCCESS;
}

mst_options_t* mst_parse_options(int argc, char** argv) {
  if (2 == argc) {
    int port = atoi(argv[1]);
    
    if (port > 0 && port < MAX_PORT) {
      mst_options_t* mst_options = malloc(sizeof(mst_options_t));
      mst_options -> port = port;
      return mst_options;      
    } else {
      return NULL;
    }
  
  } else {
    return NULL;
  }
}

void mst_print_usage() {
  printf("mstore - Memory Storage\n\n\tUsage: mstore <port number>\n");
}

int mst_server_callback(int client) {
  memset(mst_recv_buffer, 0, MST_RECV_BUFFER_LEN);
  int bytes_recv = recv(client, mst_recv_buffer, MST_RECV_BUFFER_LEN, 0);
  
  if (bytes_recv > 0) {
    mst_parser_t* parser = mst_parser_create();
    mst_parser_exec(parser, mst_recv_buffer, bytes_recv - 1);
    
    INFO("Recv %d bytes:\n===\n%s\n===\n", bytes_recv, mst_recv_buffer);
    char* key = NULL;
    char* data = NULL;
    
    switch (parser -> command) {
    case MST_PC_GET:
      key = malloc(parser -> key_len + 1);
      memcpy(key, parser -> key, parser -> key_len);
      key[parser -> key_len] = '\0';
      data = mst_storage_get(storage, key);
      send(client, data, strlen(data), 0);
      INFO("GET: key = %s, data = %s", key, data);
      break;
    case MST_PC_PUT:
      key = malloc(parser -> key_len + 1);
      data = malloc(parser -> data_len + 1);
      memcpy(key, parser -> key, parser -> key_len);
      key[parser -> key_len] = '\0';
      memcpy(data, parser -> data, parser -> data_len);
      data[parser -> data_len] = '\0';
      mst_storage_set(storage, key, data);
      INFO("PUT: key = %s : %d, data = %s : %d", 
        parser -> key, parser -> key_len, parser -> data, parser -> data_len);
      break;
    case MST_PC_DEL:
      INFO("DEL requests are not implemented");
      break;
    default:
      ERROR("Invalid command");
    }
    
    if (key) { free(key); }
    if (data) { free(data); }    
        
    mst_parser_destroy(parser);
  } 
     
  return 0;
}
