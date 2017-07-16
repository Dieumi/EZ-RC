#ifndef IRC_COMMANDS_H

    #define IRC_COMMANDS_H

    #define IRC_MESSAGE_MAX_LENGTH 513
    #define IRC_MAX_PARAMETERS 15

    #define IRC_MAX_CHANNEL_USERS 10

    #define CRLF "\r\n"

    #include "map.h"

    typedef struct Context
    {
        int sender_socket;
        char response[513];
        HashMap* nicks;
        HashMap* users;
        HashMap* channels;
        char reverse_nicks[256][512];
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

    typedef struct IRC_Channel
    {
        char users[IRC_MAX_CHANNEL_USERS][512];
        unsigned int users_size;
    } IRC_Channel;

    void irc_command_missing(Context* context, IRC_Message message);
    unsigned char in_array(char array[IRC_MAX_CHANNEL_USERS][512], char value[512]);

#endif // IRC_COMMANDS_H
