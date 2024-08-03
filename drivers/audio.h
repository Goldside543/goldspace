#ifndef AUDIO_H
#define AUDIO_H

#include <stddef.h>

void audio_init(void);
void audio_play(const unsigned char *data, size_t length);

#endif // AUDIO_H
