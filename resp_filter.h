#pragma once

#include "jsmn.h"
#include "memory.h"
#include "saavn_types.h"

#include <stdbool.h>

#define TOKEN_COUNT 768

saavn_song_arr_t* filter_songs_from_search(memory_dyn *mem);
