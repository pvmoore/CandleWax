/**
 *  Main example scene.
 *
 *  65 fps
 */

#ifdef SCENE3

float scene(vec3 p) {
    newState(p);

    // origin
    move(vec3(0,0,0));
    opUnion(sphere(3));
    apply(BROWN);

    move(vec3(10,-25,0));
    opUnion(sphere(10));
    apply(YELLOW);

    move(vec3(0,-35,0));
    opUnion(planeY(1));
    apply(YELLOW);

    move(vec3(-20, -25, 0));
    opUnion(roundedBox(vec3(10,10,10), 2));
    apply(YELLOW);

    move(vec3(40,-15,0));
    opUnion(cylinder(20, 10));
    apply(YELLOW);

    move(vec3(-50,-25,0));
    opUnion(boxCheap(vec3(10,10,10)));
    apply(YELLOW);

    move(vec3(0,-25,-60));
    opUnion(boxCheap(vec3(10)));
    apply(YELLOW);

    // Puffy box
    move(vec3(-40,0,-60));
    opUnionSoft(
        boxCheap(vec3(10)),
        sphere(15),
        10
    );
    apply(YELLOW);

    move(vec3(40,0,-60));
    opUnionSoft(
        boxCheap(vec3(10)),
        cylinder(20,2),
        5
    );
    apply(BROWN);

    // crossbeam
    opUnionSoft(
        box2DCheap(state.p.xz-vec2(100,-150), vec2(10)),
        box2DCheap(state.p.yz-vec2(60,-150), vec2(10)),
        20
    );
    apply(BLUE);

    // corner
    move(-90, 0, -90);
    opUnion(corner2D(state.p.xz));
    apply(BLUE);

    // rotating doughnut
    rotateY(radians(pc.time*20));
    move(-70,5,0);
    opUnion(ring(5,10));
    apply(GREEN);

    // repeatY in x direction 10 times
    move(150,-25,0);
    pMod(vec3(20,0,0), vec3(10,0,0));
    opUnion(sphere(10));
    apply(YELLOW);

    // chocolate ellipse :)
    move(70,-15,0);
    opUnion(ellipse(vec3(10,20,3)));
    apply(BROWN);

    move(-70,-15,10);
    opUnionChamfer(
        andMove(box(vec3(3)), vec3(3)),
        box(vec3(3)),
        0.6
    );
    apply(BLUE);

    // sphere with box cut out
    move(-110, -15, 10);
    opSubtraction(
        sphere(13),
        box(vec3(10))
    );
    apply(BROWN);

    // box with sphere cut out
    move(-150, -15, 10);
    opSubtraction(
        box(vec3(10)),
        sphere(13)
    );
    apply(BROWN);

    // intersection
    move(-190,-15,10);
    rotateX(radians(45));
    opIntersection(
        box(vec3(10)),
        ring(9,12)
    );
    apply(BROWN);

    // scaling example (this needs some work)
    float scale = 10;
    move(110,15,10);
    state.p /= scale;
    opUnion(box(vec3(1))*scale);
    apply(BLUE);

    // Lumpy ball - displacement (artifacts possible)
    move(110,15,40);
    displace(0.5);
    opUnion(sphere(15));
    apply(BLUE);

    // repeatY in 2 directions (count times)
    move(80, 0, 80);
    pMod(vec3(10,0,10), vec3(4,0,2));
    opUnion(boxCheap(vec3(2)));
    apply(BROWN);

    // repeatY in 3 directions (count times)
    move(-80, 0, 80);
    pMod(vec3(5,10,5), vec3(4,3,2));
    opUnion(roundedBox(vec3(2), 1));
    apply(BROWN);

    return state.tmin;
}

#endif