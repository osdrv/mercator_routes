#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Shape2d.h"
#include "PointLoader.h"
#include "cinder/CinderResources.h"
#include "cinder/qtime/MovieWriter.h"
#include "cinder/Utilities.h"
#include "Route.h"
#include "math.h"
#include <list>
#include <map>

#define ROUTES_SAME_TIME 50
#define AIRPORTS_SAME_TIME 250
#define DISPLAY_WIDTH 1400
#define DISPLAY_HEIGHT 800
#define MOVIE_FILENAME "avialines.mov"
#define ROUTES_LIMIT 1000

#define WRITE_MOVIE false
#define DRAW_BACKGROUND false

using namespace ci;
using namespace ci::app;
using namespace std;
using std::list;
using std::map;

class TransportApp : public AppBasic {
    PointLoader pointLoader;
    list<Airport> airports, active_airports;
    list<Route> routes, tmp_routes, active_routes;
    list<Route>::iterator active_route_iterator;
    list<Airport>::iterator active_airport_iterator;
    list< pair<int, int> > routes_map;
    map<int, Airport> airports_map;
    gl::Texture background;
    map<int, list<int> >airport_routes;
    qtime::MovieWriter movie;
  public:
    void prepareSettings( Settings *settings );
    void addActiveRoute( int to, int from );
	void setup();
	void update();
	void draw();
    bool pushRoutes();
    bool pushAirports();
};

void TransportApp::prepareSettings( Settings *settings ) {
    
    settings->setWindowSize( DISPLAY_WIDTH, DISPLAY_HEIGHT );
    settings->setFrameRate( 30.0f );
    
    if ( WRITE_MOVIE ) {
        qtime::MovieWriter::Format format;
        format.setCodec( qtime::MovieWriter::CODEC_JPEG );
        format.setQuality( 0.5f );
        movie = qtime::MovieWriter( getDocumentsDirectory() + MOVIE_FILENAME, DISPLAY_WIDTH, DISPLAY_HEIGHT, format );
    }
}

void TransportApp::setup() {
    airports = pointLoader.loadAirports( "airports.xml" );
    
    map<int, Airport>::iterator ai = airports_map.begin();
    
    for ( list<Airport>::iterator it = airports.begin(); it != airports.end(); ++it ) {
        airports_map.insert(ai, pair<int, Airport>( it->getId(), *it ) );
    }
    
    routes_map = pointLoader.loadRoutes( "routes.xml" );

    map<int, Airport>::iterator from, to;
    map<int, list<int> >::iterator f1, f2;
    
    for ( list< pair<int, int> >::iterator ri = routes_map.begin(); ri != routes_map.end(); ++ri ) {
        from = airports_map.find( ri->first );
        to = airports_map.find( ri->second );
        if ( from == airports_map.end() || to == airports_map.end()) {
            continue;
        }
        routes.push_back( Route( from->second, to->second ) );
        
//        f1 = airport_routes.find( ri->first );
//        if ( f1 == airport_routes.end() ) {
//            list<int> tmp_list;
//            tmp_list.push_back( ri->second );
//            airport_routes.insert( airport_routes.begin(), pair<int, list<int> >( ri->first, tmp_list ) );
//        } else {
//            f1->second.push_back( ri->second );
//        }
//        
//        f2 = airport_routes.find( ri->second );
//        if ( f2 == airport_routes.end() ) {
//            list<int> tmp_list;
//            tmp_list.push_back( ri->first );
//            airport_routes.insert( airport_routes.begin(), pair<int, list<int> >( ri->second, tmp_list ) );
//        } else {
//            f2->second.push_back( ri->first );
//        }
    }
    
    routes.sort( Route::sort );
    routes.resize( ROUTES_LIMIT );
    console() << routes.size() << endl;
    this->active_route_iterator = routes.begin();
    
    this->active_airport_iterator = airports.begin();
    
    background = gl::Texture( loadImage( loadResource( "map.jpg" ) ) );
    
    gl::enableAlphaBlending();
    
//    f1 = airport_routes.begin();
    
//    if ( f1 != airport_routes.end() ) {
//        addActiveRoute( f1->first, -1 );
//    }
}

void TransportApp::update() {
}

void TransportApp::addActiveRoute( int to, int from ) {
    map<int, Airport>::iterator a1, a2;
    map<int, list<int> >::iterator f1 = airport_routes.find( to );
    a1 = airports_map.find( to );
    
    if ( ( a1 != airports_map.end() ) && ( f1 != airport_routes.end() ) ) {
        // тут надо забирать город from из списка городов to
        if ( from != -1 ) {
            f1->second.remove( from );
        }
        for ( list<int>::iterator i = f1->second.begin(); i != f1->second.end(); ++i ) {
            if ( *i == from ) {
                return;
            }
            a2 = airports_map.find( *i );
            if ( a2 != airports_map.end() ) {
                active_routes.push_back( Route( a1->second, a2->second ) );
            }
        }
        
        airport_routes.erase( f1 );
    }
}

bool TransportApp::pushRoutes() {
    if ( this->active_route_iterator == this->routes.end() ) {
        return false;
    }
    
    for ( int i = 0; i < ROUTES_SAME_TIME; ++i ) {
        if ( this->active_route_iterator == this->routes.end() ) {
            return false;
        }
        this->active_routes.push_back( *this->active_route_iterator );
        ++this->active_route_iterator;
    }
    
    return true;
}

bool TransportApp::pushAirports() {
    if ( this->active_airport_iterator == this->airports.end() ) {
        return false;
    }
    
    for ( int i = 0; i < AIRPORTS_SAME_TIME; ++i ) {
        if ( this->active_airport_iterator == this->airports.end() ) {
            return false;
        }
        this->active_airports.push_back( *this->active_airport_iterator );
        ++this->active_airport_iterator;
    }
    
    return true;
}

void TransportApp::draw() {
	
    gl::clear( Color( 0, 0, 0 ) );
    
    gl::draw( background, getWindowBounds() );
    
    bool airports_drawn = true;
    
    for ( list<Airport>::iterator a = active_airports.begin(); a != active_airports.end(); ++a ) {
        a->step();
        a->draw();
        airports_drawn = airports_drawn && a->isComplete();
    }
    
    if ( airports_drawn ) {
        airports_drawn = airports_drawn && !( pushAirports() );
    }
    
    if ( airports_drawn ) {
        bool routes_drawn = true;
        for ( list<Route>::iterator ari = active_routes.begin(); ari != active_routes.end(); ++ari ) {
            ari->step();
            ari->draw();
            routes_drawn = routes_drawn && ari->isComplete();
        }

        if ( routes_drawn ) {
            routes_drawn = routes_drawn && !( pushRoutes() );
        }
        
        if ( WRITE_MOVIE ) {
            if ( routes_drawn ) {
                this->movie.finish();
                quit();
            }
        }
    }

    gl::color( Color( 1.0f, 1.0f, 1.0f ) );
    
    if ( WRITE_MOVIE ) {
        movie.addFrame( copyWindowSurface() );
    }
}

CINDER_APP_BASIC( TransportApp, RendererGl )
