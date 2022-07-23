#include "saavn_api_req.h"

static size_t search_write_cb(char *data, size_t size, size_t len, void *user_data)	{
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
		char const *song_desc, 
		size_t const song_desc_len, 
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
