#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>      
#include <netinet/in.h>     
#include <arpa/inet.h>

int ser_fd;
struct sockaddr_in ser_addr;
int Pi_Port;
char *Pi_IP;
char *message;
char *temp_r;
char *token;

#define PR_ERR(str)  printf ("!!! Error in %s function\n", str)

void sig_handler()
{
    printf("========= QUICK TURN OFF ========\n");
    free (message);
    free (Pi_IP);
    free (temp_r);
    close (ser_fd);
    exit(EXIT_SUCCESS);
}

int main(){
    Pi_IP = malloc(20);
    message = malloc(50);
    temp_r = malloc(3);
    token = malloc(10);
    socklen_t len = sizeof (struct sockaddr_in);

// Signal Ctrl C register
    if (signal(SIGINT,sig_handler) == SIG_ERR)
    {
        printf("Can not handler SIGINT\n");
    }
    
    printf ("Enter the Pi4 server IP: ");
    fflush(stdout);
    fgets (Pi_IP, 20, stdin);
    Pi_IP[strcspn(Pi_IP, "\n")] = '\0';
    printf ("Enter the Pi4 server port: ");
    fflush(stdout);
    scanf ("%d", &Pi_Port);
    getchar();

    ser_fd = socket (AF_INET, SOCK_DGRAM, 0);
    
    if (ser_fd < 0){
        PR_ERR ("socket");
    }
    
    ser_addr.sin_port = htons (Pi_Port);
    ser_addr.sin_family = AF_INET;
    inet_pton(AF_INET, Pi_IP, &ser_addr.sin_addr.s_addr);

    while(1){
        printf ("Token to connect Pi4  server: ");
        fflush(stdout);
        fgets(token, sizeof(token), stdin);
        token[strcspn(token, "\n")] = '\0';
        if (sendto(ser_fd, token, 20, 0, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr_in)) < 0){
            PR_ERR ("sendto");
        }

        int n = recvfrom(ser_fd, temp_r, 3, 0, (struct sockaddr *)&ser_addr, &len);
        if (n <= 0) {
            PR_ERR("recvfrom");
            continue;
        }
        temp_r[n] = '\0';

        if (!strncmp(temp_r, "1", 1)){
            printf("***Connection successfully***\n");
            free (token);
            break;
        }
        else{
            printf("!!! Can not connect to Pi4 server!!!\n");
            printf("---> Enter Token Again ! \n");
        }
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    while (1){
        int line, col;
        printf ("Line : ");
        fflush(stdout);
        scanf("%d", &line);
        getchar();
        printf ("Col : ");
        fflush(stdout);
        scanf("%d", &col);
        getchar();
        int n= sprintf(message, "%d %d ", line, col);
        printf("Message Display: ");
        fflush(stdout);
        fgets(message +n , 50, stdin);
        message[strcspn(message, "\n")] = '\0';
        if (sendto(ser_fd, message, strlen(message), 0, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr_in)) < 0){
            PR_ERR ("sendto");
        }
    }
    return 0;

}

