all:
	gcc -o netfetch src/netfetch.c lib/cJSON.c -lcurl -I./include/ 
