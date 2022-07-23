#pragma once

#include <curl/curl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "url.h"

bool saavn_perform_search(
	char const *song_name, 
	size_t const song_name_len, 
	char const *song_desc, 
	size_t const song_desc_len, 
	memory_dyn *mem
);
