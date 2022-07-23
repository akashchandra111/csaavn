#include <stdio.h>
#include <stdint.h>

#include "saavn_api_req.h"
#include "resp_filter.h"

const int CHAR_BUF = 256;
const int HALF_CHAR_BUF = CHAR_BUF/2;
const int RESP_BUF_LEN = 1 * 1024 * 1024;	// 1 Meg

int main(void)	{
	char search_item[CHAR_BUF/2];
	char search_desc[CHAR_BUF];

	printf("Enter song name to search: ");
	size_t song_len = scanf("HALF_CHAR_BUF%s", search_item);

	printf("Enter description to find faster (artist, album name, release year, etc.): ");
	size_t desc_len = scanf("CHAR_BUF%s", search_desc);

	memory_dyn *resp = mem_dyn_init(RESP_BUF_LEN);

	if (resp)	{
		if (saavn_perform_search(search_item, song_len, search_desc, desc_len, resp))	{
			saavn_song_arr_t *songs = filter_songs_from_search(resp);

			if (!songs)	goto end;
			
			for (int i=0; i<songs->len; ++i)	{
				printf("Song name: %s\n", songs->song[i].title);
				printf("Description: %s\n", songs->song[i].description);
				printf("Album: %s\n", songs->song[i].album);
				printf("URL: %s\n", songs->song[i].url);
				printf("ID: %s\n", songs->song[i].id);
			}
		}
	}

	mem_dyn_free(resp);

end:
	return 0;
}
