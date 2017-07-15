#ifndef IRC_COMMANDS_H

    #define IRC_COMMANDS_H

    #define IRC_MESSAGE_MAX_LENGTH 513
    #define IRC_MAX_PARAMETERS 15

    #define CRLF "\r\n"

    #include "map.h"

    typedef struct Context
    {
        int sender_socket;
        char response[513];
        HashMap* nicks;
        HashMap* users;
        HashMap* channels;
    } Context;

    typedef struct IRC_User
    {
        char pseudo[IRC_MESSAGE_MAX_LENGTH];
        char name[IRC_MESSAGE_MAX_LENGTH];
        char host[IRC_MESSAGE_MAX_LENGTH];
    } IRC_User;

    typedef struct IRC_Message
    {
        IRC_User user;
        char command[IRC_MESSAGE_MAX_LENGTH];
        char params[IRC_MAX_PARAMETERS][IRC_MESSAGE_MAX_LENGTH];
        unsigned int params_size;
    } IRC_Message;

    void irc_command_missing(Context* context, IRC_Message message);

#endif // IRC_COMMANDS_H
