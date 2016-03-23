//
//  Log.h
//  msngerTest
//
//  Created by Stanislav Slavin on 22/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#ifndef Log_h
#define Log_h

#include <cstdio>

class Log
{
public:
    static void i(const char* tag, const char* format, ...)
    {
        char str[255] = {0};
        sprintf(str, "[%s]:%s\n", tag, format);
        va_list args;
        va_start(args, format);
        vprintf(str, args);
        va_end(args);
    }
};

#endif /* Log_h */
