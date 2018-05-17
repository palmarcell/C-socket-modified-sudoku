#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <arpa/inet.h>

char buffer[1024+1];
char bemenet[1024];
char kimenet[1024];
int a;

int client_socket, port;
struct sockaddr_in server, client;

void whatToDo() {

    printf("Mit szeretnél csinálni?\n");
    printf("1 - Kitölt\t2 - Törlés\t3 - Passz\t9 - Feladom\n");

    bzero(kimenet, 1024);
    scanf("%d", &a);

    if(!(0<a<4) && a != 9) {
        printf("Csak 1 és 3 között vagy a 9-et választhatod!\n");
        whatToDo();
    }

    sprintf(kimenet,"%d",a);

    //kimenet[0] = a + '0';
}

void addNumber_row() {

    printf("Írd be a pozíció sorát: ");
    bzero(kimenet, 1024);
    scanf("%d", &a);

    if(a<0 || a>9) {
        printf("Csak 1 és 8 közötti számot adhatsz meg pozíciónak.\n\n");
        addNumber_row();
    }

    sprintf(kimenet,"%d",a);
    //kimenet[0] = a+ '0';

}

void addNumber_col() {

    printf("Írd be a pozíció oszlopát: ");
    bzero(kimenet, 1024);
    scanf("%d", &a);

    if(a<0 || a>9) {
        printf("Csak 1 és 8 közötti számot adhatsz meg pozíciónak.\n\n");
        addNumber_col();
    }

    sprintf(kimenet,"%d",a);
    //kimenet[0] = a+ '0';

}

void addNumber_value() {

    printf("Írd be az érteket: ");
    bzero(kimenet, 1024);
    scanf("%d", &a);

    if(!(a>=0 && a<9)) {
        printf("Csak 1 és 8 közötti számot adhatsz meg értéknek.\n");
        addNumber_value();
    }

    sprintf(kimenet,"%d",a);
    //kimenet[0] = a+ '0';

}

void deleteNumber_row() {

    printf("Írd be a pozíció sorát: ");
    bzero(kimenet, 1024);
    scanf("%d", &a);

    if(a<0 || a>9) {
        printf("Csak 1 és 8 közötti számot adhatsz meg pozíciónak.\n\n");
        deleteNumber_row();
    }

    sprintf(kimenet,"%d",a);
    //kimenet[0] = a+ '0';
}

void deleteNumber_col() {

    printf("Írd be a pozíció oszlopát: ");
    bzero(kimenet, 1024);
    scanf("%d", &a);

    if(a<0 || a>9) {
        printf("Csak 1 és 8 közötti számot adhatsz meg pozíciónak.\n\n");
        deleteNumber_col();
    }

    sprintf(kimenet,"%d",a);
    //kimenet[0] = a+ '0';
}

int main(int argc, char *argv[]) {


    char on = 1;
    int err;
    int trnmsize,rcvsize;

    port = atoi(argv[1]);

    server.sin_family=AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;
    //majd ez kell
    //server.sin_addr.s_addr = inet_addr(argv[1]);


    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        printf("Nem sikerult socketet nyitni, inditsd ujra a programot!\n");
        return 0;
    }

    setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof on);
    setsockopt(client_socket, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof on);

    err = connect(client_socket, (struct sockaddr *) &server, sizeof(server));
    if (err < 0) {
        printf("%s: Cannot connect to the server.\n", argv[0]);
        exit(2);
    }

    //"elso jatekos kapcsolodva"
    //"masodik jatekos kapcsolodva"
    bzero(buffer, 1024);
    recv(client_socket, buffer, sizeof(buffer)+1, 0);
    printf("%s", buffer );

    sleep(1);

    //"megerosites" 1.
    //"megerosites" 2.
    bzero(buffer, 1024);
    recv(client_socket, buffer, sizeof(buffer)+1, 0);
    printf("%s", buffer );

    while(1) {

        sleep(2);

        bzero(buffer, 1024);
        recv(client_socket, buffer, sizeof(buffer)+1, 0);
        strcpy(bemenet,buffer);

        if(strcmp(bemenet,"TEJOSSZ") == 0) {
            whatToDo();

            trnmsize = send(client_socket, kimenet, sizeof(kimenet), 0);
            printf("Választás elküldve a szervernek.\n");

            if(trnmsize > -1) {
                if(strcmp(kimenet, "9") == 0) {
                    printf("Lecsatlakozási kérelem elküldve.\n");
                    sleep(1);
                    bzero(buffer, 1024);
                    rcvsize = recv(client_socket, buffer, sizeof(buffer)+1, 0);
                    if (strcmp(buffer,"DC") == 0) {

                        printf("Kliens lecsatlakozva!\n");
                        break;
                    }
                    if(rcvsize < 0) {
                        printf("Nem tudtam lecsatlakozni.\n");
                    }
                }

            }
        }

        if(strcmp(bemenet,"ADD") == 0) {

            addNumber_row();
            trnmsize = send(client_socket, kimenet, sizeof(kimenet), 0);
            addNumber_col();

            trnmsize = send(client_socket, kimenet, sizeof(kimenet), 0);
            addNumber_value();
            trnmsize = send(client_socket, kimenet, sizeof(kimenet), 0);

            if(trnmsize > -1) {
                printf("Pozíció és érték elküldve a szervernek.\n");
            }
        }
        if(strcmp(bemenet,"DEL") == 0) {

            deleteNumber_row();
            trnmsize = send(client_socket, kimenet, sizeof(kimenet), 0);
            deleteNumber_col();
            trnmsize = send(client_socket, kimenet, sizeof(kimenet), 0);

            if(trnmsize > -1) {
                printf("Pozíció elküldve a szervernek.\n");
            }
        }
        if(strcmp(bemenet,"WIN") == 0) {

            printf("Gratulálok! Volt türelmed kitölteni a táblát.\n");
            exit(0);
        }
        if(strcmp(bemenet,"DC") == 0) {
            printf("A másik játékos lecsatlakozott, vége a játéknak.\n");
            break;
        }

    }

    close(client_socket);

    return 0;
}
