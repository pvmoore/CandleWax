
#define PI          (3.1415926535897932384626433832795)
#define UINT_MAX    (0xffffffffu)
#define FLT_MAX     (3.402823466e+38F)
#define FLT_MIN     (1.175494351e-38F)
#define FLT_MIN     (1.175494351e-38F)
#define FLT_INF     (1.0f/0.0f)
#define FLT_EPSILON (1.192092896e-07F)

#ifndef GL_AMD_shader_trinary_minmax
    float min3(float a, float b, float c) {
        return min(a, min(b, c));
    }
    float max3(float a, float b, float c) {
        return max(a, max(b, c));
    }
    vec2 min3(vec2 a, vec2 b, vec2 c) {
        return min(a, min(b, c));
    }
    vec2 max3(vec2 a, vec2 b, vec2 c) {
        return max(a, max(b, c));
    }
    vec3 min3(vec3 a, vec3 b, vec3 c) {
        return min(a, min(b, c));
    }
    vec3 max3(vec3 a, vec3 b, vec3 c) {
        return max(a, max(b, c));
    }
    vec4 min3(vec4 a, vec4 b, vec4 c) {
        return min(a, min(b, c));
    }
    vec4 max3(vec4 a, vec4 b, vec4 c) {
        return max(a, max(b, c));
    }
#endif

float saturate(float x) {
    return clamp(x, 0, 1);
}

float vmax(vec2 v) {
    return max(v.x, v.y);
}
float vmax(vec3 v) {
    return max(max(v.x, v.y), v.z);
}
float vmax(vec4 v) {
    return max(max(v.x, v.y), max(v.z, v.w));
}
float vmin(vec2 v) {
    return min(v.x, v.y);
}
float vmin(vec3 v) {
    return min(min(v.x, v.y), v.z);
}
float vmin(vec4 v) {
    return min(min(v.x, v.y), min(v.z, v.w));
}
float vadd(vec2 v) {
    return v.x+v.y;
}
float vadd(vec3 v) {
    return v.x+v.y+v.z;
}
float vadd(vec4 v) {
    return v.x+v.y+v.z+v.w;
}

float rand(vec2 c) {
    return fract(sin(dot(c, vec2(12.9898, 78.233))) * 43758.5453);
}
float rand(vec3 p) {
    p.x += p.y * 57 + p.z * 21;
    return sin(cos(p.x) * p.x);
    /* The following is an alternative for the previous line:
     * return fract(p.x * p.x * 0.0013) * 2 - 1; */
}
vec2 hash2(float n) {
    return fract(sin(vec2(n,n+1))*vec2(43758.5453123, 22578.1459123));
}

float length2(vec2 p) {
    return sqrt(p.x*p.x + p.y*p.y);
}
//float length6(vec2 p) {
//    p = p*p*p; p = p*p;
//    return pow( p.x + p.y, 1.0/6.0);
//}
float length8(vec2 p) {
    p = p*p; p = p*p; p = p*p;
    return pow(p.x + p.y, 1.0/8.0);
}
vec2 sphereUVCheap(vec3 normal) {
    return normal.xy/2+0.5;
}
vec2 sphereUV(vec3 normal) {
    return asin(normal.xy)/PI + 0.5;
}
/**
 *  Truncate vector to an axis aligned normal.
 */
vec3 normaliseToAxis(vec3 v) {
    vec3 a = abs(v);
    vec3 b = a.x>a.y && a.x>a.z ? vec3(1,0,0) :
    a.y>a.z ? vec3(0,1,0) : vec3(0,0,1);
    return b * sign(v);
}
/**
 *  Apply Lambertian reflectance to colour.
 *  Assumes normal and toLight are normalised.
 */
vec3 diffuseColour(vec3 colour, vec3 normal, vec3 toLight) {
    float lr = max(dot(normal, toLight), 0.1);
    return colour * lr;
}
/**
 *  Apply phong specular reflectance to colour.
 *  Assumes normal and toLight are normalised.
 */
vec3 specularColour(vec3 colour,
                    vec3 normal,
                    vec3 fromEye,
                    vec3 toLight,
                    float power)
{
    float cosAngle = max(0, dot(fromEye, reflect(toLight, normal)));
    return colour * pow(cosAngle, power);
}


/**
 *	Returns the linear interpolation of x and y
 *	based on weight w.
 *  Returns x when w is 0 and y when w is 1.
 */
float lerp(float x, float y, float w) {
    return w*(y-x) + x;
}

/**
 *	Trigonometry:
 *	Calculate an angle of a triangle given the length
 *  of all three sides.
 *  _c_ is the side opposite the angle to calculate.
 */
float cosAngle(float a, float b, float c) {
    return (a*a + b*b - c*c) / (2*a*b);
}

/**
 *	Parabolic curve function.
 *
 *  1||      |
 *   | \    /
 *  0|___--___
 *   0       1
 */
float uCurve(float v) {
    v = clamp(v, 0, 1) - 0.5;
    return abs(4*v*v);
}
/**
 *	Parabolic curve function.
 *
 *	1|   --
 *   | /    \
 *  0||______|
 *   0       1
 */
float nCurve(float v) {
    v = clamp(v, 0, 1) - 0.5;
    return -4*v*v + 1;
}
float distanceFromLine(vec2 p, vec2 l1, vec2 l2) {
    vec2 n  = l2 - l1;
    vec2 pa = l1 - p;
    vec2 c  = n * dot(pa,n) / dot(n,n);
    vec2 d  = pa - c;
    return sqrt(dot(d,d));
}
float distanceFromLineSquared(vec2 p, vec2 l1, vec2 l2) {
    vec2 ab = l2 - l1;
    vec2 ap = p - l1;
    vec2 bp = p - l2;
    float e = dot(ap, ab);
    return dot(ap,ap) - e*e / dot(ab,ab);
}
/**
 *  Find intersection of lines a->b and c->d.
 *
 */
vec2 lineIntersection(vec2 a, vec2 b, vec2 c, vec2 d) {
    float A1 = b.y-a.y;
    float B1 = a.x-b.x;
    float C1 = A1*a.x+B1*a.y;

    float A2 = d.y-c.y;
    float B2 = c.x-d.x;
    float C2 = A2*c.x+B2*c.y;

    float det = A1*B2 - A2*B1;
    //  if(det==0) { //Lines are parallel }
    float x = (B2*C1 - B1*C2)/det;
    float y = (A1*C2 - A2*C1)/det;
    return vec2(x,y);
}
/**
 *	Assumes all points are relative to the origin (0,0,0).
 */
float distanceFromPlane(vec3 p, vec3 planeNormal) {
    return dot(planeNormal, p);
}
float distanceFromPlane(vec3 p, vec3 v1, vec3 v2) {
    vec3 planeNormal = normalize(cross(v2, v1));
    return distanceFromPlane(p, planeNormal);
}
bool intersectsSphere(const vec3 pos, const vec3 dir, const vec3 spherePos, const float radius, out float t) {
    /// Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
    vec3 op   = spherePos-pos;
    float b   = dot(op, dir);
    float det = b*b - dot(op, op) + radius*radius;

    if(det<0) return false;

    det = sqrt(det);

    return (t=b-det)>1e-4 ? true : ((t=b+det)>1e-4 ? true : false);
}
bool intersectsBox(const vec3 pos, const vec3 invDir, const vec3 minbox, const vec3 maxbox, out float t) {
    vec3 tNearV = (minbox-pos) * invDir;
    vec3 tFarV  = (maxbox-pos) * invDir;
    vec3 tNear  = min(tNearV, tFarV);
    vec3 tFar   = max(tNearV, tFarV);

    t = max(tNear.x, max(tNear.y, tNear.z));
    float far = min(tFar.x, min(tFar.y, tFar.z));

    return far >= max(t, 0);
}