# deu-signal-cagirkan
Nowadays, instant messaging applications are used very often by end users. WhatsApp, the
most popular of these applications, recently announced that it will make some changes in the
security policy it will follow in our country. With this development, such applications
immediately have been discussed by people and the search for a more secure application has
started.
With this assignment, you will start to develop an instant messaging application. You will
implement the first phase for this application. In the first phase, people can send non-secure
messages to others using the messaging server. They can be sent and received using JSON
format that contains from, to and message key-fields. In this system, we must have a
messaging server and we may have more than a client.
## Usage
USAGE: SIMPLY COMPILE AND RUN THE CLIENT AND SERVER PROGRAM WITHOUT PARAMETERS.

PORT NUMBERS ARE IN CODE. NOT GET PORT PARAMETER.

NOTE: WHEN YOU USING '-join' COMMAND DON'T ENTER WRONG GROUP NAME OR WRONG PASSWORD. I CAN'T HANDLED IT IN TIME

PHONE NUMBER MUST BE EXACT 11 CHARS.

SOME IMPORTANT MESSAGES SHOWN IN SERVER SIDE WHEN RUNNING.

```bash
gcc -pthread 2018510100_server.c -o 2018510100_server
gcc -pthread 2018510100_client.c -o 2018510100_client
./server
./client
```

## Commands
```bash
-whoami: shows phone number of client
-gcreate groupName: creates a group
-join groupName: join a existing group
-exit groupName: remove from group
-send message : sends the message if you're in a group
-exit: shutdown the program
```

