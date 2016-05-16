/* sound.cpp
 *
 * Ben Brown, Dylan Quenneville, Max Shashoua
 *
 * Based on Johnny Huang's Code at 
 * http://hzqtc.github.io/2012/05/play-mp3-with-libmpg123-and-libao.html
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

// play audio file
void *play(void *uncastName) {
	char *name = (char *)uncastName;	// audio file name
	mpg123_handle *mh;			// mpg123 handler
	unsigned char *buffer;			// set aside audio buffer
	size_t buffer_size;			// audio buffer size
	size_t done;				// done playing audio
	int err;

	// Set up libao driver (find computer audio device)
	int driver;
	ao_device *dev;

	ao_sample_format format;
	int channels, encoding;
	long rate;

	// Initializations
	ao_initialize();
	driver = ao_default_driver_id();
	mpg123_init();
	mh = mpg123_new(NULL, &err);
	buffer_size = mpg123_outblock(mh);
	buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

	// Open the file and get the decoding format
	mpg123_open(mh, name);
	mpg123_getformat(mh, &rate, &channels, &encoding);

	// Set the output format and open the output device
	format.bits = mpg123_encsize(encoding) * BITS;
	format.rate = rate;
	format.channels = channels;
	format.byte_format = AO_FMT_NATIVE;
	format.matrix = 0;
	dev = ao_open_live(driver, &format, NULL);

	// Decode and play
	while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
		ao_play(dev, (char *) buffer, done);

	// Free buffer
	free(buffer);

	pthread_exit(NULL);

	return 0;
}

// play audio in asynchronous thread to keep video loop playing
void async_play(char *name) {
  pthread_t thread;
  pthread_create(&thread, NULL, play, (void *) name);
}
