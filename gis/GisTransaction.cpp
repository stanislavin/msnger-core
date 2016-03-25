//
//  gis.cpp
//  msngerTest
//
//  Created by Stanislav Slavin on 21/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#include <cstdlib>

#include "GisTransaction.h"
#include "HttpClient.h"
#include "core.h"
#include "jsmn.h"
#include "jsmntools.h"
#include "Log.h"

#define HTTP_GOOGLE_AUTH_KEY    "AIzaSyAoXA8z_pbs7nGN2zFoL4lwZoxvGKxUaI8"
#define HTTP_GOOGLE_URL_FORMAT  "https://maps.googleapis.com/maps/api/geocode/json?latlng=%f,%f&result_type=street_address&language=en&key=" HTTP_GOOGLE_AUTH_KEY

GisTransaction::GisTransaction()
{
    mHttp = new HttpClient();
}
GisTransaction::~GisTransaction()
{
    delete mHttp;
}

void GisTransaction::resolveAddress(double lat, double lon, IGisCallback* callback)
{
    mCallback = callback;
    
    size_t max_len = sizeof(HTTP_GOOGLE_URL_FORMAT) + 100 /*have some space for coordinates*/;
    char* url = new char[max_len];
    sprintf(url, HTTP_GOOGLE_URL_FORMAT, lat, lon);
    
    LOGI("Google", "[HTTP GET] %s", url);
    mHttp->get(url, this);
}

void GisTransaction::onHTTPResponse(int errorCode, const char* response)
{
    LOGI("onAddressResolvedGoogle", "[HTTP RSP] %s", response);

    if (errorCode != ERROR_CODE_SUCCESS)
    {
        if (mCallback != NULL)
        {
            mCallback->onAddressResolved(errorCode, NULL);
            mCallback = NULL;
        }
        return;
    }
    
    // response is huge, need to accumulate
    mGeocodeResponse += string(response);
    string address = parseAddressFromHttpResponse(mGeocodeResponse.c_str());
    string status = parseStatusFromHttpResponse(mGeocodeResponse.c_str());
    if (status.compare("ZERO_RESULTS") == 0)
    {
        LOGI("onAddressResolvedGoogle", "[HTTP RSP] STATUS: %s", status.c_str());
        errorCode = ERROR_GIS_RESULTS_EMPTY;
        mGeocodeResponse = "";
    }
    else if (address.compare("") == 0)
    {
        // wait for more data
        return;
    }
    else
    {
        LOGI("onAddressResolvedGoogle", "[HTTP RSP] ADDRESS: %s", address.c_str());
    }
    
    mGeocodeResponse = "";
    // deliver address to listener
    if (mCallback != NULL)
    {
        mCallback->onAddressResolved(errorCode, address.c_str());
        mCallback = NULL;
    }
}

string GisTransaction::parseAddressFromHttpResponse(const char* response)
{
    string json(response);
    string ret("");
    
    jsmn_parser p;
    jsmntok_t t[128];
    
    jsmn_init(&p);
    
    int r = jsmn_parse(&p, json.c_str(), json.length(), t, sizeof(t)/sizeof(t[0]));
    if (r < 0) return ret; // failed
    
    for (int i = 0; i < r; i++)
    {
        // from the whole object we need only formatted address
        if (jsoneq(json.c_str(), &t[i], "formatted_address") == 0)
        {
            ret = string(json.c_str() + t[i+1].start,t[i+1].end - t[i+1].start);
            i++;
        }
    }
    return ret;
}

string GisTransaction::parseStatusFromHttpResponse(const char* response)
{
    string json(response);
    string ret("");
    
    jsmn_parser p;
    jsmntok_t t[128];
    
    jsmn_init(&p);
    
    int r = jsmn_parse(&p, json.c_str(), json.length(), t, sizeof(t)/sizeof(t[0]));
    if (r < 0) return ret; // failed
    
    for (int i = 0; i < r; i++)
    {
        // from the whole object we need only formatted address
        if (jsoneq(json.c_str(), &t[i], "status") == 0)
        {
            ret = string(json.c_str() + t[i+1].start,t[i+1].end - t[i+1].start);
            i++;
        }
    }
    return ret;
}