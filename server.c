#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

/*struct sockaddr_in { //benne van a netinet/in-ben, nem kell leirni
        short   sin_family;
        u_short sin_port;
        struct  in_addr sin_addr; //in_addr struktra
        char    sin_zero[8];
};*/

/*struct sockaddr { //benne van a netinet/in-ben
	unsigned short sa_family; //ez mindig AF_INET lesz
	char sa_data[14];
};*/

typedef int bool;
#define true 1
#define false 0

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

char kapcsolva1[]= {"Kapcsolodva, varj amig kapcsolodik a masodik jatekos!\n"};
char kapcsolva2[]= {"Kapcsolodva, mindketten kapcsolodtatok!\n"};
char megerosites[]= {"Mindketten kapcsolodtatok!\n"};
char tejossz[]= {"TEJOSSZ"};
char add[]= {"ADD"};
char del[]= {"DEL"};
char win[]= {"WIN"};
char dc[]= {"DC"};

char buffer[1024]; //buffer amibe olvasni fogok a kliensrol
char bemenet[1024];
char on = 1;

int array[8][8],harray[8][8];
int a,b,c,todo,temp = 0;
int db = 2;

bool rajzolj = true;
bool vege;
bool lecsat = false;

int server_socket, client[2], port; 		//server_socket mondja meg hogy milyen socket lett nyitva a socket() hivasaval, port
unsigned int client1_size, client2_size; //klienc cimenek merete, lehet sima int is csak akkor a forditonak nem tetszik
struct sockaddr_in server_address, client1_address, client2_address; 	//letezo struktura hivasa a netinet/in-bol, ezekbe rakjuk el a cimek informacioit

void whatToDo(int todo);

void fillMatriceWithZeros() {

    printf("fillzero inside");

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            array[i][j] = 0;
        }
    }
    printf("filled zero inside");
}

void fillMatriceWithNumbers() {

    printf("fill inside");
    fillMatriceWithZeros();
    printf("filled inside");

    srand((unsigned)time(NULL));
    int r,akt = 0;
    bool voltemar;

    for (int i = 0; i < 8; i++) {
        for(int h = 0; h < 7; h++) {
            r = rand()%7; //sorban hely
            for (int j = 0; j < 8; j++) {
                if(j==r) {
                    voltemar = true;
                    while (voltemar) {
                        voltemar = false;
                        akt = rand() % 8+1;
                        for(int k=0; k < 8; k++) {
                            if (akt == array[i][k]) {
                                voltemar = true;
                            }
                        }
                        for (int l = 0; l < 8; l++) {
                            if (akt == array[l][j]) {
                                voltemar = true;
                            }
                        }

                    }
                    array[i][j] = akt;
                }
            }
        }
    }
    memcpy(harray, array, sizeof(array));
}

void drawGrid() {

    printf("\n");
    printf("          ");
    for(int i = 0; i < 8; i++) {
        printf(" %d. ", i+1);
    }

    printf("\n\n");
    printf("\t====================================");
    printf("\n");

    for(int i = 0; i < 8; i++) {
        printf("%d. \t||", i+1);
        for(int j = 0; j < 8; j++) {
            if ((j%4) == 3) {

                if(array[i][j] == 0) {
                    printf(ANSI_COLOR_GREEN " %d" ANSI_COLOR_RESET, array[i][j]);
                } else if(array[i][j] == harray[i][j]) {
                    printf(ANSI_COLOR_RED " %d" ANSI_COLOR_RESET, array[i][j]);
                } else {
                    printf(ANSI_COLOR_CYAN " %d" ANSI_COLOR_RESET, array[i][j]);
                }

                printf(" ||");

            } else {

                if(array[i][j] == 0) {
                    printf(ANSI_COLOR_GREEN " %d" ANSI_COLOR_RESET, array[i][j]);
                } else if(array[i][j] == harray[i][j]) {
                    printf(ANSI_COLOR_RED " %d" ANSI_COLOR_RESET, array[i][j]);
                } else {
                    printf(ANSI_COLOR_CYAN " %d" ANSI_COLOR_RESET, array[i][j]);
                }

                printf(" |");

            }
        }

        printf("\n");

        if ((i%2) == 1) {
            printf("\t====================================");
        } else {
            printf("\t------------------------------------");
        }

        printf("\n");
    }
    printf("\n");

}

void addNumber(int i) {

    printf("Pozíció és érték kérdezése a klienstől.\n");

    send(client[i], add, sizeof(add), 0);

    bzero(buffer, 1024);
    recv(client[i], buffer, sizeof(buffer)+1, 0);
    a = strtol(buffer, 0, 10);

    printf("A pozíció sora: %d\n", a);

    bzero(buffer, 1024);
    recv(client[i], buffer, sizeof(buffer)+1, 0);
    b = strtol(buffer, 0, 10);

    printf("A pozíció oszlopa: %d\n", b);

    bzero(buffer, 1024);
    recv(client[i], buffer, sizeof(buffer)+1, 0);
    c = strtol(buffer, 0, 10);

    printf("Az érték: %d\n", c);

    if(array[a-1][b-1] == 0) {

        array[a-1][b-1] = c;
        rajzolj = true;

    } else {

        printf("Itt már volt érték.\n");
        drawGrid();
        addNumber(i);
    }
}

void deleteNumber(int i) {

    printf("Pozíció kérdezése a klienstől.\n");

    send(client[i], del, sizeof(del), 0);

    bzero(buffer, 1024);
    recv(client[i], buffer, sizeof(buffer)+1, 0);
    a = strtol(buffer, 0, 10);

    printf("A pozíció sora: %d\n", a);

    bzero(buffer, 1024);
    recv(client[i], buffer, sizeof(buffer)+1, 0);
    b = strtol(buffer, 0, 10);

    printf("A pozíció oszlopa: %d\n", b);

    if(array[a-1][b-1] == 0) {

        printf("Még nem volt érték beírva a mezőre.\n");
        drawGrid();
        deleteNumber(i);
    }
    if(array[a-1][b-1] == harray[a-1][b-1]) {

        printf("Nem módosíthatsz generált értéket.\n");
        drawGrid();
        deleteNumber(i);
    }
    array[a-1][b-1] = 0;
    rajzolj = true;

}

void passing(int i) {

    printf("\e[1;1H\e[2J"); //clear
    printf("%d. játékos passzolt.\n",i+1);
    rajzolj = true;
}

bool vegeVan() {

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if(array[i][j] == 0) {
                return false;
            }
        }
    }
    return true;
}


int main(int argc, char* argv[]) {


    port = atoi(argv[1]);
    
    while(port<1024 || port>49151) {
        printf("Ervenytelen portszam! Adjon meg masik portot: ");
        scanf("%d", &port);
    }

    //AF_INET == Internet domain with 2 hosts SOCK_STREAM == TCP
    server_socket=socket(AF_INET, SOCK_STREAM, 0); //visszateresi ertek alapjan lehet hibas a socket nyitasa

    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof on);
    setsockopt(server_socket, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof on);


    if (server_socket<0) {
        printf("Nem sikerult socket-et nyitni!\n");
        return 0;
    }

    bzero((char *) &server_address, sizeof(server_address)); //mindent lenullazok benne, es csak aztan adok erteket neki
    server_address.sin_family=AF_INET; 	// szervercim struktura sin_family-je mindig AF_INET konstansra kell allitani
    server_address.sin_port = htons(port);		//megadom a portot, htons-al konvertalom a szamot network byte order-be (host to network short)
    server_address.sin_addr.s_addr = INADDR_ANY;	//szerver IP cime, INADDR_ANY-vel a gép IP címét adod meg (127.0.0.1)


    //A portszámot a kernel használja, hogy összekapcsoljon egy bejövõ csomagot egy meghatározott process socket leírójával.
    //Ha a bind() visszateresi erteke -1, akkor gond van.

    //szerver cimere sockaddr struktura pointert var, de nekunk sockaddr_in van, ezert castolni kell, harmadik parameter a cim merete
    if(bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address))<0) {
        printf("Nem sikerult bind-elni!\n");
        return 0;
    }

    listen(server_socket, 10);
    //A backlog azoknak a csatlakozásoknak a száma, amelyek a bejövõ sorban meg vannak engedve.
    //varakozok a kapcsolatra, az otos az a kapcsolatok szama amik varakoznak amig a program ezzel a kapcsolattal foglalkozik (azt mondtak a neten legyen 5 v 10, idk)

    client1_size=sizeof(client1_address);
    client2_size=sizeof(client2_address);

    //belerakja a kliens cimet es felveszi a kapcsolatot

    //client[1]-be eltarolom, hogy milyen uj socketet nyitott a kliensnek (minden kliensnek ujat kell nyitni)
    client[0] = accept(server_socket, (struct sockaddr *) &client1_address, &client1_size);

    if(client[0] < 0) {
        printf("Nem sikerult a kapcsolatot felvenni!\n");
        return 0;
    } else {
        printf("Elso jatekos kapcsolodva!\n");
        send(client[0], kapcsolva1, sizeof(kapcsolva1), 0);
    }

    client[1] = accept(server_socket, (struct sockaddr *) &client2_address, &client2_size);

    if(client[1] < 0) {
        printf("Nem sikerult a kapcsolatot felvenni!\n");
        return 0;
    } else {
        printf("Masodik jatekos kapcsolodva!\n");
        send(client[1], kapcsolva2, sizeof(kapcsolva2), 0);
    }

    sleep(1);
    send(client[0], megerosites, sizeof(megerosites), 0);
    sleep(1);
    send(client[1], megerosites, sizeof(megerosites), 0);
    sleep(1);

    printf("Mátrix generálása...\n");
    fillMatriceWithNumbers();
    printf("Mátrix generálása kész...\n");

    while (1) {

        for(int i=0; i<2; i++) {

            vege = vegeVan();
            if(vege) {
                printf("\e[1;1H\e[2J"); //clear
                printf("JUHÉ!! Ki van töltve a tábla!\n");
                send(client[0], win, sizeof(win), 0);
                sleep(1);
                send(client[1], win, sizeof(win), 0);
                sleep(1);
                exit(0);
            }

            if(client[0] < 0) {
                sleep(1);
                printf("%d. játékos lecsatlakoztatva",i+1);
                send(client[0], dc, sizeof(dc), 0);
            }

            if(client[1] < 0) {
                sleep(1);
                printf("%d. játékos lecsatlakoztatva",i+1);
                send(client[1], dc, sizeof(dc), 0);
            }

            if(client[0] < 0 && client[1] < 0) {
                close(server_socket);
                printf("Shutdown");
                exit(-1);
            }


            if(rajzolj) {

                send(client[i], tejossz, sizeof(tejossz), 0);

                drawGrid();
                printf("Várakozás a kliensre.\n");
                rajzolj = false;

                bzero(buffer, 1024);
                recv(client[i], buffer, sizeof(buffer)+1, 0);
                todo = strtol(buffer, 0, 10);
                switch (todo) {
                case 1:
                    addNumber(i);
                    break;
                case 2:
                    deleteNumber(i);
                    break;
                case 3:
                    passing(i);
                    break;
                case 9:
                    sleep(1);
                    printf("%d. játékos lecsatlakozott, vége a játéknak.\n",i+1);
                    send(client[0], dc, sizeof(dc), 0);
                    sleep(1);
                    send(client[1], dc, sizeof(dc), 0);
                    lecsat = true;
                    break;

                }
            }
        }
        if(lecsat) {
            break;
        }
    }

    close (client[0]);
    close (client[1]);
    close(server_socket);

    return 0;
}
