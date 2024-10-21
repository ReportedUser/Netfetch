#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <logos.h>
#include <errno.h>
#include <limits.h>


#define RED "\033[1;31m"
#define RESET "\033[0m"


struct ServiceConfig {
	char service[50];
	char host[50];
	char port[6];
	char key[100];
};

int parse_port(const char *port_value) {
	char *endptr;
	errno = 0;

	long port = strtol(port_value, &endptr, 10);

	if (endptr == port_value || *endptr != '\0') {
		printf("Invalid port: %s\n", port_value);
		return -1;
	}
	if ((errno == ERANGE && (port == LONG_MAX || port == LONG_MIN)) || port < 1 || port > 65535) {
		printf("Port out of range: %ld\n", port);
		return -1;
	}
	return 0;
}


int organize_service_data(char line[256], struct ServiceConfig *current_service) {
	char key[128], value[128];

        if (sscanf(line, "%[^=]=%s", key, value) == 2) {
        if (strcmp(key, "host") == 0) {
                strncpy(current_service->host, value, sizeof(current_service->host)-1);
        } else if (strcmp(key, "port") == 0) {
		char RC = parse_port(value);
		if (RC == 0) {strncpy(current_service->host, value, sizeof(current_service->host)-1);}
		else return -1;
        } else if (strcmp(key, "session_key") == 0) {
                strncpy(current_service->key, value, sizeof(current_service->key));
	}
	}
	return 0;
};


int parse_config(struct ServiceConfig *current_service, const char *filename) {

	int c, RC;
	char line[256];
	FILE *file = fopen(filename, "r");

	if (!file){
		perror(RED"Error opening config file: make sure it exists.\n"RESET);
		return -1;
	}
	while (fgets(line, sizeof(line), file)){
		if (line[0] == '[') {
			sscanf(line, "[%[^]]", current_service->service);
		}
		RC = organize_service_data(line, current_service);
		if (RC == -1) return RC;
	}
	fclose(file);

	return 0;
};


int fetch_information(char URL[200]){
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, URL);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() returned %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return 0;
}


int main(void) {
	int RC;

	struct ServiceConfig *service_to_fetch = malloc(sizeof(struct ServiceConfig));

	if (service_to_fetch == NULL) {
		perror(RED"Falied to allocate memory to server_to_fetch."RESET);
		return -1;
	}
	RC = parse_config(service_to_fetch, "config.txt");
	if (RC == -1) {return -1;}
	printf("%s\n", service_to_fetch->service);
	printf("%s\n", service_to_fetch->host);
	printf("%s\n", service_to_fetch->port);
	printf("%s\n", service_to_fetch->key);

	char buffer[200];
	snprintf(buffer, sizeof(buffer), "http://%s/admin/api.php?sumaryRaw&auth=%s", service_to_fetch->host, service_to_fetch->key);
	printf("Buffer information: ");
	puts(buffer);
	// fetch_information(buffer);

	free(service_to_fetch);
	service_to_fetch = NULL;

	return 0;
};
