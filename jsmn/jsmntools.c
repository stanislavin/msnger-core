//
//  jsmntools.c
//  msngerTest
//
//  Created by Stanislav Slavin on 23/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#include <string.h>

#include "jsmntools.h"

int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}