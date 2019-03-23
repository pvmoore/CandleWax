/**
 *  State change and volume manipulation functions.
 *
 */

State newState(vec3 p) {
    state.p      = p;
    state.pStart = p;
    state.tmin   = FLT_MAX;
    state.pTmin  = FLT_MAX;
    state.material = Material(vec3(1), -1, 0);
    return state;
}
void reset() {
    state.p = state.pStart;
}
void apply(Material m) {
    if(state.tmin < state.pTmin) {
        state.material = m;
    }
    state.p     = state.pStart;
    state.pTmin = state.tmin;
}
void paint(Material m) {
    if(state.tmin < state.pTmin) {
        state.material = m;
    }
    state.pTmin = state.tmin;
}
void paint(Material m, vec3 c1, vec3 c2, float a, float b, float c) {
    if(state.tmin < state.pTmin) {
        state.material = m;
        float f = clamp(abs(c-a) / abs(b-a), 0,1);
        state.material.colour = mix(c1,c2,f);
    }
    state.pTmin = state.tmin;
}
void move(float x, float y, float z) {
    state.p -= vec3(x,y,z);
}
void move(vec3 by) {
    state.p -= by;
}
float andMove(float res, vec3 by) {
    state.p -= by;
    return res;
}
void rotateX(float a) {
    vec3 p = state.p;
    state.p.yz = cos(a)*p.yz + sin(a)*vec2(p.z, -p.y);
}
void rotateY(float a) {
    vec3 p = state.p;
    state.p.xz = cos(a)*p.xz + sin(a)*vec2(p.z, -p.x);
}
void rotateZ(float a) {
    vec3 p = state.p;
    state.p.xy = cos(a)*p.xy + sin(a)*vec2(p.y, -p.x);
}
vec3 pMod(vec3 size, vec3 count) {
    vec3 halfsize = size*0.5;
    vec3 c        = floor((state.p + halfsize)/size);
    state.p       = mod(state.p+halfsize, size) - halfsize;

    // Clamp to count.
    vec3 a = clamp(c, vec3(0), count-1);
    vec3 b = c-a;
    state.p += size*b;
    return a;
}
// Repeat around the origin by a fixed angle.
// For easier use, num of repetitions is use to specify the angle.
//float pModPolar(inout vec2 p, float repetitions) {
//	float angle = 2*PI/repetitions;
//	float a = atan(p.y, p.x) + angle/2.;
//	float r = length(p);
//	float c = floor(a/angle);
//	a = mod(a,angle) - angle/2.;
//	p = vec2(cos(a), sin(a))*r;
//	// For an odd number of repetitions, fix cell index of the cell in -x direction
//	// (cell index would be e.g. -5 and 5 in the two halves of the cell):
//	if (abs(c) >= (repetitions/2)) c = abs(c);
//	return c;
//}
void displace(float d) {
    vec3 p   = state.p;
    state.p += sin(d*p.x)*sin(d*p.y)*sin(d*p.z);
}
void displaceX(float d) {
    vec3 p   = state.p;
    state.p.x += sin(d*p.x);
}
void displaceY(float d) {
    vec3 p   = state.p;
    state.p.y += sin(d*p.y);
}
void displaceZ(float d) {
    vec3 p   = state.p;
    state.p.z += sin(d*p.z);
}
void displaceNoise(float d) {
    vec3 p   = state.p;
    state.p += (fbm(p,3)*2-1) * d;
}
/**
 *  repeat around x axis n times (check this)
 */
float repeatX(float n) {
    vec3 p  = state.p;
    float w = 2*PI/n;
    float a = atan(p.z, p.y);
    float r = length(p.yz);
    a = mod(a+PI*0.5, w)+PI-PI/n;
    state.p.yz = r*vec2(cos(a),sin(a));
    return a;
}
/**
 *  repeat around y axis n times
 */
float repeatY(float n) {
    vec3 p  = state.p;
    float w = 2*PI/n;
    float a = atan(p.z, p.x);
    float r = length(p.xz);
    a = mod(a+PI*0.5, w)+PI-PI/n;
    state.p.xz = r*vec2(cos(a),sin(a));
    return a;
}
/**
 *  repeat around z axis n times (check this)
 */
float repeatZ(float n) {
    vec3 p  = state.p;
    float w = 2*PI/n;
    float a = atan(p.y, p.x);
    float r = length(p.xy);
    a = mod(a+PI*0.5, w)+PI-PI/n;
    state.p.xy = r*vec2(cos(a),sin(a));
    return a;
}
/**
 *  Reflect space at a plane
 */
void reflect(vec3 planeNormal, float offset) {
    vec3 p  = state.p;
    float t = dot(p, planeNormal)+offset;
    if(t < 0) {
        state.p = p - (2*t)*planeNormal;
    }
}
/**
 *  Mirror at an axis-aligned plane which is at a
 *  specified distance <dist> from the origin.
 */
void mirrorX(float dist) {
    state.p.x = abs(state.p.x)-dist;
}
void mirrorY(float dist) {
    state.p.y = abs(state.p.y)-dist;
}
void mirrorZ(float dist) {
    state.p.z = abs(state.p.z)-dist;
}
/**
 * useful with domain repetition.
 * shapes are only rendered between min and max limits
 * using abs makes this symmetric (left/right and front/ back)
 */
vec3 clamp_abs_xz(vec3 p, vec3 min , vec3 max) {
    vec3 q = p;
    q.x = clamp( abs(q.x), min.x, max.x);
    q.z = clamp( abs(q.z), min.z, max.z);
    return q;
}
void twist(float a) {
    vec3 p   = state.p;
    float  c = cos(a*p.y+a);
    float  s = sin(a*p.y+a);
    mat2   m = mat2(c,-s,s,c);
    state.p  = vec3(m*p.xz,p.y);
}
//-------------------------------------------------------------------
// Combination operators
//-------------------------------------------------------------------
float opUnion(float res) {
    state.tmin = min(state.tmin, res);
    return res;
}
float opSubtraction(float a, float b) {
    float res  = max(a, -b);
    state.tmin = min(state.tmin, res);
    return res;
}
float opIntersection(float a, float b) {
    float res  = max(a, b);
    state.tmin = min(state.tmin, res);
    return res;
}
float opUnionSoft(float a, float b, float r) {
    float e   = max(r - abs(a - b), 0);
    float res = min(a, b) - e*e*0.25/r;
    state.tmin = min(state.tmin, res);
    return res;
}
float opUnionChamfer(float a, float b, float r) {
    float res  = min(min(a, b), (a - r + b)*sqrt(0.5));
    state.tmin = min(state.tmin, res);
    return res;
}
