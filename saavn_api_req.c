#include "saavn_api_req.h"

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

const char SONG_DOWNLOAD_URL[] = "https://sklktcdnems07.cdnsrv.jio.com/jiosaavn.cdn.jio.com/";
const char SONG_DOWNLOAD_URL_1[] = "https://sklktecdnems03.cdnsrv.jio.com/jiosaavn.cdn.jio.com/";

const size_t SONG_DOWNLOAD_URL_LEN = sizeof(SONG_DOWNLOAD_URL)/sizeof(char);
const size_t SONG_DOWNLOAD_URL_1_LEN = sizeof(SONG_DOWNLOAD_URL_1_LEN)/sizeof(char);

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

bool saavn_perform_search(
		char const *song_name, 
		size_t const song_name_len, 
		memory_dyn *mem)	{

	CURL *handle; 
	curl_global_init(CURL_GLOBAL_ALL);
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
		curl_global_cleanup();
	}

	if (search_url)	free(search_url);

	return is_success;
}

bool saavn_get_song_url(char *song_id, size_t song_id_len, memory_dyn *mem)	{
	CURL *handle; 
	curl_global_init(CURL_GLOBAL_ALL);
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
		curl_global_cleanup();
	}

	if (search_url)	free(search_url);

	return is_success;
}


bool saavn_song_download(char *appended_url, size_t url_len, char *filename, size_t filename_len)	{
	CURL *handle; 
	curl_global_init(CURL_GLOBAL_ALL);
	handle = curl_easy_init();
	bool is_success = false;
	
	// Clear the junk in appended URL
	char dir1[8] = { 0 };
	char dir2[32] = { 0 };
	char ext[5] = { 0 };

	// memset(dir1, 0, 8);
	// memset(dir2, 0, 32);
	// memset(ext, 0, 5);

	char const *bitrate_320 = "_320";
	// char const *bitrate_160 = "_160";
	// char const *bitrate_96 = "_96";
	
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

	// Get extension
	for (int dot_encounter = 0; ap_idx<url_len && dot_encounter < 1; ++ap_idx)	{
		if (appended_url[ap_idx] == '.')	{
			for (int k=0; k<4; ++k)	ext[k] = appended_url[ap_idx++];
			break;
		}
	}

	// const size_t DOWNLOAD_LINK_LEN = SONG_DOWNLOAD_URL_LEN + 1 + strlen(dir1) + 1 + strlen(dir2) + strlen(bitrate_320) + strlen(ext) + 1;
	char *search_url = (char *) malloc(2048);
	memset(search_url, 0, 2048);

	// const size_t SAVED_FILENAME_LEN = filename_len + strlen(bitrate_320) + strlen(ext) + 1;
	char *saved_filename = (char *) malloc(128);
	memset(saved_filename, 0, 128);
	
	if (search_url)	snprintf(search_url, 2047, "%s%s/%s%s%s", SONG_DOWNLOAD_URL, dir1, dir2, bitrate_320, ext);
	if (saved_filename)	snprintf(saved_filename, 127, "%s%s%s", filename, bitrate_320, ext);

	/*
	printf("dir1: %s\n", dir1);
	printf("dir2: %s\n", dir2);
	printf("bitrate: %s\n", bitrate_320);
	printf("ext: %s\n", ext);
	printf("Download URL: %s\n", search_url);
	printf("Saved filename: %s\n", saved_filename);
	*/

	FILE *file_ptr = fopen(saved_filename, "wb");

	if (handle && file_ptr)	{

		curl_easy_setopt(handle, CURLOPT_URL, search_url);
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_file);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, file_ptr);

		int res = curl_easy_perform(handle);

		if (res != CURLE_OK)	{
			fprintf(stderr, "request failed: %s\n", curl_easy_strerror(res));
			goto cleanup;
		}

		is_success = true;

	cleanup:
		fclose(file_ptr);
		curl_easy_cleanup(handle);
		curl_global_cleanup();
	}

	if (search_url)	free(search_url);
	if (saved_filename)	free(saved_filename);

	return is_success;
}
