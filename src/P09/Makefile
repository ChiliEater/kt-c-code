CMP=        gcc
CMPFLAGS=   -Wall -g
LDFLAGS=
EXENAME1=   client.e
FNAM1=      client.o
EXENAME2=   client_duplex.e
FNAM2=      client_duplex.o 
EXENAME3=   server.e 
FNAM3=      server.o
EXENAME4=   server_duplex.e 
FNAM4=      server_duplex.o
EXENAME5=   server_simplex.e 
FNAM5=      server_simplex.o
LIBNAME=

compile:	$(EXENAME1) $(EXENAME2) $(EXENAME3) $(EXENAME4) $(EXENAME5)

$(EXENAME1):	$(FNAM1)
	$(CMP)  $(CMPFLAGS) $(FNAM1) $(LIBNAME) $(LDFLAGS) -o $@

$(EXENAME2):	$(FNAM2)
	$(CMP)  $(CMPFLAGS) $(FNAM2) $(LIBNAME) $(LDFLAGS) -o $@

$(EXENAME3):	$(FNAM3)
	$(CMP)  $(CMPFLAGS) $(FNAM3) $(LIBNAME) $(LDFLAGS) -o $@

$(EXENAME4):	$(FNAM4)
	$(CMP)  $(CMPFLAGS) $(FNAM4) $(LIBNAME) $(LDFLAGS) -o $@

$(EXENAME5):	$(FNAM5)
	$(CMP)  $(CMPFLAGS) $(FNAM5) $(LIBNAME) $(LDFLAGS) -o $@

.c.o:
	$(CMP) -c $(CMPFLAGS) $<

.cc.o:
	$(CMP) -c $(CMPFLAGS) $<

clean:
	rm -f *.o $(EXENAME1) $(EXENAME2) $(EXENAME3) $(EXENAME4) $(EXENAME5)

all:
	@make clean
	@make
