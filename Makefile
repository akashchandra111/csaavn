CC := gcc
OPTS := -O1 -g
LIBS := -lcurl
WARN := -Wall -Wundef -Werror

.PHONE : all clean

saavn_cp : main.c *.o
	$(CC) $(OPTS) $^ -o $@ $(LIBS) $(WARN)

%.o : %.c
	$(CC) $(OPTS) -c $^ $(WARN)

clean :
	rm *.o
	rm saavn_cp
