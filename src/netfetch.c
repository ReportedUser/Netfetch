#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <logos.h>
#include <limits.h>
#include <cJSON.h>


#define RED "\033[1;31m"
#define RESET "\033[0m"
#define SIZE 30 


#define Type(x) _Generic((x),                   \
                         int: "int",            \
                         short: "short",        \
                         long: "long",          \
                         char: "char",          \
                         float: "float",        \
                         double: "double",      \
                         default: "unknown"     \
                         )


struct ServiceConfig {
	char service[50];
	char link[200];
	char value_1[20];
	char value_2[20];
};


struct MemoryStruct {
	char *memory;
	size_t size;
};


int organize_service_data(char line[256], struct ServiceConfig *current_service) {
	char key[128], value[128];

        if (sscanf(line, "%[^=]=%s", key, value) == 2) {
        if (strcmp(key, "link") == 0) {
                strncpy(current_service->link, value, sizeof(current_service->link)-1);
        } else if (strcmp(key, "value_1") == 0) {
                strncpy(current_service->value_1, value, sizeof(current_service->value_1)-1);
        } else if (strcmp(key, "value_2") == 0) {
                strncpy(current_service->value_2, value, sizeof(current_service->value_2)-1);
        }
	}
	return 0;
};


int parse_config(struct ServiceConfig *current_service, const char *filename) {

	int c, RC;
	char line[256];
	FILE *file = fopen(filename, "r");

	if (!file){
		perror(RED"Error opening config,txt file: make sure it exists.\n"RESET);
		return -1;
	}
	while (fgets(line, sizeof(line), file)){
		if (line[0] == '[') {
			sscanf(line, "[%[^]]", current_service->service);
		}
		RC = organize_service_data(line, current_service);
	}
	fclose(file);
	return 0;
};

size_t WriteMemoryCallback(char *content, size_t size, size_t nmemb, void *userdata) {
	struct MemoryStruct *currentdata = (struct MemoryStruct*) userdata;
	size_t realsize = size * nmemb;

	char *ptr = realloc(currentdata->memory, currentdata->size + realsize + 1);
	if(!ptr){
		/* out of memory! */
		printf("not enough memory (relloc returned NULL)\n");
		return 0;
	}
	currentdata->memory = ptr;
	memcpy(&(currentdata->memory[currentdata->size]), content, realsize);
	currentdata->size += realsize;
	currentdata->memory[currentdata->size] = 0;
	return realsize;
}

int fetch_information(char URL[200], struct MemoryStruct *chunk){
	CURL *curl;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_ALL);
	// struct MemoryStruct chunk;


	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, URL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)chunk);
		// curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
		fprintf(stderr, RED"curl_easy_perform() returned %s\n"RESET, curl_easy_strerror(res));
		return -1;
        } else {
		//printf("This information was retrieved: %s \n", (char *)chunk->memory);
	}
        curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	return 1;
}


cJSON *json_parsing(char *data, int PRINT_FLAG) {
	if (data==NULL) {
		return NULL;
	}
	cJSON *json = cJSON_Parse(data);
	if (json == NULL){
		fprintf(stderr, RED"No json data to parse, returning.\n"RESET);
		return NULL;
	} else if (PRINT_FLAG == 1){ 
		char *json_string = cJSON_Print(json);
		printf("This information was retrieved: %s \n", json_string);
		free(json_string);
	}

	return json;
}


int service_print(struct ServiceConfig *service_to_print, cJSON *json_to_print) {
	
	char value_1_concated[SIZE];
	char value_2_concated[SIZE];
	cJSON *value_1 = cJSON_GetObjectItem(json_to_print, service_to_print->value_1);
	cJSON *value_2 = cJSON_GetObjectItem(json_to_print, service_to_print->value_2);

	snprintf(value_1_concated, SIZE, "%s %d", service_to_print->value_1, value_1->valueint);
	snprintf(value_2_concated, SIZE, "%s %d", service_to_print->value_2, value_2->valueint);

	printf(pihole_logo, service_to_print->service, value_1_concated, value_2_concated, "", "");
	return 0;
}


int main(void) {
	int RC = 0;
	struct ServiceConfig *service = malloc(sizeof(struct ServiceConfig));
	struct MemoryStruct chunk;
	
	chunk.memory = malloc(1);
	chunk.size = 0;

	RC = parse_config(service, "config.txt");
	if (RC == -1) return RC;
	
	RC = fetch_information(service->link, &chunk);
	if (RC == -1) return RC;

	cJSON *parsed_json = json_parsing(chunk.memory, 0);
	if (parsed_json == NULL) return -1;

	service_print(service, parsed_json);

	free(service);
	service= NULL;
	return RC;
}
