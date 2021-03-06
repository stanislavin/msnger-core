//
//  HttpClient.cpp
//  msnger-core
//
//  Created by Stanislav Slavin on 22/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#include <cstdlib>

#include "curl/curl.h"

#include "HttpClient.h"
#include "Errors.h"
#include "Log.h"

int HttpClient::curlCode2ErrorCode(int curl_code)
{
    int error = ERROR_CODE_UNSUCCESSFUL;
    switch (curl_code)
    {
        case CURLE_COULDNT_RESOLVE_HOST:
        {
            error = ERROR_HOST_NOT_FOUND;
            break;
        }
    }
    LOGI("HttpClient", "curlCode2ErrorCode: (%d) -> (%d)", curl_code, ERROR_CODE_UNSUCCESSFUL);
    // TODO: handle error code translation
    return ERROR_CODE_UNSUCCESSFUL;
}

void HttpClient::get(const char* url, IHTTPReceiver* receiver)
{
    CURL *curl = curl_easy_init();
    if (curl == NULL)
    {
        if (receiver != NULL)
        {
            receiver->onHTTPResponse(ERROR_CODE_UNSUCCESSFUL, NULL);
        }
        return;
    }

    LOGI("HttpClient", "[HTTP POST] url=%s", url);
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    curl_easy_setopt(curl, CURLOPT_HEADER, false);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, httpCallbackStatic);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, receiver);
    
    CURLcode res = curl_easy_perform(curl);
    
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK)
    {
        if (receiver != NULL)
        {
            receiver->onHTTPResponse(curlCode2ErrorCode(res), NULL);
        }
    }
}
void HttpClient::post(const char* url, const char* data, size_t data_len, const char* headers[], int nheaders, IHTTPReceiver* receiver)
{
    CURL *curl = curl_easy_init();
    if (curl == NULL)
    {
        if (receiver != NULL)
        {
            receiver->onHTTPResponse(ERROR_CODE_UNSUCCESSFUL, NULL);
        }
        return;
    }

    LOGI("HttpClient", "[HTTP GET] url=%s", url);
    LOGI("HttpClient", "[HTTP GET] data=%s", url);
    
    struct curl_slist *curl_headers = NULL;
    for (int i = 0; i < nheaders; i++)
    {
        curl_headers = curl_slist_append(curl_headers, headers[i]);
    }
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_len);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, httpCallbackStatic);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, receiver);
    
    CURLcode res = curl_easy_perform(curl);
    
    // cleanup
    curl_slist_free_all(curl_headers);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK)
    {
        if (receiver != NULL)
        {
            receiver->onHTTPResponse(curlCode2ErrorCode(res), NULL);
        }
    }
}

size_t HttpClient::httpCallbackStatic(void *contents, size_t size, size_t nmemb, void *userp)
{
    IHTTPReceiver* receiver = (IHTTPReceiver*)userp;
    if (receiver == NULL)
    {
        return 0;
    }

    char* data = new char[size*nmemb + 1];
    memset(data, 0, size * nmemb + 1);
    memcpy(data, contents, size * nmemb);
    receiver->onHTTPResponse(ERROR_CODE_SUCCESS, data);
    
    delete [] data;
    
    return size * nmemb;
}

