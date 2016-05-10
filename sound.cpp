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

class SoundPlayer {
public:
  SoundPlayer();
  int async_play(const char *name);
  void *play(void *uncastName);
private:
  mpg123_handle *mh;
  unsigned char **buffers;
  int nBuffers;
  size_t buffer_size;
  size_t done;
  int err;
  int driver;
  ao_device *dev;
  ao_sample_format format;
  int channels, encoding;
  long rate;
};
SoundPlayer::SoundPlayer() {
  ao_initialize();
  driver = ao_default_driver_id();
  mpg123_init();
  mh = mpg123_new(NULL, &err);
  buffer_size = mpg123_outblock(mh);
  nBuffers = 10;
  buffers = (unsigned char **) malloc(nBuffers * sizeof(unsigned char *));
  for (int i=0; i < nBuffers; i++){
    buffers[i] = (unsigned char *) malloc(buffer_size * sizeof(unsigned char));
  }
  /* set the output format and open the output device */
  format.bits = mpg123_encsize(encoding) * BITS;
  format.rate = rate;
  format.channels = channels;
  format.byte_format = AO_FMT_NATIVE;
  format.matrix = 0;
  dev = ao_open_live(driver, &format, NULL);
}
void *SoundPlayer::play(void *uncastName) {
  char *name = (char *) uncastName;
  mpg123_handle *mh = NULL;
  /* open the file and get the decoding format */
  mpg123_open(mh, name);
  mpg123_getformat(mh, &rate, &channels, &encoding);

  pthread_exit(NULL);
}
int SoundPlayer::async_play(const char *name) {
  pthread_t thread;
  int rc;
  rc = pthread_create(&thread, NULL, this->async_play, (void *) name);
  while (mpg123_read(mh, buffers[0], buffer_size, &done) == MPG123_OK) {
      ao_play(dev, (char *) buffers[0], done);
  }

  return 0;
}

// void *play(void *uncastName)
// {
//     char *name = (char *) uncastName;
//     mpg123_handle *mh;
//     unsigned char *buffer;
//     size_t buffer_size;
//     size_t done;
//     int err;
//
//     int driver;
//     ao_device *dev;
//
//     ao_sample_format format;
//     int channels, encoding;
//     long rate;
//
//
//     /* initializations */
//     ao_initialize();
//     driver = ao_default_driver_id();
//     mpg123_init();
//     mh = mpg123_new(NULL, &err);
//     buffer_size = mpg123_outblock(mh);
//     buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));
//
//     /* open the file and get the decoding format */
//     mpg123_open(mh, name);
//     mpg123_getformat(mh, &rate, &channels, &encoding);
//
//     /* set the output format and open the output device */
//     format.bits = mpg123_encsize(encoding) * BITS;
//     format.rate = rate;
//     format.channels = channels;
//     format.byte_format = AO_FMT_NATIVE;
//     format.matrix = 0;
//     dev = ao_open_live(driver, &format, NULL);
//
//     /* decode and play */
//     while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK) {
//         ao_play(dev, (char *) buffer, done);
//     }
//
//     /* clean up */
//     // free(buffer);
//     // ao_close(dev);
//     // mpg123_close(mh);
//     // mpg123_delete(mh);
//     // mpg123_exit();
//     // ao_shutdown();
//
//     pthread_exit(NULL);
//     return 0;
// }
//
// int async_play(const char *name){
//   pthread_t thread;
//   int rc;
//   rc = pthread_create(&thread, NULL, play, (void *) name);
//
//   return 0;
// }
