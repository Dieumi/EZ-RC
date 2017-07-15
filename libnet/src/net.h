#ifndef NET_H

    #define NET_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <string.h>
    #include <errno.h>
    #include <netinet/in.h>

    #define SERVER_BUFFER_SIZE 2048

    int socket_init(struct sockaddr_in *addr, int* sock, unsigned int port);
    fd_set socket_clone_fd_set(fd_set set, unsigned int fd_size);
    int socket_server_prepare(struct sockaddr_in *addr, int* sock, unsigned int port, unsigned int max_peer);
    int socket_bind(struct sockaddr_in *addr, int* sock);
    int socket_listen(int* sock, unsigned int max_peer);
    int socket_accept(int* sock, fd_set *clients);
    int socket_client_prepare(struct sockaddr_in *addr, int* sock, unsigned int port);
    int socket_send(int sock, void* buff, unsigned int buff_size);
    int socket_read(int sock, void* buff, unsigned int buff_size);
    int socket_select(int max_sock, fd_set *sockets);
    void socket_listener_init(fd_set *sockets, int clients[], int sock, int *max_sock);
    void socket_process(fd_set *sockets, int clients[], int sock, int *max_sock, void (*route_request)(int* client_socket, void* buffer, unsigned int buffer_size));
    void socket_manage(fd_set *sockets, int clients[], int sock, void (*route_request)(int* client_socket, void* buffer, unsigned int buffer_size));
    void socket_server_run(unsigned int port, unsigned int max_peers, void (*route_request)(int* client_socket, void* buffer, unsigned int buffer_size));
    int socket_connections_manage(fd_set *sockets, int clients[], int sock, int max_sock);
    void socket_answers_manage(fd_set *sockets, int clients[], int max_sock, void (*route_request)(int* client_socket, void* buffer, unsigned int buffer_size));
    void socket_close(int *socket);



#endif
