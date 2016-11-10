#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main(){
  int buffer_length = 1000000;
  char buffer[buffer_length];  
  int fd = open("/dev/process_list", O_RDONLY);
  int bytes_read = read(fd, buffer, buffer_length);
  printf("%s\n", buffer);
  close(fd);
  return 1;
}
