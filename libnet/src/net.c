#include "net.h"

int socket_init(struct sockaddr_in *addr, int* sock, unsigned int port)
{
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    bzero(addr, sizeof(*addr));

    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = htons(INADDR_ANY);
    addr->sin_port = htons(port);

    return 0;
}

int socket_server_prepare(struct sockaddr_in *addr, int* sock, unsigned int port, unsigned int max_peer)
{
    int err_code = 0;

    err_code |= socket_init(addr, sock, port);

    err_code |= socket_bind(addr, sock);

    err_code |= socket_listen(sock, max_peer);

    if(err_code)
    {
        fprintf(stderr, "Failed to prepare server socket %d on port %u\n", *sock, port);
        exit(err_code);
    }
    else
    {
        printf("Server started on port %d with socket %d\n", port, *sock);
    }

    return err_code;
}

int socket_bind(struct sockaddr_in *addr, int* sock)
{
    int err_code = bind(*sock, (struct sockaddr*) addr, sizeof(*addr));

    if(err_code)
    {
        fprintf(stderr, "Could not bind socket %d\n", *sock);
        exit(err_code);
    }

    return err_code;
}

int socket_listen(int* sock, unsigned int max_peer)
{
    int err_code = listen(*sock, max_peer);

    if(err_code)
    {
        fprintf(stderr, "Could not start listening on socket %d\n", *sock);
        exit(err_code);
    }
    else
    {
        printf("Client connected with host with socket %d\n", *sock);
    }

    return 0;
}

int socket_accept(int* sock, fd_set *clients)
{
    int opened_sock = accept(*sock, NULL, NULL);

    if(opened_sock < 0)
    {
        fprintf(stderr, "Server could not accept connection for socket %d\n", *sock);
        exit(opened_sock);
    }

    else
    {
        printf("New client detected on socket %d\n", opened_sock);
        FD_SET(opened_sock, clients);
    }

    return opened_sock;
}

int socket_client_prepare(struct sockaddr_in *addr, int* sock, unsigned int port)
{
    socket_init(addr, sock, port);
    int opened_sock = connect(*sock, (struct sockaddr*) addr, sizeof(*addr));

    if(opened_sock < 0)
    {
        fprintf(stderr, "Client could not connect to socket %d on port %u\n", *sock, port);
        exit(opened_sock);
    }

    return opened_sock;
}

int socket_send(int sock, void* buff, unsigned int buff_size)
{
    int written_bytes = write(sock, buff, buff_size);
    printf("Sent %d bytes out of %d on socket %d\n", written_bytes, buff_size, sock);

    return written_bytes;
}

int socket_read(int sock, void* buff, unsigned int buff_size)
{
    bzero(buff, buff_size);
    int read_bytes = read(sock, buff, SERVER_BUFFER_SIZE);
    printf("Received %d bytes out of %d on socket %d\n", read_bytes, buff_size, sock);

    return read_bytes;
}

int socket_select(int max_sock, fd_set *sockets) {
  int activity = select(max_sock + 1, sockets, NULL, NULL, NULL);

  if ((activity < 0) && (errno!=EINTR))
  {
      fprintf(stderr, "Error occured while selecting active sockets");
  }

  return activity;
}

void socket_listener_init(fd_set *sockets, int clients[], int sock, int *max_sock)
{
    int sd;
    int i;

    FD_ZERO(sockets);

    FD_SET(sock, sockets);
    *max_sock = sock;

    for ( i = 0 ; i < *max_sock ; i++)
    {
        sd = clients[i];

        if(sd > 0)
        {
            FD_SET( sd , sockets);
        }

        if(sd > *max_sock)
        {
            *max_sock = sd;
        }
    }
}

void socket_process(fd_set *sockets, int clients[], int sock, int *max_sock, void (*route_request)(int* client_socket, void* buffer, unsigned int buffer_size)) {
  socket_listener_init(sockets, clients, sock, max_sock);

  socket_select(*max_sock, sockets);

  socket_connections_manage(sockets, clients, sock, *max_sock);

  socket_answers_manage(sockets, clients, *max_sock, route_request);
}

void socket_manage(fd_set *sockets, int clients[], int sock, void (*route_request)(int* client_socket, void* buffer, unsigned int buffer_size)) {
  int max_sock = 0;

  while(1) {
    socket_process(sockets, clients, sock, &max_sock, route_request);
  }
}

void socket_server_run(unsigned int port, unsigned int max_peers, void (*route_request)(int* client_socket, void* buffer, unsigned int buffer_size)) {
  struct sockaddr_in addr;
  int sock;
  fd_set sockets;
  int clients[max_peers];

  bzero(clients, sizeof(int) * max_peers);

  socket_server_prepare(&addr, &sock, port, max_peers);

  socket_manage(&sockets, clients, sock, route_request);

  printf("Closing socket\n");
  close(sock);
}

int socket_connections_manage(fd_set *sockets, int clients[], int sock, int max_sock)
{
    int new_socket = -1;
    int i;

    if (FD_ISSET(sock, sockets))
    {
        new_socket = socket_accept(&sock, sockets);

        for (i = 0; i < max_sock; i++)
        {
            if( clients[i] == 0 )
            {
                clients[i] = new_socket;
                break;
            }
        }
    }

    return new_socket;
}

void socket_answers_manage(fd_set *sockets, int clients[], int max_sock, void (*route_request)(int* client_socket, void* buffer, unsigned int buffer_size))
{
    int socket;
    int i;
    int val_read;
    char buffer [2048] = "";

    for (i = 0; i < max_sock; i++)
    {
        socket = clients[i];

        if (FD_ISSET(socket , sockets))
        {
            val_read = socket_read(socket, (void*) buffer, SERVER_BUFFER_SIZE);
            if (val_read <= 0)
            {
                close(socket);
            }

            else
            {
                buffer[val_read] = '\0';
                route_request(&socket, buffer, val_read);
            }
            bzero(buffer, 2048);
        }
    }
}

void socket_close(int *socket) {
  close(*socket);
  *socket = 0;
}
