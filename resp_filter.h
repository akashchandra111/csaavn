#ifndef SAAVN_RESP_FILTER_H
#define SAAVN_RESP_FILTER_H

#include "memory.h"
#include "saavn_types.h"

#include <stdbool.h>

#define TOKEN_COUNT 1024

saavn_song_arr_t* filter_songs_from_search(memory_dyn *mem);
void filter_song_url_from_search(size_t id_len, saavn_song_t *song, memory_dyn *mem);

#endif
