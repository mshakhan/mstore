#ifndef SERVER_H
#define SERVER_H

#define MAX_PORT 65536

typedef int(*mst_server_cb_t)(int);

typedef struct mst_server {
  mst_server_cb_t callback;
  int fd;
  int port;
  int backlog;
} mst_server_t;

mst_server_t*   mst_server_create(int port, int backlog, mst_server_cb_t callback);
int             mst_server_dispatch(mst_server_t* mst_server);
void            mst_server_destroy(mst_server_t* mst_server);

#endif

