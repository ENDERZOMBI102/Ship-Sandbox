#ifndef _PHYS_H_INCLUDED_
#define _PHYS_H_INCLUDED_

#include <map>
#include <set>
#include <vector>
#include "material.h"
#include "scheduler.h"
#include "vec.h"

namespace phys
{
    class point; class spring; struct ship; class game; struct AABB; struct BVHNode;
    class world
    {
        friend class point;
        friend class spring;
        friend class ship;
        struct springCalculateTask;
        struct pointIntegrateTask;
        scheduler springScheduler;
        std::vector <point*> points;
        std::vector <spring*> springs;
        std::vector <ship*> ships;
        BVHNode *collisionTree;
        double waterheight(double x);
        double oceanfloorheight(double x);
        void doSprings(double dt);
        vec2 gravity;
        void buildBVHTree(bool splitInX, std::vector<point*> &pointlist, BVHNode *thisnode, int depth = 1);
    public:
        double *oceandepthbuffer;
        double buoyancy;
        double strength;
        double waterpressure;
        double waveheight;
        double seadepth;
        bool showstress;
        bool quickwaterfix;
        bool xraymode;
        double time;
        void update(double dt);
        void render(double left, double right, double bottom, double top);
        void renderLand(double left, double right, double bottom, double top);
        void renderWater(double left, double right, double bottom, double top);
        void destroyAt(vec2 pos);
        void drawTo(vec2 target);
        explicit world(vec2 _gravity = vec2(0, -9.8), double _buoyancy = 4, double _strength = 0.01);
        ~world();
    };

    struct world::springCalculateTask: scheduler::task
    {
        springCalculateTask(world *_wld, int _first, int _last);
        world *wld;
        int first, last;
        void process() override;
    };

    struct world::pointIntegrateTask: scheduler::task
    {
        pointIntegrateTask(world *_wld, int _first, int _last, double _dt);
        world *wld;
        double dt;
        int first, last;
        void process() override;
    };


    struct ship
    {
        world *wld;
        struct triangle {
            ship *parent;
            point *a, *b, *c;
            triangle(phys::ship *_parent, point *_a, point *_b, point *_c);
            ~triangle();
            };
        std::set<point*> points;
        std::set<spring*> springs;
        std::map<point*, std::set<point*> > adjacentnodes;
        std::set<triangle*> triangles;
        void render();
        void leakWater(double dt);
        void gravitateWater(double dt);
        void balancePressure(double dt);

        explicit ship(world *_parent);
        ~ship();
        void update(double dt);
    };

    class point
    {
        world *wld;
        friend class spring;
        friend class world;
        friend class ship;
        constexpr static const double radius = 0.4f;
        vec2 pos;
        vec2 lastpos;
        vec2 force;
        double buoyancy;
        double water;
        double getPressure();
    public:
        std::set<ship::triangle*> tris;
        material *mtl;
        bool isLeaking;
        point(world *_parent, vec2 _pos, material *_mtl, double _buoyancy);
        ~point();
        void applyForce(vec2 f);
        void breach();  // set to leaking and remove any incident triangles
        void update(double dt);
        vec2 getPos();
        [[nodiscard]] vec3f getColour(vec3f basecolour) const;
        AABB getAABB();
        void render() const;
    };

    class spring
    {
        friend class world;
        friend class point;
        friend class ship;
        world *wld;
        point *a, *b;
        double length;
        material *mtl;
    public:
        spring(world *_parent, point *_a, point *_b, material *_mtl, double _length = -1);
        ~spring();
        void update();
        void damping(double amount);
        void render(bool isStressed = false);
        bool isStressed();
        bool isBroken();
    };

    struct AABB
    {
        vec2 bottomleft, topright;
        AABB() = default;
        AABB(vec2 _bottomleft, vec2 _topright);
        void extendTo(AABB other);
        void render() const;
    };

    struct BVHNode
    {
        AABB volume;
        BVHNode *l, *r;
        bool isLeaf;
        int pointCount;
        static const int MAX_DEPTH = 15;
        static const int MAX_N_POINTS = 10;
        point* points[MAX_N_POINTS];
        static BVHNode *allocateTree(int depth = MAX_DEPTH);
    };
}


#endif // _PHYS_H_INCLUDED_
