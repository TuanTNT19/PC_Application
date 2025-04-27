#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>      
#include <netinet/in.h>     
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>

int ser_id, cli_id;
struct sockaddr_in ser_addr, cli_addr;
char *message;
char *client_ip;

void sig_handler()
{
    printf("========= QUICK TURN OFF ========\n");
    close (ser_id);
    close(cli_id);
    free (message);
    free(client_ip);
    exit(EXIT_SUCCESS);
}

int send_func(int fd, char *msg){
    return write(fd, msg, strlen(msg));
}

int main(int argc, char *argv[]){
    message = malloc(20);
    client_ip = malloc(20);

    printf ("=============== Start PC program =============\n");

// Signal Ctrl C register
    if (signal(SIGINT,sig_handler) == SIG_ERR)
    {
        printf("Can not handler SIGINT\n");
    }

    ser_id = socket(AF_INET , SOCK_STREAM, 0);

    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(atoi(argv[1]));
    ser_addr.sin_addr.s_addr = INADDR_ANY;

    bind(ser_id, (struct sockaddr *)&ser_addr, sizeof(ser_addr));

    listen(ser_id, 5);

    int len = sizeof(cli_addr);
    int client_portnum;

    while(1) {
        cli_id = accept(ser_id, (struct sockaddr *)&cli_addr, &len);
        inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, client_ip, 20);
        client_portnum = ntohs(cli_addr.sin_port);

        printf("=========== Connect from IP: %s, Port: %d ===========\n", client_ip, client_portnum);

        while(1){
            do {
                printf(".. Message to Pi4: ..\n");
                printf("....1. Led on\n");
                printf("....0. Led off\n");
                printf("....q. Quit Pi4 Program\n");
                printf (".. Want to exit PC program --> Enter 'E'\n");
                printf("Enter your chosen: ");
                fflush(stdout);
                fgets(message, 20, stdin);
                if (message[0] != '1' && message[0] != '0' && message[0] != 'q' && message[0] != 'E'){
                    printf(" === Invalid chosen !! Do again ===\n");
                }
            } while (message[0] != '1' && message[0] != '0' && message[0] != 'q' && message[0] != 'E');

            if (message[0] == 'E'){
                message[0] = 'q';
                send_func (cli_id, message);
                goto exit_program;
            }

            if (send_func (cli_id, message) < 0){
                printf("=== ERROR===: Can not send message to Pi4\n");
                sig_handler();
                return -1;
            }
        }
    }

    exit_program: 
        printf ("=============== Exit PC program =============\n");
        close (ser_id);
        close(cli_id);
        free (message);
        free(client_ip);
    
    return 0;
}