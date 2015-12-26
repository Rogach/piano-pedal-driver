#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <esd.h>
#include <fcntl.h>
#include <xdo.h>

#define THRESHOLD 2

int main() {
  int esd_socket = 0;

  esd_format_t format = ESD_BITS8|ESD_STEREO|ESD_STREAM|ESD_MONITOR;
  while (esd_socket == 0) {
    if ((esd_socket = esd_record_stream_fallback(format, 1000, NULL, "ppd")) < 0) {
      printf("failed to open ESD socket\n");
      return 1;
    }
    if (esd_socket == 0) {
      printf("ESD not ready: got socket #0\n");
      close(esd_socket);
      sleep(3);
    }
  }

  printf("opened ESD socket #%d\n", esd_socket);

  if (fcntl(esd_socket, F_SETFL, O_NONBLOCK) == -1) {
    printf("failed to set ESD socket to non-blocking mode\n");
    return 1;
  }

  printf("set ESD socket to non-blocking mode\n");

  xdo_t *xdo = xdo_new(NULL);

  int ctrl_on = 0;

  while (1) {
    usleep(10000);

    unsigned char buffer[1024*64];
    int read_bytes = read(esd_socket, buffer, sizeof(buffer));
    for (int i = 1; i < read_bytes; i += 2) {
      if (buffer[i] - 128 > THRESHOLD && !ctrl_on) {
        printf("on\n");
        ctrl_on = 1;
        xdo_keysequence_down(xdo, CURRENTWINDOW, "37", 0);
      } else if (buffer[i] - 128 < - THRESHOLD && ctrl_on) {
        printf("off\n");
        ctrl_on = 0;
        xdo_keysequence_up(xdo, CURRENTWINDOW, "37", 0);
      }
    }
  }

  close(esd_socket);

  return 0;
}
