/**
 *  Example scenes
 */

#define SCENE (3)

#if SCENE==0

/**
 *  Empty scene.
 */
float scene(vec3 p) {
    newState(p);
    return MAX_DISTANCE;
}

#elif SCENE==1

float scene(vec3 p) {
    newState(p);

    Material m1 = Material(vec3(1,0,0), 0, 64);
    Material m2 = Material(vec3(0,1,0), 0, 64);
    Material m3 = Material(vec3(0.6,0.6,0.6), 0, 64);
    Material m4 = Material(vec3(0,0,1), 0, 64);

    // right box
    move(vec3(12,0,0));
    opUnion(displacedRoundedBox(vec3(10), 3));
    apply(m1);

    // left box
    move(vec3(-12,0,0));
    opUnion(displacedRoundedBox(vec3(10), 3));
    apply(m2);

    // sphere
    move(vec3(0,25,0));
    opUnion(displacedSphere(15));
    apply(m3);

    // right box2
    move(vec3(30,30,0));
    opUnion(displacedBox(vec3(10)));
    apply(m3);

    // left box2
    move(vec3(-30,30,0));
    opUnion(displacedBox(vec3(10)));
    apply(m3);

    // ground
    move(vec3(0,-11,0));
    opUnion(displacedRoundedBox(vec3(30,1,30), 1));
    apply(m4);

    return state.tmin;
}

#elif SCENE==2

float scene(vec3 p) {
    newState(p);

    Material yellow = Material(vec3(1,1,0)*0.75,    -1, 64);
    Material red    = Material(vec3(1,0,0)*0.75,    -1, 64);
    Material green  = Material(vec3(0,1,0)*0.5,     -1, 64);
    Material purple = Material(vec3(1,0,1)*0.5,     -1, 64);
    Material brown  = Material(vec3(1,0.5,0.1)*0.5, -1, 16);

    // interesting organic ball
    move(240,50,50);
    rotateY(radians(pc.time));
    repeatY(3);
    rotateX(radians(90));
    opUnion(capsule(3,10));
    apply(purple);

    // beach ball shape
    move(240,50,100);
    rotateY(radians(pc.time));
    repeatY(13);
    rotateX(radians(90));
    opUnion(capsule(3,15));
    apply(purple);

    move(240,50,0);
    rotateX(radians(90));
    opUnion(capsule(5,1));
    apply(yellow);

    move(260,50,0);
    rotateX(radians(90));
    opUnion(cylinder(10,2));
    apply(red);

    move(-200,50,-80);
    rotateX(radians(270));
    opUnion(cone(10, 0, 10));
    apply(red);

    move(-200,50,0);
    opUnion(triangleWedge(10, 30));
    apply(red);

    move(-200,50,80);
    rotateX(radians(90));
    twist(mod(pc.time/1000, 0.25));
    opUnion(triangleWedge(10, 30));
    apply(red);

    move(-50,50,-100);
    opUnion(ellipse(vec3(5, 2, 7)));
    apply(red);

    move(50,50,-100);
    opUnion(ring(0.5, 10));
    apply(green);

    move(20,40,-100);
    opUnion(tube(10, 0.5, 10));
    apply(green);

    move(75,50,-100);
    opUnion(squareRing(vec2(10,0.5)));
    apply(green);

    move(-125,50,-100);
    rotateZ(radians(90));
    opUnion(hexagon(10,1));
    apply(green);

    move(125,50,-100);
    rotateZ(radians(90));
    opUnion(octahedron(10));
    apply(green);


    move(400,50,0);
    rotateY(radians(pc.time));
    repeatY(9);
    reflect(vec3(1,0,0), 20);
    mirrorX(15);
    rotateX(radians(90));
    opUnion(capsule(2,10));
    apply(brown);

    // four-leaved clover
    move(0,50,100);
    mirrorX(6);
    mirrorZ(6);
    opUnion(ring(0.5, 10));
    apply(green);

    return state.tmin;
}

#elif SCENE==3

float scene(vec3 p) {
    newState(p);

    Material m0 = Material(vec3(1,0.4,0)*0.75, -1, 64); // brown
    Material m1 = Material(vec3(1,0.8,0)*0.75, -1, 64); // yellow
    Material m2 = Material(vec3(0,0.8,1)*0.75, -1, 64); // blue
    Material m3 = Material(vec3(0.8,1,0)*0.75, -1, 64); // green

    // origin
    move(vec3(0,0,0));
    opUnion(sphere(3));
    apply(m0);

    move(vec3(10,-25,0));
    opUnion(sphere(10));
    apply(m1);

    move(vec3(0,-35,0));
    opUnion(planeY(1));
    apply(m1);

    move(vec3(-20, -25, 0));
    opUnion(roundedBox(vec3(10,10,10), 2));
    apply(m1);

    move(vec3(40,-15,0));
    opUnion(cylinder(20, 10));
    apply(m1);

    move(vec3(-50,-25,0));
    opUnion(boxCheap(vec3(10,10,10)));
    apply(m1);

    move(vec3(0,-25,-60));
    opUnion(boxCheap(vec3(10)));
    apply(m1);

    // Puffy box
    move(vec3(-40,0,-60));
    opUnionSoft(
        boxCheap(vec3(10)),
        sphere(15),
        10
    );
    apply(m1);

    move(vec3(40,0,-60));
    opUnionSoft(
        boxCheap(vec3(10)),
        cylinder(20,2),
        5
    );
    apply(m0);

    // crossbeam
    opUnionSoft(
        box2DCheap(state.p.xz-vec2(100,-150), vec2(10)),
        box2DCheap(state.p.yz-vec2(60,-150), vec2(10)),
        20
    );
    apply(m2);

    // corner
    move(-90, 0, -90);
    opUnion(corner2D(state.p.xz));
    apply(m2);

    // rotating doughnut
    rotateY(radians(pc.time*20));
    move(-70,5,0);
    opUnion(ring(5,10));
    apply(m3);

    // repeatY in x direction 10 times
    move(150,-25,0);
    pMod(vec3(20,0,0), vec3(10,0,0));
    opUnion(sphere(10));
    apply(m1);

    // chocolate ellipse :)
    move(70,-15,0);
    opUnion(ellipse(vec3(10,20,3)));
    apply(m0);

    move(-70,-15,10);
    opUnionChamfer(
        andMove(box(vec3(3)), vec3(3)),
        box(vec3(3)),
        0.6
    );
    apply(m2);

    // sphere with box cut out
    move(-110, -15, 10);
    opSubtraction(
        sphere(13),
        box(vec3(10))
    );
    apply(m0);

    // box with sphere cut out
    move(-150, -15, 10);
    opSubtraction(
        box(vec3(10)),
        sphere(13)
    );
    apply(m0);

    // intersection
    move(-190,-15,10);
    rotateX(radians(45));
    opIntersection(
        box(vec3(10)),
        ring(9,12)
    );
    apply(m0);

    // scaling example (this needs some work)
    float scale = 10;
    move(110,15,10);
    state.p /= scale;
    opUnion(box(vec3(1))*scale);
    apply(m2);

    // Lumpy ball - displacement (artifacts possible)
    move(110,15,40);
    displace(0.5);
    opUnion(sphere(15));
    apply(m2);

    // repeatY in 2 directions (count times)
    move(80, 0, 80);
    pMod(vec3(10,0,10), vec3(4,0,2));
    opUnion(boxCheap(vec3(2)));
    apply(m0);

    // repeatY in 3 directions (count times)
    move(-80, 0, 80);
    pMod(vec3(5,10,5), vec3(4,3,2));
    opUnion(roundedBox(vec3(2), 1));
    apply(m0);

    return state.tmin;
}

#endif