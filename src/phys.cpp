#include "glad/glad.h"
#include <algorithm>
#include <iostream>

#include "render.hpp"

#include "phys.h"


// W     W    OOO    RRRR     L        DDDD
// W     W   O   O   R   RR   L        D  DDD
// W     W  O     O  R    RR  L        D    DD
// W     W  O     O  R   RR   L        D     D
// W     W  O     O  RRRR     L        D     D
// W  W  W  O     O  R RR     L        D     D
// W  W  W  O     O  R   R    L        D    DD
//  W W W    O   O   R    R   L        D  DDD
//   W W      OOO    R     R  LLLLLLL  DDDD

int imax( int a, int b ) {
	return a > b ? a : b;
}

void phys::world::update( double dt ) {
	time += dt;
	// Advance simulation for points: (velocity and forces)
	for ( auto& point: points )
		point->update( dt );
	// Iterate the spring relaxation (can tune this parameter, or make it scale automatically depending on free time)
	doSprings( dt );
	// Check if any springs exceed their breaking strain:
	for ( auto spr: springs ) {
		if ( spr->isBroken() )// have to delete after erasure - else there is a possibility of
			delete spr;       // other objects accessing a bad pointer during this cleanup
	}
	// Tell each ship to update all of its water stuff
	for ( auto& ship: ships )
		ship->update( dt );
}

void phys::world::doSprings( double dt ) {
	auto springchunk = springs.size() / springScheduler.getNThreads() + 1;
	for ( int outiter = 0; outiter < 3; outiter++ ) {
		for ( int iteration = 0; iteration < 8; iteration++ ) {
			for ( int i = springs.size() - 1; i > 0; i -= springchunk ) {
				springScheduler.schedule( new springCalculateTask( this, imax( i - springchunk, 0 ), i ) );
			}
			springScheduler.wait();
		}
		double dampingamount = ( 1 - pow( 0.0, dt ) ) * 0.5;
		for ( unsigned int i = 0; i < springs.size(); i++ )
			springs[ i ]->damping( dampingamount );
	}
}

phys::world::springCalculateTask::springCalculateTask( world* _wld, int _first, int _last ) {
	wld = _wld;
	first = _first;
	last = _last;
}
void phys::world::springCalculateTask::process() {
	for ( int i = first; i <= last; i++ )
		wld->springs[ i ]->update();
}

phys::world::pointIntegrateTask::pointIntegrateTask( world* _wld, int _first, int _last, double _dt ) {
	wld = _wld;
	first = _first;
	last = _last;
	dt = _dt;
}
void phys::world::pointIntegrateTask::process() {
	for ( int i = first; i <= last; i++ ) {
		wld->points[ i ]->pos += wld->points[ i ]->force * dt;
		wld->points[ i ]->force = vec2f{ 0, 0 };
	}
}

void phys::world::render( double left, double right, double bottom, double top ) {
	// Draw the ocean floor
	renderLand( left, right, bottom, top );
	if ( quickwaterfix )
		renderWater( left, right, bottom, top );
	// Draw all the points and springs
	for ( unsigned int i = 0; i < points.size(); i++ )
		points[ i ]->render();
	for ( unsigned int i = 0; i < springs.size(); i++ )
		springs[ i ]->render();
	if ( !xraymode )
		for ( unsigned int i = 0; i < ships.size(); i++ )
			ships[ i ]->render();
	if ( showstress )
		for ( unsigned int i = 0; i < springs.size(); i++ )
			if ( springs[ i ]->isStressed() )
				springs[ i ]->render( true );
	if ( !quickwaterfix )
		renderWater( left, right, bottom, top );
	glBegin( GL_LINES );
	glLineWidth( 1.f );
	glEnd();
	//buildBVHTree(true, points, collisionTree);
}

void swapf( double& x, double& y ) {
	double temp = x;
	x = y;
	y = temp;
}

double medianOf3( double a, double b, double c ) {
	if ( a < b )
		swapf( a, b );
	if ( b < c )
		swapf( b, c );
	if ( a < b )
		swapf( a, b );
	return b;
}

void phys::world::buildBVHTree( bool splitInX, std::vector< point* >& pointlist, BVHNode* thisnode, int depth ) {
	int npoints = pointlist.size();
	if ( npoints )
		thisnode->volume = pointlist[ 0 ]->getAABB();
	for ( unsigned int i = 1; i < npoints; i++ )
		thisnode->volume.extendTo( pointlist[ i ]->getAABB() );

	thisnode->volume.render();
	if ( npoints <= BVHNode::MAX_N_POINTS || depth >= BVHNode::MAX_DEPTH ) {
		thisnode->isLeaf = true;
		thisnode->pointCount = npoints;
		for ( int i = 0; i < npoints; i++ )
			thisnode->points[ i ] = pointlist[ i ];
	} else {
		double pivotline = splitInX ? medianOf3( pointlist[ 0 ]->pos.x, pointlist[ npoints / 2 ]->pos.x, pointlist[ npoints - 1 ]->pos.x ) : medianOf3( pointlist[ 0 ]->pos.y, pointlist[ npoints / 2 ]->pos.y, pointlist[ npoints - 1 ]->pos.y );
		std::vector< point* > listL;
		std::vector< point* > listR;
		listL.reserve( npoints / 2 );
		listR.reserve( npoints / 2 );
		for ( int i = 0; i < npoints; i++ ) {
			if ( splitInX ? pointlist[ i ]->pos.x < pivotline : pointlist[ i ]->pos.y < pivotline )
				listL.push_back( pointlist[ i ] );
			else
				listR.push_back( pointlist[ i ] );
		}
		buildBVHTree( !splitInX, listL, thisnode->l, depth + 1 );
		buildBVHTree( !splitInX, listR, thisnode->r, depth + 1 );
	}
}

void phys::world::renderLand( double left, double right, double bottom, double top ) {
	glColor4f( 0.5, 0.5, 0.5, 1 );
	double slicewidth = ( right - left ) / 200.0;
	for ( double slicex = left; slicex < right; slicex += slicewidth ) {
		glBegin( GL_TRIANGLE_STRIP );
		glVertex3f( slicex, oceanfloorheight( slicex ), -1 );
		glVertex3f( slicex + slicewidth, oceanfloorheight( slicex + slicewidth ), -1 );
		glVertex3f( slicex, bottom, -1 );
		glVertex3f( slicex + slicewidth, bottom, -1 );
		glEnd();
	}
}

void phys::world::renderWater( double left, double right, double bottom, double top ) {
	// Cut the water into vertical slices (to get the different heights of waves) and draw it
	glColor4f( 0, 0.25, 1, 0.5 );
	double slicewidth = ( right - left ) / 100.0;
	for ( double slicex = left; slicex < right; slicex += slicewidth ) {
		glBegin( GL_TRIANGLE_STRIP );
		glVertex3f( slicex, waterheight( slicex ), -1 );
		glVertex3f( slicex + slicewidth, waterheight( slicex + slicewidth ), -1 );
		glVertex3f( slicex, bottom, -1 );
		glVertex3f( slicex + slicewidth, bottom, -1 );
		glEnd();
	}
}

double phys::world::oceanfloorheight( double x ) {
	/*x += 1024.f;
    x = x - 2048.f * floorf(x / 2048.f);
    double t = x - floorf(x);
    return oceandepthbuffer[(int)floorf(x)] * (1 - t) + oceandepthbuffer[((int)ceilf(x)) % 2048] * t;*/
	return ( sinf( x * 0.005f ) * 10.f + sinf( x * 0.015f ) * 6.f - sin( x * 0.0011f ) * 45.f ) - seadepth;
}

// Function of time and x (though time is constant during the update step, so no need to parameterise it)
double phys::world::waterheight( double x ) {
	return ( sinf( x * 0.1f + time ) * 0.5f + sinf( x * 0.3f - time * 1.1f ) * 0.3f ) * waveheight;
}

// Destroy all points within a 0.5m radius (could parameterise the radius but...)
void phys::world::destroyAt( vec2f pos ) {
	for ( auto p: points ) {
		if ( ( p->pos - pos ).length() < 0.5f )
			delete p;// have to remove reference before deleting, else other cleanup code will use bad memory!
	}
}

// Attract all points to a single position
void phys::world::drawTo( vec2f target ) {
	for ( std::vector< point* >::iterator iter = points.begin(); iter != points.end(); iter++ ) {
		vec2f& pos = ( *iter )->pos;
		vec2f dir = ( target - pos );
		double magnitude = 50000 / sqrt( 0.1 + dir.length() );
		( *iter )->applyForce( dir.normalise() * magnitude );
	}
}

// Copy parameters and set up initial params:
phys::world::world( vec2f _gravity, double _buoyancy, double _strength ) {
	time = 0;
	gravity = _gravity;
	buoyancy = _buoyancy;
	strength = _strength;
	waterpressure = 0.3;
	waveheight = 1.0;
	seadepth = 150;
	collisionTree = BVHNode::allocateTree();
}

// Destroy everything in the set order
phys::world::~world() {
	// DESTROY THE WORLD??? Y/N
	for ( unsigned int i = 0; i < springs.size(); i++ )
		delete springs[ i ];
	springs.clear();
	for ( unsigned int i = 0; i < points.size(); i++ )
		delete points[ i ];
	for ( unsigned int i = 0; i < ships.size(); i++ )
		delete ships[ i ];
}

// PPPP       OOO    IIIIIII  N     N  TTTTTTT
// P   PP    O   O      I     NN    N     T
// P    PP  O     O     I     N N   N     T
// P   PP   O     O     I     N N   N     T
// PPPP     O     O     I     N  N  N     T
// P        O     O     I     N   N N     T
// P        O     O     I     N   N N     T
// P         O   O      I     N    NN     T
// P          OOO    IIIIIII  N     N     T

// Just copies parameters into relevant fields:
phys::point::point( world* _parent, vec2f _pos, Material* _mtl, double _buoyancy ) {
	wld = _parent;
	wld->points.push_back( this );
	pos = _pos;
	lastpos = pos;
	mtl = _mtl;
	buoyancy = _buoyancy;
	isLeaking = false;
	water = 0;
}

void phys::point::applyForce( vec2f f ) {
	force += f;
}

void phys::point::update( double dt ) {
	double mass = mtl->mass;
	this->applyForce( wld->gravity * ( mass * ( 1 + fmin( water, 1 ) * wld->buoyancy * buoyancy ) ) );// clamp water to 1, so high pressure areas are not heavier.
	// Buoyancy:
	if ( pos.y < wld->waterheight( pos.x ) )
		this->applyForce( wld->gravity * ( -wld->buoyancy * buoyancy * mass ) );
	vec2f newlastpos = pos;
	// Water drag:
	if ( pos.y < wld->waterheight( pos.x ) )
		lastpos += ( pos - lastpos ) * ( 1 - pow( 0.6, dt ) );
	// Apply verlet integration:
	pos += ( pos - lastpos ) + force * ( dt * dt / mass );
	// Collision with seafloor:
	double floorheight = wld->oceanfloorheight( pos.x );
	if ( pos.y < floorheight ) {
		// -1 / derivative => perpendicular to surface!
		vec2f dir = vec2f{ static_cast<float>( floorheight - wld->oceanfloorheight( pos.x + 0.01f ) ), 0.01f }.normalise();
		pos += dir * static_cast<float>( floorheight - pos.y );
	}
	lastpos = newlastpos;
	force = vec2f{ 0, 0 };
}

vec2f phys::point::getPos() {
	return pos;
}

vec3f phys::point::getColor( vec3f basecolor ) const {
	double wetness = fmin( water, 1 ) * 0.7;
	return basecolor * static_cast<float>( 1 - wetness ) + vec3f( 0, 0, 0.8 ) * static_cast<float>( wetness );
}

void phys::point::breach() {
	isLeaking = true;
	for ( auto t: tris )
		delete t;
}

void phys::point::render() const {
	// Put a blue blob on leaking nodes (was more for debug purposes, but looks better IMO)
	if ( isLeaking ) {
		glColor3f( 0, 0, 1 );
		glBegin( GL_POINTS );
		glVertex3f( pos.x, pos.y, -1 );
		glEnd();
	}
}

double phys::point::getPressure() {
	return wld->gravity.length() * fmax( -pos.y, 0 ) * 0.1;// 0.1 = scaling constant, represents 1/ship width
}

phys::AABB phys::point::getAABB() {
	return { pos - vec2f{ radius, radius }, pos + vec2f{ radius, radius } };
}

phys::point::~point() {
	// get rid of any attached triangles:
	breach();
	// remove any springs attached to this point:
	for ( auto spr: wld->springs ) {
		if ( spr->a == this || spr->b == this )
			delete spr;
	}
	// remove any references:
	for ( auto& ship: wld->ships )
		ship->points.erase( this );
	auto iter = std::find( wld->points.begin(), wld->points.end(), this );
	if ( iter != wld->points.end() )
		wld->points.erase( iter );
}

//   SSS    PPPP     RRRR     IIIIIII  N     N    GGGGG
// SS   SS  P   PP   R   RR      I     NN    N   GG
// S        P    PP  R    RR     I     N N   N  GG
// SS       P   PP   R   RR      I     N N   N  G
//   SSS    PPPP     RRRR        I     N  N  N  G
//      SS  P        R RR        I     N   N N  G  GGGG
//       S  P        R   R       I     N   N N  GG    G
// SS   SS  P        R    R      I     N    NN   GG  GG
//   SSS    P        R     R  IIIIIII  N     N    GGGG

phys::spring::spring( world* _parent, point* _a, point* _b, Material* _mtl, double _length ) {
	wld = _parent;
	_parent->springs.push_back( this );
	a = _a;
	b = _b;
	if ( _length == -1 )
		length = ( a->pos - b->pos ).length();
	else
		length = _length;
	mtl = _mtl;
}

phys::spring::~spring() {
	// Used to do more complicated checks, but easier (and better) to make everything leak when it breaks
	a->breach();
	b->breach();
	// Scour out any references to this spring
	for ( auto shp: wld->ships ) {
		if ( shp->adjacentnodes.find( a ) != shp->adjacentnodes.end() )
			shp->adjacentnodes[ a ].erase( b );
		if ( shp->adjacentnodes.find( b ) != shp->adjacentnodes.end() )
			shp->adjacentnodes[ b ].erase( a );
	}
	auto iter = std::find( wld->springs.begin(), wld->springs.end(), this );
	if ( iter != wld->springs.end() )
		wld->springs.erase( iter );
}

void phys::spring::update() {
	// Try to space the two points by the equilibrium length
	// (need to iterate to actually achieve this for all points, but it's FAAAAST for each step)
	vec2f correction_dir = ( b->pos - a->pos );
	double currentlength = correction_dir.length();
	// * 0.8 => 25% overcorrection (stiffer, converges faster)
	correction_dir *= static_cast<float>( ( length - currentlength ) / ( length * ( a->mtl->mass + b->mtl->mass ) * 0.85 ) );
	// if b is heavier, a moves more.
	a->pos -= correction_dir * b->mtl->mass;
	// (and vice versa...)
	b->pos += correction_dir * a->mtl->mass;
}

void phys::spring::damping( double amount ) {
	vec2f springdir = ( a->pos - b->pos ).normalise();
	// relative velocity � spring direction = projected velocity, amount = amount of projected velocity that remains after damping
	springdir *= static_cast<float>( ( a->pos - a->lastpos - ( b->pos - b->lastpos ) ).dot( springdir ) * amount );
	a->lastpos += springdir;
	b->lastpos -= springdir;
}

void phys::spring::render( bool showStress ) {
	// If a member is heavily stressed, highlight it in red (ignored if the world's showstress field is false)
	glBegin( GL_LINES );
	if ( showStress )
		glColor3f( 1, 0, 0 );
	else
		render::setColour( a->getColor( mtl->color ) );
	glVertex3f( a->pos.x, a->pos.y, -1 );
	if ( !showStress )
		render::setColour( b->getColor( mtl->color ) );
	glVertex3f( b->pos.x, b->pos.y, -1 );
	glEnd();
}

bool phys::spring::isStressed() {
	// Check whether strain is more than the word's base strength * this object's relative strength
	return ( a->pos - b->pos ).length() / this->length > 1 + ( wld->strength * mtl->strength ) * 0.25;
}

bool phys::spring::isBroken() {
	// Check whether strain is more than the word's base strength * this object's relative strength
	return ( a->pos - b->pos ).length() / this->length > 1 + ( wld->strength * mtl->strength );
}


//   SSS    H     H  IIIIIII  PPPP
// SS   SS  H     H     I     P   PP
// S        H     H     I     P    PP
// SS       H     H     I     P   PP
//   SSS    HHHHHHH     I     PPPP
//      SS  H     H     I     P
//       S  H     H     I     P
// SS   SS  H     H     I     P
//   SSS    H     H  IIIIIII  P

phys::ship::ship( world* _parent ) {
	wld = _parent;
	wld->ships.push_back( this );
}

void phys::ship::update( double dt ) {
	leakWater( dt );
	for ( int i = 0; i < 4; i++ ) {
		gravitateWater( dt );
		balancePressure( dt );
	}
	for ( int i = 0; i < 4; i++ )
		balancePressure( dt );
}

void phys::ship::leakWater( double dt ) {
	// Stuff some water into all the leaking nodes, if they're not under too much pressure
	for ( auto p: points ) {
		double pressure = p->getPressure();
		if ( p->isLeaking && p->pos.y < wld->waterheight( p->pos.x ) && p->water < 1.5 ) {
			p->water += dt * wld->waterpressure * ( pressure - p->water );
		}
	}
}

void phys::ship::gravitateWater( double dt ) {
	// Water flows into adjacent nodes in a quantity proportional to the cos of angle the beam makes
	// against gravity (parallel with gravity => 1 (full flow), perpendicular = 0)
	for ( auto [ a, second ]: adjacentnodes ) {
		for ( auto b: second ) {
			double cos_theta = ( b->pos - a->pos ).normalise().dot( wld->gravity.normalise() );
			if ( cos_theta > 0 ) {
				double correction = std::min( 0.5 * cos_theta * dt, a->water );// The 0.5 can be tuned, it's just to stop all the water being stuffed into the first node...
				a->water -= correction;
				b->water += correction;
			}
		}
	}
}

void phys::ship::balancePressure( double dt ) {
	// If there's too much water in this node, try and push it into the others
	// (This needs to iterate over multiple frames for pressure waves to spread through water)
	for ( auto [ a, second ]: adjacentnodes ) {
		if ( a->water < 1 )// if water content is not above the threshold, no need to force water out
			continue;
		for ( auto b: second ) {
			double correction = ( b->water - a->water ) * 8 * dt;// can tune this number; value of 1 means will equalise in 1 second.
			a->water += correction;
			b->water -= correction;
		}
	}
}

void phys::ship::render() {
	for ( auto t: this->triangles )
		render::triangle(
			t->a->pos, t->b->pos, t->c->pos,
			t->a->getColor( t->a->mtl->color ),
			t->b->getColor( t->b->mtl->color ),
			t->c->getColor( t->c->mtl->color )
		);
}

phys::ship::~ship() {
	for ( const auto& item: this->triangles )
		delete item;
	triangles.clear();
}

phys::ship::triangle::triangle( phys::ship* _parent, point* _a, point* _b, point* _c ) : parent( _parent ), a(_a), b(_b), c(_c) {
	this->a->tris.insert( this );
	this->b->tris.insert( this );
	this->c->tris.insert( this );
}

phys::ship::triangle::~triangle() {
	this->parent->triangles.erase( this );
	this->a->tris.erase( this );
	this->b->tris.erase( this );
	this->c->tris.erase( this );
}

phys::AABB::AABB( vec2f _bottomleft, vec2f _topright ) : bottomleft( _bottomleft ), topright( _topright ) { }

void phys::AABB::extendTo( phys::AABB other ) {
	if ( other.bottomleft.x < bottomleft.x )
		bottomleft.x = other.bottomleft.x;
	if ( other.bottomleft.y < bottomleft.y )
		bottomleft.y = other.bottomleft.y;
	if ( other.topright.x > topright.x )
		topright.x = other.topright.x;
	if ( other.topright.y > topright.y )
		topright.y = other.topright.y;
}

void phys::AABB::render() const {
	render::box( bottomleft, topright );
}

phys::BVHNode* phys::BVHNode::allocateTree( int depth ) { // NOLINT(misc-no-recursion)
	if ( depth <= 0 )
		return nullptr;
	auto* thisnode = new BVHNode;
	thisnode->l = allocateTree( depth - 1 );
	thisnode->r = allocateTree( depth - 1 );
	return thisnode;
}
