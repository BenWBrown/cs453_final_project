/* soundtest.cpp
 *
 * Name: Dylan Quenneville
 */


#include <stdlib.h>
#include "ao/include/ao/ao.h"
#include "mpg123/src/libmpg123/mpg123.h"
#include <iostream>
#include <pthread.h>

#define BITS 8

void *helperFunction(void *x);

class SoundPlayer {
public:
  SoundPlayer();
  int async_play(char *name);
  void *play(void *uncastName);
  void clean(void);
private:
  mpg123_handle *mh;
  //unsigned char **buffers;
  unsigned char *buffer;
  //int nBuffers;
  size_t buffer_size;
  size_t done;
  int err;
  int driver;
  ao_device *dev;
  ao_sample_format format;
  int channels, encoding;
  long rate;
};

struct thread_data {
  char *name;
  SoundPlayer *player;
};

SoundPlayer::SoundPlayer() {
  ao_initialize();
  driver = ao_default_driver_id();
  mpg123_init();
  mh = mpg123_new(NULL, &err);
  buffer_size = mpg123_outblock(mh);
  // nBuffers = 10;
  // buffers = (unsigned char **) malloc(nBuffers * sizeof(unsigned char *));
  // for (int i=0; i < nBuffers; i++){
  //   buffers[i] = (unsigned char *) malloc(buffer_size * sizeof(unsigned char));
  // }
  buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));
}

void *SoundPlayer::play(void *uncastName) {
  char *name = (char *) uncastName;
  /* open the file and get the decoding format */
  mpg123_open(mh, name);
  mpg123_getformat(mh, &rate, &channels, &encoding);

/* set the output format and open the output device */
  format.bits = mpg123_encsize(encoding) * BITS;
  format.rate = rate;
  format.channels = channels;
  format.byte_format = AO_FMT_NATIVE;
  format.matrix = 0;
  dev = ao_open_live(driver, &format, NULL);

  while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK) {
      ao_play(dev, (char *) buffer, done);
  }

  return NULL;
}

int SoundPlayer::async_play(char *name) {
  pthread_t thread;
  int rc;
  struct thread_data *data = (struct thread_data *) malloc(sizeof(struct thread_data));
  data->name = name;
  data->player = this;
  rc = pthread_create(&thread, NULL, helperFunction, (void *) data);

  return 0;
}

void SoundPlayer::clean(void) {

  /* clean up */
  free(buffer);
  ao_close(dev);
  mpg123_close(mh);
  mpg123_delete(mh);
  mpg123_exit();
  ao_shutdown();
}

void *helperFunction(void *x) {
  struct thread_data *data = (struct thread_data *) x;
  SoundPlayer *p = data->player;
  p->play(data->name);

  pthread_exit(NULL);
  return NULL;
}
