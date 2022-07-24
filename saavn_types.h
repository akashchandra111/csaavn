#ifndef SAAVN_TYPES_H
#define SAAVN_TYPES_H

#include <stdlib.h>

typedef struct	{
	char url[256];
	char description[128];
	char title[64];
	char album[64];
	char id[16];
} saavn_album_t;

typedef struct	{
	saavn_album_t *album;
	size_t len;
} saavn_album_arr_t;

typedef struct	{
	char url[256];
	char description[128];
	char title[64];
	char album[64];
	char id[16];
} saavn_song_t;

typedef struct	{
	saavn_song_t *song;
	size_t len;
} saavn_song_arr_t;

saavn_album_t* saavn_album_init();
void saavn_album_free(saavn_album_t *);

saavn_song_t* saavn_song_init();
void saavn_song_free(saavn_song_t *);

saavn_album_arr_t* saavn_album_arr_init(size_t album_count);
void saavn_album_arr_free(saavn_album_arr_t*);

saavn_song_arr_t* saavn_song_arr_init(size_t song_count);
void saavn_song_arr_free(saavn_song_arr_t*);

#endif
