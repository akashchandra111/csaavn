#include "saavn_api_req.h"

const char SEARCH_API_URL[] = "https://www.jiosaavn.com/api.php?__call=autocomplete.get&_format=json&_marker=0&cc=in&includeMetaTags=1&query=";
const char SONG_API_URL[] = "https://www.jiosaavn.com/api.php?__call=song.getDetails&cc=in&_marker=0%3F_marker%3D0&_format=json&pids=";

const size_t SEARCH_API_URL_LEN = sizeof(SEARCH_API_URL);
const size_t SONG_API_URL_LEN = sizeof(SONG_API_URL);

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

static size_t write_buffer_cb(char *data, size_t size, size_t len, void *user_data)	{
	memory_dyn *mem = (memory_dyn *) user_data;

	if (len > 0 && len <= mem->buf_sz-mem->size)	{
		memcpy(&mem->buffer[mem->size], data, len);
		mem->size += len;
	} 

	return size * len;
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
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_buffer_cb);
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
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_buffer_cb);
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

bool saavn_image_art_download(char *image_url, memory_dyn *mem)	{
	bool is_success = false;

	CURL *handle = curl_easy_init();

	if (handle)	{
		curl_easy_setopt(handle, CURLOPT_URL, image_url);
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_buffer_cb);
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

	return is_success;
}

bool saavn_song_download(char *appended_url, size_t url_len, saavn_song_t *song_details)	{
	bool is_success = false;

	CURL *handle = curl_easy_init();
	//if (!handle)	goto cleanup;
	
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

	memory_dyn *song_meta = mem_dyn_init(32 * 1024);			// 32 KB
	memory_dyn *song_buffer = mem_dyn_init(24 * 1024 * 1024);	// 24 MB
	
	if (!search_url || !saved_filename || !song_meta || !song_buffer)	goto cleanup;

	write_id3(song_details, song_meta);

	while (true)	{
		const char *EXT = EXTENSION[ext_idx];
		const char *BR = BITRATE[bitrate_idx];
		const char *DL_URL = SONG_DOWNLOAD_URL[download_url_idx];

		if (search_url)	snprintf(search_url, SEARCH_URL_BUFFER_LEN-1, "%s%s/%s%s%s", DL_URL, dir1, dir2, BR, EXT);
		if (saved_filename)	snprintf(saved_filename, FILENAME_BUFFER_LEN-1, "%s%s%s", song_details->title, BR, EXT);

		// set-up cURL
		curl_easy_setopt(handle, CURLOPT_URL, search_url);
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_buffer_cb);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, song_buffer);

		int res = curl_easy_perform(handle);

		if (res != CURLE_OK)	{
			fprintf(stderr, "request failed: %s\n", curl_easy_strerror(res));
			break;
		}
		
		if (song_buffer->size > 1024)	{
			// remove unwanted data
			uint32_t header_size = *((uint32_t*) &song_buffer->buffer[6]);
			uint32_t music_start_idx = id3_decode_size(header_size);


			FILE *file_ptr = fopen(saved_filename, "wb");
			if (file_ptr)	{
				fwrite(song_meta->buffer, song_meta->size, 1, file_ptr);
				fwrite(&song_buffer->buffer[music_start_idx], song_buffer->size - music_start_idx, 1, file_ptr);
				fclose(file_ptr);
			}

			is_success = true;
			break;
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

		song_buffer->size = 0;
	}

cleanup:
	if (search_url)	free(search_url);
	if (saved_filename)	free(saved_filename);
	if (song_meta)	mem_dyn_free(song_meta);
	if (song_buffer)	mem_dyn_free(song_buffer);

	if (handle)	curl_easy_cleanup(handle);
	return is_success;
}
