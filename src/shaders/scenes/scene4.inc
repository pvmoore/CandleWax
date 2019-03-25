/**
 *  Test scene featuring lots of different objects.
 *
 *  FPS ~285
 *
 */

#ifdef SCENE4

float scene(vec3 p) {
    newState(p);

    move(vec3(30, 0, 0));
    opUnion(sphere(5));
    apply(BROWN);

    move(vec3(30, 10, 0));
    opUnion(boxCheap(vec3(5,5,5)));
    apply(BROWN);

    move(vec3(30, -10, 0));
    opUnion(roundedBox(vec3(5,5,5), 2));
    apply(BROWN);

    move(vec3(30,20,0));
    opUnion(cylinder(5, 2));
    apply(BROWN);
    move(vec3(30,-20,0));
    opUnion(cylinder(5, 2));
    apply(BROWN);

    move(vec3(40,0,0));
    opUnion(capsule(5, 2));
    apply(BROWN);

    move(vec3(40,10,0));
    opUnion(cone(5, 0.1, 5));
    apply(BROWN);
    move(vec3(40,-10,0));
    opUnion(cone(0.1, 5, 5));
    apply(BROWN);

    move(vec3(40,20,0));
    opUnion(triangleWedge(7, 5));
    apply(BROWN);

    move(vec3(40,-20,0));
    opUnion(ellipse(vec3(5,3,1)));
    apply(BROWN);

    move(vec3(40,30,0));
    opUnion(ring(2,5));
    apply(BROWN);

    move(vec3(40,-30,0));
    opUnion(tube(5,2, 3));
    apply(BROWN);

    move(vec3(40,40,0));
    opUnion(squareRing(vec2(5,2)));
    apply(BROWN);

    move(vec3(40,-40,0));
    opUnion(hexagon(5,2));
    apply(BROWN);

    move(vec3(40,50,0));
    opUnion(octahedron(5));
    apply(BROWN);




    move(vec3(-30, 0, 0));
    opUnion(sphere(5));
    apply(YELLOW);

    move(vec3(-30, 10, 0));
    opUnion(boxCheap(vec3(5,5,5)));
    apply(YELLOW);

    move(vec3(-30, -10, 0));
    opUnion(roundedBox(vec3(5,5,5), 2));
    apply(YELLOW);

    move(vec3(-30,20,0));
    opUnion(cylinder(5, 2));
    apply(YELLOW);
    move(vec3(-30,-20,0));
    opUnion(cylinder(5, 2));
    apply(YELLOW);

    move(vec3(-40,0,0));
    opUnion(capsule(5, 2));
    apply(YELLOW);

    move(vec3(-40,10,0));
    opUnion(cone(5, 0.1, 5));
    apply(YELLOW);
    move(vec3(-40,-10,0));
    opUnion(cone(0.1, 5, 5));
    apply(YELLOW);

    move(vec3(-40,20,0));
    opUnion(triangleWedge(7, 5));
    apply(YELLOW);

    move(vec3(-40,-20,0));
    opUnion(ellipse(vec3(5,3,1)));
    apply(YELLOW);

    move(vec3(-40,30,0));
    opUnion(ring(2,5));
    apply(YELLOW);

    move(vec3(-40,-30,0));
    opUnion(tube(5,2, 3));
    apply(YELLOW);

    move(vec3(-40,40,0));
    opUnion(squareRing(vec2(5,2)));
    apply(YELLOW);

    move(vec3(-40,-40,0));
    opUnion(hexagon(5,2));
    apply(YELLOW);

    move(vec3(-40,50,0));
    opUnion(octahedron(5));
    apply(YELLOW);

    return state.tmin;
}

#endif