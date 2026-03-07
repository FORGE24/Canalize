#pragma once
#include <cmath>
#include <immintrin.h>
#include <algorithm>
#include <vector>

// =========================================================================================
// PERMUTATION TABLE
// =========================================================================================

static const int p[512] = {
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
    190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,
    125,136,171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
    55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,
    159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,
    47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,129,22,39,
    253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,
    81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,
    205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
    190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,
    125,136,171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
    55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,
    159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,
    47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,129,22,39,
    253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,
    81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,
    205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

// =========================================================================================
// MATH UTILITIES
// =========================================================================================

inline float lerp(float t, float a, float b) { return a + t*(b-a); }
inline float clamp(float x, float lo, float hi){ return x<lo?lo:(x>hi?hi:x); }
inline float smoothstep(float e0, float e1, float x){
    x=clamp((x-e0)/(e1-e0),0.0f,1.0f); return x*x*(3.0f-2.0f*x);
}
inline float grad(int hash, float x, float y){
    int h=hash&15;
    float u=h<8?x:y, v=h<4?y:(h==12||h==14?x:0.0f);
    return ((h&1)==0?u:-u)+((h&2)==0?v:-v);
}
// 3D Gradient for 3D Simplex
inline float grad3(int hash, float x, float y, float z) {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

// =========================================================================================
// NOISE PRIMITIVES
// =========================================================================================

inline float simplex2(float x, float y){
    const float F2=0.5f*(sqrtf(3.0f)-1.0f), G2=(3.0f-sqrtf(3.0f))/6.0f;
    float s=(x+y)*F2;
    int i=(int)floorf(x+s), j=(int)floorf(y+s);
    float t=(float)(i+j)*G2;
    float x0=x-((float)i-t), y0=y-((float)j-t);
    int i1=x0>y0?1:0, j1=x0>y0?0:1;
    float x1=x0-(float)i1+G2, y1=y0-(float)j1+G2;
    float x2=x0-1.0f+2.0f*G2, y2=y0-1.0f+2.0f*G2;
    int ii=i&255, jj=j&255;
    float n0=0,n1=0,n2=0;
    float t0=0.5f-x0*x0-y0*y0; if(t0>=0){t0*=t0;n0=t0*t0*grad(p[ii+p[jj]],x0,y0);}
    float t1=0.5f-x1*x1-y1*y1; if(t1>=0){t1*=t1;n1=t1*t1*grad(p[ii+i1+p[jj+j1]],x1,y1);}
    float t2=0.5f-x2*x2-y2*y2; if(t2>=0){t2*=t2;n2=t2*t2*grad(p[ii+1+p[jj+1]],x2,y2);}
    return 70.0f*(n0+n1+n2);
}

// 3D Simplex Noise for Caves
inline float simplex3(float x, float y, float z) {
    const float F3 = 1.0f / 3.0f;
    const float G3 = 1.0f / 6.0f;
    float s = (x + y + z) * F3;
    int i = (int)floorf(x + s);
    int j = (int)floorf(y + s);
    int k = (int)floorf(z + s);
    float t = (i + j + k) * G3;
    float x0 = x - (i - t);
    float y0 = y - (j - t);
    float z0 = z - (k - t);
    
    int i1, j1, k1; // The integer offsets for the second simplex corner
    int i2, j2, k2; // The integer offsets for the third simplex corner
    if (x0 >= y0) {
        if (y0 >= z0) { i1=1; j1=0; k1=0; i2=1; j2=1; k2=0; } // X Y Z order
        else if (x0 >= z0) { i1=1; j1=0; k1=0; i2=1; j2=0; k2=1; } // X Z Y order
        else { i1=0; j1=0; k1=1; i2=1; j2=0; k2=1; } // Z X Y order
    } else { // x0 < y0
        if (y0 < z0) { i1=0; j1=0; k1=1; i2=0; j2=1; k2=1; } // Z Y X order
        else if (x0 < z0) { i1=0; j1=1; k1=0; i2=0; j2=1; k2=1; } // Y Z X order
        else { i1=0; j1=1; k1=0; i2=1; j2=1; k2=0; } // Y X Z order
    }
    
    float x1 = x0 - i1 + G3;
    float y1 = y0 - j1 + G3;
    float z1 = z0 - k1 + G3;
    float x2 = x0 - i2 + 2.0f * G3;
    float y2 = y0 - j2 + 2.0f * G3;
    float z2 = z0 - k2 + 2.0f * G3;
    float x3 = x0 - 1.0f + 3.0f * G3;
    float y3 = y0 - 1.0f + 3.0f * G3;
    float z3 = z0 - 1.0f + 3.0f * G3;
    
    int ii = i & 255;
    int jj = j & 255;
    int kk = k & 255;
    
    float n0, n1, n2, n3; // Noise contributions from the four corners
    
    float t0 = 0.6f - x0*x0 - y0*y0 - z0*z0;
    if (t0 < 0) n0 = 0.0f;
    else {
        t0 *= t0;
        n0 = t0 * t0 * grad3(p[ii + p[jj + p[kk]]], x0, y0, z0);
    }
    
    float t1 = 0.6f - x1*x1 - y1*y1 - z1*z1;
    if (t1 < 0) n1 = 0.0f;
    else {
        t1 *= t1;
        n1 = t1 * t1 * grad3(p[ii + i1 + p[jj + j1 + p[kk + k1]]], x1, y1, z1);
    }
    
    float t2 = 0.6f - x2*x2 - y2*y2 - z2*z2;
    if (t2 < 0) n2 = 0.0f;
    else {
        t2 *= t2;
        n2 = t2 * t2 * grad3(p[ii + i2 + p[jj + j2 + p[kk + k2]]], x2, y2, z2);
    }
    
    float t3 = 0.6f - x3*x3 - y3*y3 - z3*z3;
    if (t3 < 0) n3 = 0.0f;
    else {
        t3 *= t3;
        n3 = t3 * t3 * grad3(p[ii + 1 + p[jj + 1 + p[kk + 1]]], x3, y3, z3);
    }
    
    return 32.0f * (n0 + n1 + n2 + n3);
}

// Worley - FULLY FIXED: both axes use full 8-bit (0-255), completely decoupled
inline float worley(float x, float y){
    int xi=(int)floorf(x), yi=(int)floorf(y);
    float xf=x-xi, yf=y-yi, minSq=1e9f;
    for(int i=-1;i<=1;i++) for(int j=-1;j<=1;j++){
        int hx = p[((xi+i)&255) + p[(yi+j)&255]];
        int hy = p[((xi+i+127)&255) + p[((yi+j+311)&255)]];
        float rx = (float)hx / 255.0f;
        float ry = (float)hy / 255.0f;
        float dx=(float)i+rx-xf, dy=(float)j+ry-yf;
        float d=dx*dx+dy*dy; if(d<minSq) minSq=d;
    }
    return sqrtf(minSq);
}

// Standard FBM
inline float fbm(float x, float y, int oct, float pers, float lac){
    float tot=0,freq=1,amp=1,maxV=0;
    for(int i=0;i<oct;i++){
        tot+=simplex2(x*freq,y*freq)*amp;
        maxV+=amp; amp*=pers; freq*=lac;
    }
    return tot/maxV;
}

// Pure ridged FBM
inline float ridged_fbm(float x, float y, int oct, float pers, float lac){
    float tot=0,freq=1,amp=1,maxV=0;
    for(int i=0;i<oct;i++){
        float n=clamp(simplex2(x*freq,y*freq),-1.0f,1.0f);
        n=1.0f-fabsf(n);
        n=n*n;
        tot+=n*amp; maxV+=amp;
        amp*=pers; freq*=lac;
    }
    return tot/maxV;
}

// Hash a region cell (low-frequency grid)
inline float region_val(int rx, int ry, int salt, float lo, float hi){
    int h = p[((rx*7+salt)&255) + p[(ry*13+salt*3)&255]];
    return lo + (hi-lo) * ((float)h / 255.0f);
}
