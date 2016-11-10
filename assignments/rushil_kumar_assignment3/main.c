#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

int main(){
  int buffer_length = 500;
  /* char buffer[buffer_length] = ""; */
  /* char *buffer = malloc(sizeof(char) * buffer_length); */
  /* memset(buffer, '\0', sizeof(char) * buffer_length); */
  char *buffer = calloc(sizeof(char), buffer_length);
  /* printf("Opening device\n"); */
  int fd = open("/dev/process_list", O_RDONLY);
  /* printf("Opened device\n"); */
  int bytes_read = 1;
  while(bytes_read > 0){
    bytes_read = read(fd, buffer, buffer_length);
    printf("%s", buffer);
    buffer[0] = '\0';
  }
  close(fd);
  free(buffer);
  return 1;
}
