//
//  gis.cpp
//  msngerTest
//
//  Created by Stanislav Slavin on 21/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#include "Gis.h"

const char* HTTP_HEADER_GOOGLE_AUTH = "";
const char* HTTP_URL_GOOGLE         = "https://maps.googleapis.com/maps/api/geocode/json?latlng=40.714224,-73.961452&key=YOUR_API_KEY";

Gis::Gis(AddressListener& listener):
mListener(listener)
{
    
}

void Gis::resolveAddress(double lat, double lon)
{
    
}