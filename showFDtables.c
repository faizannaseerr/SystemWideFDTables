#include "showFDtables.h"

process *newProcess(char *PID, char *FD, unsigned long int inode, char *location)
{
  process *p = NULL;
  p = (process *)calloc(1, sizeof(process));
  strcpy(p->PID, PID);
  strcpy(p->FD, FD);
  strcpy(p->location, location);
  p->inode = inode;
  p->next = NULL;
  return p;
}

process *insert(process *head, process *newProcess)
{
  if (head == NULL)
  {
    return newProcess;
  }

  process *temp = NULL;
  temp = head;
  while (temp->next != NULL)
  {
    temp = temp->next;
  }
  temp->next = newProcess;
  return head;
}

int main(int argc, char *argv[])
{
  AssessArguments(argc, argv);

  return 0;
}

void AssessArgument(char *argument, char *PID, process *head)
{
  if (strcmp(argument, "--per-process") == 0)
  {
    PrintPerProcessFDTable(PID, head);
  }
  else if (strcmp(argument, "--systemWide") == 0)
  {
    PrintSystemWideFDTable(PID, head);
  }
  else if (strcmp(argument, "--Vnodes") == 0)
  {
    PrintVnodesTable(PID, head);
  }
  else if (strcmp(argument, "--composite") == 0)
  {
    PrintCompositeTable(PID, head);
  }
  else if (ThresholdPresent(argument) == 1)
  {
    char *token = strtok(argument, "=");
    token = strtok(NULL, "=");
    int X = atoi(token);
    PrintThresholdTable(X, PID, head);
  }
  else if (strcmp(argument, "--output_TXT") == 0)
  {
    CreateCompositeTxt(PID, head);
  }
  else if (strcmp(argument, "--output_binary") == 0)
  {
    CreateCompositeBin(PID, head);
  }
}

process *StoreProcesses(process *head)
{
  DIR *dir;
  struct dirent *entry;
  struct stat proc_stat;

  dir = opendir("/proc");
  while ((entry = readdir(dir)) != NULL)
  {
    if (atoi(entry->d_name) != 0)
    {
      char fd_path[256];
      snprintf(fd_path, sizeof(fd_path), "/proc/%s/fd", entry->d_name);
      DIR *fd_dir = opendir(fd_path);
      if (fd_dir != NULL)
      {
        struct dirent *fd_entry;
        while ((fd_entry = readdir(fd_dir)) != NULL)
        {
          if (strcmp(fd_entry->d_name, ".") == 0 || strcmp(fd_entry->d_name, "..") == 0)
          {
            continue;
          }
          // entry->d_name is PID, fd_entry->d_name is FD
          char location_path[256];
          char location[256];
          char proc_path[256];
          snprintf(location_path, sizeof(location_path), "%s/%s", fd_path, fd_entry->d_name);
          int location_path_length = readlink(location_path, location, sizeof(location));
          location[location_path_length] = '\0';
          sprintf(proc_path, "/proc/%s", entry->d_name);
          if (lstat(proc_path, &proc_stat) == -1)
          {
            perror("lstat failed");
            exit(1);
          }
          process *P = newProcess(entry->d_name, fd_entry->d_name, proc_stat.st_ino, location);
          head = insert(head, P);
        }
        closedir(fd_dir);
      }
    }
  }
  closedir(dir);
  return head;
}

char *checkForPID(int argcount, char *argvalues[])
{
  for (int i = 1; i < argcount; i++)
  {
    if (digits_only(argvalues[i]) == 1)
    {
      return argvalues[i];
    }
  }
  return "00";
}

void AssessArguments(int argcount, char *argvalues[])
{
  process *head = NULL;
  head = StoreProcesses(NULL);
  char PID[1024];
  strcpy(PID, checkForPID(argcount, argvalues));
  if (argcount == 1)
  {
    AssessArgument("--composite", "-1", head);
  }
  else if (argcount == 2 && strcmp(PID, "00") != 0)
  {
    AssessArgument("--composite", PID, head);
  }
  else if (strcmp(PID, "00") != 0)
  {
    for (int i = 1; i < argcount; i++)
    {
      AssessArgument(argvalues[i], PID, head);
    }
  }
  else
  {
    for (int i = 1; i < argcount; i++)
    {
      AssessArgument(argvalues[i], "-1", head);
    }
  }
}

int ThresholdPresent(char *argument)
{
  char input[strlen(argument) + 1];
  strcpy(input, argument);
  char *token = strtok(input, "=");
  if (token != NULL && strcmp(token, "--threshold") == 0)
  {
    return 1;
  }
  return 0;
}

int digits_only(char *argument)
{
  for (int i = 0; argument[i] != '\0'; i++)
  {
    if (!isdigit(argument[i]))
    {
      return 0;
    }
  }
  return 1;
}

void PrintPerProcessFDTable(char *PID, process *head)
{
  process *temp = NULL;
  temp = head;
  printf(" PID     FD \n");
  printf("=============\n");
  if (strcmp(PID, "-1") == 0)
  {
    while (temp->next != NULL)
    {
      printf("%s    %s \n", temp->PID, temp->FD);
      temp = temp->next;
    }
  }
  else
  {
    while (temp != NULL)
    {
      if (strcmp(temp->PID, PID) == 0)
      {
        printf("%s    %s \n", temp->PID, temp->FD);
      }
      temp = temp->next;
    }
  }
}
void PrintSystemWideFDTable(char *PID, process *head)
{
  process *temp = NULL;
  temp = head;
  printf(" PID     FD        Filename \n");
  printf("================================\n");
  if (strcmp(PID, "-1") == 0)
  {
    while (temp != NULL)
    {
      printf("%s    %s        %s\n", temp->PID, temp->FD, temp->location);
      temp = temp->next;
    }
  }
  else
  {
    while (temp != NULL)
    {
      if (strcmp(temp->PID, PID) == 0)
      {
        printf("%s    %s      %s \n", temp->PID, temp->FD, temp->location);
      }
      temp = temp->next;
    }
  }
}

void PrintVnodesTable(char *PID, process *head)
{
  process *temp = NULL;
  temp = head;
  printf(" FD     Inode \n");
  printf("=============\n");
  if (strcmp(PID, "-1") == 0)
  {
    while (temp != NULL)
    {
      printf("%s    %lu \n", temp->FD, temp->inode);
      temp = temp->next;
    }
  }
  else
  {
    while (temp != NULL)
    {
      if (strcmp(temp->PID, PID) == 0)
      {
        printf("%s    %lu \n", temp->FD, temp->inode);
      }
      temp = temp->next;
    }
  }
}

void PrintCompositeTable(char *PID, process *head)
{
  process *temp = NULL;
  temp = head;
  printf(" PID     FD       Filename       Inode \n");
  printf("========================================\n");

  if (strcmp(PID, "-1") == 0)
  {
    while (temp != NULL)
    {
      printf("%s    %s       %lu        %s \n", temp->PID, temp->FD, temp->inode, temp->location);
      temp = temp->next;
    }
  }
  else
  {
    while (temp != NULL)
    {
      if (strcmp(temp->PID, PID) == 0)
      {
        printf("%s    %s       %lu        %s \n", temp->PID, temp->FD, temp->inode, temp->location);
      }
      temp = temp->next;
    }
  }
}

void PrintThresholdTable(int X, char *PID, process *head)
{
  PrintCompositeTable(PID, head);
  printf("## Offending Processes: \n");
  process *current = NULL;
  current = head->next;
  process *previous = NULL;
  previous = head;
  int count = 1;
  while (current != NULL)
  {
    if (strcmp(previous->PID, current->PID) == 0)
    {
      count += 1;
    }
    else
    {
      if (count > X)
      {
        printf("%s (%d) ", previous->PID, count);
      }
      count = 1;
    }
    previous = previous->next;
    current = current->next;
  }
  printf("\n");
}

void CreateCompositeBin(char *PID, process *head)
{
  process *temp = NULL;
  temp = head;
  FILE *fp;
  fp = fopen("compositeTable.bin", "wb");
  char *title = " PID     FD       Filename       Inode \n";
  fwrite(title, sizeof(char), strlen(title) + 1, fp);
  char *border = "========================================\n";
  fwrite(border, sizeof(char), strlen(border) + 1, fp);
  if (strcmp(PID, "-1") == 0)
  {
    while (temp != NULL)
    {
      char *data = malloc(sizeof(char) * 2048);
      sprintf(data, "%s    %s       %lu        %s \n", temp->PID, temp->FD, temp->inode, temp->location);
      fwrite(data, sizeof(char), strlen(data) + 1, fp);
      free(data);
      temp = temp->next;
    }
  }
  else
  {
    while (temp != NULL)
    {
      if (strcmp(PID, temp->PID) == 0)
      {
        char *data = malloc(sizeof(char) * 2048);
        sprintf(data, "%s    %s       %lu        %s \n", temp->PID, temp->FD, temp->inode, temp->location);
        fwrite(data, sizeof(char), strlen(data) + 1, fp);
        free(data);
      }
      temp = temp->next;
    }
  }

  fclose(fp);
}

void CreateCompositeTxt(char *PID, process *head)
{
  process *temp = NULL;
  temp = head;
  FILE *fp;
  fp = fopen("compositeTable.txt", "w");
  fprintf(fp, " PID     FD       Filename       Inode \n");
  fprintf(fp, "========================================\n");
  if (strcmp(PID, "-1") == 0)
  {
    while (temp != NULL)
    {
      fprintf(fp, "%s    %s       %lu        %s \n", temp->PID, temp->FD, temp->inode, temp->location);
      temp = temp->next;
    }
  }
  else
  {
    while (temp != NULL)
    {
      if (strcmp(PID, temp->PID) == 0)
      {
        fprintf(fp, "%s    %s       %lu        %s \n", temp->PID, temp->FD, temp->inode, temp->location);
      }
      temp = temp->next;
    }
  }
  fclose(fp);
}