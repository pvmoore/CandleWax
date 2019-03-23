
float map(vec3 p) {
    return scene(p);
}

float nlerp(float t, float low, float high) {
    //float maxval = MAX_DISTANCE*MAX_DISTANCE;
    //float v = ((t*t)/maxval) / MAX_DISTANCE;
    //return 0.1 * smoothstep(0, MAX_DISTANCE, (t*t)/1000000);
    //    t = (t)/100000;
    //    float v = clamp((t - low) / (high - low), 0.0001, 1);
    //    return v*v*(3 - 2*v);
    return clamp((10*t) / MAX_DISTANCE, low, high);
}

//bool march(Ray ray, out float time) {
//    const float EPS = 0.001;
//    float t    = 1; // Clear a small space around the camera.
//    int steps  = 0;
//    float dist = EPS;
//    do{
//        dist = map(ray.origin + ray.dir*t);
//        if(dist<=EPS) {
//            time = t + dist;
//            return true;
//        }
//        t += dist*0.75;
//    }while(t<MAX_DISTANCE && ++steps<600);
//    return false;
//}
bool march(Ray ray, out float time) {
    float t   = 1; // Clear a small space around the camera.
    int steps = 0;
    do{
        vec3 p     = ray.origin + ray.dir*t;
        float dist = map(p);
        if(dist <= 0.001) {
            time = t + dist;
            return true;
        }
        float step = nlerp(t, 0.8, 10);
        t += step*dist + 0.0;

    }while(t<MAX_DISTANCE && ++steps<600);
    return false;
}
vec3 calcNormal(vec3 p) {
    vec2 e = vec2(0.01, 0);
    vec3 n = vec3(
        map(p + e.xyy) - map(p - e.xyy),
        map(p + e.yxy) - map(p - e.yxy),
        map(p + e.yyx) - map(p - e.yyx)
    );
    return normalize(n);
}
/**
 *  Return: full shadow [0 .. 1] no shadow.
 */
float softShadow(vec3 ro, vec3 rd, float maxt, float k) {
    float res = 1;
    float MIN = 0.0001;
    float t   = 0.01;
    int steps = 0;
    do{
        vec3 p  = ro + rd*t;
        float h = map(p);
        if(h<=MIN) {
            return 0;
        }
        res  = min(res, k*h/t);
        t   += h;
        // Optimisation.
        MIN += 0.0001;
    }while(t<maxt && ++steps<300);
    return saturate(res);
}
/**
 *  Return: full shadow [0 .. 1] no shadow.
 */
float hardShadow(vec3 ro, vec3 rd, float maxt) {
    float t   = 0.01;
    int steps = 0;
    do{
        vec3 p  = ro + rd*t;
        float dist = map(p);
        if(dist<=0.0001) {
            return 0;
        }
        t += dist;
    }while(t<maxt && ++steps<300);
    return 1;
}
float ao(vec3 pos, vec3 nor, float ra) {
    float occ = 0;
    int iter  = 32;
    for(int i=0; i<iter; i++) {
        float h = 0.01 + 4.0*pow(float(i)/31,2);
        vec2 an = hash2(ra + float(i)*13.1 )*vec2(3.14159, 6.2831);
        vec3 dir = vec3(sin(an.x)*sin(an.y), sin(an.x)*cos(an.y), cos(an.x));
        dir *= (dot(dir,nor) >=0 ? 1 : -1);
        occ += clamp(5.0*map(pos + h*dir).x/h, -1, 1);
    }
    return clamp(occ/iter, 0, 1);
}
// from https://www.shadertoy.com/view/Xds3zN
float calcAO(vec3 pos, vec3 nor ) {
    float occ = 0.0;
    float sca = 1.0;
    for( int i=0; i<5; i++ )
    {
        float hr = 0.01 + 0.12*float(i)/4.0;
        vec3 aopos =  nor * hr + pos;
        float dd = map( aopos ).x;
        occ += -(dd-hr)*sca;
        sca *= 0.95;
    }
    return clamp( 1.0 - 3.0*occ, 0.0, 1.0 );
}
/**
 *  Return: dark [0 .. 1] light.
 */
float aoCheap(vec3 p, vec3 n, float k) {
    float colour = 1;
    for(float i=0; i<5; i++) {
        float f = map(p + n * i * k);
        colour -= (i*k - f) / pow(2,i);
    }
    return colour;
}
