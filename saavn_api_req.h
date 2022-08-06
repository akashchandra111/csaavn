#ifndef SAAVN_API_REQ_H
#define SAAVN_API_REQ_H 

#include <curl/curl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "saavn_types.h"

bool saavn_perform_search(
	char const *song_name, 
	size_t const song_name_len, 
	memory_dyn *mem
);

bool saavn_get_song_url(
	char *song_id, 
	size_t song_id_len, 
	memory_dyn *mem
);

bool saavn_song_download(
	char *appended_url, 
	size_t url_len, 
	saavn_song_t *song_details
);

#endif
