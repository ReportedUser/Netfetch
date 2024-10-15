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

void fetch_information(struct server_to_read* directions) {
}


int main(void) {
}
