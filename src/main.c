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

void mst_command_put(int client, mst_parser_t* parser) {
  char* key = malloc(parser -> key_len + 1);
  char* data = malloc(parser -> data_len + 1);
  
  memcpy(key, parser -> key, parser -> key_len);
  key[parser -> key_len] = '\0';

  memcpy(data, parser -> data, parser -> data_len);
  data[parser -> data_len] = '\0';
  
  mst_storage_set(storage, key, data);
  send(client, "OK", 2, 0);
}

void mst_command_get(int client, mst_parser_t* parser) {
  char* key = malloc(parser -> key_len + 1);
  
  memcpy(key, parser -> key, parser -> key_len);
  key[parser -> key_len] = '\0';

  
  char* data = mst_storage_get(storage, key);
  send(client, data, strlen(data), 0);  
}

void mst_command_none(int client, mst_parser_t* parser) {
  send(client, "ERROR", 5, 0);
}

int mst_server_callback(int client) {
  mst_parser_t* parser; 
  for (;;) {
    memset(mst_recv_buffer, 0, MST_RECV_BUFFER_LEN);
    int bytes_recv = recv(client, mst_recv_buffer, MST_RECV_BUFFER_LEN, 0);
    
    if (bytes_recv > 0) {
      parser = mst_parser_create(); 
      mst_parser_exec(parser, mst_recv_buffer, bytes_recv);
#ifdef MST_DEBUG
      mst_parser_dump(parser);
#endif
      switch (parser -> command) {
        case MST_PC_PUT:
          mst_command_put(client, parser);
        break;

        case MST_PC_GET:
          mst_command_get(client, parser);
        break;

        case MST_PC_DEL:
          mst_command_none(client, parser);
        break;

        default:
          mst_command_none(client, parser);
        break;
      }
      mst_parser_destroy(&parser);
    } else {
      break;
    }    
  }     
  return 0;
}
