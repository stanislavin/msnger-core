//
//  message.cpp
//  msngerTest
//
//  Created by Stanislav Slavin on 21/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include "Message.h"

int sendMessage(const char* message, const char* recipient, double lat, double lon)
{
    return false;
}

Message::Message(wchar_t* text, wchar_t* recipient, double lat, double lon):
    mText(text),
    mLat(lat),
    mLon(lon),
    mRecipientNumber(recipient)
{
    
}

Message::~Message()
{
}

wstring Message::toJSON()
{
    // create simple json
    wstring json = L"";
    json += L"{";
    json +=     L"\"from\":\"msnger\",";
    json +=     L"\"to\":\"" + mRecipientNumber + L"\",";
    json +=     L"\"text\":\"" + mText + L"(" + mAddress + L")\"";
    json += L"}";
    
    return json;
}

const wstring& Message::getText()
{
    return mText;
}

void Message::setAddress(const wstring& address)
{
    mAddress = address;
}
const wstring& Message::getAddress()
{
    return mAddress;
}
double Message::getLat()
{
    return mLat;
}
double Message::getLon()
{
    return mLon;
}
const wstring& Message::getRecipient()
{
    return mRecipientNumber;
}