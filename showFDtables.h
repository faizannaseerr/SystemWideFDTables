#ifndef SHOWFDTABLES_H
#define SHOWFDTABLES_H

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

typedef struct process
{
  char PID[1024];
  char FD[1024];
  unsigned long int inode;
  char location[1024];
  struct process *next;
} process;

process *newProcess(char *PID, char *FD, unsigned long int inode, char *location);
process *insert(process *head, process *newProcess);
void AssessArgument(char *argument, char *PID, process *head);
process *StoreProcesses(process *head);
char *checkForPID(int argcount, char *argvalues[]);
void AssessArguments(int argcount, char *argvalues[]);
int ThresholdPresent(char *argument);
int digits_only(char *argument);
void PrintPerProcessFDTable(char *PID, process *head);
void PrintSystemWideFDTable(char *PID, process *head);
void PrintVnodesTable(char *PID, process *head);
void PrintCompositeTable(char *PID, process *head);
void PrintThresholdTable(int X, char *PID, process *head);
void CreateCompositeBin(char *PID, process *head);
void CreateCompositeTxt(char *PID, process *head);

#endif