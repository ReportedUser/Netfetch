#include <curl/easy.h>
#include <stdio.h>
#include <curl/curl.h>
#include <logos.h>

struct service_information {
    char first[256];
    char second[256];
    char third[256];
    char fourth[256];
    char fifth[256];
    char sixth[256];
};

struct server_to_read {
    char ip[16];
    char port[6];

};

intfetch_information(char URL){
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
}
