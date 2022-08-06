#include "id3v2lib/include/id3v2lib.h"
#include "saavn_api_req.h"

const char SEARCH_API_URL[] = "https://www.jiosaavn.com/api.php?__call=autocomplete.get&_format=json&_marker=0&cc=in&includeMetaTags=1&query=";
const char SONG_API_URL[] = "https://www.jiosaavn.com/api.php?__call=song.getDetails&cc=in&_marker=0%3F_marker%3D0&_format=json&pids=";
// const char ALBUM_API_URL[] = "https://www.jiosaavn.com/api.php?__call=content.getAlbumDetails&_format=json&cc=in&_marker=0%3F_marker%3D0&albumid=";
// const char PLAYLIST_API_URL[] = "https://www.jiosaavn.com/api.php?__call=playlist.getDetails&_format=json&cc=in&_marker=0%3F_marker%3D0&listid=";
// const char LYRICS_API_URL[] = "https://www.jiosaavn.com/api.php?__call=lyrics.getLyrics&ctx=web6dot0&api_version=4&_format=json&_marker=0%3F_marker%3D0&lyrics_id=";

const size_t SEARCH_API_URL_LEN = sizeof(SEARCH_API_URL);
const size_t SONG_API_URL_LEN = sizeof(SONG_API_URL);
// const size_t ALBUM_API_URL_LEN = sizeof(ALBUM_API_URL);
// const size_t PLAYLIST_API_URL_LEN = sizeof(PLAYLIST_API_URL);
// const size_t LYRICS_API_URL_LEN = sizeof(LYRICS_API_URL);

const char SONG_DOWNLOAD_URL[][64] = {
	"https://sklktcdnems07.cdnsrv.jio.com/jiosaavn.cdn.jio.com/",
	"https://sklktecdnems03.cdnsrv.jio.com/jiosaavn.cdn.jio.com/"
};

const char BITRATE[][5] = { "_320", "_256", "_128", "_96" };

const char EXTENSION[][5] = { ".mp3", ".mp4" };

const size_t SONG_DOWNLOAD_URL_LEN[] = {
	sizeof(SONG_DOWNLOAD_URL[0]),
	sizeof(SONG_DOWNLOAD_URL[1]),
};

const size_t DOWNLOAD_URLS_LEN = sizeof(SONG_DOWNLOAD_URL)/sizeof(SONG_DOWNLOAD_URL[0]);
const size_t BITRATES_LEN = sizeof(BITRATE)/sizeof(BITRATE[0]);
const size_t EXTENSIONS_LEN = sizeof(EXTENSION)/sizeof(EXTENSION[0]);

static size_t search_write_cb(char *data, size_t size, size_t len, void *user_data)	{
	memory_dyn *mem = (memory_dyn *) user_data;

	if (len > 0 && len <= mem->buf_sz-mem->size)	{
		memcpy(&mem->buffer[mem->size], data, len);
		mem->size += len;
	} 

	return size * len;
}

static size_t write_file(char *data, size_t size, size_t len, void *user_data)	{
	FILE *file = (FILE *) user_data;

	long total_written = 0;

	if (len > 0)	{
		total_written = fwrite(data, size, len, file);
	} 

	return total_written;
}

static bool add_id3v2_tag_details(saavn_song_t *song_details, char const *filename)	{
	ID3v2_tag *tag = load_tag(filename);

	if (tag == NULL)	tag = new_tag();
	tag_set_title(song_details->title, 0, tag);
	tag_set_album(song_details->album, 0, tag);

	set_tag(filename, tag);

	free_tag(tag);

	return true;
}

bool saavn_perform_search(
		char const *song_name, 
		size_t const song_name_len, 
		memory_dyn *mem)	{

	CURL *handle; 
	handle = curl_easy_init();
	bool is_success = false;

	const size_t SEARCH_URL_LEN = SEARCH_API_URL_LEN + song_name_len + 1;
	char *search_url = (char *) malloc(SEARCH_URL_LEN);
	
	if (search_url)	snprintf(search_url, SEARCH_URL_LEN, "%s%s", SEARCH_API_URL, song_name);
	
	if (handle)	{
		curl_easy_setopt(handle, CURLOPT_URL, search_url);
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, search_write_cb);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, mem);

		int res = curl_easy_perform(handle);

		if (res != CURLE_OK)	{
			fprintf(stderr, "request failed: %s\n", curl_easy_strerror(res));
			goto cleanup;
		} 

		is_success = true;

	cleanup:
		curl_easy_cleanup(handle);
	}

	if (search_url)	free(search_url);

	return is_success;
}

bool saavn_get_song_url(char *song_id, size_t song_id_len, memory_dyn *mem)	{
	CURL *handle; 
	handle = curl_easy_init();
	bool is_success = false;

	const size_t SONG_URL_LEN = SONG_API_URL_LEN + song_id_len + 1;
	char *search_url = (char *) malloc(SONG_URL_LEN);
	
	if (search_url)	snprintf(search_url, SONG_URL_LEN, "%s%s", SONG_API_URL, song_id);

	if (handle)	{
		curl_easy_setopt(handle, CURLOPT_URL, search_url);
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, search_write_cb);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, mem);

		int res = curl_easy_perform(handle);

		if (res != CURLE_OK)	{
			fprintf(stderr, "request failed: %s\n", curl_easy_strerror(res));
			goto cleanup;
		}

		is_success = true;

	cleanup:
		curl_easy_cleanup(handle);
	}

	if (search_url)	free(search_url);

	return is_success;
}


bool saavn_song_download(char *appended_url, size_t url_len, saavn_song_t *song_details)	{
	CURL *handle; 
	handle = curl_easy_init();

	bool is_success = false;
	
	// Clear the junk in appended URL
	char dir1[8] = { 0 };
	char dir2[64] = { 0 };

	// Get sub-directory in URL
	int ap_idx = 0;
	for (int j=0, slash_count = 0; ap_idx<url_len && slash_count < 2; ++ap_idx)	{
		if (appended_url[ap_idx] == '/')	++slash_count;
		else if (appended_url[ap_idx] == '\\');
		else	{
			dir1[j++] = appended_url[ap_idx];
		}
	}

	// Get hash of song in URL
	for (int j=0; ap_idx<url_len; ++ap_idx)	{
		if (appended_url[ap_idx] == '_')	{
			dir2[j] = '\0';
			break;
		}
		else if (appended_url[ap_idx] == '\\');
		else	{
			dir2[j] = appended_url[ap_idx];
			++j;
		}
	}

	size_t ext_idx = 0;
	size_t bitrate_idx = 0;
	size_t download_url_idx = 0;

	const size_t SEARCH_URL_BUFFER_LEN = 2048;
	const size_t FILENAME_BUFFER_LEN = 128;

	char * const search_url = (char *) malloc(SEARCH_URL_BUFFER_LEN);
	char * const saved_filename = (char *) malloc(FILENAME_BUFFER_LEN);

	// printf("total urls: %zu, total bitrates: %zu, total ext: %zu\n", DOWNLOAD_URLS_LEN, BITRATES_LEN, EXTENSIONS_LEN);

	while (true)	{
		const char *EXT = EXTENSION[ext_idx];
		const char *BR = BITRATE[bitrate_idx];
		const char *DL_URL = SONG_DOWNLOAD_URL[download_url_idx];

		if (search_url)	snprintf(search_url, SEARCH_URL_BUFFER_LEN-1, "%s%s/%s%s%s", DL_URL, dir1, dir2, BR, EXT);
		if (saved_filename)	snprintf(saved_filename, FILENAME_BUFFER_LEN-1, "%s%s%s", song_details->title, BR, EXT);

		// printf("search url: %s\nfilename: %s\n", search_url, saved_filename);

		FILE *file_ptr = fopen(saved_filename, "wb");

		// set-up cURL
		curl_easy_setopt(handle, CURLOPT_URL, search_url);
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_file);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, file_ptr);

		int res = curl_easy_perform(handle);

		if (res != CURLE_OK)	{
			fprintf(stderr, "request failed: %s\n", curl_easy_strerror(res));
			break;
		}
		
		// file must be generated, check file length to retry if size is small
		fseek(file_ptr, 0, SEEK_END);
		size_t const file_len = ftell(file_ptr);
		fclose(file_ptr);

		if (file_len > 1024)	{
			is_success = true;
			// Add ID3v2 tag details
			if (EXT[3] == '3')	{
				printf("Song type is mp3, adding song metadata...\n");
				add_id3v2_tag_details(song_details, saved_filename);
			} else	{
				printf("Song type is mp4, cant add song metadata.\n");
			}

			break;
		} else	{
			fprintf(stderr, "Couldn't download, retrying...\n");
			// fprintf(stderr, "selected ext: %s, selected bitrate: %s, selected url: %s\n", EXT, BR, DL_URL);
			if (remove(saved_filename) != 0)	{
				fprintf(stderr, "Couldn't remove file: %s, exiting...\n", saved_filename);
				break;
			}
		}

		// failed attempt?
		if (bitrate_idx == BITRATES_LEN-1)	{
			bitrate_idx = 0;
			if (ext_idx == EXTENSIONS_LEN-1)	{
				ext_idx = 0;
				if (download_url_idx == DOWNLOAD_URLS_LEN-1)	{
					download_url_idx = 0;
					break;
				} else	++download_url_idx;
			} else	++ext_idx;
		} else	++bitrate_idx;
	}

	if (search_url)	free(search_url);
	if (saved_filename)	free(saved_filename);

	if (handle)	curl_easy_cleanup(handle);
	return is_success;
}
