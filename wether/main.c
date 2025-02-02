#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include <curl/curl.h>

struct MemoryStruct {
    char *memory;
    size_t size;
};


typedef struct weather {
    float feelsLikeC;
    float windspeedKmph;
    char* winddir16Point;
    char** weatherDescs;
} weather;

typedef struct WeatherRep {
    weather *pWeather;
} WeatherRep;

void get_float(float* value, struct json_object* json_obj, const char* name) {
    struct json_object * temp  = NULL;;
    json_object_object_get_ex(json_obj, name, &temp);
    *value = json_object_get_double(temp);
}

void get_string(char** value, struct json_object* json_obj, const char* name) {
    struct json_object* temp  = NULL;
    json_object_object_get_ex(json_obj, name, &temp);
    const char* retval = json_object_to_json_string(temp);
    const unsigned len = strlen(retval) + 1;
    *value = NULL;
    *value = calloc(sizeof(*value), len);
    if (*value == NULL) {
        printf("Unable to allocate memory for string name\n");
    }
    memcpy(*value, retval, sizeof(*retval) * len);
}




int parse_weather_json(char* buffer) {

    WeatherRep wj;
    struct json_object* obj = json_tokener_parse(buffer);
    struct json_object* current = NULL;

    json_object_object_get_ex(obj, "current_condition", &current);
    const int size_weathers = json_object_array_length(current);
    wj.pWeather = NULL;
    wj.pWeather = calloc(sizeof(*wj.pWeather), size_weathers);
    if (wj.pWeather == NULL) {
        printf("Failed to allocate memory for name\n");
        json_object_put(obj);
        return -1;
    }
    for (int i = 0; i < size_weathers; i++) {
        struct json_object* arr_item = json_object_array_get_idx(current, i);
        get_float(&(wj.pWeather[i].feelsLikeC), arr_item, "FeelsLikeC");
        get_string(&(wj.pWeather[i].winddir16Point), arr_item, "winddir16Point");
        get_float(&(wj.pWeather[i].windspeedKmph), arr_item, "windspeedKmph");

        printf("FeelsLikeC: %f\n"
               "Winddir16Point: %s\n"
               "WindspeedKmph: %f\n",
               wj.pWeather[i].feelsLikeC,
               wj.pWeather[i].winddir16Point,
               wj.pWeather[i].windspeedKmph);

        struct json_object* extTokens = NULL;
        json_object_object_get_ex(arr_item, "weatherDesc", &extTokens);
        const int size_descWeathers = json_object_array_length(extTokens);
        wj.pWeather[i].weatherDescs = NULL;
        wj.pWeather[i].weatherDescs = calloc(sizeof(*wj.pWeather[i].weatherDescs), size_weathers);
        for (int j = 0; j < size_descWeathers; j++) {
            struct json_object* itemDesc = json_object_array_get_idx(extTokens, j);
            get_string(&(wj.pWeather[i].weatherDescs[j]), itemDesc, "value");

            printf("weatherDesc%i: %s\n",
                   (j+1),
                   wj.pWeather[i].weatherDescs[j]);

            free(wj.pWeather[i].weatherDescs[j]);
        }
        free(wj.pWeather[i].winddir16Point);
        free(wj.pWeather[i].weatherDescs);
    }
    free(wj.pWeather);
    json_object_put(obj);

    return 0;
}



static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *) userp;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}


char *str_concat(char *str1, char *str2)
{
    char *str3;
    str3 = (char *)malloc((strlen(str1) + strlen(str2) + 1) * sizeof(char));
    strcpy(str3, str1);
    strcat(str3, str2);
    return str3;
}

int main(int argc, char* argv[]) {

char *city ;


            if (argc < 3) {
              printf("missing argument\n");
               printf("you will get data from current location\n");
           city = "";
            }  else
            {
                 city = argv[1];
            }

    CURL *curl_handle;
    CURLcode res;

    struct MemoryStruct chunk;

    char *service_name, *format_type, *url;
    service_name = "http://wttr.in/";
    format_type = "?format=j1";
    url = str_concat(service_name, city);
    url = str_concat(url, format_type);
    printf("url =  %s\n", url);

    chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    } else {
        printf("%lu bytes retrieved\n", (long) chunk.size);
    }
    /* cleanup curl stuff */
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    int value = parse_weather_json(chunk.memory);
    if (value == 0)  {printf(" data is true \n");}
  MC_SET_DEBUG(1);
    return 0;
}
