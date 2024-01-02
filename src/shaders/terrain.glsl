#version 410 core

out vec4 outColor;

in vec2 iUV;
in vec3 iPos;
uniform mat4 uProj;
uniform mat4 uView;

void main(){
    vec2 worldp = (inverse(uView) * inverse(uProj) * vec4(iUV, 0., 1.)).xy;
    float x = worldp.x;
    float y = sin(x/3.);
    if (iUV.y < y) {
        outColor = vec4(0., 0., 0., 1.);
    } else {
        outColor = vec4(0., 0., 0.5, 1.);
    }
    // outColor = vec4(1., 0., 0., 1.);
}
