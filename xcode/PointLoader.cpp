//
//  PointLoader.cpp
//  Transport
//
//  Created by 4pcbr on 10/27/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "cinder/app/AppBasic.h"
#include "PointLoader.h"
#include "Airport.h"
#include "Route.h"
#include "cinder/Xml.h"
#include "cinder/CinderResources.h"

using namespace ci;
using std::list;

PointLoader::PointLoader() {}


std::list<Airport> PointLoader::loadAirports( const std::string &path ) {
    
    XmlTree doc( app::loadResource( path ) );
    XmlTree points = doc.getChild( "/hash/points" );
    std::list<Airport> collection;
    for ( XmlTree::Iter child = points.begin(); child != points.end(); ++child ) {
        int id = child->getChild( "id" ).getValue<int>();
        float lat = child->getChild( "lat" ).getValue<float>();
        float lon = child->getChild("lon").getValue<float>();
        Airport ap = Airport( id, Vec2f( lat, lon ) );
        collection.push_back( ap );
    }
    return collection;
}

std::list< std::pair<int, int> > PointLoader::loadRoutes( const std::string &path ) {
    
    XmlTree doc( app::loadResource( path ) );
    XmlTree points = doc.getChild( "/hash/routes" );
    std::list< std::pair<int, int> > collection;
    for ( XmlTree::Iter child = points.begin(); child != points.end(); ++child ) {
        collection.push_back( std::pair<int, int>( child->getChild( "from" ).getValue<int>(), child->getChild( "to" ).getValue<int>() ) );
    }
    return collection;
}
