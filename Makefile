CC := clang
OPTS := -Os -s #-fsanitize=address -fsanitize=undefined
LIBS := -lcurl -lid3v2
LIBDIR := -L ./id3v2lib/src
WARN := -Wall -Wundef -Werror
MAKE := make

.PHONE : all clean

saavn_cp : main.c memory.c saavn_types.c resp_filter.c saavn_api_req.c
	cd id3v2lib/src && $(MAKE)
	$(CC) $(OPTS) $^ -o $@ $(LIBDIR) $(LIBS) $(WARN)

clean :
	cd id3v2lib/src && $(MAKE) clean
	rm saavn_cp
