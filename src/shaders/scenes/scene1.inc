/**
 *  Displaced boxes and spheres.
 *
 *  250 fps
 */

#ifdef SCENE1

float scene(vec3 p) {
    newState(p);

    // right box
    move(vec3(12,0,0));
    opUnion(displacedRoundedBox(vec3(10), 3));
    apply(RED);

    // left box
    move(vec3(-12,0,0));
    opUnion(displacedRoundedBox(vec3(10), 3));
    apply(GREEN);

    // sphere
    move(vec3(0,25,0));
    opUnion(displacedSphere(15));
    apply(BROWN);

    // right box2
    move(vec3(30,30,0));
    opUnion(displacedBox(vec3(10)));
    apply(BROWN);

    // left box2
    move(vec3(-30,30,0));
    opUnion(displacedBox(vec3(10)));
    apply(BROWN);

    // ground
    move(vec3(0,-11,0));
    opUnion(displacedRoundedBox(vec3(30,1,30), 1));
    apply(YELLOW);

    return state.tmin;
}

#endif