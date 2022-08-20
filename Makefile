CC := cc 
OPTS := -Os -g -fsanitize=address -fsanitize=undefined
LIBS := -lcurl
WARN := -Wall -Wundef -Werror
MAKE := make

.PHONE : all clean

saavn_cp : main.c memory.c saavn_types.c resp_filter.c saavn_api_req.c id3_helper.c
	$(CC) $(OPTS) $^ -o $@ $(LIBDIR) $(LIBS) $(WARN)

clean :
	rm saavn_cp
