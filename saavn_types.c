#include "saavn_types.h"

saavn_album_t* saavn_album_init()	{
	return (saavn_album_t *) malloc(sizeof(saavn_album_t));
}

void saavn_album_free(saavn_album_t *album)	{
	if (album)	{
		free(album);
		album = NULL;
	}
}

saavn_song_t* saavn_song_init()	{
	return (saavn_song_t *) malloc(sizeof(saavn_song_t));
}

void saavn_song_free(saavn_song_t *song)	{
	if (song)	{
		free(song);
		song = NULL;
	}
}

saavn_album_arr_t* saavn_album_arr_init(size_t album_count)	{
	saavn_album_arr_t *albums = (saavn_album_arr_t *) malloc(sizeof(saavn_album_arr_t));

	if (albums)	{
		albums->album = (saavn_album_t *) malloc(sizeof(saavn_album_t) * album_count);
		
		if (albums->album)	{
			albums->len = album_count;
		} else	{
			free(albums);
			albums = NULL;
		}
	}

	return albums;
}

void saavn_album_arr_free(saavn_album_arr_t *albums)	{
	if (albums)	{
		if (albums->album)	{
			free(albums->album);
			albums->album = NULL;
		}

		free(albums);
		albums = NULL;
	}
}

saavn_song_arr_t* saavn_song_arr_init(size_t song_count)	{
	saavn_song_arr_t *songs = (saavn_song_arr_t *) malloc(sizeof(saavn_song_arr_t));

	if (songs)	{
		songs->song = (saavn_song_t *) malloc(sizeof(saavn_album_t) * song_count);
		
		if (songs->song)	{
			songs->len = song_count;
		} else	{
			free(songs);
			songs = NULL;
		}
	}

	return songs;
}

void saavn_song_arr_free(saavn_song_arr_t *songs)	{
	if (songs)	{
		if (songs->song)	{
			free(songs->song);
			songs->song = NULL;
		}

		free(songs);
		songs = NULL;
	}
}
