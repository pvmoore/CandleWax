/**
 *
 *
 */

#ifdef SCENE2

float scene(vec3 p) {
    newState(p);

    // interesting organic ball
    move(240,50,50);
    rotateY(radians(pc.time));
    repeatY(3);
    rotateX(radians(90));
    opUnion(capsule(3,10));
    apply(PURPLE);

    // beach ball shape
    move(240,50,100);
    rotateY(radians(pc.time));
    repeatY(13);
    rotateX(radians(90));
    opUnion(capsule(3,15));
    apply(PURPLE);

    move(240,50,0);
    rotateX(radians(90));
    opUnion(capsule(5,1));
    apply(YELLOW);

    move(260,50,0);
    rotateX(radians(90));
    opUnion(cylinder(10,2));
    apply(RED);

    move(-200,50,-80);
    rotateX(radians(270));
    opUnion(cone(10, 0, 10));
    apply(RED);

    move(-200,50,0);
    opUnion(triangleWedge(10, 30));
    apply(RED);

    move(-200,50,80);
    rotateX(radians(90));
    twist(mod(pc.time/1000, 0.25));
    opUnion(triangleWedge(10, 30));
    apply(RED);

    move(-50,50,-100);
    opUnion(ellipse(vec3(5, 2, 7)));
    apply(RED);

    move(50,50,-100);
    opUnion(ring(0.5, 10));
    apply(GREEN);

    move(20,40,-100);
    opUnion(tube(10, 0.5, 10));
    apply(GREEN);

    move(75,50,-100);
    opUnion(squareRing(vec2(10,0.5)));
    apply(GREEN);

    move(-125,50,-100);
    rotateZ(radians(90));
    opUnion(hexagon(10,1));
    apply(GREEN);

    move(125,50,-100);
    rotateZ(radians(90));
    opUnion(octahedron(10));
    apply(GREEN);


    move(400,50,0);
    rotateY(radians(pc.time));
    repeatY(9);
    reflect(vec3(1,0,0), 20);
    mirrorX(15);
    rotateX(radians(90));
    opUnion(capsule(2,10));
    apply(BROWN);

    // four-leaved clover
    move(0,50,100);
    mirrorX(6);
    mirrorZ(6);
    opUnion(ring(0.5, 10));
    apply(GREEN);

    return state.tmin;
}

#endif