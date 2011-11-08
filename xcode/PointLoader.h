//
//  PointLoader.h
//  Transport
//
//  Created by 4pcbr on 10/27/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef Transport_PointLoader_h
#define Transport_PointLoader_h
#pragma once
#include "Airport.h"
#include "Route.h"
#include <list>

class PointLoader {
public:
    PointLoader();
    std::list<Airport> loadAirports( const std::string &path );
    std::list< std::pair<int, int> > loadRoutes( const std::string &path );
};

#endif
