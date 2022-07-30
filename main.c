#include <stdio.h>
#include <stdint.h>

#include "saavn_api_req.h"
#include "resp_filter.h"

#define CHAR_BUF 256
#define HALF_CHAR_BUF (CHAR_BUF/2)
const int RESP_BUF_LEN = 1 * 1024 * 1024;	// 1 Meg

static void url_escaped(char *str, size_t len)	{
	for (size_t i=0; i<len; ++i)	{
		if (str[i] == ' ')	str[i] = '+';
		if (str[i] == '\n')	str[i] = '\0';
	}
}

int main(void)	{
	char search_item[HALF_CHAR_BUF];

	printf("Enter song name to search: ");
	fgets(search_item, HALF_CHAR_BUF, stdin);
	size_t song_len = strlen(search_item);
	url_escaped(search_item, song_len--);	// reducing char len by 1 to remove \n

	curl_global_init(CURL_GLOBAL_DEFAULT);

	memory_dyn *resp = mem_dyn_init(RESP_BUF_LEN);

	if (resp)	{
		if (saavn_perform_search(search_item, song_len, resp))	{
			saavn_song_arr_t *songs = filter_songs_from_search(resp);

			if (!songs)	goto end;
			
			for (int i=0; i<songs->len; ++i)	{
				printf("Song name: %s\n", songs->song[i].title);
				printf("Description: %s\n", songs->song[i].description);
				printf("Album: %s\n", songs->song[i].album);
				printf("URL: %s\n", songs->song[i].url);
				printf("ID: %s\n", songs->song[i].id);
				printf("===========================\n");
			}

			printf("Choose (%zu - %zu): ", 1ul, songs->len);
			int choice = 0;
			int retries = 3;
			while (choice < 1ul || choice > songs->len)	{
				scanf("%d", &choice);
				if (choice < 1ul || choice > songs->len)	printf("Invalid input, try again: ");

				if (!retries)	goto end;
				else	retries--;
			}

			saavn_song_t *song = &songs->song[choice-1];
			char *id = song->id;
			int const id_len = strlen(song->id);

			memset(resp->buffer, 0, resp->size);
			resp->size = 0;

			if (saavn_get_song_url(id, id_len, resp))	{
				filter_song_url_from_search(id_len, song, resp);

				printf("Song name: %s\n", song->title);
				printf("Description: %s\n", song->description);
				printf("Album: %s\n", song->album);
				printf("URL: %s\n", song->url);
				printf("ID: %s\n", song->id);
				printf("===========================\n");
			
			}

			if (saavn_song_download(song->url, strlen(song->url), song->title, strlen(song->title)))	{
				printf("Downloaded %s!\n", song->title);
			}

			saavn_song_arr_free(songs);
		}
	}

end:
	mem_dyn_free(resp);
	curl_global_cleanup();
	return 0;
}
