#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <logos.h>
#include <limits.h>
#include <cJSON.h>


#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define RESET "\033[0m"
#define SIZE 30 


struct ServiceConfig {
	char service[50];
	char link[200];
	char value_1[SIZE];
	char value_2[SIZE];
	char value_3[SIZE];
	char value_4[SIZE];
	char value_5[SIZE];
	char value_6[SIZE];
};


struct MemoryStruct {
	char *memory;
	size_t size;
};


typedef struct {
	const char* name;
	const char* logo;
} Logo;


int organize_service_data(char line[256], struct ServiceConfig *current_service) {
	char key[128], value[128];

        if (sscanf(line, "%[^=]=%s", key, value) == 2) {
        if (strcmp(key, "link") == 0) {
                strncpy(current_service->link, value, sizeof(current_service->link)-1);
        } else if (strcmp(key, "value_1") == 0) {
                strncpy(current_service->value_1, value, sizeof(current_service->value_1)-1);
        } else if (strcmp(key, "value_2") == 0) {
                strncpy(current_service->value_2, value, sizeof(current_service->value_2)-1);
        } else if (strcmp(key, "value_3") == 0) {
                strncpy(current_service->value_3, value, sizeof(current_service->value_3)-1);
        } else if (strcmp(key, "value_4") == 0) {
                strncpy(current_service->value_4, value, sizeof(current_service->value_4)-1);
	} else if (strcmp(key, "value_5") == 0) {
                strncpy(current_service->value_5, value, sizeof(current_service->value_5)-1);
	} else if (strcmp(key, "value_6") == 0) {
                strncpy(current_service->value_6, value, sizeof(current_service->value_6)-1);
	}
	}
	return 0;
}


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


const char *search_logo(const char *service_to_match) {
	const char *logo;
	Logo logo_list[] = {
		{"pi-hole", pihole_logo},
	};
	const int num_logos = sizeof(logo_list) / sizeof(logo_list[0]);

	for (int i = 0; i<num_logos; i++) {
		if (strcmp(logo_list[i].name, service_to_match) == 0) {
			return logo_list[i].logo;
		}
	}
	return NULL;
}


int service_print(struct ServiceConfig *service_to_print, cJSON *json_to_print) {

	char concatenated_values[6][SIZE];
	char *values_list[6] = {
		service_to_print->value_1,
		service_to_print->value_2,
		service_to_print->value_3,
		service_to_print->value_4,
		service_to_print->value_5,
		service_to_print->value_6,
	};

	for (int i = 0; i<5; i++) {
		cJSON *value = cJSON_GetObjectItem(json_to_print, values_list[i]);
		if (value != NULL && cJSON_IsString(value)) {
		snprintf(concatenated_values[i], SIZE, "%s: %s", values_list[i], value->valuestring);
		} else if (value != NULL && cJSON_IsNumber(value)) {
		snprintf(concatenated_values[i], SIZE, "%s: %d", values_list[i], value->valueint);
		}
	}
  
	const char *logo = search_logo(service_to_print->service);
	if (logo == NULL) return -1;
	
	printf(logo, service_to_print->service, concatenated_values[0], concatenated_values[1], concatenated_values[2], concatenated_values[3],
	concatenated_values[4], concatenated_values[5]);
	return 0;
}


int main(void) {
	int RC = 0;
	struct ServiceConfig *service = malloc(sizeof(struct ServiceConfig));
	struct MemoryStruct chunk;
	
	chunk.memory = malloc(1);
	chunk.size = 0;

	RC = parse_config(service, "config.txt");
	if (RC == -1) {
		printf(RED"An error ocurred when trying to read the config file. Make sure it exists and it's properly configured.\n"RESET);
		return RC;
	}
	
	RC = fetch_information(service->link, &chunk);
	if (RC == -1) {
		printf(RED"There was an error fetching the information. \n Check your connection and make sure the link is correct.\n"RESET);
		return RC;
	}

	cJSON *parsed_json = json_parsing(chunk.memory, 0);
	if (parsed_json == NULL) {
		printf(RED"There was an error when trying to parse the json.\n"RESET);
		return -1;
	}

	RC = service_print(service, parsed_json);
	if (RC == -1) {
		printf(RED"There was an error when trying to print the information.\n"RESET);
		return RC;
	}

	free(service);
	service= NULL;
	return RC;
}
