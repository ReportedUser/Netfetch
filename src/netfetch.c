#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <logos.h>
#include <limits.h>
#include <cJSON.h>
#include <argp.h>


#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define RESET "\033[0m"
#define SIZE 30 
#define SERVICESQUANTITY 5

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


int parse_config(struct ServiceConfig *current_service[SERVICESQUANTITY], const char *filename) {
	struct ServiceConfig *service = malloc(sizeof(struct ServiceConfig));
	int RC, i = 0;
	char line[256];
	current_service[i] = malloc(sizeof(struct ServiceConfig));
	FILE *file = fopen(filename, "r");

	if (!file){
		perror(RED"Error opening config.txt file: make sure it exists.\n"RESET);
		return -1;
	}
	while (fgets(line, sizeof(line), file)){
		if (line[0] == '[') {
			sscanf(line, "[%[^]]", current_service[i]->service);
			i ++;
			current_service[i] = malloc(sizeof(struct ServiceConfig));
		} else {
			RC = organize_service_data(line, current_service[i-1]);
			if (RC == -1) return RC;
		}
	}
	if (feof(file)) current_service[i] = service;
	fclose(file);
	return RC;
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
		{"server-test", test_logo},
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
	if (logo == NULL) {printf("Can't find the specified logo."); return -1;}
	printf(logo, service_to_print->service, concatenated_values[0], concatenated_values[1], concatenated_values[2], concatenated_values[3],
	concatenated_values[4], concatenated_values[5]);
	return 0;
}


const char *argp_program_version =
  "Netfetch version 0.1";

const char *argp_program_bug_address =
  "<pleasefirsttry@gmail.com>";

static struct argp_option options[] = {
	{"show-all", 'a', 0, 0, "Display all the current monitored services."},
	{"display", 'd', "service", 0, "Show more information of a specific service."},
	{ 0 }
};

static char doc[] =
  "This is a simple cli application to display information fetched from your chosen services.";

struct arguments {
	char *service;
	int showall, temp;
};


static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct arguments *arguments = state->input;

	switch (key)
		{
		case 'a':
			arguments->showall = 1;
			break;
		case 'd':
			arguments->service = arg;
			break;
		case ARGP_KEY_ARG:
			if (state->arg_num < 2)	argp_usage(state);
			break;
		default:
			return ARGP_ERR_UNKNOWN;
		}
	return 0;
}


static struct argp argp = { options, parse_opt, 0, doc };


int main(int argc, char **argv) {
	struct arguments arguments;
	arguments.showall = 0;
	arguments.temp = 0;

	argp_parse(&argp, argc, argv, 0, 0, &arguments);
	
	int RC = 0;
	struct ServiceConfig* ServiceArray[SERVICESQUANTITY];
	struct MemoryStruct chunk;

	chunk.memory = malloc(1);
	chunk.size = 0;


	RC = parse_config(ServiceArray, "config.txt");
	if (RC == -1) {
		printf(RED"An error ocurred when trying to read the config file. Make sure it exists and it's properly configured.\n"RESET);
		return RC;
	}

	if (arguments.showall == 1) {
		printf("This implementation is not yet implemented by the implementer. \n");
	} else {
		for (int i = 0; i < SERVICESQUANTITY; i++) {
			if (ServiceArray[i] == NULL) break;
			if (strcmp(ServiceArray[i]->service, arguments.service) == 0) {
				printf("This is the choosen service: %s\n", arguments.service);
				RC = fetch_information(ServiceArray[i]->link, &chunk);
				if (RC == -1) {
					printf(RED"There was an error fetching the information. \n Check your connection and make sure the link is correct.\n"RESET);
					return RC;
				}
				cJSON *parsed_json = json_parsing(chunk.memory, 0);
				if (parsed_json == NULL) {
					printf(RED"There was an error when trying to parse the json.\n"RESET);
					return -1;
				}
				RC = service_print(ServiceArray[i], parsed_json);
				if (RC == -1) {
					printf(RED"There was an error when trying to print the information.\n"RESET);
					return RC;
				}
			}
		}
	}
	return RC;
}
