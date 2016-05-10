#include "ao/include/ao/ao.h"
#include "mpg123/src/libmpg123/mpg123.h"

class SoundPlayer {
public:
  SoundPlayer();
  int async_play(char *name);
  void *play(void *uncastName);
  void clean(void);
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
