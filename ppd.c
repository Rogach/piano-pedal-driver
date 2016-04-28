#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <xdo.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#define THRESHOLD 30
#define SAMPLE_RATE 1000
#define BUFSIZE 20

int main() {
  static const pa_sample_spec sample_spec = {
    .format = PA_SAMPLE_U8,
    .rate = SAMPLE_RATE,
    .channels = 2
  };

  pa_simple *pulse_socket = NULL;
  int error;

  if (!(pulse_socket = pa_simple_new(NULL, "piano-pedal-driver", PA_STREAM_RECORD, NULL, "record", &sample_spec, NULL, NULL, &error))) {
    fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
    return 1;
  }

  uint8_t buffer[BUFSIZE];

  xdo_t *xdo = xdo_new(NULL);

  int ctrl_on = 0;

  while (1) {
    if (pa_simple_read(pulse_socket, buffer, BUFSIZE, &error) < 0) {
      fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
      pa_simple_free(pulse_socket);
      return 1;
    }

    for (int i = 1; i < BUFSIZE; i += 2) {
      if (buffer[i] - 128 > THRESHOLD && !ctrl_on) {
        printf("on\n");
        ctrl_on = 1;
        xdo_send_keysequence_window_down(xdo, CURRENTWINDOW, "37", 0);
      } else if (buffer[i] - 128 < - THRESHOLD && ctrl_on) {
        printf("off\n");
        ctrl_on = 0;
        xdo_send_keysequence_window_up(xdo, CURRENTWINDOW, "37", 0);
      }
    }
  }

  if (pulse_socket) {
    pa_simple_free(pulse_socket);
  }

  return 0;
}
