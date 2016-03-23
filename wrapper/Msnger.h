//
//  Msnger.hpp
//  msngerTest
//
//  Created by Stanislav Slavin on 23/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#ifndef Msnger_h
#define Msnger_h

typedef void* Msnger;
typedef void (*onMessageSentCallback)(int error);

Msnger createMsnger();
void   releaseMsnger(Msnger msnger);
void   sendMessage(Msnger msnger, char* number, char* message, double lat, double lon, onMessageSentCallback callback);

#endif /* Msnger_h */
