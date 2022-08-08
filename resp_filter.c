#define JSMN_STATIC

#include "resp_filter.h"
#include "jsmn.h"

#include <stdio.h>
#include <string.h>

static bool does_match(char const *buf, jsmntok_t const *token, jsmntype_t type, char const *key, size_t const key_len)	{
	int const half_len = key_len/2;

	if (token->type != type || (token->end-token->start) != key_len)	return false;

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

	// Find token 'songs'
	while (tok_idx <= parsed_count && !(matches = does_match(mem->buffer, &tok[tok_idx], JSMN_STRING, "songs", 5)))	{
		++tok_idx;
	}

	// then find token 'data'
	while (tok_idx <= parsed_count && !(matches = does_match(mem->buffer, &tok[tok_idx], JSMN_STRING, "data", 4)))	{
	 	++tok_idx;
	}

	// then find array
	tok_idx += 1;

	if (tok[tok_idx-1].size == 1 && tok[tok_idx].type == JSMN_ARRAY)	{
		int const total_songs = 3; // tok[tok_idx].size;
		printf("count: %d\n", total_songs);
		songs = saavn_song_arr_init(total_songs);

		if (songs)	{
			for (int i=0; i<total_songs; ++i)	{
				while (tok_idx <= parsed_count && !(matches = does_match(mem->buffer, &tok[tok_idx], JSMN_STRING, "id", 2))) ++tok_idx;

				/*
					if (tok_idx <= parsed_count)	{
						switch (tok[tok_idx].type)	{
							case JSMN_OBJECT: printf("object -> "); break;
							case JSMN_ARRAY: printf("array -> "); break;
							case JSMN_STRING: printf("string -> "); break;
							case JSMN_PRIMITIVE: printf("prim -> "); break;
							case JSMN_UNDEFINED: printf("prim -> "); break;
						}
				
						for (int j=tok[tok_idx].start; j<tok[tok_idx].end; ++j)	printf("%c", mem->buffer[j]);
						printf("\n");
					}
				*/
					
					tok_idx += 1;	// id
					memcpy(songs->song[i].id, &mem->buffer[tok[tok_idx].start], tok[tok_idx].end-tok[tok_idx].start);

					tok_idx += 2;	// title
					memcpy(songs->song[i].title, &mem->buffer[tok[tok_idx].start], tok[tok_idx].end-tok[tok_idx].start);

					tok_idx += 2;	// image url
					memcpy(songs->song[i].img_url, &mem->buffer[tok[tok_idx].start], tok[tok_idx].end-tok[tok_idx].start);

					tok_idx += 2;	// album
					memcpy(songs->song[i].album, &mem->buffer[tok[tok_idx].start], tok[tok_idx].end-tok[tok_idx].start);

					tok_idx += 6;	// desc
					memcpy(songs->song[i].description, &mem->buffer[tok[tok_idx].start], tok[tok_idx].end-tok[tok_idx].start);
			}
		}
	}

	return songs;
}

void filter_song_url_from_search(size_t id_len, saavn_song_t *song, memory_dyn *mem)	{
	jsmn_parser parser;
	jsmntok_t tok[TOKEN_COUNT];

	jsmn_init(&parser);
	int parsed_count = jsmn_parse(&parser, mem->buffer, mem->size, tok, TOKEN_COUNT);

	bool matches = false;
	size_t tok_idx = 0;
	
	// Find token 'year'
	while (tok_idx < parsed_count && !(matches = does_match(mem->buffer, &tok[tok_idx], JSMN_STRING, "year", 4)))	++tok_idx;

	tok_idx += 1;	// Switched to values
	memcpy(song->year, &mem->buffer[tok[tok_idx].start], tok[tok_idx].end-tok[tok_idx].start);

	tok_idx += 14;	// singers
	memcpy(song->singers, &mem->buffer[tok[tok_idx].start], tok[tok_idx].end-tok[tok_idx].start);

	tok_idx += 10;	// language
	memcpy(song->language, &mem->buffer[tok[tok_idx].start], tok[tok_idx].end-tok[tok_idx].start);

	// Find token 'media_preview_url'
	while (tok_idx < parsed_count && !(matches = does_match(mem->buffer, &tok[tok_idx], JSMN_STRING, "media_preview_url", 17)))	++tok_idx;

	tok_idx += 1;
	size_t split_idx = 0;

	for (size_t i=tok[tok_idx].end, j=0; j < 2 && i >= tok[tok_idx].start; --i)	{
		if (mem->buffer[i] == '/')	{
			split_idx = i;
			++j;
		}
	}

	memcpy(song->url, &mem->buffer[split_idx], tok[tok_idx].end-split_idx);
}
