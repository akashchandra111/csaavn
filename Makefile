CC := gcc
OPTS := -Os -s #-fsanitize=address -fsanitize=undefined
LIBS := -lcurl
WARN := -Wall -Wundef -Werror

.PHONE : all clean

saavn_cp : main.c memory.c saavn_types.c resp_filter.c saavn_api_req.c
	$(CC) $(OPTS) $^ -o $@ $(LIBS) $(WARN)

clean :
	rm saavn_cp
