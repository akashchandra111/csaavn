#ifndef IDE_HELPER_H
#define IDE_HELPER_H

#include <endian.h>
#include <stdint.h>
#include <string.h>

#include "saavn_types.h"
#include "memory.h"

uint32_t id3_decode_size(uint32_t size);
uint32_t id3_encode_size(uint32_t size);
uint32_t id3_dec_ext(uint8_t size[4]);
void write_id3(saavn_song_t const* song, memory_dyn* mem);
void write_id3_image(memory_dyn* mem, memory_dyn const* image_data, char const* mime_type);

#endif
