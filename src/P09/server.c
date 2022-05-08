/*
---------------------------------------------------------------------------

 Program:   server

 Zweck:     Erzeugt einen Socket und f�hrt wiederholt aus:
              1) Warte auf eine Verbindung von einem Client
              2) Sende eine Meldung an den Client
              3) Beende die Verbindung

 Syntax:    server [ port ]
                port  - Protokoll-Port

 Anmerkung: Die Angabe von "port" ist fakultativ. Falls nicht angegeben,
            wird der Default-Wert "DefaultPortNumber" verwendet.

---------------------------------------------------------------------------
*/

/* Betriebsystem-spezifisch Header-Files einbinden                       */
#ifndef unix
# include <winsock.h> 
# define  close closesocket 
#else
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Konstanten definieren                                                 */
const int DefaultPortNumber = 4711;      /* Default-Protokoll-Port       */
const int QueueLength = 10;              /* Laenge der Request Queue     */
const int BufferSize = 1000;

/* Macro um eine beliebige Datenstruktur (mittels Nullen) zu l�schen     */
#define ClearMemory(s) memset((char*)&(s),0,sizeof(s))

/* Prozedur zur Fehlerabfrage und Behandlung                             */
void ExitOnError(int Status, char* Text) {
        if (Status < 0) {
                fprintf(stderr,"%s\n", Text);
                exit(1);
        }
}

/* Definition: Wartet auf Enter */
void wait(char* prompt){
   fflush(stdin);
   printf("\nPress [Enter] to proceed with %s ", prompt); 
   getchar();
}

int main(int ArgumentCount, char* ArgumentValue[]) {

   typedef struct sockaddr* SockAddrPtr; /* Pointer auf sockaddr         */

   /* Struktur f�r Server-Adresse und Pointer f�r Parameter-�bergabe     */
   struct sockaddr_in ServerAddr; 
   const  SockAddrPtr ServerAddrPtr = (SockAddrPtr)&ServerAddr;

   /* Struktur f�r Client-Adresse und Pointer f�r Parameter-�bergabe     */
   struct sockaddr_in ClientAddr;
   const  SockAddrPtr ClientAddrPtr = (SockAddrPtr)&ClientAddr;
   
   int             ListeningSocket;    /* Socket f�r Verbindungsaufbau   */
   int             ConnectedSocket;    /* Socket f�r Daten�bertragung    */
   int             UntestedPort;       /* Port Nummer (wie eingegeben)   */
   unsigned short  Port;               /* Protokoll-Port-Nummer          */
   unsigned        AddrLen;            /* Laenge der Adresse             */
   char            Buffer[BufferSize]; /* Daten-Buffer                   */
   int             Status;             /* Status-Zwischenspeicher        */
   int             Visits;             /* bisherige Anzahl Verbindungen  */

/* Windows-spezifische Initialisierung                                   */
#ifndef unix
   WSADATA wsaData;
   Status = WSAStartup(0x0101, &wsaData);
   ExitOnError(Status, "Winsock-Startup fehlgeschlagen\n");
#endif

  /*
   Kommandozeile verarbeiten:

   Falls eine Port-Nummer angegeben wurde, soll diese f�r das Protokoll
   verwendet werden, sonst der Default-Wert (Konstante DefaultPortNumber).
   Falls der Wert ung�ltig ist, wird das Programm mit einer Fehlermeldung
   beendet, sonst wird der Wert in der Variablen Port abgelegt.
   */

   if (ArgumentCount > 1) {            /* Falls Port-Parameter angegeben */
      UntestedPort = atoi(ArgumentValue[1]);     /* String bin�r wandeln */
   } else {
      UntestedPort = DefaultPortNumber;  /* sonst Default-Port verwenden */
   }

   if (UntestedPort>0 && UntestedPort<65536) { /* gueltige Port-Nummer?  */
      Port = (unsigned short) UntestedPort;  /* Typ anpassen, �bernehmen */
   }
   else {                          /* Fehlermeldung ausgeben und beenden */
      fprintf(stderr, "Ung�ltige Port-Nummer %d\n", UntestedPort);
      exit(1);
   }

   /* Socket f�r Verbindungsaufbau erzeugen                              */
   ListeningSocket = socket(PF_INET, SOCK_STREAM, 0);
   ExitOnError(ListeningSocket, "socket fehlgeschlagen\n");
 
   /* Server Adresse und Port f�r den Dienst (Protokoll) definieren      */
   ClearMemory(ServerAddr);                 /* Alles mit Nullen loeschen */
   ServerAddr.sin_family = AF_INET;         /* Address Family InterNET   */
   ServerAddr.sin_addr.s_addr = INADDR_ANY; /* Beliebiges Interface      */
   ServerAddr.sin_port = htons(Port);       /* Port, ggf. Bytes tauschen */

   /* Dem Socket die lokale Adresse und Port-Nummer zuordnen             */
   Status = bind(ListeningSocket, ServerAddrPtr, sizeof(ServerAddr));
   ExitOnError(Status, "bind fehlgeschlagen\n");

   /* Socket in passiv Modus versetzen und Warteschlagengr�sse festlegen */   
   Status = listen(ListeningSocket, QueueLength);
   ExitOnError(Status, "listen fehlgeschlagen\n");

   Visits = 0; /* Noch keine Verbindungen */
   printf("Server wartet an Port %d auf die erste Verbindung\n", Port);

   while (1) { /* Server Loop */

      AddrLen = sizeof(ClientAddr);     /* ... wird von accept ver�ndert */

   /* Auf Client-Verbindung (connect) warten                             */
      ConnectedSocket = accept(ListeningSocket, ClientAddrPtr, &AddrLen);
      ExitOnError(ConnectedSocket, "accept fehlgeschlagen\n");

      Visits++;
      printf("%d. Verbindung von %s, Port %d\n",
             Visits, inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port));

      wait("Text");
   /* Daten-Buffer aufbereiten                                           */
      sprintf(Buffer,"Dies ist die %d. Verbindung.\n",Visits);

   /* Daten-Buffer senden                                                */
      Status = send(ConnectedSocket, Buffer, strlen(Buffer), 0);
      ExitOnError(Status, "send fehlgeschlagen\n");

   /* Client-Verbindung beenden                                          */
      Status = close(ConnectedSocket);
      ExitOnError(Status, "close fehlgeschlagen\n");

   } /* Server Loop */
}
