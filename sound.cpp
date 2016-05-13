/* soundtest.cpp
 *
 * Name: Dylan Quenneville
 */


#include <stdlib.h>
#include "ao/include/ao/ao.h"
#include "mpg123/src/libmpg123/mpg123.h"
#include <iostream>
#include <pthread.h>
#include <string>
#include <map>
#include <vector>

#define BITS 8

using namespace std;

void *play(void *uncastName) {
  char *name = (char *)uncastName;
  mpg123_handle *mh;
  unsigned char *buffer;
  size_t buffer_size;
  size_t done;
  int err;

  int driver;
  ao_device *dev;

  ao_sample_format format;
  int channels, encoding;
  long rate;

  /* initializations */
  ao_initialize();
  driver = ao_default_driver_id();
  mpg123_init();
  mh = mpg123_new(NULL, &err);
  buffer_size = mpg123_outblock(mh);
  buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

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

  /* decode and play */
  while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
      ao_play(dev, (char *) buffer, done);

  /* clean up */
  free(buffer);


  pthread_exit(NULL);

  return 0;
}

void async_play(char *name) {
  pthread_t thread;
  pthread_create(&thread, NULL, play, (void *) name);
}
