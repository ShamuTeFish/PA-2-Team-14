#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>

typedef struct hash_struct
{
  uint32_t hash;
  char name[50];
  uint32_t salary;
  struct hash_struct *next;
} hashRecord;

long long current_timestamp()
{
  struct timeval te;
  gettimeofday(&te, NULL);                                     // get current time
  long long microseconds = (te.tv_sec * 1000000) + te.tv_usec; // calculate milliseconds
  return microseconds;
}

uint32_t jenkins_one_at_a_time_hash(const uint8_t *key, size_t length)
{
  size_t i = 0;
  uint32_t hash = 0;
  while (i != length)
  {
    hash += key[i++];
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash;
}

int main()
{
  char data;
  int sentInstruct = 0;
  int sentName = 0;
  int sentSalary = 0;
  int sentOrder = 0;
  char TempInstruct[30];
  char TempName[50];
  char TempSal[30];
  char TempOrder[20];
  int Order;
  int sal;
  int i = 0;
  FILE *fileR = fopen("commands.txt", "r");

  while ((data = fgetc(fileR)) != EOF)
  {
    if (sentInstruct != 1)
    {

      if (data == ',')
      {
        TempInstruct[i] = '\0';
        sentInstruct = 1;
        i = 0;
        printf("%s", TempInstruct);
        if (strcmp(TempInstruct, "threads") == 0)
        {
          while ((data = fgetc(fileR)) != EOF && data != '\n')
          {
            // Throw away threads line until I know what to do with it.
            sentInstruct = 0;
          }
        }
      }
      else
      {
        TempInstruct[i] = data;
        i += 1;
      }
    }
    else if (sentName != 1)
    {

      if (data == ',')
      {
        TempName[i] = '\0';
        sentName = 1;
        i = 0;
        printf("%s", TempName);
      }
      else
      {
        TempName[i] = data;
        i += 1;
      }
    }
    else if (sentSalary != 1)
    {

      if (data == ',')
      {

        TempSal[i] = '\0';
        sentSalary = 1;
        sal = atoi(TempSal);

        i = 0;
        printf("%d", sal);
      }
      else
      {
        TempSal[i] = data;
        i += 1;
      }
    }
    else if (sentOrder != 1)
    {

      if (data == '\n')
      {

        TempOrder[i] = '\0';
        sentOrder = 1;
        Order = atoi(TempOrder);

        i = 0;
        printf(",%d", Order);

        sentInstruct = 0;
        sentName = 0;
        sentSalary = 0;
        sentOrder = 0;
      }
      else
      {
        TempOrder[i] = data;
        i += 1;
      }
    }
  }

  fclose(fileR);
  return 0;
}
