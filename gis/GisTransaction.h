//
//  gis.hpp
//  msngerTest
//
//  Created by Stanislav Slavin on 21/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#ifndef GisTransaction_h
#define GisTransaction_h

#include <stdio.h>
#include <string>

#include "IHttpClient.h"

using namespace std;

class GisTransaction: public IHTTPClient::IHTTPReceiver
{
public:
    class IGisCallback
    {
    public:
        virtual void onAddressResolved(int error, const char* address) = 0;
    };
    
    GisTransaction();
    ~GisTransaction();
    
    void resolveAddress(double lat, double lon, IGisCallback* callback);
    
private:
    void onHTTPResponse(int errorCode, const char* response);
    string parseAddressFromHttpResponse(const char* response);
    string parseStatusFromHttpResponse(const char* response);
    
    IGisCallback* mCallback = NULL;
    IHTTPClient*  mHttp = NULL;
    string        mGeocodeResponse = string("");
};

#endif /* GisTransaction_h */
