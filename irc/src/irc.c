#include "irc.h"
#include "net.h"

#include <stdio.h>

void irc_command_missing(Context* context, IRC_Message message)
{
    char buffer[512] = "ERROR :Command \"%s\" does not exist";
    sprintf(context->response, buffer, message.command);

    strcpy(buffer, context->response);
    strcat(buffer, CRLF);
    socket_send(context->sender_socket, buffer, strlen(buffer));
}

void irc_command_nick(Context* context, IRC_Message message)
{
    char buffer[512] = "";
    char ok_buffer[512] = "New pseudo has been created : %s";
    char already_used_buffer[512] = "433 %s :Nickname is already in use";

    if(HashMap_get(context->nicks, message.params[0]) != NULL)
    {
        sprintf(context->response, already_used_buffer, message.params[0]);
    }
    else
    {
        sprintf(context->response, ok_buffer, message.params[0]);
        HashMap_put(context->nicks, message.params[0], &(context->sender_socket), sizeof(context->sender_socket));
    }

    strcpy(buffer, context->response);
    strcat(buffer, CRLF);

    socket_send(context->sender_socket, buffer, strlen(buffer));
}

void irc_command_user(Context* context, IRC_Message message)
{
    char buffer[512] = "";
    char ok_buffer[512] = "New user has been registered : %s";
    char already_used_buffer[512] = "462 :You may not reregister";
    IRC_User user;

    strcpy(user.pseudo, message.params[0]);
    strcpy(user.host, message.params[1]);
    strcpy(user.name, message.params[3] + 1);

    if(HashMap_get(context->nicks, message.params[0]) != NULL)
    {
        sprintf(context->response, already_used_buffer);
    }
    else
    {
        sprintf(context->response, ok_buffer, user.name);
        HashMap_put(context->users, user.pseudo, &(user), sizeof(user));
    }

    strcpy(buffer, context->response);
    strcpy(buffer, context->response);
    strcat(buffer, CRLF);

    socket_send(context->sender_socket, buffer, strlen(buffer));
}

void irc_command_ping(Context* context, IRC_Message message)
{
    char buffer[512] = "PONG %s";

    sprintf(context->response, buffer, message.params[0]);
    strcpy(buffer, context->response);
    strcat(buffer, CRLF);

    socket_send(context->sender_socket, buffer, strlen(buffer));
}
