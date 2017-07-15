#include <string.h>
#include <ctype.h>

#include "net.h"
#include "dll.h"
#include "irc.h"
#include "map.h"

#define EXPOSED_PORT 6667
#define MAX_PEERS 30

#define CRLF "\r\n"

#define IRC_COMMAND_PREFIX "irc_command_"
#define IRC_COMMAND_MISSING "irc_command_missing"

#define IRC_LIB_PATH "./dll/irc.so"
#define IRC_SEPARATOR ' '

void* irc_dll_handle = NULL;
HashMap* nicks;
HashMap* users;
HashMap* channels;

unsigned int sizeof_nullable(char* str)
{
    return str != NULL ? sizeof(str) : 0;
}

void irc_user_serialize(IRC_User* user, char* output)
{
    strcpy(output, "");
    if(user != NULL)
    {
        if(user->pseudo != NULL)
        {
            strcat(output, ":");
            strcat(output, user->pseudo);
            if(user->name != NULL)
            {
                strcat(output, "!");
                strcat(output, user->name);
            }

            if(user->host != NULL)
            {
                strcat(output, "@");
                strcat(output, user->host);
            }
        }
    }
}

void irc_message_serialize(IRC_Message* message, char* output)
{
    char user[IRC_MESSAGE_MAX_LENGTH] = "";
    strcpy(output, "");
    char* param = NULL;
    const char* command = NULL;
    if (message != NULL)
    {
        irc_user_serialize(&message->user, user);
        if(strlen(user) > 0)
        {
            strcat(output, user);
            strcat(output, " ");
        }
        command = message->command;
        strcat(output, command);

        param = message->params != NULL ? message->params[0] : NULL;
        while(param != NULL)
        {
            strcat(output, " ");
            strcat(output, param);
            param++;
        }
        strcat(output, CRLF);
    }
}

unsigned int irc_get_parameters(char parameters[][IRC_MESSAGE_MAX_LENGTH], char* data)
{
    char* next = NULL;
    unsigned int counter = 0;
    unsigned int length = 0;

    if(data[0] == ':')
    {
        data = strchr(data, IRC_SEPARATOR) + 1;
    }

    data = strchr(data, IRC_SEPARATOR) + 1;

    while((next = strchr(data, IRC_SEPARATOR)) != NULL)
    {
        length = next - data;
        strncpy(parameters[counter], data, length);
        parameters[counter][length] = '\0';
        data = next + 1;
        counter++;
    }

    if(*data != '\0')
    {
        strcpy(parameters[counter], data);
        counter++;
    }

    return counter;
}

void irc_get_command(char command[], char data[])
{
    if(data[0] == ':')
    {
        data = strchr(data, IRC_SEPARATOR) + 1;
    }

    char* command_end = strchr(data, IRC_SEPARATOR);
    command_end = command_end != NULL ? command_end : data + strlen(data);
    size_t command_length = command_end - data;
    char *ptr;

    *command = '\0';
    strncpy(command, data, command_length);
    command[command_length] = '\0';

    ptr = command;

    while(*ptr) {
        *ptr = tolower((unsigned char)*ptr);
        ptr++;
    }
}

void irc_user_deserialize(IRC_User* user, char* input)
{
    int counter = 0;
    if(user != NULL)
    {
        if(input[0] == ':')
        {
            counter = 0;
            while(*input && *input != ' ' && *input != '!' && *input != '@')
            {
                user->pseudo[counter] = *input;
                input++;
                counter++;
            }
            user->pseudo[counter] = '\0';

            counter = 0;
            while(*input && *input != ' ' && *input != '@')
            {
                user->name[counter] = *input;
                input++;
                counter++;
            }
            user->name[counter] = '\0';

            counter = 0;
            while(*input && *input != ' ')
            {
                user->host[counter] = *input;
                input++;
                counter++;
            }
            user->host[counter] = '\0';
        }
    }
}

void irc_message_deserialize(IRC_Message* message, char* input)
{
    irc_user_deserialize(&(message->user), input);
    irc_get_command(message->command, input);
    message->params_size = irc_get_parameters(message->params, input);
}

void irc_command_call(Context* context, char* data)
{
    char function_name[IRC_MESSAGE_MAX_LENGTH] = IRC_COMMAND_PREFIX;
    void (*function) (Context*, IRC_Message) = NULL;
    IRC_Message message;

    irc_message_deserialize(&message, data);

    strcat(function_name, message.command);

    function = dll_handle_lookup(irc_dll_handle, function_name);

    if(function == NULL)
    {
        strcpy(function_name, IRC_COMMAND_MISSING);
        function = dll_handle_lookup(irc_dll_handle, function_name);
    }

    fprintf(stdout, "command %s loaded at address %p\n", function_name, function);

    function(context, message);
}

void irc_request_route(int* client_socket, void* buffer, unsigned int buffer_size)
{
    Context context;

    context.sender_socket = *client_socket;
    context.users = users;
    context.nicks = nicks;
    context.channels = channels;
    irc_command_call(&context, (char*)buffer);
    fprintf(stdout, "Replied \"%s\"\n", context.response);
}

/// SERVER
int main(int argc, char** argv)
{
    users = HashMap_create(10, 0.7, 2);
    nicks = HashMap_create(10, 0.7, 2);
    channels = HashMap_create(10, 0.7, 2);
    irc_dll_handle = dll_handle_open(IRC_LIB_PATH);

    socket_server_run(EXPOSED_PORT, MAX_PEERS, irc_request_route);

    HashMap_free(&users);
    HashMap_free(&nicks);
    HashMap_free(&channels);
    dll_handle_close(irc_dll_handle);

    return 0;
}
