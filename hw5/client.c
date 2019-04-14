#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MAXLINE 1000

int open_clientfd(char *hostname, char *port) {
	int clientfd;
	struct addrinfo hints, *listp, *p;
	/* Get a list of potential server addresses */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM; /* Open a connection */
	hints.ai_flags = AI_NUMERICSERV; /* …using numeric port arg. */
	hints.ai_flags |= AI_ADDRCONFIG; /* Recommended for connections */
	getaddrinfo(hostname, port, &hints, &listp);

	/* Walk the list for one that we can successfully connect to */
	for (p = listp; p; p = p->ai_next) {
	/* Create a socket descriptor */
		if ((clientfd = socket(p->ai_family, p->ai_socktype,
		p->ai_protocol)) < 0)
			continue; /* Socket failed, try the next */
		/* Connect to the server */
		if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1)
			break; /* Success */
		close(clientfd); /* Connect failed, try another */
	}
	/* Clean up */
	freeaddrinfo(listp);
	if (!p) /* All connects failed */
	return -1;
	else /* The last connect succeeded */
	return clientfd;
}



int main(int argc, char const *argv[])
{
	if (argc != 1){
		fprintf(stderr, 
			"Wrong number of command-line arguments. Please reference script.\n");
		return -1;
	}
	//Port = 2000
	//Host = localhost
	int clientfd;
	char buf[MAXLINE];
	float input;
	clientfd = open_clientfd("localhost", "2000");
	while (fgets(buf, MAXLINE, stdin) != NULL) {
		write(clientfd, buf, strlen(buf));
		memset(buf, 0, MAXLINE);
		read(clientfd, buf, MAXLINE);
		fputs(buf, stdout);
		printf("\n");
		memset(buf, 0, MAXLINE);
	}
	close(clientfd);
	exit(0);
}