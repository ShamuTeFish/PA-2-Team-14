#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>

#include "rwlock.h"
#include "chash.h"



typedef struct hash_struct
{
  uint32_t hash;
  char name[50];
  uint32_t salary;
  struct hash_struct *next;
} hashRecord;

//LL global 
hashRecord * head = NULL;

//Log global
FILE *hlog;

//Thread counter
int thread_cnt = 0;
//aquire and release counter
int lockAcq = 0; 
int lockRel = 0;
//lock
rwlock_t lock;


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
//command functions
void insert(char *INname, uint32_t INsalary)
{
  hashRecord *current = head;
  hashRecord *traverse = head;
  uint32_t NewHas = jenkins_one_at_a_time_hash((const uint8_t*)INname, strlen(INname));
  
  

  //If head is null I.E start of list
  if(head == NULL){
  int time = current_timestamp();
  fprintf(hlog,"%u: THREAD %d INSERT,%u,%s,%u \n",time,thread_cnt,NewHas,INname,INsalary);
  
  fprintf(hlog,"%u: THREAD %d ",time,thread_cnt);
  rwlock_write_lock(&lock);

  hashRecord *dataNone = malloc(1 * sizeof(hashRecord));
  dataNone->hash = NewHas;
  strcpy(dataNone->name,INname);
  dataNone->salary = INsalary;
  dataNone->next = NULL;
  head = dataNone;
  
  printf("Inserted %u,%s,%d \n",NewHas,INname,INsalary);
  fprintf(hlog,"%u: THREAD %d ",time,thread_cnt);
  rwlock_write_unlock(&lock);
  thread_cnt += 1;
  return;
  }

  //Check for duplicates
  while (traverse != NULL)
  {
    if (strcmp(traverse->name, INname) == 0 || traverse->hash == NewHas)
    {
      printf("User Already Exsists");
      return;
    }
    else
    {
      traverse = traverse->next;
    }
  }

  //if not a duplicate create new node and insert at tail
  int time = current_timestamp();
  fprintf(hlog,"%u: THREAD %d INSERT,%u,%s,%u \n",time,thread_cnt,NewHas,INname,INsalary);
  
  fprintf(hlog,"%u: THREAD %d ",time,thread_cnt);
  rwlock_write_lock(&lock);

  hashRecord *tailData = malloc(1 * sizeof(hashRecord));
  tailData->hash = NewHas;
  strcpy(tailData->name,INname);
  tailData->salary = INsalary;
  tailData->next = NULL;

  while (current->next != NULL)
  {
    current = current->next;
  }
  current->next = tailData;
  printf("Inserted %u,%s,%d \n",NewHas,INname,INsalary);
  fprintf(hlog,"%u: THREAD %d ",time,thread_cnt);
  rwlock_write_unlock(&lock);
  thread_cnt += 1;
}

void delete(char * INname){
  hashRecord *traverse = head;

  uint32_t tempHash = jenkins_one_at_a_time_hash((const uint8_t*)INname,strlen(INname));

  int time = current_timestamp();
  fprintf(hlog,"%u: THREAD %d DELETE,%u,%s\n",time,thread_cnt,tempHash,INname);

  fprintf(hlog,"%u: THREAD %d ",time,thread_cnt);
  rwlock_write_lock(&lock);

  if(head == NULL){
    printf("LL is empty");
    return;
  }

  if(head->hash == tempHash){
    
    hashRecord * dataNone = head;
    head = head->next;
    free(dataNone);
    return;
  }
  


  while (traverse != NULL)
  {
    if(traverse->next != NULL){
      if (traverse->next->hash == tempHash)
      {
        hashRecord * del = traverse->next;
       
        traverse->next = traverse->next->next;
        free(del);
        printf("Deleted record for %u,%s,%d \n",tempHash,del->name,del->salary);
        fprintf(hlog,"%u: THREAD %d ",time,thread_cnt);
        rwlock_write_unlock(&lock);
        thread_cnt += 1;
        return;
      }
      else
      {
        traverse = traverse->next;
      }
    }
  }
  

}

void update(char * INname, uint32_t newsalary){
  hashRecord *traverse = head;

  uint32_t tempHash = jenkins_one_at_a_time_hash((const uint8_t*)INname,strlen(INname));

  int time = current_timestamp();
  fprintf(hlog,"%u: THREAD %d UPDATE,%u,%s,%u\n",time,thread_cnt,tempHash,INname,newsalary);
  fprintf(hlog,"%u: THREAD %d ",time,thread_cnt);
  rwlock_write_lock(&lock);

  if(head == NULL){
    printf("LL is empty");
    return;
  }

  while(traverse != NULL){
    if(traverse->hash == tempHash){
      traverse->salary = newsalary;
      printf("Updated record %u from %u,%s,%d to %u,%s,%d \n",tempHash,tempHash,traverse->name,traverse->salary,tempHash,traverse->name,newsalary);
      fprintf(hlog,"%u: THREAD %d ",time,thread_cnt);
      rwlock_write_unlock(&lock);
      thread_cnt += 1;
      return;
    }
    else{
      traverse = traverse->next;
    }
  }
  printf("Update failed. Entry %u not found.\n",tempHash);
  fprintf(hlog,"%u: THREAD %d ",time,thread_cnt);
  rwlock_write_unlock(&lock);
  thread_cnt += 1;
  return;
}

void search(char * INname){
  hashRecord *traverse = head;

  uint32_t tempHash = jenkins_one_at_a_time_hash((const uint8_t*)INname,strlen(INname));
  int time = current_timestamp();
  fprintf(hlog,"%u: THREAD %d SEARCH,%u,%s\n",time,thread_cnt,tempHash,INname);
  fprintf(hlog,"%u: THREAD %d ",time,thread_cnt);
  rwlock_read_lock(&lock);

  if(head == NULL){
    printf("LL is empty");
    return;
  }

  while(traverse != NULL){
    if(traverse->hash == tempHash){
      printf("Found: %u,%s,%d \n",tempHash,traverse->name,traverse->salary);
      fprintf(hlog,"%u: THREAD %d ",time,thread_cnt);
      rwlock_read_unlock(&lock);
      thread_cnt += 1;
      return;
    }
    else{
      traverse = traverse->next;
    }
  }
  printf("%s not found\n",INname);
  fprintf(hlog,"%u: THREAD %d ",time,thread_cnt);
  rwlock_read_unlock(&lock);
  thread_cnt += 1;
  return;
}

void print(){
hashRecord *traverse = head;

  if(head == NULL){
    printf("LL is empty");
    return;
  }
  int time = current_timestamp();
  fprintf(hlog,"%u: THREAD %d PRINT\n",time,thread_cnt);
  fprintf(hlog,"%u: THREAD %d ",time,thread_cnt);
  rwlock_read_lock(&lock);
  printf("Current Database:\n");
  while(traverse != NULL){
    //Print data per node
    printf("%u,%s,%d\n",traverse->hash,traverse->name,traverse->salary);
    traverse = traverse->next;
  }
  fprintf(hlog,"%u: THREAD %d ",time,thread_cnt);
  rwlock_read_unlock(&lock);
  thread_cnt += 1;
}

void WaitThread(int threadNum){
  for(int i = 0; i< threadNum;i++){
    int time = current_timestamp();
    fprintf(hlog,"%u: THREAD %d WAITING FOR MY TURN\n",time,i);
  }
}

void FinalTablePrint(){
  hashRecord *traverse = head;
  fprintf(hlog,"Number of lock acquisitions: %d\n",lockAcq);
  fprintf(hlog,"Number of lock releases: %d\n",lockRel);

  if(head == NULL){
    printf("LL is empty");
    return;
  }
  printf("Final Table:\n");
  fprintf(hlog,"Final Table:\n");
  while(traverse != NULL){
    //Print data per node
    printf("%u,%s,%d\n",traverse->hash,traverse->name,traverse->salary);
    fprintf(hlog,"%u,%s,%d\n",traverse->hash,traverse->name,traverse->salary);
    traverse = traverse->next;
  }
}
int main()
{

  //initalise read write lock
  rwlock_init(&lock);

  //Read by char 
  char data;

  //Sentinal Values
  int sentInstruct = 0;
  int sentName = 0;
  int sentSalary = 0;
  int sentOrder = 0;

  //TempValues
  char TempInstruct[30];
  char TempName[50];
  char TempSal[30];
  char TempThread[30];
  //Conver to ints
  
  uint32_t sal;

  


 

  int i = 0;
  int j = 0;
  //Read input
  FILE *fileR = fopen("commands.txt", "r");
  //write to log
  hlog = fopen("hash.log","a");

  while ((data = fgetc(fileR)) != EOF)
  {
    if (sentInstruct != 1)
    {

      if (data == ',')
      {
        TempInstruct[i] = '\0';
        sentInstruct = 1;
        i = 0;
        
        if (strcmp(TempInstruct, "threads") == 0)
        {
          while ((data = fgetc(fileR)) != EOF && data != '\n')
          {
            TempThread[j] = data;
            if (data == ',')
            {
              TempThread[j] = '\0';
              int thredNumber = atoi(TempThread);
              WaitThread(thredNumber);
              j = 0;
            }
            else{
              j += 1;
            }
            
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
        sentOrder = 1;
        i = 0;
        //Now do all command stuff
        if(strcmp(TempInstruct,"insert") == 0){
          int time = current_timestamp();
          fprintf(hlog,"%u: THREAD %d AWAKENED FOR WORK\n",time,thread_cnt);
          
          insert(TempName,sal);
        }
        else if(strcmp(TempInstruct,"delete") == 0){
          int time = current_timestamp();
          fprintf(hlog,"%u: THREAD %d AWAKENED FOR WORK\n",time,thread_cnt);

          delete(TempName);
        }
        else if(strcmp(TempInstruct,"update") == 0){
          int time = current_timestamp();
          fprintf(hlog,"%u: THREAD %d AWAKENED FOR WORK\n",time,thread_cnt);

          update(TempName,sal);
        }
        else if(strcmp(TempInstruct,"search") == 0){
          int time = current_timestamp();
          fprintf(hlog,"%u: THREAD %d AWAKENED FOR WORK\n",time,thread_cnt);

          search(TempName);
        }
        else if(strcmp(TempInstruct,"print") == 0){
          int time = current_timestamp();
          fprintf(hlog,"%u: THREAD %d AWAKENED FOR WORK\n",time,thread_cnt);

          print();
        }
        else{
          printf("Undefined instruction\n");
        }

        sentInstruct = 0;
        sentName = 0;
        sentSalary = 0;
        sentOrder = 0;
      }
      else
      {
        i += 1;
      }
    }
    
  }

  fclose(fileR);
  //Final print
  FinalTablePrint();
  return 0;
}
