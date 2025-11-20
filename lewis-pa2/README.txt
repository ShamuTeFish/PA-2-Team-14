PA2 – Concurrent Hash Table
Lewis Midlane
COP 4600

-----------------------------------
BUILD INSTRUCTIONS
-----------------------------------

This project uses a Makefile to compile all source files.

To build:
    make

This will produce an executable named:
    chash

To clean all compiled files:
    make clean


-----------------------------------
RUN INSTRUCTIONS
-----------------------------------

Place your commands file in the same directory and name it:
    commands.txt



Then run the program:
    1: make
    2: ./chash

The program will:
  - Execute each command using a separate thread
  - Enforce priority scheduling
  - Use a custom reader-writer lock for concurrency
  - Print results to stdout
  - Write detailed lock and thread events to:
        hash.log


-----------------------------------
FILE DESCRIPTIONS
-----------------------------------

chash.c
    Main program. Handles command parsing, thread creation,
    priority scheduling, and calls the hash table operations.

hash.c / hash.h
    Implements the concurrent hash table, Jenkins hash function,
    and insert/delete/update/search/print functionality.

rwlock.c / rwlock.h
    Implements the custom reader-writer lock using pthreads.
    Logs all lock operations.

logging.c / logging.h
    Handles timestamped logging to hash.log.

Makefile
    Builds the project using gcc and pthreads support.



