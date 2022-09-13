
OBJECTS = db_access.o sqlite3.o main.o
CFLAGS = -Wall
CC = gcc





run: Bank
	@ rm Bank.db
	@ ./Bank

Bank: $(OBJECTS)
	@ $(CC) $^ -o $@

sqlite3.o main.o : sqlite3.h
db_access.o main.o : db_access.h 


.phony: clean
clean:
	@ rm *.o Bank

