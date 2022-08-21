#include "id3_helper.h"

uint8_t const ID3_VERSION = 0x04;
uint8_t const ID3_REVISION = 0x00;
uint8_t const ID3_FLAG = 0x00;

typedef enum {
	ID3_TITLE,
	ID3_ARTIST,
	ID3_ALBUM,
	ID3_YEAR,
	ID3_LANG,
	ID3_DESC,
} id3_tag;

uint32_t id3_decode_size(uint32_t size)	{
#if LITTLE_ENDIAN
	uint8_t const fourth = (size >> 24) & 0x7F;
	uint8_t const third = (size >> 16) & 0x7F;
	uint8_t const second = (size >> 8) & 0x7F;
	uint8_t const first = (size) & 0x7F;
#elif BIG_ENDIAN
	uint8_t const first = (size >> 24) & 0x7F;
	uint8_t const second = (size >> 16) & 0x7F;
	uint8_t const third = (size >> 8) & 0x7F;
	uint8_t const fourth = (size) & 0x7F;
#else
#warn "Not supported"
#endif

	return (first << 21) | (second << 14) | (third << 7) | fourth; 
}

uint32_t id3_encode_size(uint32_t size)	{
#if LITTLE_ENDIAN
	uint8_t const fourth = (size >> 21) & 0x7F;
	uint8_t const third = (size >> 14) & 0x7F;
	uint8_t const second = (size >> 7) & 0x7F;
	uint8_t const first = (size) & 0x7F;
#elif BIG_ENDIAN
	uint8_t const first = (size >> 21) & 0x7F;
	uint8_t const second = (size >> 14) & 0x7F;
	uint8_t const third = (size >> 7) & 0x7F;
	uint8_t const fourth = (size) & 0x7F;
#else
#warn "Not supported"
#endif

	return (first << 24) | (second << 16) | (third << 8) | fourth;
}

uint32_t id3_dec_ext(uint8_t size[4])	{
	uint32_t ret = 0;
	for (size_t i=0; i<4; ++i)	{
		ret += size[3-i] << (7*i);
	}

	return ret;
}

static void write_frame(uint8_t const frame_data[static 10], uint32_t const frame_len, char const *data, uint32_t const data_len, memory_dyn *mem)	{
	// tag
	memcpy(&mem->buffer[mem->size], frame_data, frame_len);
	mem->size += frame_len;

	// data
	mem->buffer[mem->size] = '\0';
	mem->size += 1;
	memcpy(&mem->buffer[mem->size], data, data_len);
	mem->size += data_len;
}

void write_id3(saavn_song_t const *song, memory_dyn *mem)	{
	size_t const id3_header_len = 10;
	size_t const id3_single_frame_len = 10;
	size_t const total_frames = 6;

	uint8_t id3_header[10] = { 
		'I', 'D', '3', 		// ID3 TAG
		ID3_VERSION, 		// ID3 VER
		ID3_REVISION, 		// ID3 REV
		ID3_FLAG, 			// ID3 FLG
		0, 0, 0, 0 			// ID3 SZ
	};

	uint8_t id3_frames[][10] = {
		[ID3_TITLE] = {
			'T', 'I', 'T', '2',		// TAG NAME
			0, 0, 0, 0,				// TAG SZ
			0, 0,					// TAG FLG
		},
		[ID3_ARTIST] = {
			'T', 'O', 'P', 'E',		// TAG NAME
			0, 0, 0, 0,				// TAG SZ
			0, 0,					// TAG FLG
		},
		[ID3_ALBUM] = {
			'T', 'A', 'L', 'B',		// TAG NAME
			0, 0, 0, 0,				// TAG SZ
			0, 0,					// TAG FLG
		},
		[ID3_YEAR] = {
			'T', 'Y', 'E', 'R',		// TAG NAME
			0, 0, 0, 0,				// TAG SZ
			0, 0,					// TAG FLG
		},
		[ID3_LANG] = {
			'T', 'L', 'A', 'N',		// TAG NAME
			0, 0, 0, 0,				// TAG SZ
			0, 0,					// TAG FLG
		},
		[ID3_DESC] = {
			'T', 'I', 'T', '1',		// TAG NAME
			0, 0, 0, 0,				// TAG SZ
			0, 0,					// TAG FLG
		},
	};

	uint32_t const id3_frame_len[6] = {
		[ID3_TITLE] = strlen(song->title),
		[ID3_ARTIST] = strlen(song->singers),
		[ID3_ALBUM] = strlen(song->album),
		[ID3_YEAR] = strlen(song->year),
		[ID3_LANG] = strlen(song->language),
		[ID3_DESC] = strlen(song->description),
	};

	uint32_t const id3_frame_usync_len[6] = {
		[ID3_TITLE] = id3_encode_size(id3_frame_len[ID3_TITLE] + 1),
		[ID3_ARTIST] = id3_encode_size(id3_frame_len[ID3_ARTIST] + 1),
		[ID3_ALBUM] = id3_encode_size(id3_frame_len[ID3_ALBUM] + 1),
		[ID3_YEAR] = id3_encode_size(id3_frame_len[ID3_YEAR] + 1),
		[ID3_LANG] = id3_encode_size(id3_frame_len[ID3_LANG] + 1),
		[ID3_DESC] = id3_encode_size(id3_frame_len[ID3_DESC] + 1),
	};

	uint32_t total_frame_data_len = 0;
	for (size_t i=0; i<total_frames; ++i)	total_frame_data_len += id3_frame_len[i];

	uint32_t const total_tag_len = id3_header_len + ((id3_single_frame_len + 1) * total_frames) + total_frame_data_len;
	uint32_t const total_tag_usync_len = id3_encode_size(total_tag_len);

	// size_modification
	memcpy(&id3_header[6], &total_tag_usync_len, 4);

	for (size_t i=0; i<total_frames; ++i)	memcpy(&id3_frames[i][4], &id3_frame_usync_len[i], 4);

	// memory copy
	// ID3 header
	memcpy(&mem->buffer[mem->size], id3_header, id3_header_len);
	mem->size += id3_header_len;

	// frame and data
	write_frame(id3_frames[ID3_LANG], id3_single_frame_len, song->language, id3_frame_len[ID3_LANG], mem);		// Language
	write_frame(id3_frames[ID3_DESC], id3_single_frame_len, song->description, id3_frame_len[ID3_DESC], mem);	// Description
	write_frame(id3_frames[ID3_ARTIST], id3_single_frame_len, song->singers, id3_frame_len[ID3_ARTIST], mem);	// Artist
	write_frame(id3_frames[ID3_TITLE], id3_single_frame_len, song->title, id3_frame_len[ID3_TITLE], mem);		// Title
	write_frame(id3_frames[ID3_YEAR], id3_single_frame_len, song->year, id3_frame_len[ID3_YEAR], mem);			// Year
	write_frame(id3_frames[ID3_ALBUM], id3_single_frame_len, song->album, id3_frame_len[ID3_ALBUM], mem); 		// Album
}

void write_id3_image(memory_dyn* mem, memory_dyn const* image_data, char const* mime_type)	{
	uint32_t const image_frame_len = 11;
	uint32_t const mime_type_len = strlen(mime_type);

	uint8_t image_frame[11] = {
		'A', 'P', 'I', 'C',		// TAG NAME
		0, 0, 0, 0,				// TAG SIZE
		0, 0,					// TAG FLAG
		0						// ENC
	};

	uint8_t image_frame_ext[3] = {
		0,						// MIME END
		3,						// PIC TYPE (Cover)
		0,						// DESC
	};

	uint32_t total_frame_len = image_frame_len + mime_type_len + image_data->size;
	uint32_t total_frame_usync_len = id3_encode_size(total_frame_len);

	memcpy(&image_frame[4], &total_frame_usync_len, 4);	// modify total size in frame
	
	memcpy(&mem->buffer[mem->size], image_frame, image_frame_len);
	mem->size += image_frame_len;

	memcpy(&mem->buffer[mem->size], mime_type, mime_type_len);
	mem->size += mime_type_len;

	memcpy(&mem->buffer[mem->size], image_frame_ext, 3);
	mem->size += 3;

	memcpy(&mem->buffer[mem->size], image_data->buffer, image_data->size);
	mem->size += image_data->size;

	// modify new size;
	uint32_t const new_tag_usync_size = id3_encode_size(mem->size);
	
	// size_modification
	memcpy(&mem->buffer[6], &new_tag_usync_size, 4);
}
