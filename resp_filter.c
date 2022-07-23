#include "resp_filter.h"

static bool does_key_match(char const *buf, jsmntok_t const *token, char const *key, size_t const key_len)	{
	int const half_len = key_len/2;

	if (token->type != JSMN_OBJECT)	return false;

	for (int i=0; i<half_len; ++i)	{
		if (buf[token->start+i] != key[i] || buf[token->end-1-i] != key[key_len-1-i])	return false;
	}

	return true;
}

saavn_song_arr_t* filter_songs_from_search(memory_dyn *mem)	{
	jsmn_parser parser;
	jsmntok_t tok[TOKEN_COUNT];

	saavn_song_arr_t *songs = NULL;

	jsmn_init(&parser);
	int parsed_count = jsmn_parse(&parser, mem->buffer, mem->size, tok, TOKEN_COUNT);

	bool matches = false;
	size_t tok_idx = 0;

	while (!(matches = does_key_match(mem->buffer, &tok[tok_idx++], "songs", 5)));	

	if (tok[tok_idx-1].size == 1 && tok[tok_idx].type == JSMN_ARRAY)	{
		int const total_songs = tok[tok_idx].size;
		songs = saavn_song_arr_init(total_songs);

		if (songs)	{
			// start from here
		}
	}

	return songs;
}
