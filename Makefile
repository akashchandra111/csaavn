CC := gcc
OPTS := -O3 -s
LIBS := -lcurl
WARN := -Wall -Wundef -Werror

.PHONE : all clean

saavn_cp : resp_filter.o saavn_api_req.o memory.o saavn_types.o main.c
	$(CC) $(OPTS) $^ -o $@ $(LIBS) $(WARN)

memory.o : memory.c
	$(CC) $(OPTS) $^ -c $@ $(WARN)

saavn_types.o : saavn_types.c
	$(CC) $(OPTS) $^ -c $@ $(WARN)

resp_filter.o : resp_filter.c
	$(CC) $(OPTS) $^ -c $@ $(WARN)

saavn_api_req.o : saavn_api_req.c
	$(CC) $(OPTS) $^ -c $@ $(WARN)

clean :
	rm *.o
	rm saavn_cp
