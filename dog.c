#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <err.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

//reads file descriptor
void dog(int rfiledesc) {

  //write file descriptor
  int wfiledesc;
  //creates a buffer
  static char *buffer;
  //creates the blocksize
  static size_t blocksize;
  //creates a stat that will return the file stats
  struct stat sbuffer;

  //returns the file descriptor writer number
  wfiledesc = fileno(stdout);

  //checks if the file descriptor writer returns any errors
  if (fstat(wfiledesc, &sbuffer)) {
    err(1, "stdout");
  }

  //finds the best block size to be used
  if (!buffer) {
    blocksize = sbuffer.st_blksize;
    buffer = malloc(blocksize);
    //checks if the buffer returns any errors
    if(!buffer) {
      err(1, 0);
    }
  }
  //creates file name reader and file name writer
  ssize_t fnread, fnwrite;
  //creates loop because sometimes the write function
  //doesn't read the whole file and this variable will make it keep trying to read.
  int loop = 0;

  //reads file name
  fnread = read(rfiledesc, buffer, blocksize);

  while (fnread > 0) {
    //runs the write function until it writes the whole file
    for(loop = 0; fnread > 0; fnread -= fnwrite, loop += fnwrite) {
      fnwrite = write(wfiledesc, buffer + loop, fnread);
      //if the file is empty returns an error.
      if (fnread < 0) {
        err(0, "stdout");
      }
    }
    //reads the file
    fnread = read(rfiledesc, buffer, blocksize);
  }
}

int main(int argc, char const *argv[]) {
  //creates file descriptor
  int filedesc;
  //increments the argv
  ++argv;
  //returns the file descriptor number
  filedesc = fileno(stdin);

  do
  {
    if (*argv) {
      //checks if the file has - if so, opens it as read-only.
      if(strcmp(*argv, "-") == 0) {
        filedesc = fileno(stdin);
      }
      else {
        filedesc = open(*argv, O_RDONLY);
      }
      //if the file is empty returns an error
      if (filedesc < 0) {
        err(1, "%s", *argv);
      }
      //after checks for errors, if it returns none, increments the argv
      ++argv;
    }
    //calls the dog command
    dog(filedesc);
    //if the file descriptor was different than the file desctiptor number, ends the command.
    if(filedesc != fileno(stdin)){
      close(filedesc);
    }
  } while (*argv);
}
