
struct Ray {
    vec3 origin;
    vec3 dir;
};
struct Material {
    vec3 colour;
    int texture;
    int specPower;
};
struct State {
    vec3 p;
    vec3 pStart;
    float tmin;
    float pTmin;
    Material material;
};

