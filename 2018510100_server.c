//USAGE: SIMPLY COMPILE AND RUN THE CLIENT AND SERVER PROGRAM WITHOUT PARAMETERS. 

//PORT NUMBERS ARE IN CODE. NOT GET PORT PARAMETER.

//NOTE: WHEN YOU USING '-join' COMMAND DON'T ENTER WRONG GROUP NAME OR WRONG PASSWORD. I CAN'T HANDLED IT IN TIME

//PHONE NUMBER MUST BE EXACT 11 CHARS.

//SOME IMPORTANT MESSAGES SHOWN IN SERVER SIDE WHEN RUNNING.

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <string.h>

#include <pthread.h>

#include <sys/types.h>



#define MAX_CLIENTS 100

#define MAX_GROUPS 10

#define GROUP_CAPACITY 30

#define BUFFER_SZ 2048



static int uid = 10;



/* Client structure */

typedef struct

{

    struct sockaddr_in address;

    int sockfd;

    int uid;

    char phoneNumber[11];

} client_t;



/* Group structure */

typedef struct

{

    char *groupName;

    char *groupPass;

    client_t *users[GROUP_CAPACITY];

} group_t;



/* Message split structure */

typedef struct

{

    char *command;

    char *param;

    char *remainder;

} messageFields;

//Groups stored here

group_t *groups[MAX_GROUPS];

//Client mutex initialize

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

//For flush stdout

void str_overwrite_stdout()

{

    printf("\r%s", "> ");

    fflush(stdout);

}

//trims lf char

void str_trim_lf(char *arr, int length)

{

    int i;

    for (i = 0; i < length; i++)

    { // trim \n

        if (arr[i] == '\n')

        {

            arr[i] = '\0';

            break;

        }

    }

}

/* Get clients message and split */

messageFields getCommandParam(char *s)

{

    char delimiter[] = " ";

    //command is command, param is command's parameter, remainder is message.

    char *command, *param, *remainder, *context, *sCopy;



    int inputLength = strlen(s);

    sCopy = (char *)calloc(inputLength + 1, sizeof(char));

    strncpy(sCopy, s, inputLength);



    command = strtok_r(sCopy, delimiter, &context);

    param = strtok_r(NULL, delimiter, &context);

    remainder = context;



    messageFields msgFields;

    msgFields = (messageFields){.command = command, .param = param, .remainder = remainder};



    return msgFields;

}



/* Create group and add first client*/

void createGroup(messageFields fields, client_t *cl)

{

    group_t *group = (group_t *)malloc(sizeof(group_t));

    group->groupName = fields.param;

    group->groupPass = fields.remainder;

    group->users[0] = cl;

    pthread_mutex_lock(&clients_mutex);



    for (int i = 0; i < MAX_GROUPS; ++i)

    {

        if (!groups[i])

        {

            groups[i] = group;

            break;

        }

    }

    pthread_mutex_unlock(&clients_mutex);

    printf("Group %s created by %s with password %s\n", groups[0]->groupName, groups[0]->users[0]->phoneNumber, groups[0]->groupPass);

}



/* Remove clients to group */

void exitGroup(messageFields fields, client_t *cl)

{

    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_GROUPS; ++i)

    {

        if (groups[i])

        {

            if (strcmp(groups[i]->groupName, fields.param) == 0)

            {

                for (int j = 0; j < GROUP_CAPACITY; j++)

                {

                    if (groups[i]->users[j])

                    {

                        if (strcmp(groups[i]->users[j]->phoneNumber, cl->phoneNumber) == 0)

                        {

                            groups[i]->users[j] = NULL;

                            printf("User: %s removed from Group: %s\n", cl->phoneNumber, groups[i]->groupName);

                            break;

                        }

                    }

                }

            }

        }

        pthread_mutex_unlock(&clients_mutex);

    }

}



/* Add clients to group */

void joinGroup(messageFields fields, client_t *cl)

{

    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_GROUPS; ++i)

    {

        if (groups[i])

        {

            if (strcmp(groups[i]->groupName, fields.param) == 0)

            {

                if (strcmp(groups[i]->groupPass, fields.remainder) != 0)

                {

                    printf("Wrong password for group: %s\n", groups[i]->groupName);

                    break;

                }

                for (int j = 0; j < GROUP_CAPACITY; j++)

                {

                    if (!groups[i]->users[j])

                    {

                        printf("User: %s added to Group: %s\n", cl->phoneNumber, groups[i]->groupName);

                        groups[i]->users[j] = cl;

                        break;

                    }

                }

                break;

            }

        }

    }



    pthread_mutex_unlock(&clients_mutex);

}

//Send message to every group member except sender.

void sendMessage(messageFields fields, client_t *cl)

{

    pthread_mutex_lock(&clients_mutex);



    for (int i = 0; i < MAX_GROUPS; ++i)

    {

        if (groups[i])

        {

            if (strcmp(groups[i]->groupName, fields.param) == 0)

            {

                for (int j = 0; j < GROUP_CAPACITY; j++)

                {

                    if (groups[i]->users[j])

                    {

                        if (strcmp(groups[i]->users[j]->phoneNumber, cl->phoneNumber) != 0)

                        {

                            if (write(groups[i]->users[j]->sockfd, fields.remainder, strlen(fields.remainder)) < 0)

                            {

                                printf("ERROR: write to descriptor failed\n");

                                break;

                            }

                        }

                    }

                }

            }

        }

    }

    pthread_mutex_unlock(&clients_mutex);

}

//I used this for debugging the groups array of struct.

void showGroups()

{

    for (int i = 0; i < MAX_GROUPS; i++)

    {

        if (groups[i])

        {

            printf("Group Name = %s\nGroup Pass = %s\n", groups[i]->groupName, groups[i]->groupPass);

            for (int j = 0; j < GROUP_CAPACITY; j++)

            {

                if (groups[i]->users[j])

                {

                    printf("User-%d Phone = %s\n", j, groups[i]->users[j]->phoneNumber);

                }

            }

        }

    }

}



/* Get command and interpret */

void commandInterpreter(char *s, client_t *cl)

{

    char commands[5][10] = {"-gcreate", "-join", "-exit", "-send"};

    char *command = "";

    messageFields fields;



    //Get message and find command

    for (int i = 0; i < 4; i++)

    {

        command = strstr(s, commands[i]);

        if (command == s)

        {

            command = commands[i];

            break;

        }

        command = "";

    }

    fields = getCommandParam(s);

    if (!strcmp(command, commands[0]))

    {

        createGroup(fields, cl);

    }

    else if (!strcmp(command, commands[1]))

    {

        joinGroup(fields, cl);

    }

    else if (!strcmp(command, commands[2]))

    {

        exitGroup(fields, cl);

    }

    else if (!strcmp(command, commands[3]))

    {

        sendMessage(fields, cl);

    }

}



/* Handle all communication with the client */

void *handle_client(void *arg)

{

    char buff_out[BUFFER_SZ];

    char phoneNumber[11];

    int leave_flag = 0;



    client_t *cli = (client_t *)arg;



    // phoneNumber

    if (recv(cli->sockfd, phoneNumber, 11, 0) != 11)

    {

        printf("Didn't enter the Phone Number.\n");

        leave_flag = 1;

    }

    else

    {

        strcpy(cli->phoneNumber, phoneNumber);

        sprintf(buff_out, "%s has joined\n", cli->phoneNumber);

        printf("%s", buff_out);

        commandInterpreter(buff_out, cli);

    }



    bzero(buff_out, BUFFER_SZ);



    while (1)

    {

        if (leave_flag)

        {

            break;

        }



        int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);

        if (receive > 0)

        {

            if (strlen(buff_out) > 0)

            {

                commandInterpreter(buff_out, cli);

                str_trim_lf(buff_out, strlen(buff_out));

                printf("%s -> %s\n", buff_out, cli->phoneNumber);

            }

        }

        else if (receive == 0 || strcmp(buff_out, "exit") == 0)

        {

            sprintf(buff_out, "%s has left\n", cli->phoneNumber);

            printf("%s", buff_out);

            commandInterpreter(buff_out, cli);

            leave_flag = 1;

        }

        else

        {

            printf("ERROR: -1\n");

            leave_flag = 1;

        }



        bzero(buff_out, BUFFER_SZ);

    }



    /*Yield thread */

    close(cli->sockfd);

    free(cli);

    pthread_detach(pthread_self());



    return NULL;

}



int main(int argc, char **argv)

{

    char *ip = "127.0.0.1";

    int port = 3205;

    int option = 1;

    int listenfd = 0, connfd = 0;

    struct sockaddr_in serv_addr;

    struct sockaddr_in cli_addr;

    pthread_t tid;



    /* Socket settings */

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;

    serv_addr.sin_addr.s_addr = inet_addr(ip);

    serv_addr.sin_port = htons(port);



    if (setsockopt(listenfd, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char *)&option, sizeof(option)) < 0)

    {

        perror("ERROR: setsockopt failed");

        return 1;

    }



    /* Bind */

    if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)

    {

        perror("ERROR: Socket binding failed");

        return 1;

    }



    /* Listen */

    if (listen(listenfd, 10) < 0)

    {

        perror("ERROR: Socket listening failed");

        return 1;

    }



    printf("=== WELCOME TO THE SIGNAL V1.0 ===\n");



    while (1)

    {

        socklen_t clilen = sizeof(cli_addr);

        connfd = accept(listenfd, (struct sockaddr *)&cli_addr, &clilen);



        /* Client settings */

        client_t *cli = (client_t *)malloc(sizeof(client_t));

        cli->address = cli_addr;

        cli->sockfd = connfd;

        cli->uid = uid++;



        /* Add client to the queue and fork thread */

        pthread_create(&tid, NULL, &handle_client, (void *)cli);



        /* Reduce CPU usage */

        sleep(1);

    }



    return 0;

}