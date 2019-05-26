#version 330 core
#define MAX_POINTS 128
layout(location = 0) in float t;

uniform int size;
uniform vec3 points[MAX_POINTS];

int factorial(int n){
    int r = 1;
    for(int i = n; i>0; i--){
        r *= i;
    }
    return r;
}

float bernstein(int i, int n, float t){
    float r =  float(factorial(n)) / float((factorial(i) * factorial(n - i)));
    r *= pow(t,i);
    r *= pow(1-t,n-i);
    return r;
}

void main(){
    vec3 res;
    int n = size-1;
    for (int i = 0; i < size; i++){
        float b = bernstein(i, n, t);
        res += points[i] * b;
    }
    gl_Position = vec4(res, 1.0f);
}
