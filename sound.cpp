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
#define MAX_NAME_SIZE 40

using namespace std;

void *helperFunction(void *x);

class SoundPlayer {
public:
  SoundPlayer();
  int async_play(char *name);
  void *play(void *uncastName);
  void clean(void);
private:
  mpg123_handle *mh;
  vector<unsigned char *> buffers;
  //unsigned char *buffer;
  int nBuffers;
  size_t buffer_size;
  int err;
  int driver;
  ao_device *dev;
  ao_sample_format format;
  int channels, encoding;
  long rate;
  map<string, int> names;
  map<string, size_t> sizes;
  int lastBuffer;
  int fileOpened;
};

struct thread_data {
  char *name;
  SoundPlayer *player;
};

SoundPlayer::SoundPlayer() {
  fileOpened = 0;
  lastBuffer = -1;
  ao_initialize();
  driver = ao_default_driver_id();
  mpg123_init();
  mh = mpg123_new(NULL, &err);
  buffer_size = mpg123_outblock(mh);
  nBuffers = 10;
  printf("mallocing buffers...\n");

  buffers.reserve(nBuffers);
  for (int i = 0; i < nBuffers; i++) {
    buffers[i] = (unsigned char *) malloc(buffer_size * sizeof(unsigned char));
  }

  // printf("done mallocing.\n");
  // usleep(10000000);
  // printf("done sleeping\n");
  //buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));
}

void *SoundPlayer::play(void *uncastName) {
  cout << "play called\n";
  size_t done;
  char *name = (char *) uncastName;
  string strName(name);
  map<string,int>::iterator it;

  /* open the file and get the decoding format */
  mpg123_open(mh, name);
  mpg123_getformat(mh, &rate, &channels, &encoding);

/* set the output format and open the output device */

  printf("about to encode\n");
  format.bits = mpg123_encsize(encoding) * BITS;
  format.rate = rate;
  format.channels = channels;
  format.byte_format = AO_FMT_NATIVE;
  format.matrix = "L,R";
  dev = ao_open_live(driver, &format, NULL);

  printf("about to play\n");

  while (mpg123_read(mh, buffers[lastBuffer], buffer_size, &done) == MPG123_OK)
    ao_play(dev, (char *) buffers[lastBuffer], done);

  /*************************************************/

  // it = names.find(strName);
  //
  // //if this file hasn't been loaded before
  // if (it == names.end()) {
  //
  //   cout << "new file" << endl << strName << endl;
  //
  //   //add filename to map
  //   lastBuffer++;
  //   names[strName] = lastBuffer;
  //
  //   /* open the file and get the decoding format */
  //   mpg123_open(mh, name);
  //   if (!fileOpened) {
  //     mpg123_getformat(mh, &rate, &channels, &encoding);
  //
  //   /* set the output format and open the output device */
  //     format.bits = mpg123_encsize(encoding) * BITS;
  //     format.rate = rate;
  //     format.channels = channels;
  //     format.byte_format = AO_FMT_NATIVE;
  //     format.matrix = "L,R";
  //     fileOpened = 1;
  //   }
  //   dev = ao_open_live(driver, &format, NULL);
  //
  //   mpg123_read(mh, buffers[lastBuffer], buffer_size, &done);
  //   sizes[strName] = done;
  //   ao_play(dev, (char *) buffers[lastBuffer], done);
  //
  // } else { //file has been loaded before. play the correct buffer
  //
  //   cout << strName << endl;
  //   int buffNum = names.at(strName);
  //   size_t size = sizes.at(strName);
  //
  //   cout << "buffNum: " << buffNum << " size; " << size << endl;
  //
  //   dev = ao_open_live(driver, &format, NULL);
  //   ao_play(dev, (char *) buffers[buffNum], size);
  // }

  return NULL;
}

int SoundPlayer::async_play(char *name) {
  char localName[MAX_NAME_SIZE];
  strcpy(localName, name);

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
  for (int i = 0; i < nBuffers; i++) {
    free(buffers[i]);
  }
  ao_close(dev);
  mpg123_close(mh);
  mpg123_delete(mh);
  mpg123_exit();
  ao_shutdown();
}

void *helperFunction(void *x) {
  printf("helperfunction called");
  struct thread_data *data = (struct thread_data *) x;
  SoundPlayer *p = data->player;
  p->play(data->name);

  pthread_exit(NULL);
  return NULL;
}
