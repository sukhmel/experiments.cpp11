/*
 * File:   memory_stats.cpp
 * Author: couozu
 *
 * Created on March 11, 2015, 2:14 PM
 */

#include "memory_stats.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int parseLine (char* line) {
  int i = strlen(line);
  while (*line < '0' || *line > '9') line++;
  line[i-3] = '\0';
  i = atoi(line);
  return i;
}


int get_virtual_memory () { //Note: this value is in KB!
  FILE* file = fopen("/proc/self/status", "r");
  int result = -1;
  char line[128];


  while (fgets(line, 128, file) != NULL){
      if (strncmp(line, "VmSize:", 7) == 0){
          result = parseLine(line);
          break;
      }
  }
  fclose(file);
  return result;
}

int get_physical_memory() { //Note: this value is in KB!
  FILE* file = fopen("/proc/self/status", "r");
  int result = -1;
  char line[128];


  while (fgets(line, 128, file) != NULL){
      if (strncmp(line, "VmRSS:", 6) == 0){
          result = parseLine(line);
          break;
      }
  }
  fclose(file);
  return result;
}