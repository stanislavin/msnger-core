//
//  core.h
//  msngerTest
//
//  Created by Stanislav Slavin on 22/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#ifndef core_h
#define core_h

#define ERROR_CODE_SUCCESS                      (0)
#define ERROR_CODE_UNSUCCESSFUL                 (-1)
#define ERROR_CODE_TIMEOUT                      (-5)
#define ERROR_CODE_GIS_FAILURE                  (-6)
#define ERROR_GIS_TIMEOUT                       (-7)
#define ERROR_INFOBIP_TIMEOUT                   (-8)
#define ERROR_INFOBIP_REJECTED_NO_DESTINATION   (-9)
#define ERROR_INFOBIP_PENDING                   (-10)

int sendMessage(const char* message, const char* recipient, double lat, double lon);

#endif /* core_h */