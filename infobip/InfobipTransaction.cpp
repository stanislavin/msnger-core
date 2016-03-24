//
//  Infobip.cpp
//  msngerTest
//
//  Created by Stanislav Slavin on 23/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#include <cstdlib>

#include "InfobipTransaction.h"
#include "HttpClient.h"
#include "jsmn.h"
#include "jsmntools.h"
#include "Log.h"
#include "core.h"

using namespace std;

#define HTTP_URL_INFOBIP         "https://api.infobip.com/sms/1/text/single"
#define HTTP_HEADER_INFOBIP_AUTH "Authorization: Basic c3RhbmlzbGF2c2xhdmluOjA3MDI4OQ=="
#define HTTP_HEADER_CONTENT_JSON "Content-Type: application/json"

#define INFOBIP_STATUS_PENDING           (7)
#define INFOBIP_STATUS_NO_DESTINATION    (51)

class InfobipSentMessageResponse
{
public:
    InfobipSentMessageResponse(string json)
    {
        jsmn_parser p;
        jsmntok_t t[128];
        
        jsmn_init(&p);
        
        int r = jsmn_parse(&p, json.c_str(), json.length(), t, sizeof(t)/sizeof(t[0]));
        if (r < 0) return; // failed
        
        for (int i = 0; i < r; i++)
        {
            // do not parse messages array since we expect only one message in response
            if (jsoneq(json.c_str(), &t[i], "to") == 0)
            {
                to = string(json.c_str() + t[i+1].start,t[i+1].end - t[i+1].start);
                i++;
            }
            else if (jsoneq(json.c_str(), &t[i], "id") == 0)
            {
                string sid = string(json.c_str() + t[i+1].start,t[i+1].end - t[i+1].start);
                char* end;
                status.id = (int)strtol(sid.c_str(), &end, 10);
                i++;
            }
            else if (jsoneq(json.c_str(), &t[i], "groupId") == 0)
            {
                string sGroupId = string(json.c_str() + t[i+1].start,t[i+1].end - t[i+1].start);
                char* end;
                status.groupId = (int)strtol(sGroupId.c_str(), &end, 10);
                i++;
            }
            else if (jsoneq(json.c_str(), &t[i], "groupName") == 0)
            {
                status.groupName = string(json.c_str() + t[i+1].start,t[i+1].end - t[i+1].start);
                i++;
            }
            else if (jsoneq(json.c_str(), &t[i], "name") == 0)
            {
                status.name = string(json.c_str() + t[i+1].start,t[i+1].end - t[i+1].start);
                i++;
            }
            else if (jsoneq(json.c_str(), &t[i], "description") == 0)
            {
                status.description = string(json.c_str() + t[i+1].start,t[i+1].end - t[i+1].start);
                i++;
            }
            else if (jsoneq(json.c_str(), &t[i], "smsCount") == 0)
            {
                string sSmsCount = string(json.c_str() + t[i+1].start,t[i+1].end - t[i+1].start);
                char* end;
                smsCount = (int)strtol(sSmsCount.c_str(), &end, 10);
                i++;
            }
            else if (jsoneq(json.c_str(), &t[i], "messageId") == 0)
            {
                messageId = string(json.c_str() + t[i+1].start,t[i+1].end - t[i+1].start);
                i++;
            }
        }
    }
    
    string to;
    struct
    {
        int    id;
        int    groupId;
        string groupName;
        string name;
        string description;
    }      status;
    int    smsCount;
    string messageId;
};

InfobipTransaction::InfobipTransaction()
{
    mHttp = new HttpClient();
}

InfobipTransaction::~InfobipTransaction()
{
    delete mHttp;
}

void InfobipTransaction::sendMessage(const wstring& message, IInfobipCallback* callback)
{
    mCallback = callback;

    // convert wstring to char*
    // tbd: unicode
    size_t len = wcstombs(NULL, message.c_str(), 0);
    char* data = new char[len +1];
    len = wcstombs(data, message.c_str(), len + 1);
    
    const char* headers[] =
    {
        HTTP_HEADER_INFOBIP_AUTH,
        HTTP_HEADER_CONTENT_JSON
    };
    
    LOGI("Infobip", "[HTTP POST] %s", data);
    mHttp->post(HTTP_URL_INFOBIP, data, len, headers, sizeof(headers)/sizeof(headers[0]), this);
}

int infobipStatus2Error(int infobipStatusId)
{
    switch (infobipStatusId)
    {
        case INFOBIP_STATUS_NO_DESTINATION:
        {
            return ERROR_INFOBIP_REJECTED_NO_DESTINATION;
        }
        case INFOBIP_STATUS_PENDING:
        {
            return ERROR_INFOBIP_PENDING;
        }
        default:
        {
            break;
        }
    }
    
    return ERROR_CODE_SUCCESS;
}

void InfobipTransaction::onHTTPResponse(int errorCode, const char* response)
{
    LOGI("Infobip", "[HTTP] %d %s", errorCode, response);
    if (errorCode != ERROR_CODE_SUCCESS)
    {
        if (mCallback != NULL)
        {
            mCallback->onInfobipMessageSent(errorCode);
        }
        mCallback = NULL;
        return;
    }

    InfobipSentMessageResponse iresp(response);
    
    LOGI("Infobip", "[MSG] to:               %s", iresp.to.c_str());
    LOGI("Infobip", "[MSG] status.id:        %d", iresp.status.id);
    LOGI("Infobip", "[MSG] status.groupId:   %d", iresp.status.groupId);
    LOGI("Infobip", "[MSG] status.groupName: %s", iresp.status.groupName.c_str());
    LOGI("Infobip", "[MSG] status.name:      %s", iresp.status.description.c_str());
    LOGI("Infobip", "[MSG] smsCount:         %d", iresp.smsCount);
    LOGI("Infobip", "[MSG] messageId:        %s", iresp.messageId.c_str());
    
    if (mCallback != NULL)
    {
        mCallback->onInfobipMessageSent(infobipStatus2Error(iresp.status.id));
    }
    mCallback = NULL;
}