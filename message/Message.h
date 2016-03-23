//
//  message.hpp
//  msngerTest
//
//  Created by Stanislav Slavin on 21/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#ifndef message_h
#define message_h

#include <stdio.h>
#include <string>

using namespace std;

class Message
{
public:
    Message(const wchar_t* text, const wchar_t* recipient, double lat, double lon);
    ~Message();
    
    wstring toJSON();
    
    const wstring& getText();
    void setAddress(const wstring& address);
    const wstring& getAddress();
    double getLat();
    double getLon();
    const wstring& getRecipient();
    
private:
    wstring  mText;
    wstring  mRecipientNumber;
    double   mLat = 0;
    double   mLon = 0;
    wstring  mAddress;
};

#endif /* message_h */