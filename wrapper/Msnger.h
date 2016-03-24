//
//  Msnger.hpp
//  msngerTest
//
//  Created by Stanislav Slavin on 23/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#ifndef Msnger_h
#define Msnger_h

#ifdef __cplusplus
extern "C" {
#endif

typedef void* Msnger;
typedef void (*onMessageSentCallback)(int error);

Msnger createMsnger();
void   releaseMsnger(Msnger msnger);
void   sendMessage(Msnger msnger, const char* number, const char* message, double lat, double lon, onMessageSentCallback callback);

#ifdef __cplusplus
}
#endif
    
#endif /* Msnger_h */
