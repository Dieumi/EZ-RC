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
        strcpy(context->reverse_nicks[context->sender_socket], message.params[0]);
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
        strcpy(context->response, already_used_buffer);
    }
    else
    {
        sprintf(context->response, ok_buffer, user.name);
        HashMap_put(context->users, user.pseudo, &(user), sizeof(user));
    }

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

void irc_command_pong(Context* context, IRC_Message message)
{
    strcpy(context->response, "");
}

void irc_command_all(Context* context, IRC_Message message)
{
    int i;
    HashMapEntry* entry;
    int sock;
    char buffer[512] = "";

    for(i = 0 ; i < message.params_size ; i++)
    {
        strcpy(buffer, message.params + i);
    }

    strcpy(context->response, buffer);

    strcat(buffer, CRLF);

    for(i = 0 ; i < 256 ; i++)
    {
        if(strcmp(context->reverse_nicks[i], "") != 0)
        {
            entry = HashMap_get(context->nicks, context->reverse_nicks[i]);
            sock = entry != NULL ? *((int*) entry->value) : 0;

            if(sock != 0 && sock != context->sender_socket)
            {
                socket_send(sock, buffer, strlen(buffer));
            }
        }
    }
}

void irc_command_privmsg(Context* context, IRC_Message message)
{
    int i;
    int sock;
    char buffer[512] = "";
    HashMapEntry* entry = NULL;
    char *ptr_beg, *ptr_end = message.params[0];
    char nick[512] = "";

    for(i = 1 ; i < message.params_size ; i++)
    {
        strcat(buffer, message.params + i);
        strcat(buffer, " ");
    }

    strcpy(context->response, buffer);
    strcat(buffer, CRLF);


    while(ptr_end != NULL)
    {
        ptr_beg = ptr_end;
        ptr_end = strchr(ptr_beg, ',');

        if(ptr_end != NULL)
        {
            strncpy(nick, ptr_beg, ptr_end - ptr_beg);
            ptr_end++;
        }
        else
        {
            strncpy(nick, ptr_beg, strlen(ptr_beg));
        }

        entry = HashMap_get(context->nicks, message.params[0]);

        if(entry != NULL)
        {
            sock = entry != NULL ? *((int*) entry->value) : 0;

            if(sock != 0 && sock != context->sender_socket)
            {
                socket_send(sock, buffer, strlen(buffer));
            }
        }
        else
        {
            entry = HashMap_get(context->channels, message.params[0]);

            if(entry != NULL)
            {
                IRC_Channel* channel = (IRC_Channel*) entry->value;
                for(i = 0 ; i < channel->users_size ; i++)
                {
                    entry = HashMap_get(context->nicks, channel->users[i]);

                    if(entry != NULL)
                    {
                        sock = entry != NULL ? *((int*) entry->value) : 0;

                        if(sock != 0 && sock != context->sender_socket)
                        {
                            socket_send(sock, buffer, strlen(buffer));
                        }
                    }
                }
            }
        }
    }
}

void irc_command_join(Context* context, IRC_Message message)
{
    char buffer[512] = "";
    char full_channel_buffer[512] = "471 %s :Cannot join channel (+l)";
    char already_in_channel_buffer[512] = "443 %s %s :is already on channel";
    char ok_buffer[512] = "331 %s :No topic is set";
    int i;
    unsigned char is_in_array = 0;

    IRC_Channel channel;
    IRC_Channel* channel_ptr;
    HashMapEntry* entry = HashMap_get(context->channels, message.params[0]);

    if(entry == NULL)
    {
        for(i = 0 ; i < IRC_MAX_CHANNEL_USERS ; i++)
            strcpy(channel.users[i], "");

        channel.users_size = 0;
        HashMap_put(context->channels, message.params[0], &channel, sizeof(channel));
        entry = HashMap_get(context->channels, message.params[0]);
        channel_ptr = (IRC_Channel*) entry->value;

    }
    else
    {
        channel_ptr = (IRC_Channel*) entry->value;
    }

    if(channel_ptr->users_size < IRC_MAX_CHANNEL_USERS && (is_in_array = in_array(channel_ptr->users, context->reverse_nicks[context->sender_socket])) == 0)
    {
        strcpy(channel_ptr->users[channel_ptr->users_size], context->reverse_nicks[context->sender_socket]);
        channel_ptr->users_size++;
        sprintf(context->response, ok_buffer, message.params[0]);
    }
    else if (is_in_array)
    {
        sprintf(context->response, already_in_channel_buffer, context->reverse_nicks[context->sender_socket], message.params[0]);
    }
    else
    {
        sprintf(context->response, full_channel_buffer, message.params[0]);
    }

    strcpy(buffer, context->response);
    strcat(buffer, CRLF);

    socket_send(context->sender_socket, buffer, strlen(buffer));
}

unsigned char in_array(char array[IRC_MAX_CHANNEL_USERS][512], char value[512])
{
    int i;
    for(i = 0 ; i < IRC_MAX_CHANNEL_USERS ; i++)
    {
        if(strcmp(array[i], value) == 0)
        {
            printf("%s == %s\n", array[i], value);
            return 1;
        }
    }
    return 0;
}
