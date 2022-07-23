#pragma once

#include <stddef.h>

const char SEARCH_API_URL[] = "https://www.jiosaavn.com/api.php?__call=autocomplete.get&_format=json&_marker=0&cc=in&includeMetaTags=1&query=";
const size_t SEARCH_API_URL_LEN = sizeof(SEARCH_API_URL)/sizeof(char);

const char SONG_API_URL[] = "https://www.jiosaavn.com/api.php?__call=song.getDetails&cc=in&_marker=0%3F_marker%3D0&_format=json&pids=";
const size_t SONG_API_URL_LEN = sizeof(SONG_API_URL)/sizeof(char);

const char ALBUM_API_URL[] = "https://www.jiosaavn.com/api.php?__call=content.getAlbumDetails&_format=json&cc=in&_marker=0%3F_marker%3D0&albumid=";
const size_t ALBUM_API_URL_LEN = sizeof(ALBUM_API_URL)/sizeof(char);

const char PLAYLIST_API_URL[] = "https://www.jiosaavn.com/api.php?__call=playlist.getDetails&_format=json&cc=in&_marker=0%3F_marker%3D0&listid=";
const size_t PLAYLIST_API_URL_LEN = sizeof(PLAYLIST_API_URL)/sizeof(char);

const char LYRICS_API_URL[] = "https://www.jiosaavn.com/api.php?__call=lyrics.getLyrics&ctx=web6dot0&api_version=4&_format=json&_marker=0%3F_marker%3D0&lyrics_id=";
const size_t LYRICS_API_URL_LEN = sizeof(LYRICS_API_URL)/sizeof(char);
