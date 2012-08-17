#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
int main(int argc, char *argv[]) {
  int i, fd;
  for (i = 1; i < argc; i++) {
    if ((fd = open(argv[i], O_RDONLY)) != -1) {
      struct stat st;
      fstat(fd, &st);
      posix_fadvise(fd, 0, st.st_size, POSIX_FADV_DONTNEED);
      close(fd);
    }
  }
  return 0;
}
