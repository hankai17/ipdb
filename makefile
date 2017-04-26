.PHONY:clean all
WORKDIR = .
VPATH = ./src
CC=g++
CFLGS= -Wall -g -I./inc/

BIN = spider #qip
all:$(BIN)

spider:spider_ip.o tb.o ip138.o ipcn.o ipip.o log.o main.o  
	$(CC) -lcurl -lpthread -lboost_regex -L/usr/local/lib $^ -o $@ 

#qip:qip.o  
#	$(CC) -lcurl -lboost_regex -L/usr/local/lib $^ -o $@

%.o:%.cc
	$(CC) $(CFLGS) -c $< -o $@ 

clean:
	rm -f *.o $(BIN)

