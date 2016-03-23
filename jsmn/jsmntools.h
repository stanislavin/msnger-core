//
//  jsmntools.h
//  msngerTest
//
//  Created by Stanislav Slavin on 23/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#ifndef jsmntools_h
#define jsmntools_h

#include "jsmn.h"

#ifdef __cplusplus
extern "C" {
#endif

int jsoneq(const char *json, jsmntok_t *tok, const char *s);
    
#ifdef __cplusplus
}
#endif

#endif /* jsmntools_h */
