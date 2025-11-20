// logging.h
#ifndef LOGGING_H
#define LOGGING_H

long long current_timestamp(void);

void log_init(const char *filename);
void log_close(void);

// For lines like:
//  <ts>: THREAD <priority>WAITING FOR MY TURN
void log_simple(int priority, const char *text);

// For command lines like:
//  <ts>: THREAD <priority>,INSERT,...
void log_command(int priority, const char *fmt, ...);

#endif
