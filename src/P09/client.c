/*
---------------------------------------------------------------------------
 Program:   client

 Zweck:     Erstellt eine Verbindung zu einem Server und zeigt die 
            vom Server gesendeten Daten an.
 
 Syntax:    client [ server [port] ]
 
               server - IP-Adresse des Servers
               port   - Protokoll-Port-Nummer des Servers
 
 Anmerkung: Beide Parameter sind fakultativ.  
            - Falls "server" fehlt, wird "LocalHost" angenommen
            - Falls "port" fehlt, wird der Default-Wert
              "DefaultPortNumber" verwendet.

 --------------------------------------------------------------------------
 */

/* Betriebsystem-spezifisch Header-Files einbinden */
#ifndef unix
# include <windows.h>
# include <winsock.h>
#define  close closesocket
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
const int  DefaultPortNumber = 4711;    /* Default-Protokoll-Port        */
const char LocalHost[] = "localhost";   /* Default-Server-Name           */
const int	 Buffersize = 1000;

/* Macro um eine beliebige Datenstruktur (mittels Nullen) zu l�schen     */
#define ClearMemory(s) memset((char*)&(s), 0, sizeof(s))

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
   const  SockAddrPtr ServerAddrPtr=(SockAddrPtr)&ServerAddr;

   const char*     ServerName;          /* Temp.Pointer auf Server-Namen */
   struct hostent* HostEntryPtr;        /* R�ckgabewert von gethostbyname: 
                                           Zeiger auf statische Struktur */
   unsigned long   BinaryAddr;          /* Bin�re Server-Adresse         */
   int             CommunicationSocket; /* Socket(-Descriptor)           */
   int             UntestedPort;        /* Port-Nummer (wie eingegeben)  */
   unsigned short  Port;                /* Protokoll-Port-Nummer         */
   char            Buffer[Buffersize];  /* Daten-Buffer                  */
   int             Status;              /* Status-Zwischenspeicher       */
   int             CharsReceived;       /* Anzahl empfangener Zeichen    */

   /* Windows-spezifische Initialisierung */
#ifndef unix
   WSADATA wsaData;
   Status = WSAStartup(0x0101, &wsaData);
   ExitOnError(Status, "Winsock-Startup fehlgeschlagen\n");
#endif

   /*
      1. Parameter der Kommandozeile verarbeiten:

   Falls eine Server-Adresse angegeben wurde, soll diese verwendet werden,
   sonst der Default-Wert (Konstante "LocalHost").
   */

   if (ArgumentCount > 1) {           /* Falls Server-Argument angegeben */
      ServerName = ArgumentValue[1];
   } else {
      ServerName = LocalHost;
   }

   /*
   Es wird versucht die numerische oder symbolische angegebene Server-
   Adresse in eine bin�re Adresse umzuwandeln. Falls dies nicht gelingt,
   wird das Programm mit einer Fehlermeldung beendet, sonst wird der
   Wert in der Variablen "BinaryAddr" abgelegt.
   Anmerkung: Unter Unix gen�gt gethostbyname (inet_addr ist unn�tig)    */
   
   /* 1.Versuch: Numerische IP-Adresse?
      Anmerkung: Unter Unix gen�gt gethostbyname (inet_addr ist unn�tig) */
   BinaryAddr = inet_addr(ServerName);
   if (BinaryAddr == INADDR_NONE) {

   /* 2.Versuch: Symbolische IP-Adresse?                                 */
      HostEntryPtr = gethostbyname(ServerName);
      if (HostEntryPtr != NULL) {
         memcpy(&BinaryAddr, HostEntryPtr->h_addr, sizeof(BinaryAddr));
      }
      else {
         fprintf(stderr,"Ungueltiger Server-Adresse: %s\n", ServerName);
         exit(1);
      }
   }

   /*
   2. Parameter der Kommandozeile verarbeiten:

   Falls eine Port-Nummer angegeben wurde, soll diese f�r das Protokoll
   verwendet werden, sonst der Default-Wert (Konstante DefaultPortNumber).
   Falls der Wert ung�ltig ist, wird das Programm mit einer Fehlermeldung
   beendet, sonst wird der Wert in der Variablen Port abgelegt.
   */

   if (ArgumentCount > 2) {            /* Falls Port-Parameter angegeben */
      UntestedPort = atoi(ArgumentValue[2]);     /* String bin�r wandeln */
   } else {
      UntestedPort = DefaultPortNumber;             /* Default verwenden */
   }

   if (UntestedPort>0 && UntestedPort<65536) {  /* Port-Nummer g�ltig ?  */
      Port = (unsigned short)UntestedPort;   /* Typ anpassen, �bernehmen */
   }
   else {                                      /* Fehlermeldung und Ende */
      fprintf(stderr, "Ung�ltige Port-Nummer %d\n", UntestedPort);
      exit(1);
   }

   /* Socket f�r Verbindungsaufbau und Datentransfer erzeugen            */
   CommunicationSocket = socket(PF_INET, SOCK_STREAM, 0);
   ExitOnError(CommunicationSocket, "socket fehlgeschlagen\n");

   /* Server-Adresse und Port (Protokoll) definieren                     */
   ClearMemory(ServerAddr);                 /* Alles mit Nullen loeschen */
   ServerAddr.sin_family = AF_INET;         /* Address Family InterNET   */
   ServerAddr.sin_addr.s_addr = BinaryAddr; /* Binary Server-Adresse     */
   ServerAddr.sin_port = htons(Port);       /* Port, ggf. Bytes tauschen */

   /* Verbindung zum Server und Dienst erstellen                         */
   Status=connect(CommunicationSocket, ServerAddrPtr, sizeof(ServerAddr));
   ExitOnError(Status, "connect fehlgeschlagen\n");

   wait("Text");
   /* Wiederholt Daten vom Server lesen und am Bildschirm anzeigen       */
   CharsReceived = recv(CommunicationSocket, Buffer, sizeof(Buffer), 0);
   while (CharsReceived > 0) {
      Buffer[CharsReceived] = 0;
      fprintf(stdout,"%s", Buffer); fflush(stdout); 
      CharsReceived = recv(CommunicationSocket, Buffer, sizeof(Buffer), 0);
   }

   /* Close the socket                                                   */
   close(CommunicationSocket);
   ExitOnError(Status, "close fehlgeschlagen\n");

   /* Programm mit positivem Status beenden */
   exit(0);
}

