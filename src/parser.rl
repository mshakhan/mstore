#include <stdlib.h>
#include "parser.h" 

%%{
  machine mst_parser;
  
  action get   { parser -> command = MST_PC_GET; pk = p; }
  action put   { parser -> command = MST_PC_PUT; pk = p; }
  action del   { parser -> command = MST_PC_DEL; pk = p; }
  
  action skey  { parser -> key = (char*)p; }
  action ekey  { parser -> key_len = p - pk - 1; }
  
  action done  { parser -> data = (char*)(p + 1); parser -> data_len = pe - p - 1; }
  
  nl     = "\n" | ";";
  ws     = " ";
  
  method = "GET" %get
         | "PUT" %put
         | "DEL" %del;
         
  
  char   = [a-zA-Z0-9_];
  key    = char+ >skey %ekey;
  
  request = method ws key (nl @done);
  
  main := request;
}%%

%% write data;

mst_parser_t* mst_parser_create() {
  int cs = 0;

  mst_parser_t* parser = malloc(sizeof(mst_parser_t));
  parser -> command = MST_PC_NONE;
  parser -> key = NULL;
  parser -> key_len = 0;
  parser -> data = NULL;
  parser -> data_len = 0;
  
  %% write init;
  
  parser -> cs = cs;
  
  return parser;
}

void mst_parser_destroy(mst_parser_t* parser) {
  free(parser);
}

void mst_parser_exec(mst_parser_t* parser, char* request, int size) {
  int cs = parser -> cs;

  const char *p, *pk, *pe, *eof = 0;    
  p = request;
  pk = request;
  pe = p + size; 
  
  %% write exec;
  
  parser -> cs = cs;    
}
