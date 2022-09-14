OBJECTS = db_access.o sqlite3.o view_controller.o
CFLAGS = -Wall -g
CC = gcc


all: Bank
	@ rm Bank.db
	@ ./Bank

Bank: $(OBJECTS)
	@ $(CC) $^ -o $@

db_access.o view_controller.o : db_access.h 


.phony: clean
clean:
	@ rm *.o Bank

