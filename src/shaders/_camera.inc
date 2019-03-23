


vec4 screenToClipSpace(vec2 screenPos) {
    float z = 0.5;
    return vec4((screenPos/SCREEN_SIZE)*2-1, z, 1);
}
vec3 screenToWorld(vec2 p) {
    // Convert p to camera space
    vec2 pos = 2*((p+0.5)/SCREEN_SIZE);
    pos.x -= 1;
    pos.y  = 1-pos.y;
    pos *= ASPECT_RATIO * ubo.tanFov2;
    //    vec2 pos = vec2(
    //          2*((p.x+0.5)/SCREEN_SIZE.x)-1,
    //        1-2*((p.y+0.5)/SCREEN_SIZE.y)
    //    ) * aspectRatio * ubo.tanFov2;

    // Convert pos from camera space to world space
    return (ubo.invView * vec4(pos, -1, 1)).xyz;
}
/**
 *  World coords to screen coords.
 */
vec2 worldToScreen(vec3 w) {
    vec3 cam = (ubo.view * vec4(w, 1)).xyz;
    vec2 a   = cam.xy / (-cam.z * ASPECT_RATIO * ubo.tanFov2);

    return vec2(a.x+1, 1-a.y) / 2 * SCREEN_SIZE - 0.5;
}

Ray getRay(vec2 pixel) {
    // Old method.
    //vec3 cameraRight = cross(pc.cameraDir, pc.cameraUp);
    //vec3 forward = pc.cameraDir;
    //vec3 up      = pc.cameraUp;
    //vec3 right   = cameraRight;
    //vec2 uv      = pixel - SCREEN_SIZE/2;
    //r.dir = normalize(FOCAL_LENGTH*forward + right*uv.x - up*uv.y);

    // Calculate pixel position in camera space
    vec2 pos = vec2(
        2*((pixel.x+0.5)/SCREEN_SIZE.x)-1,
        1-2*((pixel.y+0.5)/SCREEN_SIZE.y)
    ) * ASPECT_RATIO * ubo.tanFov2;

    // Convert positions from camera space to world space
    //vec4 O  = ubo.invView * vec4(0,0,0,1);
    //vec4 P  = ubo.invView * vec4(cam, -1, 1);
    //vec3 OP = (P-O).xyz;

    // Convert direction vector from camera space to world space
    vec3 P  = vec3(pos, -1);
    vec3 OP = (ubo.invView * vec4(P, 0)).xyz;

    Ray r;
    r.origin = ubo.cameraPos;
    r.dir    = normalize(OP);
    return r;
}
vec4 screenQuadForBox(vec3 pos, vec3 radius) {
    vec2 a = worldToScreen(vec3(pos.x-radius.x, pos.y+radius.y, pos.z+radius.z));
    vec2 b = worldToScreen(vec3(pos.x-radius.x, pos.y-radius.y, pos.z+radius.z));
    vec2 c = worldToScreen(vec3(pos.x+radius.x, pos.y-radius.y, pos.z+radius.z));
    vec2 d = worldToScreen(vec3(pos.x+radius.x, pos.y+radius.y, pos.z+radius.z));

    vec2 e = worldToScreen(vec3(pos.x-radius.x, pos.y+radius.y, pos.z-radius.z));
    vec2 f = worldToScreen(vec3(pos.x-radius.x, pos.y-radius.y, pos.z-radius.z));
    vec2 g = worldToScreen(vec3(pos.x+radius.x, pos.y-radius.y, pos.z-radius.z));
    vec2 h = worldToScreen(vec3(pos.x+radius.x, pos.y+radius.y, pos.z-radius.z));

    vec2 mn = min3(a, b, min3(c, d, min3(e, f, min(g, h))));
    vec2 mx = max3(a, b, max3(c, d, max3(e, f, max(g, h))));

    return vec4(mn, mx);
}
//vec4 screenQuadForSphere(vec3 pos, float radius) {
//    vec3 p    = (ubo.view * vec4(pos, 1)).xyz;
//
//    vec3 camDir = normalize(pos-pc.cameraPos);
//    vec3 front  = pos+camDir*radius;
//    vec3 back   = pos-camDir*radius;
////    vec2 a = worldToScreen(front);
//    //vec2 b =
//    return screenQuadForBox(front, radius.xxx);
//}

//vec4 screenQuadForSphere(vec3 worldPos, float radius) {
//
//    // Constants unless focal length or screen size change.
//    vec3 uv      = vec3(SCREEN_SIZE/2, FOCAL_LENGTH);
//    // Stuff that changes when the camera moves.
//    vec3 xv      = cameraRight*uv.x;
//    vec3 yv      = -pc.cameraUp*uv.y;
//    vec3 zv      = pc.cameraDir*uv.z;
//
//    // Stuff that changes if worldPos or camera moves.
//    vec3 v       = worldPos-pc.cameraPos;
//    float vlen   = length(v);
//
//    // calculate x screen pos
//    float xvvlen = distance(xv,v);
//    float distx  = cosAngle(uv.x, vlen, xvvlen) * vlen;
//    float x      = distx / uv.x;
//
//    // calculate y screen pos
//    float yvvlen = distance(yv,v);
//    float disty  = cosAngle(uv.y, vlen, yvvlen) * vlen;
//    float y      = disty / uv.y;
//
//    // calculate z screen pos (depth)
//    float zvvlen = distance(zv,v);
//    float distz  = cosAngle(FOCAL_LENGTH, vlen, zvvlen) * vlen;
//    float frontz = (distz - radius) / uv.z;
//    float backz  = (distz + radius) / uv.z;
//
//    float frontmul = (frontz==0 ? 0 : 1/frontz);
//    float backmul  = (backz==0  ? 0 : 1/backz);
//
//    vec2 front = vec2(x,y) * frontmul * uv.xy + uv.xy;
//    vec2 back  = vec2(x,y) * backmul * uv.xy + uv.xy;
//
//    float fr = radius * frontmul;
//    float br = radius * backmul;
//
//    vec2 frontTL = front-fr;
//    vec2 frontBR = front+fr;
//    vec2 backTL  = back-br;
//    vec2 backBR  = back+br;
//
//    vec2 mn = min3(frontTL, frontBR, min(backTL, backBR));
//    vec2 mx = max3(frontTL, frontBR, max(backTL, backBR));
//
//    return vec4(mn, mx);
//}