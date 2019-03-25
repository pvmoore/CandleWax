
struct Ray {
    vec3 origin;
    vec3 dir;
};
struct Material {
    vec3 colour;
    int texture;

    int specPower;
    float _pad1;
    float _pad2;
    float _pad3;    // align to 16 bytes
};
struct State {
    vec3 p;
    vec3 pStart;
    float tmin;
    float pTmin;
    uint material;
};
struct SDFObject {
    vec3 move;
    uint type;

    vec4 params1;
    vec4 params2;

    uint materialIndex;
    float _pad1;
    float _pad2;
    float _pad3;    // align to 16 bytes
};

