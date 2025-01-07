#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

static const char *AESD_PORT = "9000";
static const int AESD_CONN_BACKLOG = 10;
static const char *DATA_FILE_NAME = "/var/tmp/aesdsocketdata";
static const int BUFF_SIZE = 1024;
static int DATA_FILE_FD = -1;
static int SERVER_SOCK_FD = -1;

static void termination_handler(int signum) {
    close(SERVER_SOCK_FD);
    close(DATA_FILE_FD);
    remove(DATA_FILE_NAME);
    syslog(LOG_INFO, "Caught signal %d, exiting ", signum);
}

static void regSigHandler() {
    struct sigaction sigAction;
    sigAction.sa_handler = termination_handler;
    sigemptyset(&sigAction.sa_mask);
    sigAction.sa_flags = 0;

    sigaction(SIGINT, &sigAction, NULL);
    sigaction(SIGTERM, &sigAction, NULL);
}

static void daemonize() {
    pid_t pid;

    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    if (setsid() < 0) exit(EXIT_FAILURE);

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    umask(0);
    chdir("/");

    close(STDIN_FILENO); /* Reopen standard fd's to /dev/null */
    int fd = open("/dev/null", O_RDWR);
    if (fd != STDIN_FILENO) {
        syslog(LOG_ERR, "/dev/null fd not zero error\n");
        exit(EXIT_FAILURE);
    }

    if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO) {
        syslog(LOG_ERR, "dup2 STDOUT_FILENO error\n");
        exit(EXIT_FAILURE);
    }
    if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO) {
        syslog(LOG_ERR, "dup2 STDERR_FILENO error\n");
        exit(EXIT_FAILURE);
    }
}

static int sendall(int cfd, char *buf, int *len) {
    int total = 0;
    int bytesleft = *len;
    int n;

    while (total < *len) {
        n = send(cfd, buf + total, bytesleft, 0);
        if (n == -1) {
            break;
        }
        total += n;
        bytesleft -= n;
    }

    *len = total;
    if (n <= 0) {
        printf("Error sendall n = %d, errno = %d\n", n, errno);
    }
    if (n == -1 && errno == EPIPE) {
        return 2;
    }
    return n == -1 ? 1 : 0;
}

static int inetPassiveSocket(const char *service, int type,
                             socklen_t *addrlen) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, optval, s;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = type;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;

    s = getaddrinfo(NULL, service, &hints, &result);
    if (s != 0) return -1;

    optval = 1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) continue; /* On error, try next address */

        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval,
                       sizeof(optval)) == -1) {
            close(sfd);
            freeaddrinfo(result);
            return -1;
        }

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) break; /* Success */

        /* bind() failed: close this socket and try next address */

        close(sfd);
    }

    freeaddrinfo(result);

    if (rp != NULL && addrlen != NULL)
        *addrlen = rp->ai_addrlen; /* Return address structure size */

    return (rp == NULL) ? -1 : sfd;
}

static int inetListen(const char *service, int backlog, socklen_t *addrlen,
                      bool isDeamon) {
    int sfd = inetPassiveSocket(service, SOCK_STREAM, addrlen);
    if (isDeamon) {
        daemonize();
    }
    if (sfd != -1) {
        if (listen(sfd, backlog) == -1) {
            return -1;
        }
    }
    return sfd;
}

static int makeServer(bool isDeamon) {
    socklen_t len;
    int listenFd = inetListen(AESD_PORT, AESD_CONN_BACKLOG, &len, isDeamon);
    return listenFd;
}

static void appendData(char buff) {
    if (write(DATA_FILE_FD, &buff, 1) < 1) {
        syslog(LOG_ERR, "write data error\n");
    }
}

static void sendData(int cfd) {
    char dataDuff[BUFF_SIZE];
    FILE *fp = fopen(DATA_FILE_NAME, "r");
    if (fp == NULL) {
        syslog(LOG_ERR, "open data for read error\n");
        return;
    }
    int bytesRead = 0;
    while ((bytesRead = fread(dataDuff, 1, BUFF_SIZE, fp)) > 0) {
        sendall(cfd, dataDuff, &bytesRead);
    }
}

static void processConn(int cfd) {
    char buff;
    while (recv(cfd, &buff, 1, 0) > 0) {
        appendData(buff);
        if (buff == '\n') {
            break;
        }
    }
    sendData(cfd);
    close(cfd);
}

int main(int argc, char *argv[]) {
    openlog(argv[0], LOG_PID | LOG_CONS | LOG_NOWAIT, LOG_USER);

    regSigHandler();

    int dataFd = open(DATA_FILE_NAME, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (dataFd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    DATA_FILE_FD = dataFd;

    bool isDeamon = (argc > 1 && strcmp(argv[1], "-d") == 0);

    int listenFd = makeServer(isDeamon);
    if (listenFd != -1) {
        SERVER_SOCK_FD = listenFd;
        struct sockaddr_in connAddr;
        int clientLen = sizeof(struct sockaddr);
        for (;;) {
            int cfd = accept(listenFd, (struct sockaddr *)&connAddr,
                             (socklen_t *)&clientLen);
            if (cfd != -1) {
                syslog(LOG_INFO, "Accepted connection from %s\n",
                       inet_ntoa(connAddr.sin_addr));
                processConn(cfd);
                syslog(LOG_INFO, "Closed connection from %s\n",
                       inet_ntoa(connAddr.sin_addr));
            } else {
                syslog(LOG_ERR, "Accept error\n");
                break;
            }
        }
    } else {
        printf("Make server failed\n");
    }
    return 0;
}