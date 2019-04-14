#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MAXLINE 1000


int open_listenfd(char *port) {
	struct addrinfo hints, *listp, *p;
	int listenfd, optval=1;
	/* Get a list of potential server addresses */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM; /* Accept connect. */
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; /* …on any IP addr */
	hints.ai_flags |= AI_NUMERICSERV; /* …using port no. */
	getaddrinfo(NULL, port, &hints, &listp);
	/* Walk the list for one that we can bind to */
	for (p = listp; p; p = p->ai_next) {
		/* Create a socket descriptor */
		if ((listenfd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) < 0)
			continue; /* Socket failed, try the next */
		/* Eliminates "Address already in use" error from bind */
		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
			(const void *)&optval , sizeof(int));
		/* Bind the descriptor to the address */
		if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
			break; /* Success */
		close(listenfd); /* Bind failed, try the next */
	}
	/* Clean up */
	freeaddrinfo(listp);
	if (!p) /* No address worked */
		return -1;
	/* Make it a listening socket ready to accept conn. requests */
	if (listen(listenfd, 1) < 0) { //Only listen for one connection
		close(listenfd);
		return -1;
	}
	return listenfd;
}

void echo(int connfd){
	size_t n;
	char buf[MAXLINE];
	float sum = 0;
	while((n = read (connfd, buf, MAXLINE)) != 0) {
		//Start what I do
		//printf("server received %d bytes\n", (int)n);
		sum+=atof(buf);
		snprintf(buf, MAXLINE, "%f", sum);
		write(connfd, buf, strlen(buf));
		memset(buf, 0, MAXLINE);
	}
}

int main(int argc, char const *argv[]){
	if (argc != 1){
		fprintf(stderr, 
			"Wrong number of command-line arguments. Please reference script.\n");
		return -1;
	}
	//Port = 2000
	//Host = localhost
	int listenfd, connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	char client_hostname[MAXLINE], client_port[MAXLINE];
	listenfd = open_listenfd("2000");
	while (1) {
		clientlen = sizeof(struct sockaddr_storage); /* Important! */
		connfd = accept(listenfd,
			(struct sockaddr * restrict)&clientaddr, &clientlen);
		getnameinfo((const struct sockaddr *) &clientaddr, clientlen,
			client_hostname, MAXLINE, client_port, MAXLINE, 0);
		printf("Connected to (%s, %s)\n", client_hostname, client_port);
		echo(connfd);
		close(connfd);
	}
	exit(0);
	return 0;
}