/*
---------------------------------------------------------------------------
 Program:   client_www

 Zweck:     Erstellt eine Verbindung zu einem Server, sendet einen HTTP-
            Request-Header und zeigt die Server gesendeten Daten an.

 Syntax:    client_www [ server [port] ]

               server - IP-Adresse des Servers
               port   - Protokoll-Port-Nummer des Servers

 Anmerkung: Beide Parameter sind fakultativ.
            - Falls "server" fehlt, wird "LocalHost" angenommen
            - Falls "port" fehlt, wird der Default-Wert
              "DefaultPortNumber" verwendet.

 --------------------------------------------------------------------------
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Konstanten definieren
   const char DefaultPortNumber[] = "4711";    // Default-Protokoll-Port
   const char LocalHost[] = "localhost";       // Default-Server-Name
   const char REQUEST[] = "GET / HTTP/1.0\r\nAccept: */*\r\nUser-Agent: client_www\r\n\r\n";

// Macro um eine beliebige Datenstruktur (mittels Nullen) zu löschen
#define ClearMemory(s) memset((char*)&(s), 0, sizeof(s))

// Prozedur zur Fehlerabfrage und Behandlung
void ExitOnError(int Status, char* Text) {
   if (Status < 0) {
      fprintf(stderr, "%s: %s\n", Text, gai_strerror(Status));
      exit(1);
   }
}

int main(int ArgumentCount, char* ArgumentValue[]) {

   const char*      ServerName;          // Temp.Pointer auf Servernamen
   struct addrinfo  Hints;
   struct addrinfo* SrvInfo;             // Resultat von getaddrinfo
   int              CommunicationSocket; // Socket(-Descriptor)
   const char*      PortNumber;
   char             Buffer[1000];        // Daten-Buffer
   int              Status;              // Status-Zwischenspeicher
   int              CharsReceived;       // Anzahl empfangener Zeichen


   /*
      1. Parameter der Kommandozeile verarbeiten:

   Falls eine Server-Adresse angegeben wurde, soll diese verwendet werden,
   sonst der Default-Wert (Konstante "LocalHost").
   */

   if (ArgumentCount > 1) {           // Falls Server-Argument angegeben
      ServerName = ArgumentValue[1];
   } else {
      ServerName = LocalHost;
   }


   /*
   2. Parameter der Kommandozeile verarbeiten:

   Falls eine Port-Nummer angegeben wurde, soll diese für das Protokoll
   verwendet werden, sonst der Default-Wert (Konstante DefaultPortNumber).
   */

   if (ArgumentCount > 2) {            // Falls Port-Parameter angegeben
      PortNumber = ArgumentValue[2];
   } else {
      PortNumber = DefaultPortNumber;               // Default verwenden
   }

   // Adressinformationen bereitstellen
   ClearMemory(Hints);
   Hints.ai_family = AF_INET;       // Nur IPv4
   Hints.ai_socktype = SOCK_STREAM; // TCP Stream Sockets
   Status = getaddrinfo(ServerName, PortNumber, &Hints, &SrvInfo);
   ExitOnError(Status, "getaddrinfo fehlgeschlagen");

   // Socket für Verbindungsaufbau und Datentransfer erzeugen
   CommunicationSocket = socket(PF_INET, SOCK_STREAM, 0);
   ExitOnError(CommunicationSocket, "socket fehlgeschlagen");

   // Verbindung zum Server und Dienst erstellen
   Status=connect(CommunicationSocket, SrvInfo->ai_addr, SrvInfo->ai_addrlen);
   ExitOnError(Status, "connect fehlgeschlagen");

   freeaddrinfo(SrvInfo);                 // Wird nicht mehr gebraucht

   /**********************************************************************/
   // Request-Header senden - Bitte ergänzen!
   /**********************************************************************/
   ExitOnError(Status, "send fehlgeschlagen");


   // Wiederholt Daten vom Server lesen und am Bildschirm anzeigen
   CharsReceived = recv(CommunicationSocket, Buffer, sizeof(Buffer), 0);
   while (CharsReceived > 0) {
      Buffer[CharsReceived] = 0;
      fprintf(stdout,"%s", Buffer);
      CharsReceived = recv(CommunicationSocket, Buffer, sizeof(Buffer), 0);
   }

   // Close the socket
   close(CommunicationSocket);
   ExitOnError(Status, "close fehlgeschlagen");

   // Programm mit positivem Status beenden
   exit(0);
}

