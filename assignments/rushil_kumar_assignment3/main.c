#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(){
  int buffer_length = 100000;
  char buffer[100000];
  int fd = open("/dev/process_list", O_RDONLY);
  int bytes_read = read(fd, buffer, buffer_length);
  printf("%s", buffer);
  close(fd);
  return 1;
}
