

#ifdef SCENE4b

// ~100 FPS

/**
 *  Type                | Params1                             | Params2
 * ---------------------|-------------------------------------|---------------------
 *  0 - Sphere          | size, ?, ?, ?                       |
 *  1 - Box Cheap       | x,y,z,      ?                       |
 *  2 - Box             | x,y,z,      ?                       |
 *  3 - Rounded Box     | x,y,z,      w=corner radius         |
 *  4 - Capsule         | x=height, y=radius                  |
 *  5 - Cylinder        | x=height, y=radius                  |
 *  6 - Cone            | x=lrg rad, y=sml rad, z=half height |
 *  7 - Triangle Wedge  | x=height, y=length                  |
 *  8 - Ellipse         | xyz                                 |
 *  9 - Ring            | x = sml rad, y = lrg rad            |
 * 10 - Tube            | x=r, y=thickness, z=height          |
 * 11 - Square Ring     | xy                                  |
 * 12 - Hexagon         | x=size, y=depth                     |
 * 13 - Octahedron      | x=size                              |
 *
 */

float scene(vec3 p) {

    uint numObjects = ubo.numObjects;

    newState(p);

    for(int i=0; i<numObjects; i++) {

        SDFObject o = objects[i];

        move(o.move);
        float f = 0;

        switch(o.type) {
            case 0 : {
                f = sphere(o.params1.x);
                break;
            }
            case 1 : {
                f = boxCheap(o.params1.xyz);
                break;
            }
            case 2 : {
                f = box(o.params1.xyz);
                break;
            }
            case 3 : {
                f = roundedBox(o.params1.xyz, o.params1.w);
                break;
            }
            case 4 : {
                f = capsule(o.params1.x, o.params1.y);
                break;
            }
            case 5 : {
                f = cylinder(o.params1.x, o.params1.y);
                break;
            }
            case 6: {
                f = cone(o.params1.x, o.params1.y, o.params1.z);
                break;
            }
            case 7 : {
                f = triangleWedge(o.params1.x, o.params1.y);
                break;
            }
            case 8 : {
                f = ellipse(o.params1.xyz);
                break;
            }
            case 9 : {
                f = ring(o.params1.x, o.params1.y);
                break;
            }
            case 10 : {
                f = tube(o.params1.x, o.params1.y, o.params1.z);
                break;
            }
            case 11 : {
                f = squareRing(o.params1.xy);
                break;
            }
            case 12 : {
                f = hexagon(o.params1.x, o.params1.y);
                break;
            }
            case 13 : {
                f = octahedron(o.params1.x);
                break;
            }
        }

        opUnion(f);
        apply(o.materialIndex);
    }

    return state.tmin;
}

#endif
