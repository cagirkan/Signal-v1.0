//USAGE: SIMPLY COMPILE AND RUN THE CLIENT AND SERVER PROGRAM WITHOUT PARAMETERS. 

//PORT NUMBERS ARE IN CODE. NOT GET PORT PARAMETER.

//NOTE: WHEN YOU USING '-join' COMMAND DON'T ENTER WRONG GROUP NAME OR WRONG PASSWORD. I CAN'T HANDLED IT IN TIME

//PHONE NUMBER MUST BE EXACT 11 CHARS.

//SOME IMPORTANT MESSAGES SHOWN IN SERVER SIDE WHEN RUNNING.

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <pthread.h>



#define LENGTH 2048



typedef struct

{

    char *command;

    char *param;

    char *remainder;

} messageFields;



// Global variables

int flag = 0;

int sockfd = 0;

char phoneNum[11];

char groupName[32];



void str_overwrite_stdout()

{

    printf("%s", "> ");

    fflush(stdout);

}



void str_overwrite_stdout_group(char *groupName)

{

    printf("%s>",groupName);

    fflush(stdout);

}



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



messageFields getCommandParam(char *s)

{

    char delimiter[] = " ";

    char *cmd, *param, *remainder, *context, *sCopy;



    int inputLength = strlen(s);

    sCopy = (char *)calloc(inputLength + 1, sizeof(char));

    strncpy(sCopy, s, inputLength);



    cmd = strtok_r(sCopy, delimiter, &context);

    param = strtok_r(NULL, delimiter, &context);

    remainder = context;

    messageFields msgFields;

    msgFields = (messageFields){.command = cmd, .param = param, .remainder = remainder};



    return msgFields;

}



char* getCommand(char *s)

{

    char commands[5][10] = {"-gcreate", "-join", "-exit", "-send", "-groups"};

    char *command = "";



    //Get message and find command

    for (int i = 0; i < 5; i++)

    {

        command = strstr(s, commands[i]);

        if (command == s)

        {

            command = commands[i];

            break;

        }

        command = "";

    }

    return command;

}

//Control exit command in group

int exitGroup(char *s)

{

    char *command = "";

    command = strstr(s, "-exit");

    if (command == s)

        return 1;

    return 0;

}



int sendListener(char *s)

{

    char *command = "";

    command = strstr(s, "-send");

    if (command == s)

        return 1;

    return 0;

}



void send_msg_handler()

{

    char message[LENGTH] = {};

    char buffer[LENGTH + 32] = {};

    char msgCopy[LENGTH + 32] = {};

    char password[16] = {};

    char passMessage[16] = {};

    char *command;

    



    while (1)

    {

        messageFields *msgF = (messageFields*)malloc(sizeof(messageFields));

        

        str_overwrite_stdout();

        fgets(message, LENGTH, stdin);

        str_trim_lf(message, LENGTH);

        sprintf(msgCopy,"%s",message);

        *msgF = getCommandParam(msgCopy);

        command = getCommand(message);



        



        if (strcmp(message, "-exit") == 0)

        {

            flag = 1;

            break;

        }

        else if (strcmp(message, "-whoami") == 0)

        {

            printf("Your phone number is: %s\n", &phoneNum);

        }

        else if (strcmp(command, "-gcreate") == 0)

        {

            printf("Please Create a Group Password: ");

            fgets(password, 16, stdin);

            str_trim_lf(password, 16);

            sprintf(buffer, "%s %s\n",message, password);

            send(sockfd, buffer, strlen(buffer), 0);

            strcpy(groupName, msgF->param);

            while (!exitGroup(message))

            {

                str_overwrite_stdout_group(groupName);

                fgets(message, LENGTH, stdin);

                *msgF = getCommandParam(message);

                if(strcmp(msgF->command,"-exit") == 0 && strcmp(msgF->param,groupName))

                {

                    sprintf(buffer, "%s %s %s %s\n",msgF->command, groupName, msgF->param, msgF->remainder);

                    send(sockfd, buffer, strlen(buffer), 0);

                    break;

                }

                while(!sendListener(message)){

                    printf("Please use -send command to send a message!..\nOr -exit groupName command to exit group\n");

                    str_overwrite_stdout_group(groupName);

                    fgets(message, LENGTH, stdin);

                    *msgF = getCommandParam(message);

                    if(strcmp(msgF->command,"-exit") == 0 && strcmp(msgF->param,groupName))

                    {

                        sprintf(buffer, "%s %s %s %s\n",msgF->command, groupName, msgF->param, msgF->remainder);

                        send(sockfd, buffer, strlen(buffer), 0);

                        break;

                    }

                }

                str_trim_lf(message, LENGTH);

                sprintf(buffer, "%s %s %s says: %s %s\n",msgF->command, groupName, phoneNum, msgF->param, msgF->remainder);

                send(sockfd, buffer, strlen(buffer), 0);

            }

            

        }

        else if (strcmp(command, "-join") == 0)

        {

            printf("Please Enter Group Password: ");

            fgets(password, 16, stdin);

            str_trim_lf(password, 16);

            sprintf(buffer, "%s %s\n",message, password);

            send(sockfd, buffer, strlen(buffer), 0);

            strcpy(groupName, msgF->param);

            while (!exitGroup(message))

            {

                str_overwrite_stdout_group(groupName);

                fgets(message, LENGTH, stdin);

                *msgF = getCommandParam(message);

                if(strcmp(msgF->command,"-exit") == 0 && strcmp(msgF->param,groupName))

                {

                    sprintf(buffer, "%s %s %s %s\n",msgF->command, groupName, msgF->param, msgF->remainder);

                    send(sockfd, buffer, strlen(buffer), 0);

                    break;

                }

                while(!sendListener(message)){

                    printf("Please use -send command to send a message!..\nOr -exit groupName command to exit group\n");

                    str_overwrite_stdout_group(groupName);

                    fgets(message, LENGTH, stdin);

                    *msgF = getCommandParam(message);

                    if(strcmp(msgF->command,"-exit") == 0 && strcmp(msgF->param,groupName))

                    {

                        sprintf(buffer, "%s %s %s %s\n",msgF->command, groupName, msgF->param, msgF->remainder);

                        send(sockfd, buffer, strlen(buffer), 0);

                        break;

                    }

                }

                str_trim_lf(message, LENGTH);

                sprintf(buffer, "%s %s %s says: %s %s\n",msgF->command, groupName, phoneNum, msgF->param, msgF->remainder);

                send(sockfd, buffer, strlen(buffer), 0);

            }

        }

        else if(strcmp(command, "-send") == 0)

        {

            printf("First you must join a group or creaate a new one!.\n");

        }

         else if(strcmp(command, "-groups") == 0)

        {

            

        }

        else{

            printf("\nPlease use -gcreate, -join, -exit, -whoami commands\n");

        }



        bzero(message, LENGTH);

        bzero(buffer, LENGTH + 32);

    }

    

}



void recv_msg_handler()

{

    char message[LENGTH] = {};

    while (1)

    {

        int receive = recv(sockfd, message, LENGTH, 0);

        if (receive > 0)

        {

            printf("%s", message);

            str_overwrite_stdout();

        }

        else if (receive == 0)

        {

            break;

        }

        else

        {

            // -1

        }

        memset(message, 0, sizeof(message));

    }

}



int main(int argc, char **argv)

{

    char *ip = "127.0.0.1";

    int port = 3205;



    printf("Please enter your phone number: ");

    fgets(phoneNum, 12, stdin);

    printf("\n%s", &phoneNum);

    str_trim_lf(phoneNum, strlen(phoneNum));



    if (strlen(phoneNum) != 11)

    {

        printf("phoneNum must be 11 characters.\n");

        return 1;

    }



    struct sockaddr_in server_addr;



    /* Socket settings */

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;

    server_addr.sin_addr.s_addr = inet_addr(ip);

    server_addr.sin_port = htons(port);



    // Connect to Server

    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (err == -1)

    {

        printf("ERROR: connect\n");

        return 1;

    }



    // Send phoneNum

    send(sockfd, phoneNum, 11, 0);



    printf("=== WELCOME TO THE SIGNAL V1.0 ===\n");



    pthread_t send_msg_thread;

    if (pthread_create(&send_msg_thread, NULL, (void *)send_msg_handler, NULL) != 0)

    {

        printf("ERROR: pthread\n");

        return 1;

    }



    pthread_t recv_msg_thread;

    if (pthread_create(&recv_msg_thread, NULL, (void *)recv_msg_handler, NULL) != 0)

    {

        printf("ERROR: pthread\n");

        return 1;

    }



    while (1)

    {

        if (flag)

        {

            printf("\nExiting the program.\n");

            sleep(1);

            printf("See you soon.\n");

            break;

        }

    }



    close(sockfd);



    return 0;

}