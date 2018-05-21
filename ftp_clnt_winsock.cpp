// ftp_clnt_winsock.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include <winsock2.h>	// Use winsock.h if you're at WinSock 1.1
#include <stdio.h>
#pragma comment(lib, "Ws2_32.lib")


void errexit(const char *, ...);
void pause(void);
void replylogcode(int code)
{
	switch(code){
		case 200:
			printf("Command okay");
			break;
		case 500:
			printf("Syntax error, command unrecognized.");
			printf("This may include errors such as command line too long.");
			break;
		case 501:
			printf("Syntax error in parameters or arguments.");
			break;
		case 202:
			printf("Command not implemented, superfluous at this site.");
			break;
		case 502:
			printf("Command not implemented.");
			break;
		case 503:
			printf("Bad sequence of commands.");
			break;
		case 530:
			printf("Not logged in.");
			break;
	}
	printf("\n");
}

char* sendCommand(char str[100])
{
	//sprintf(buf,"USER %s\r\n",info);
	return NULL;
}


int _tmain(int argc, char* argv[])
{
	WORD wVersionRequested;
    WSADATA wsaData;
	int retcode;

	SOCKET socket_descriptor;

	char ServerName[64];
	HOSTENT *pHostEnt;
	struct sockaddr_in sin;

	char Message[] = "Greetings from the client";

	char Buffer[4096];
	int length;

	
	/* 
	[1] WSAStartup() must be called before any other socket
	routines.  The following code prints all returned
	information valid in WinSock 2.

    WSAStartup returns the following information in the
	WSAData structure.

	struct WSAData { 
		WORD            wVersion; 
		WORD            wHighVersion; 
		char            szDescription[WSADESCRIPTION_LEN+1]; 
		char            szSystemStatus[WSASYSSTATUS_LEN+1]; 
		unsigned short  iMaxSockets; (ignore in WinSock 2)
		unsigned short  iMaxUdpDg;   (ignore in WinSock 2)
		char FAR *      lpVendorInfo;(ignore in WinSock 2)
	};

    Note that WSAGetLastError() should not be used since the
	error code may not be saved properly if winsock.dll did
	not load.
	*/


	printf("WSAStartup()\n");
	
	wVersionRequested = MAKEWORD(2,2);	// Use MAKEWORD(1,1) if you're at WinSock 1.1
	retcode = WSAStartup(wVersionRequested, &wsaData);
	if(retcode != 0)
		errexit("Startup failed: %d\n", retcode);

	printf("Return Code: %i\n", retcode);
	printf("Version Used: %i.%i\n", LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
	printf("Version Supported:  %i.%i\n", LOBYTE(wsaData.wHighVersion), HIBYTE(wsaData.wHighVersion));
	printf("Implementation: %s\n", wsaData.szDescription);
	printf("System Status: %s\n", wsaData.szSystemStatus);
    printf("\n");
	pause();

	if ( LOBYTE( wsaData.wVersion ) != LOBYTE( wVersionRequested ) ||
			HIBYTE( wsaData.wVersion ) !=  HIBYTE( wVersionRequested ) )
	{
		printf("Supported version is too low\n");
		WSACleanup( );
		return 0; 
	}


	/* 
	[2] Once WSAStartup has been called, the socket can be
	created using the socket() call.  The following creates an
	Internet protocol family (PF_INET) socket providing stream
	service (SOCK_STREAM).
	*/

	printf("socket()\n");
	
	socket_descriptor = socket(PF_INET, SOCK_STREAM, 0);
	if(socket_descriptor == INVALID_SOCKET)
		errexit("Socket creation failed: %d\n", WSAGetLastError());

	printf("Socket Descriptor: %i\n", socket_descriptor);
	printf("\n");
	pause();


	/*
    [3] Before making a connection, an Internet address
	family structure must be initialized.

	struct sockaddr_in {
		short sin_family;
		u_short sin_port;
		struct in_addr sin_addr;
		char sin_zero[8];
	};

	It is first set to all zeros.  The family is AF_INET.
	We'll use 4984 as the port number for this example.
	htons() converts to network byte ordering.

	gethostbyname() is used to determine the address of the
    remote host.  For this example, we connect to the host
	specified in the first command line argument or the local
	host ("localhost") if no command line arguments are
	provided, using the first address for this host in the
	h_addr_list.

	gethostbyname() returns a pointer to the following
	structure.  Some of the contents are printed below.

	struct hostent {
		char FAR * h_name;
		char FAR * FAR * h_aliases;
		short h_addrtype;
		short h_length;
		char FAR * FAR * h_addr_list;
	};
    */

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(21);

	if(argc > 1) strcpy(ServerName, argv[1]);
	else strcpy(ServerName, "localhost");
	
	
	printf("gethostbyname(\"%s\")\n", ServerName);

	if (pHostEnt = gethostbyname(ServerName)) {
		memcpy(&sin.sin_addr, pHostEnt->h_addr_list[0], pHostEnt->h_length);

		printf("Address Length: %d\n", pHostEnt->h_length);
		printf("Host Address: %s\n", inet_ntoa(sin.sin_addr));
		printf("Host Name: %s\n", pHostEnt->h_name);
		printf("\n");
		pause();
	}
	else errexit("Can't get %s\" host entry: %d\n", ServerName, WSAGetLastError());

	
	/*
	[4] connect() is used to establish a connection to a remote
	endpoint.
	*/

	printf("connect()\n");
	
	retcode = connect(socket_descriptor, (struct sockaddr *) &sin, sizeof(sin));
	if(retcode == SOCKET_ERROR)
		errexit("Connect failed: %d\n", WSAGetLastError());

	printf("Return Code: %d\n", retcode);
	printf("\n");
	pause();


	/*
	[5] send() is used to send a message to the server.  No
	flags are set.
	

	printf("send()\n");
	
	retcode = send(socket_descriptor, Message, sizeof Message, 0);
	if(retcode == SOCKET_ERROR)
		errexit("Send failed: %d\n", WSAGetLastError());

	printf("Bytes Sent: %d\n", retcode);
	printf("\n");
	pause();
	*/

	char buf[BUFSIZ+1];
	int tmpres, size, status;
	/*
	Connection Establishment
	   120
		  220
	   220
	   421
	Login
	   USER
		  230
		  530
		  500, 501, 421
		  331, 332
	   PASS
		  230
		  202
		  530
		  500, 501, 503, 421
		  332
	*/
	char * str;
	int codeftp;
	printf("Connection established, waiting for welcome message...\n");
	//How to know the end of welcome message: http://stackoverflow.com/questions/13082538/how-to-know-the-end-of-ftp-welcome-message
	memset(buf, 0, sizeof buf);
	while((tmpres = recv(socket_descriptor, buf, BUFSIZ, 0)) > 0){
		sscanf(buf,"%d", &codeftp);
		printf("%s", buf);
		if(codeftp != 220) //120, 240, 421: something wrong
		{
			replylogcode(codeftp);
			exit(1);
		}

		str = strstr(buf, "220");//Why ???
		if(str != NULL){
			break;
		}
		memset(buf, 0, tmpres);
	}
	//Send Username
	char info[50];
	printf("Name (%s): ", inet_ntoa(sin.sin_addr));
	memset(buf, 0, sizeof buf);
	scanf("%s", info);

	sprintf(buf,"USER %s\r\n",info);
	tmpres = send(socket_descriptor, buf, strlen(buf), 0);

	memset(buf, 0, sizeof buf);
	tmpres = recv(socket_descriptor, buf, BUFSIZ, 0);

	sscanf(buf,"%d", &codeftp);
	if(codeftp != 331)
	{
		replylogcode(codeftp);
		exit(1);
	}
	printf("%s", buf);

	//Send Password
	memset(info, 0, sizeof info);
	printf("Password: ");
	memset(buf, 0, sizeof buf);
	scanf("%s", info);

	sprintf(buf,"PASS %s\r\n",info);
	tmpres = send(socket_descriptor, buf, strlen(buf), 0);

	memset(buf, 0, sizeof buf);
	tmpres = recv(socket_descriptor, buf, BUFSIZ, 0);

	sscanf(buf,"%d", &codeftp);
	if(codeftp != 230)
	{
		replylogcode(codeftp);
		exit(1);
	}
	printf("%s", buf);
	pause();

	/////////////////
	//My code 
	//Tao thu muc rong
	memset(info, 0, sizeof info);
	printf("Directory name: ");
	memset(buf, 0, sizeof buf);
	scanf("%s", info);
	sprintf(buf, "XMKD %s\r\n", info);
	tmpres = send(socket_descriptor, buf, strlen(buf), 0);
	
	memset(buf, 0, sizeof buf);
	tmpres = recv(socket_descriptor, buf, BUFSIZ, 0);
	sscanf(buf, "%d", &codeftp);
	if (codeftp != 257)
	{
		replylogcode(codeftp);
		exit(1);
	}
	printf("%s", buf);
	pause();

	//Xoa thu muc rong
	memset(info, 0, sizeof info);
	printf("Directory name: ");
	memset(buf, 0, sizeof buf);
	scanf("%s", info);
	sprintf(buf, "XRMD %s\r\n", info);
	tmpres = send(socket_descriptor, buf, strlen(buf), 0);

	memset(buf, 0, sizeof buf);
	tmpres = recv(socket_descriptor, buf, BUFSIZ, 0);
	sscanf(buf, "%d", &codeftp);
	if (codeftp != 250)
	{
		replylogcode(codeftp);
		exit(1);
	}
	printf("%s", buf);
	pause();

	//Lenh DETELE Xoa mot file
	memset(info, 0, sizeof info);
	printf("File name: ");
	memset(buf, 0, sizeof buf);
	scanf("%s", info);
	sprintf(buf, "DELE %s\r\n", info);
	tmpres = send(socket_descriptor, buf, strlen(buf), 0);

	memset(buf, 0, sizeof buf);
	tmpres = recv(socket_descriptor, buf, BUFSIZ, 0);
	sscanf(buf, "%d", &codeftp);
	if (codeftp != 250)
	{
		replylogcode(codeftp);
		exit(1);
	}
	printf("%s", buf);
	pause();

	//Lenh QUIT
	memset(buf, 0, sizeof buf);
	sprintf(buf, "QUIT\r\n");
	tmpres = send(socket_descriptor, buf, strlen(buf), 0);

	memset(buf, 0, sizeof buf);
	tmpres = recv(socket_descriptor, buf, BUFSIZ, 0);
	sscanf(buf, "%d", &codeftp);
	if (codeftp != 221)
	{
		replylogcode(codeftp);
		exit(1);
	}
	printf("%s", buf);
	pause();

	


	///////////////////////
	/*
	[7] recv() is used to receive a message from the server.
	No flags are set.  This code assumes that all data is
	read with the first and only recv() call.  In general,
	this is not a good assumption since a stream transport
	service is used.
	

	printf("recv()\n");
	
	length = recv(socket_descriptor, Buffer, sizeof Buffer, 0);
	if(length == SOCKET_ERROR)
		errexit("Receive failed: %d\n", WSAGetLastError());

	printf("Bytes received: %d\n", length);
	printf("Message: %s\n", Buffer);
	printf("\n");
	pause();
	*/

	/*
	[8] The client closes its socket using closesocket();
	*/

	printf("closesocket()\n");
	
	retcode = closesocket(socket_descriptor);
	if(retcode == SOCKET_ERROR)
		errexit("Close socket failed: %d\n", WSAGetLastError());

    printf("Return Code: %d\n", retcode);
	printf("\n");
	pause();


	/*
	[9] WSACleanup() is used to terminate use of socket services.
	*/

    printf("WSACleanup()\n");
	
	retcode = WSACleanup();
	if(retcode == SOCKET_ERROR)
		errexit("Cleanup failed: %d\n", WSAGetLastError());

	printf("Return Code: %d\n", retcode);
	printf("\n");
	pause();
	
	return 0;
}



void errexit(const char *format, ...)
{
	va_list	args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	WSACleanup();
	pause();
	exit(1);
}


void pause(void)
{
	char c;

	printf("Press Enter to continue\n");
	scanf("%c", &c);
}


