#include "`sys/time.h"

typedef struct hash_struct
{
  uint32_t hash;
  char name[50];
  uint32_t salary;
  struct hash_struct *next;
} hashRecord;

long long current_timestamp() {  
  struct timeval te;  
  gettimeofday(&te, NULL); // get current time  
  long long microseconds = (te.tv_sec * 1000000) + te.tv_usec; // calculate milliseconds  
  return microseconds;  
} 

