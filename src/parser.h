#ifndef PARSER_H
#define PARSER_H

#define MST_PC_NONE   0
#define MST_PC_PUT    1
#define MST_PC_GET    2
#define MST_PC_DEL    3

typedef struct mst_parser {
  int command;
  char* key;
  int key_len;
  char* data;
  int data_len;
  int cs;
} mst_parser_t;

mst_parser_t* mst_parser_create();
void mst_parser_destroy(mst_parser_t** parser);
void mst_parser_exec(mst_parser_t* parser, char* request, int size);

#endif
