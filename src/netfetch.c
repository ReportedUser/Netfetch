#include <ctype.h>
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
#define BOLD "\033[1m"
#define RESET "\033[0m"
#define SIZE 30 
#define SERVICESQUANTITY 5
#define MAX_URL_LENGTH 200
#define MAX_KEY_AND_VALUE_LENGTH 400

struct ServiceConfig {
	char service[50];
	char link[200];
	char **values;
	char **key_value_pair;
	size_t values_count;
	int available;
};

struct MemoryStruct {
	char *memory;
	size_t size;
	int available;
};

typedef struct {
	const char* name;
	const char* logo;
} Logo;


const char *argp_program_version =
  "Netfetch version 0.4.2";

const char *argp_program_bug_address =
  "<pleasefirsttry@gmail.com>";

static struct argp_option options[] = {
	{"show-all", 'a', "status", OPTION_ARG_OPTIONAL, "Display information for all monitored services. You have the choice to use on or off to display only online or offline services, or leave it empty to display all services."},
	{"display", 'd', "service", 0, "Show detailed information for a specific service."},
	{"list", 'l', 0, 0, "List all services defined in the configuration."},
	{ 0 }
};

static char doc[] =
  "A simple command-line application to display information fetched from selected services. Designed to be easy to use and provide concise, relevant information."; 

struct arguments {
	char *service;
	char *status;
	int showall;
	int list;
};


static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct arguments *arguments = state->input;

	switch (key)
		{
		case 'a':
			arguments->showall = 1;
			arguments->status = arg;
			break;
		case 'l':
			arguments->list= 1;
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


int string_compare(const char *key, const char *key_list[], size_t list_size) {
	for (int i = 0; i < list_size; i++) {
		if (strcmp(key, key_list[i]) == 0) {
			return 1;
		}
	}
	return 0;
}


int initialize_service(struct ServiceConfig **service) {
	*service = malloc(sizeof(struct ServiceConfig));
	if (*service == NULL) {
		fprintf(stderr, "Error: unable to allocate memory\n”");
		return -1;
	}
	(*service)->values = NULL;
	(*service)->values_count = 0;
	return 1;
}

int allocate_service_link(struct ServiceConfig *current_service, char *value) {
	if (strlen(value) > MAX_URL_LENGTH || value == NULL) {
		printf(RED"Error:"RESET" There is an issue with the URL.\n");
		return -1;
	} else
		strncpy(current_service->link, value, sizeof(current_service->link)-1);
	return 0;
}


int allocate_service_key(struct ServiceConfig *current_service, char *value) {
	char **new_values = realloc(current_service->values, (current_service->values_count +1) * sizeof(char *));

	if (new_values == NULL) {
		perror("Failed to allocate memory\n");
		return -1;
	}
	current_service->values = new_values;
	current_service->values[current_service->values_count] = malloc((strlen(value) + 1) * sizeof(char));
	if (current_service->values[current_service->values_count] == NULL) {
		perror("Failed to allocate memory\n");
		return -1;
	}
	strcpy(current_service->values[current_service->values_count], value);
	current_service->values_count++;
	return 0;
}


int read_service_key(const char line[MAX_KEY_AND_VALUE_LENGTH], struct ServiceConfig *current_service) {
	char key[MAX_KEY_AND_VALUE_LENGTH];
	char *value = malloc(MAX_KEY_AND_VALUE_LENGTH);
	int RC = 0;

	if (sscanf(line, "%[^=]=%s", key, value) == 2) {
        if (!strcmp(key, "link")) {
		RC = allocate_service_link(current_service, value);
		if (RC == -1) return -1;
	} else if (!strcmp(key, "value") && current_service->values_count < 17) {
		RC = allocate_service_key(current_service, value);
		if (RC == -1) return RC;
		}
	}
	return RC;
}


int parse_config(struct ServiceConfig *current_service[SERVICESQUANTITY], const char *filename) {

	int RC = 0, i = 0, value_counter = 0;
	char line[MAX_KEY_AND_VALUE_LENGTH];
	FILE *file = fopen(filename, "r");

	if (!file){
		perror(RED"Error opening the configuration file: make sure it exists.\n"RESET);
		return -1;
	}
	while (fgets(line, sizeof(line), file)) {
		if (line[0] == '[') {
			RC = initialize_service(&current_service[i]);
			if (RC == -1) {
				fclose(file);
				return -1;
			}
			sscanf(line, "[%[^]]", current_service[i]->service);
			i ++;
			value_counter = 0;
		} else {
			RC = read_service_key(line, current_service[i-1]);
			if (RC == -1) {
				fclose(file);
				printf(RED"Error:"RESET" The issue is caused by the %s service.\n", current_service[i-1]->service);
				return RC;
			}
		}
	}
	fclose(file);
	return RC;
};


size_t WriteMemoryCallback(char *content, size_t size, size_t nmemb, void *userdata) {
	struct MemoryStruct *currentdata = (struct MemoryStruct*) userdata;
	size_t realsize = size * nmemb;

	char *ptr = realloc(currentdata->memory, currentdata->size + realsize + 1);
	if(!ptr){
		/* out of memory! */
		perror("not enough memory (relloc returned NULL)\n");
		return 0;
	}
	currentdata->memory = ptr;
	memcpy(&(currentdata->memory[currentdata->size]), content, realsize);
	currentdata->size += realsize;
	currentdata->memory[currentdata->size] = 0;
	return realsize;
}


int fetch_information(char *URL, struct MemoryStruct *chunk, int error){
	/*
	Curl to download the json.
	The int error options is added to either end the program
	with the error code or just check if the service is up.
	*/

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
        if(res != CURLE_OK && error == 1) {
		fprintf(stderr, RED"curl_easy_perform() returned %s\n"RESET, curl_easy_strerror(res));
		return -1;
	}
        if(res != CURLE_OK && !error) {
		return -1;
	}
  curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	return 1;
}


cJSON *json_parsing(char *data, int PRINT_FLAG) {
	if (!data) {
		return NULL;
	}
	cJSON *json = cJSON_Parse(data);
	if (!json){
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
	/*
	The logo is searched using this function.
	Make sure the ascii logo is in the include/logos.h file.
	Then add in the logo_list the name defined on the 
	netfetch-services.conf and the logo variable name.
	*/


	Logo logo_list[] = {
		{"pi-hole", pihole_logo},
		{"server-test", test_logo},
	};
	const int num_logos = sizeof(logo_list) / sizeof(logo_list[0]);

	for (int i = 0; i<num_logos; i++) {
		if (!strcmp(logo_list[i].name, service_to_match)) {
			return logo_list[i].logo;
		}
	}
	return NULL;
}

char* replace_char(char* str, char find, char replace) {
	char *current_pos = strchr(str, find);
	while (current_pos) {
		*current_pos = replace;
		current_pos = strchr(current_pos, find);
	}
	return str;
}


int concat_key_value_pair(struct ServiceConfig *Service, cJSON *json_information) {
	/* 
	This function uses the retrieved keys from the configuration file to loook for the value inside the json.
	Once it is found, the key gets the transformed to something more presentable and then concatenated with the value.
	
	Returns 0 on success or -1 on error.
	*/
	int RC = 0;
	char temp_value[256];
	Service->key_value_pair = malloc(Service->values_count * sizeof(char *));
	if (Service->key_value_pair == NULL) {
		perror("Couldn't allocate memory for key_value_pair \n");
		return -1;
	}

	for (int i = 0; i < Service->values_count; i++) {
		
		cJSON *value = cJSON_GetObjectItem(json_information, Service->values[i]);
		if (value == NULL) {
			printf("The key %i does not exist.\n", i);
			return -1;
		}

		snprintf(temp_value, sizeof(temp_value), "%s%s%s",BOLD, Service->values[i], RESET);
		temp_value[4] = toupper(temp_value[4]);
		replace_char(temp_value, '_', ' ');

		size_t value_length = (value != NULL && cJSON_IsString(value)) ? strlen(value->valuestring) : 12;
		size_t allocation_size = strlen(temp_value) + strlen(": ") + value_length + 1;
		Service->key_value_pair[i] = malloc(allocation_size);
		if (Service->key_value_pair[i] == NULL) {
			perror("Couldn't allocate memory for key_value_pair \n");
			return -1;
		}

		if (value != NULL && cJSON_IsString(value)) {
			snprintf(Service->key_value_pair[i], allocation_size, "%s: %s", temp_value, value->valuestring);
		} else if (value != NULL && cJSON_IsNumber(value)) {
			snprintf(Service->key_value_pair[i], allocation_size, "%s: %d", temp_value, value->valueint);
		}
	}

	for (int i = Service->values_count; i <= 17; i++) {
		Service->key_value_pair[i] = malloc(strlen(" "));
		Service->key_value_pair[i] = " ";
	}

	return 0;
}


int service_print(struct ServiceConfig *service_to_print, cJSON *json_to_print) {
	int RC = 0;
	char concatenated_values[6][256];

	RC = concat_key_value_pair(service_to_print, json_to_print);
	if (RC == -1) return -1;

	const char *logo = search_logo(service_to_print->service);
	if (!logo) {perror("Can't find the specified logo. Make sure a logo exists for the choosen service."); return -1;}
	printf(logo, service_to_print->service,
	service_to_print->key_value_pair[0], service_to_print->key_value_pair[1], service_to_print->key_value_pair[2], service_to_print->key_value_pair[3],
	service_to_print->key_value_pair[4], service_to_print->key_value_pair[5], service_to_print->key_value_pair[6], service_to_print->key_value_pair[7],
	service_to_print->key_value_pair[8], service_to_print->key_value_pair[9], service_to_print->key_value_pair[10], service_to_print->key_value_pair[11],
	service_to_print->key_value_pair[11], service_to_print->key_value_pair[12], service_to_print->key_value_pair[13], service_to_print->key_value_pair[14],
	service_to_print->key_value_pair[15], service_to_print->key_value_pair[16], service_to_print->key_value_pair[17]);
	return RC;
}


const char* add_status_color(int status){
	if (status == 1) return "\t\033[32m●\033[0m ";
	else return "\t\033[31m●\033[0m ";
}


int general_print(struct ServiceConfig* ServerList[SERVICESQUANTITY], int *status_code) {
	int RC = 0;
	char concatenated_values[6][50];
	memset(concatenated_values, 0, sizeof(concatenated_values));

	for (int i = 0; i < SERVICESQUANTITY; i++){
		if (!ServerList[i]) {
			break;
		}
		if (*status_code == 1 && ServerList[i]->available == 1) {
			snprintf(concatenated_values[i], 50, "%s %s", add_status_color(ServerList[i]->available), ServerList[i]->service);
		} else if (*status_code == 0 && ServerList[i]->available == 0) {
			snprintf(concatenated_values[i], 50, "%s %s", add_status_color(ServerList[i]->available), ServerList[i]->service);
		} else if (*status_code == 2) {
			snprintf(concatenated_values[i], 50, "%s %s", add_status_color(ServerList[i]->available), ServerList[i]->service);
		}


	}
	printf(show_general, concatenated_values[0], concatenated_values[1], concatenated_values[2], concatenated_values[3], concatenated_values[4]);
	return RC;
}


int check_status(char *status, int *value) {

	if (status == NULL) {
	} else if (!strcmp(status, "on")) {
		*value = 1;
	} else if (!strcmp(status, "off")) {
		*value = 0;
	} else if (status != NULL) {
		perror("You can only choose between on, off or leave it empty to show all.\n");
		return -1;
	}

	return *value;
}

int general_view(struct ServiceConfig* ServiceList[SERVICESQUANTITY], char *status) {
	int RC = 0;
	int value = 2;
	struct MemoryStruct chunk;

	for (int i = 0; i < SERVICESQUANTITY; i++) {
		if (!ServiceList[i]) break;
		chunk.memory = malloc(1);
		chunk.size = 0;
		RC = fetch_information(ServiceList[i]->link, &chunk, 0);
		if (RC == -1) {
			continue;
			ServiceList[i]->available = 0;
		} else {
			ServiceList[i]->available = 1;
		}
	}
	value = check_status(status, &value);

	RC = general_print(ServiceList, &value);
	return RC;
}


int find_service(struct ServiceConfig* ServiceList[SERVICESQUANTITY], struct arguments arguments) {
	int RC = 0;
	struct MemoryStruct chunk;

	chunk.memory = malloc(1);
	chunk.size = 0;

	for (int i = 0; i < SERVICESQUANTITY; i++) {
		if (!ServiceList[i]) break;
		else if (!strcmp(ServiceList[i]->service, arguments.service)) {
			RC = fetch_information(ServiceList[i]->link, &chunk, 1);
			if (RC == -1) {
				perror(RED"Error: Unable to fetch information. Please check your network connection and verify that the link is correct.\n"RESET);
				return RC;
			}
			cJSON *parsed_json = json_parsing(chunk.memory, 0);
			if (!parsed_json) {
				perror(RED"Error: An issue occurred while parsing the JSON data. Please ensure the data format is correct.\n"RESET);
				return -1;
			}
			RC = service_print(ServiceList[i], parsed_json);
			if (RC == -1) {
				perror(RED" Error: An issue occurred while displaying the information.\n"RESET);
				return RC;
			} else
				return RC;
		}
	}
	printf("Couldn't find information for %s.\n", arguments.service);
	return -1;
}


int list_services(struct ServiceConfig* ServiceList[SERVICESQUANTITY]) {

	printf("The current available services to display are the following:\n");
	for (int i = 0; i < SERVICESQUANTITY; i++) {
		if (!ServiceList[i]) break;
		printf("%s \t", ServiceList[i]->service);
	}
	printf("\n");

	return 0;
}


int get_config_path(char *config_path, size_t config_size) {
	const char *home = getenv("HOME");
	const char *path = "/.config/netfetch/netfetch-services.conf";

	if (!home) {
		fprintf(stderr, "Error: Could not find HOME environment.\n");
		return -1;
	}

	if (strlen(home) + strlen(path) + 1 > config_size) {
		fprintf(stderr, "Error: config_size too small.\n");
		return -1;
	}
	strcpy(config_path, home);
	strcat(config_path, path);

	return 0;
}


int main(int argc, char **argv) {
	struct arguments arguments;
	arguments.showall = 0;
	arguments.list = 0;
	arguments.status = NULL;

	argp_parse(&argp, argc, argv, 0, 0, &arguments);
	
	int RC = 0;
	struct ServiceConfig* ServiceArray[SERVICESQUANTITY];

	char config_path[200];
	RC = get_config_path(config_path, sizeof(config_path));
	if (RC == -1) return -1;
	
	RC = parse_config(ServiceArray, config_path);
	if (RC == -1) {
		printf(RED"An error ocurred when trying to read the config file. Make sure it exists and it's properly configured.\n"RESET);
		return RC;
	}
	if (arguments.list == 1){
		RC = list_services(ServiceArray);
	} else if (arguments.service != NULL) {
		RC = find_service(ServiceArray, arguments);
		if (RC == -1) return RC;
	} else if ((arguments.showall == 1) || (arguments.showall == 1 && arguments.status != NULL) || (!arguments.list && !arguments.showall && !arguments.service)) {
		RC = general_view(ServiceArray, arguments.status);
	}
	return RC;
}
