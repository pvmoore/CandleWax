/**
 *  Signed Distance Field functions.
 *
 *  Useful resources:
 *
 *  http://iquilezles.org/www/index.htm
 *  http://9bitscience.blogspot.co.uk/2013/07/raymarching-distance-fields_14.html
 *  http://www.pouet.net/topic.php?which=7931&page=1
 *  http://www.alanzucconi.com/2016/07/01/signed-distance-functions/
 *  http://mercury.sexy/hg_sdf/
 *
 *  https://www.shadertoy.com/view/Xds3zN
 *  https://www.shadertoy.com/view/ldK3zD
 *  https://www.shadertoy.com/view/MlVGRh
 */

// Inaccurate but fast box
float boxCheap(vec3 b) {
    vec3 p = abs(state.p);
    return vmax(p - b);
}
// Accurate box
float box(vec3 b) {
    vec3 p = state.p;
    vec3 d = abs(p) - b;
    return length(max(d, vec3(0))) + vmax(min(d, vec3(0)));
}
// Box with rounded edges
float roundedBox(vec3 size, float r) {
    vec3 p = state.p;
    return length(max(abs(p)-size+vec3(r),0))-r;
}

float sphere(float r) {
    vec3 p = state.p;
    return length(p) - r;
}

float planeY(float y) {
    vec3 p = state.p;
    return p.y - y;

}

// Box with displacement using texture @ sampler0
float displacedBox(vec3 b) {
    vec3 p      = state.p;
    vec3 normal = normaliseToAxis(p);
    vec2 uv     = (normal.x*p.yz +
    normal.y*p.xz +
    normal.z*p.xy) / 30;
    float f = texture(sampler0, uv).r;

    return vmax(abs(state.p) - b) - f*0.5;
}
// Rounded box with displacement using texture @ sampler0
float displacedRoundedBox(vec3 s, float r) {
    vec3 p      = state.p;
    vec3 normal = normaliseToAxis(p);
    vec2 uv     = (normal.x*p.yz +
    normal.y*p.xz +
    normal.z*p.xy) / 30;
    float f = texture(sampler0, uv).r;

    return length(max(abs(state.p)-s+vec3(r),0))-r - f;
}
// Sphere with displacement using texture @ sampler0
float displacedSphere(float r) {
    vec3 p      = state.p;
    vec3 normal = normalize(p);
    vec2 uv     = sphereUV(normal);
    //vec2 uv     = sphereUVCheap(normal);
    float f     = texture(sampler0, uv).r;

    return length(p) - (r + f);
}

float capsule(float height, float r) {
    vec3 p = state.p;
    return mix(length(p.xz) - r, length(vec3(p.x, abs(p.y) - height, p.z)) - r, step(height, abs(p.y)));
}
float cylinder(float height, float r) {
    vec3 p  = state.p;
    float d = length(p.xz) - r;
    return max(d, abs(p.y) - height);
}
/**
 *  r1: large radius
 *  r2: smallradius
 *  c:  height/2
 */
float cone(float r1, float r2, float c) {
    vec3 p  = state.p;
    float d = length(p.xz)-mix(r1, r2, (c+p.y)/(c+c));
    d = max(d, -p.y-c);
    return max(d,p.y-c);
}
/**
 *  h.x = height (y)
 *  h.y = length (z)
 */
float triangleWedge(float height, float len) {
    vec3 p = state.p;
    vec3 q = abs(p);
    return max(q.z-len,max(q.x*0.866025+p.y*0.5,-p.y)-height*0.5);
}
float ellipse(vec3 r) {
    vec3 p = state.p;
    return (length( p/r ) - 1) * min(min(r.x,r.y),r.z);
}
/**
 *  Torus in the XZ plane
 */
float ring(float smallRadius, float largeRadius) {
    vec3 p = state.p;
    return length(vec2(length(p.xz) - largeRadius, p.y)) - smallRadius;
}
/**
 *  A hollow cylinder.
 */
float tube(float r, float thickness, float height) {
    vec3 p = state.p;
    return max(abs(length(p.xz)-r)-thickness,abs(p.y)-height);
}
/**
 *  Square torus in XZ plane.
 */
float squareRing(vec2 t) {
    vec3 p = state.p;
    vec2 q = vec2(length8(p.xz)-t.x,p.y);
    return length8(q)-t.y;
}
/**
 *
 */
float hexagon(float size, float depth) {
    vec3 p = abs(state.p);
    return max(p.x-depth,max(p.y+p.z*.5,p.z)-size);
}
float octahedron(float size) {
    vec3 p = abs(state.p);
    return (p.x+p.y+p.z-size) / 3;
}
//----------------------------------------------------------------
// Endless 2D shapes. These require state.p to be passed in
//----------------------------------------------------------------
// Endless box
float box2DCheap(vec2 p, vec2 b) {
    return vmax(abs(p)-b);
}
// Endless corner
float corner2D(vec2 p) {
    return length(max(p, vec2(0))) + vmax(min(p, vec2(0)));
}
