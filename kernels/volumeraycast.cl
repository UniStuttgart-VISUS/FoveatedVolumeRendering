/**
 * \file
 *
 * \author Valentin Bruder
 *
 * \copyright Copyright (C) 2018 Valentin Bruder
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma OPENCL EXTENSION cl_khr_3d_image_writes : enable

#define ERT_THRESHOLD 0.98

constant sampler_t linearSmp = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP_TO_EDGE |
                               CLK_FILTER_LINEAR;
constant sampler_t nearestSmp = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP |
                                CLK_FILTER_NEAREST;
constant sampler_t nearestIntSmp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP |
                                   CLK_FILTER_NEAREST;

// init random number generator (hybrid tausworthy)
uint4 initRNG()
{
    uint4 taus;
    taus.x = 128U + (uint)(get_global_size(0));
    taus.y = 128U + (uint)(get_global_size(1));
    taus.z = 128U + (uint)(get_global_size(0) + get_global_size(1));
    taus.w = 128U + (uint)(get_global_size(0) * get_global_size(1));
    return taus;
}

// S1, S2, S3, and M are all constants, and z is part of the
// private per-thread generator state.
uint tausStep(uint4 *taus, uint p, int s1, int s2, int s3, uint m)
{
    uint4 loc = *taus;
    uint locZ;
    if (p == 0) locZ = loc.x;
    if (p == 1) locZ = loc.y;
    if (p == 2) locZ = loc.z;
    uint b = (((locZ << (uint)(s1)) ^ locZ) >> (uint)(s2));
    locZ = (((locZ & m) << (uint)(s3)) ^ b);
    if (p == 0) *taus = (uint4)(locZ, loc.yzw);
    if (p == 1) *taus = (uint4)(loc.x, locZ, loc.zw);
    if (p == 2) *taus = (uint4)(loc.xy, locZ, loc.w);
    return locZ;
}

// A and C are constants
uint lcgStep(uint4 *taus, uint a, uint c)
{
    uint4 loc = *taus;
    *taus = (uint4)(loc.xyz, a*loc.w + c);
    return loc.w;
}

float hybridTaus(uint4 *taus)
{
    // Combined period is lcm(p1,p2,p3,p4)~ 2^121
    return 2.3283064365387e-10 * (float)(tausStep(taus, 0, 13, 19, 12, 4294967294U) ^  // p1=2^31-1
                                         tausStep(taus, 1, 2, 25, 4, 4294967288U)   ^  // p2=2^30-1
                                         tausStep(taus, 2, 3, 11, 17, 4294967280U)  ^  // p3=2^28-1
                                         lcgStep(taus, 1664525U, 1013904223U));        // p4=2^32
}

// intersect ray with the 'unit-box': (-1,-1,-1) to (1,1,1)
// http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
int intersectBox(float3 rayOrig, float3 rayDir, float *tnear, float *tfar)
{
    // compute intersection of ray with all six bbox planes
    float3 invRay = native_divide((float3)(1.0f), rayDir);
    float3 tBot = invRay * ((float3)(-1.0f) - rayOrig);
    float3 tTop = invRay * ((float3)(1.0f) - rayOrig);

    // re-order intersections to find smallest and largest on each axis
    float3 tMin = min(tTop, tBot);
    float3 tMax = max(tTop, tBot);

    // find the largest tMin and the smallest tMax
    float maxTmin = max(max(tMin.x, tMin.y), max(tMin.x, tMin.z));
    float minTmax = min(min(tMax.x, tMax.y), min(tMax.x, tMax.z));

    *tnear = maxTmin;
    *tfar = minTmax;

    return (int)(minTmax > maxTmin);
}

// intersect a ray with a defined box
int intersectBBox(float3 rayOrig, float3 rayDir, float3 lower, float3 upper,
                  float *tnear, float *tfar)
{
    // compute intersection of ray with all six bbox planes
    float3 invRay = native_divide((float3)(1.0f), rayDir);
    float3 tBot = invRay * (lower - rayOrig);
    float3 tTop = invRay * (upper - rayOrig);

    // re-order intersections to find smallest and largest on each axis
    float3 tMin = min(tTop, tBot);
    float3 tMax = max(tTop, tBot);

    // find the largest tMin and the smallest tMax
    float maxTmin = max(max(tMin.x, tMin.y), max(tMin.x, tMin.z));
    float minTmax = min(min(tMax.x, tMax.y), min(tMax.x, tMax.z));

    *tnear = maxTmin;
    *tfar = minTmax;

    return (int)(minTmax > maxTmin);
}

// ray-plane intersection
int intersectPlane(const float3 rayOrigin, const float3 rayDir,
                   const float3 planeNormal, const float3 planePos, float *t)
{
    float denom = dot(rayDir, planeNormal);
    if (denom > 1e-6)   // check if parallel
    {
        float3 origin2point = planePos - rayOrigin;
        *t = dot(origin2point, planeNormal) / denom;
        return (t >= 0);
    }
    return false;
}

// Compute gradient using central difference: f' = ( f(x+h)-f(x-h) )
float4 gradientCentralDiff(read_only image3d_t vol, const float4 pos)
{
    float3 volResf = convert_float3(get_image_dim(vol).xyz);
    float3 offset = native_divide((float3)(1.0f), volResf);
    float3 s1;
    float3 s2;
    s1.x = read_imagef(vol, linearSmp, pos + (float4)(-offset.x, 0, 0, 0)).x;
    s1.y = read_imagef(vol, linearSmp, pos + (float4)(0, -offset.y, 0, 0)).x;
    s1.z = read_imagef(vol, linearSmp, pos + (float4)(0, 0, -offset.z, 0)).x;

    s2.x = read_imagef(vol, linearSmp, pos + (float4)(+offset.x, 0, 0, 0)).x;
    s2.y = read_imagef(vol, linearSmp, pos + (float4)(0, +offset.y, 0, 0)).x;
    s2.z = read_imagef(vol, linearSmp, pos + (float4)(0, 0, +offset.z, 0)).x;

    float3 normal = fast_normalize(s2 - s1).xyz;
    if (length(normal) == 0.0f) // TODO: zero correct
        normal = (float3)(0.57735f);

    return (float4)(normal, fast_length(s2 - s1));
}

// Compute gradient using central difference: f' = ( f(x+h)-f(x-h) )
float4 gradientCentralDiffTff(read_only image3d_t vol, const float4 pos, read_only image1d_t tff)
{
    float3 volResf = convert_float3(get_image_dim(vol).xyz);
    float3 offset = native_divide((float3)(1.0f), volResf);
    float3 s1;
    float3 s2;
    s1.x = read_imagef(tff, linearSmp,
                       read_imagef(vol, linearSmp, pos + (float4)(-offset.x, 0, 0, 0)).x).w;
    s1.y = read_imagef(tff, linearSmp,
                       read_imagef(vol, linearSmp, pos + (float4)(0, -offset.y, 0, 0)).x).w;
    s1.z = read_imagef(tff, linearSmp,
                       read_imagef(vol, linearSmp, pos + (float4)(0, 0, -offset.z, 0)).x).w;

    s2.x = read_imagef(tff, linearSmp,
                       read_imagef(vol, linearSmp, pos + (float4)(+offset.x, 0, 0, 0)).x).w;
    s2.y = read_imagef(tff, linearSmp,
                       read_imagef(vol, linearSmp, pos + (float4)(0, +offset.y, 0, 0)).x).w;
    s2.z = read_imagef(tff, linearSmp,
                       read_imagef(vol, linearSmp, pos + (float4)(0, 0, +offset.z, 0)).x).w;

    float3 normal = fast_normalize(s2 - s1).xyz;
    if (length(normal) == 0.0f) // TODO: zero correct
        normal = (float3)(0.57735f);

    return (float4)(normal, fast_length(s2 - s1));
}

float getf4(float4 v, int id)
{
    if (id == 0) return v.x;
    if (id == 1) return v.y;
    if (id == 2) return v.z;
    if (id == 3) return v.w;
}

// Compute gradient using a sobel filter (1,2,4)
float4 gradientSobel(read_only image3d_t vol, const float4 pos)
{
    float sobelWeights[3][3][3][3] = {
            {{{-1, -2, -1},
              {-2, -4, -2},
              {-1, -2, -1}},
             {{ 0,  0,  0},
              { 0,  0,  0},
              { 0,  0,  0}},
             {{ 1,  2,  1},
              { 2,  4,  2},
              { 1,  2,  1}}},
            {{{-1, -2, -1},
              { 0,  0,  0},
              { 1,  2,  1}},
             {{-2, -4, -2},
              { 0,  0,  0},
              { 2,  4,  2}},
             {{-1, -2, -1},
              { 0,  0,  0},
              { 1,  2,  1}}},
            {{{-1,  0,  1},
              {-2,  0,  2},
              {-1,  0,  1}},
             {{-2,  0,  2},
              {-4,  0,  4},
              {-2,  0,  2}},
             {{-1,  0,  1},
              {-2,  0,  2},
              {-1,  0,  1}}}
    };
    float3 volResf = convert_float3(get_image_dim(vol).xyz);
    float3 offset = native_divide((float3)(1.0f), volResf);

    float4 gradient = (float4)(0.f);
    for (int dir = 0; dir < 3; dir++)   // partial derivatives in xyz directions
    {
        for (int i = -1; i < 2; i++)
        {
            for (int j = -1; j < 2; j++)
            {
                for (int k = -1; k < 2; k++)
                {
                    float4 samplePos = pos + (float4)(offset*(float3)(i,j,k), 0);
                    float weight = sobelWeights[dir][i + 1][j + 1][k + 1]
                                        * read_imagef(vol, linearSmp, samplePos).x;
                    if (dir == 0) gradient.x += weight;
                    else if (dir == 1) gradient.y += weight;
                    else if (dir == 2) gradient.z += weight;
                }
            }
        }
    }
    gradient.xyz /= 27.f;
    gradient.w = fast_length(gradient.xyz);
    if (gradient.w == 0)    // TODO
        gradient.xyz = (float3)(1.f);
    gradient.xyz = fast_normalize(gradient.xyz);

    return gradient;
}

// specular part of blinn-phong shading model
float3 specularBlinnPhong(float3 lightColor, float specularExp, float3 materialColor,
                          float3 normal, float3 toLightDir, float3 toCameraDir)
{
    float3 h = toCameraDir + toLightDir;
    // check for special case where the light source is exactly opposite
    // to the view direction, i.e. the length of the halfway vector is zero
    if (dot(h, h) < 1.e-6f) // check for squared length
        return (float3)(0.0f);

    h = normalize(h);
    return materialColor * lightColor * native_powr(max(dot(normal, h), 0.f), specularExp);
}

// blinn phong illumination based gradients evaluating density values
float3 illumination(read_only image3d_t vol, const float4 pos, float3 color, float3 toLightDir,
                    float3 n)
{
    float3 l = fast_normalize(toLightDir.xyz);

    float3 amb = color * 0.2f;
    float3 diff = color * max(0.f, dot(n, l)) * 0.8f;
    float3 spec = specularBlinnPhong((float3)(1.f), 40.f, (float3)(1.f), n, l, toLightDir) * 0.2f;

    return (amb + diff + spec);
}

// check for border edges
bool checkBoundingBox(float3 pos, float3 voxLen, float2 bound)
{
    if (
        (pos.x < voxLen.x     && pos.z < bound.x + voxLen.z) ||
        (pos.x < voxLen.x     && pos.y < voxLen.y) ||
        (pos.y < voxLen.y     && pos.z < bound.x + voxLen.z) ||
        (pos.x > 1.f-voxLen.x && pos.z < bound.x + voxLen.z) ||
        (pos.y > 1.f-voxLen.y && pos.z < bound.x + voxLen.z) ||
        (pos.x > 1.f-voxLen.x && pos.z > bound.y - voxLen.z) ||
        (pos.y > 1.f-voxLen.y && pos.z > bound.y - voxLen.z) ||
        (pos.x < voxLen.x     && pos.z > bound.y - voxLen.z) ||
        (pos.y < voxLen.y     && pos.z > bound.y - voxLen.z) ||
        (pos.x > 1.f-voxLen.x && pos.y < voxLen.y) ||
        (pos.x > 1.f-voxLen.x && pos.y > 1.f -voxLen.y) ||
        (pos.x < voxLen.x     && pos.y > 1.f -voxLen.y)
       )
        return true;
    else
        return false;
}

bool checkBoundingCell(int3 cell, int3 volRes, int size)
{
    if (any(cell > volRes - 1) || any(cell < size))
        return true;
    else
        return false;
}

// Uniform Sampling of the hemisphere above the normal n
float3 getUniformRandomSampleDirectionUpper(float3 n, uint4 *taus)
{
    float z = (hybridTaus(taus) * 2.f) - 1.f;
    float phi = hybridTaus(taus) * 2.f * M_PI_F;

    float3 sampleDirection = (float3)(sqrt(1.f - z*z) * sin(phi), sqrt(1.f - z*z) * cos(phi), z);
    float cosTheta = dot(n, sampleDirection);

    if (cosTheta < 0)
        sampleDirection *= -1;
    return sampleDirection;
}


// Calculate ambient occlusion factor with monte carlo sampling
float calcAO(float3 n, uint4 *taus, image3d_t volData, float3 pos, float stepSize, float r)
{
    float ao = 0.f;
    int rays = 12;
    // rays
    for (int i = 0; i < rays; ++i)
    {
        float3 dir = getUniformRandomSampleDirectionUpper(n, taus);
        float sample = 0.f;
        int cnt = 0;

        while (cnt*stepSize < r)
        {
            ++cnt;
            sample += read_imagef(volData, linearSmp, (float4)(pos + dir*cnt*stepSize, 1.f)).x;
        }
        sample /= cnt;
        ao += sample;
    }
    ao /= (float)(rays);

    return ao;
}

// check wheather a given point is in the rectangle
// rectPos is bottom left point of rect
bool checkPointInRectangle(float2 rectPos, float2 rectExtends, float2 point){
	if(rectExtends.x == 0.0 || rectExtends.y == 0.0) return false;
    if(point.x < rectPos.x || point.y < rectPos.y) return false;    // left and bottom edge
    if(point.x > rectPos.x + rectExtends.x || point.y > rectPos.y + rectExtends.y) return false;    // top and right edge
    return true;
}

// returns the 1d index for a 2d point in an grid with width m
int index_from_2d(int2 coord, int m){
    return coord.y * m + coord.x;
}

// returns the 2d coord for a point given as 1d index in a grid with width m
int2 coord_from_1d(int index, int m){
    return (int2)(index % m, index / m);
}

// returns the mapped 1d index with a gap_size of g - 1 and a grid width of m
int mapped_index(int old_index, int m, int g){
    return index_from_2d((int2)(old_index * g % m, ((old_index * g) / m) * g), m);
}

// returns the mapped 2d cord with a gap size of g - 1 and a grid width of m
int2 mapped_2d_index_from_1d(int old_index, int m, int g){
    return (int2)(old_index * g % m, ((old_index * g) / m) * g);
}

// returns the mapped 2d coord from old coord, where m is the new width of the grid and old_m is the old m of the grid
int2 old_2d_to_new_2d_coord(int2 old_coord, int m, int g, int old_m){
    return mapped_2d_index_from_1d(index_from_2d(old_coord, old_m), m, g);
}

/*
checks whether a point is inside the disk bounded by an ellipse
*/
bool checkPointInEllipse(float2 ellipse_center, float rx, float ry, float2 point){
    return native_divide(pown(point.x - ellipse_center.x, 2), pown(rx, 2)) + native_divide(pown(point.y - ellipse_center.y, 2), pown(ry, 2)) <= 1.0;
}

// wirtes image data but checks whether it is in bounds: bounds starting from 0
void write_imagef_with_bounds_check(int2 bounds, __write_only image2d_t outData, int2 img_coord, float4 color){
    if(img_coord.x <= bounds.x && img_coord.y <= bounds.y && img_coord.x >= 0 && img_coord.y >= 0){
        write_imagef(outData, img_coord, color);
    }
    return;
}

/**
 * direct volume raycasting kernel
 */
__kernel void volumeRender_standard(  __read_only image3d_t volData
                           , __read_only image3d_t volBrickData
                           , __read_only image1d_t tffData     // constant transfer function values
                           , __write_only image2d_t outData
                           , const float samplingRate
                           , const float16 viewMat
                           , const uint orthoCam
                           , const uint illumType
                           , const uint useBox
                           , const uint useLinear
                           , const float4 background
                           , __read_only image1d_t tffPrefix
                           , const uint useAO
                           , const float3 modelScale
                           , const uint contours
                           , const uint aerial
                           )
{
    int2 globalId = (int2)(get_global_id(0), get_global_id(1));
    if(any(globalId >= get_image_dim(outData)))
        return;

    uint4 taus = initRNG();
    // pseudo random number [0,1] for ray offsets to avoid moire patterns
    float iptr;
    float rand = fract(sin(dot(convert_float2(globalId),
                       (float2)(12.9898f, 78.233f))) * 43758.5453f, &iptr);

    float maxRes = (float)max(get_image_dim(volData).x, get_image_dim(volData).z);

    int2 texCoords = globalId;
    float aspectRatio = native_divide((float)get_global_size(1), (float)(get_global_size(0)));
    aspectRatio = min(aspectRatio, native_divide((float)get_global_size(0), (float)(get_global_size(1))));
    int maxSize = max(get_global_size(0), get_global_size(1));
    float2 imgCoords;
    imgCoords.x = native_divide((globalId.x + 0.5f), convert_float(maxSize)) * 2.f;
    imgCoords.y = native_divide((globalId.y + 0.5f), convert_float(maxSize)) * 2.f;
    // calculate correct offset based on aspect ratio
    imgCoords -= get_global_size(0) > get_global_size(1) ?
                        (float2)(1.0f, aspectRatio) : (float2)(aspectRatio, 1.0);
    imgCoords.y *= -1.f;   // flip y coord

    // z position of view plane is -1.0 to fit the cube to the screen quad when axes are aligned,
    // zoom is -1 and the data set is uniform in each dimension
    // (with FoV of 90° and near plane in range [-1,+1]).
    float3 nearPlanePos = fast_normalize((float3)(imgCoords, -1.0f));
    // transform nearPlane from view space to world space
    float3 rayDir = (float3)(0.f);
    rayDir.x = dot(viewMat.s012, nearPlanePos);
    rayDir.y = dot(viewMat.s456, nearPlanePos);
    rayDir.z = dot(viewMat.s89a, nearPlanePos);

    // camera position in world space (ray origin) is translation vector of view matrix
    float3 camPos = viewMat.s37b*modelScale;

    if (orthoCam)
    {
        camPos = (float3)(viewMat.s37b);
        float3 viewPlane_x = viewMat.s048;
        float3 viewPlane_y = viewMat.s159;
        float3 viewPlane_z = viewMat.s26a;
        rayDir = -viewPlane_z;
        nearPlanePos = camPos + imgCoords.x*viewPlane_x + imgCoords.y*viewPlane_y;
        nearPlanePos *= length(camPos);
        camPos = nearPlanePos * modelScale;
    }
    rayDir = fast_normalize(rayDir*modelScale);

    float tnear = FLT_MIN;
    float tfar = FLT_MAX;
    int hit = 0;
    // bbox from (-1,-1,-1) to (+1,+1,+1)
    hit = intersectBox(camPos, rayDir, &tnear, &tfar);
    if (!hit || tfar < 0)
    {
        write_imagef(outData, texCoords, background);
        return;
    }

    float sampleDist = tfar - tnear;
    if (sampleDist <= 0.f)
        return;
    int3 volRes = get_image_dim(volData).xyz;
    float stepSize = min(sampleDist, sampleDist /
                            (samplingRate*length(sampleDist*rayDir*convert_float3(volRes))));
    float samples = ceil(sampleDist/stepSize);
    stepSize = sampleDist/samples;
    float offset = stepSize*rand*0.9f; // offset by 'random' distance to avoid moiré pattern

    // raycast parameters
    tnear = max(0.f, tnear);    // clamp to near plane
    float4 result = background;
    float alpha = 0.f;
    float3 pos = (float3)(0);
    float density = 0.f;
    float4 tfColor = (float4)(0);
    float opacity = 0.f;
    float t = tnear;

    float3 voxLen = (float3)(1.f) / convert_float3(volRes);
    float refSamplingInterval = 1.f / samplingRate;
    float precisionDiv = 1.f;
    if (get_image_channel_data_type(volData) == CLK_UNORM_INT16)
        precisionDiv = 8.f;

    float t_exit = tfar;

#ifdef ESS
    // 3D DDA initialization
    int3 bricksRes = get_image_dim(volBrickData).xyz;
    float3 brickLen = (float3)(1.f) / convert_float3(bricksRes);
    float3 invRay = 1.f/rayDir;
    int3 step = convert_int3(sign(rayDir));
    if (rayDir.x == 0.f)
    {
        invRay.x = FLT_MAX;
        step.x = 1;
    }
    if (rayDir.y == 0.f)
    {
        invRay.y = FLT_MAX;
        step.y = 1;
    }
    if (rayDir.z == 0.f)
    {
        invRay.z = FLT_MAX;
        step.z = 1;
    }
    float3 deltaT = convert_float3(step)*(brickLen*2.f*invRay);
    float3 voxIncr = (float3)0;

    // convert ray starting point to cell coordinates
    float3 rayOrigCell = (camPos + rayDir * tnear) - (float3)(-1.f);
    int3 cell = clamp(convert_int3(floor(rayOrigCell / (2.f*brickLen))),
                        (int3)(0), convert_int3(bricksRes.xyz) - 1);

    // add +1 to cells if ray dir component is negative: rayDir >= 0 ? (-1) : 0
    float3 tv = tnear + (convert_float3(cell - isgreaterequal(rayDir, (float3)(0)))
                            * (2.f*brickLen) - rayOrigCell) * invRay;
    int3 exit = step * bricksRes.xyz;
    if (exit.x < 0) exit.x = -1;
    if (exit.y < 0) exit.y = -1;
    if (exit.z < 0) exit.z = -1;
    // length of diagonal of a brick => longest distance through brick
    float brickDia = length(brickLen)*2.f;

    // 3D DDA loop over low-res grid for image order empty space skipping
    while (t < tfar)
    {
        float2 minMaxDensity = read_imagef(volBrickData, (int4)(cell, 0)).xy;

        // increment to next brick
        voxIncr.x = (tv.x <= tv.y) && (tv.x <= tv.z) ? 1 : 0;
        voxIncr.y = (tv.y <= tv.x) && (tv.y <= tv.z) ? 1 : 0;
        voxIncr.z = (tv.z <= tv.x) && (tv.z <= tv.y) ? 1 : 0;
        cell += convert_int3(voxIncr) * step;    // [0; res-1]

        t_exit = dot((float3)(1), tv * voxIncr);
        t_exit = clamp(t_exit, t+stepSize, t+brickDia);
        tv += voxIncr*deltaT;

        // skip bricks that contain only fully transparent voxels
        float alphaMax = read_imagef(tffData, linearSmp, minMaxDensity.y).w;
        if (alphaMax < 1e-6f)
        {
            uint prefixMin = read_imageui(tffPrefix, nearestSmp, minMaxDensity.x).x;
            uint prefixMax = read_imageui(tffPrefix, nearestSmp, minMaxDensity.y).x;
            if (prefixMin == prefixMax)
            {
                t = t_exit;
                continue;
            }
        }
#endif
        // standard raycasting loop
        while (t < t_exit)
        {
            pos = camPos + (t-offset)*rayDir;
            pos = pos * 0.5f + 0.5f;    // normalize to [0,1]

            float4 gradient = (float4)(0.f);
            if (illumType == 4)   // gradient magnitude based shading
            {
                gradient = -gradientCentralDiff(volData, (float4)(pos, 1.f));
                tfColor = read_imagef(tffData, linearSmp, -gradient.w);
            }
            else    // density based shading and optional illumination
            {
                density = useLinear ? read_imagef(volData,  linearSmp, (float4)(pos, 1.f)).x :
                                      read_imagef(volData, nearestSmp, (float4)(pos, 1.f)).x;
                density = clamp(density, 0.f, 1.f);
                tfColor = read_imagef(tffData, linearSmp, density);  // map density to color

                if (tfColor.w > 0.1f && illumType)
                {
                    if (illumType == 1)
                        gradient = -gradientCentralDiff(volData, (float4)(pos, 1.f));
                    else if (illumType == 2)
                        gradient = -gradientCentralDiffTff(volData, (float4)(pos, 1.f), tffData);
                    else if (illumType == 3)
                        gradient = -gradientSobel(volData, (float4)(pos, 1.f));

                    tfColor.xyz = illumination(volData, (float4)(pos, 1.f), tfColor.xyz, -rayDir,
                                               gradient.xyz);
                }
                if (tfColor.w > 0.1f && contours) // edge enhancement
                {
                    if (!illumType)
                        gradient = -gradientCentralDiff(volData, (float4)(pos, 1.f));
                    tfColor.xyz *= fabs(dot(rayDir, gradient.xyz));
                }
            }
            tfColor.xyz = background.xyz - tfColor.xyz;
            if (aerial) // depth cue as aerial perspective
            {
                float depthCue = 1.f - (t - tnear)/sampleDist; // [0..1]
                tfColor.w *= depthCue;
            }

            // Taylor expansion approximation
            opacity = 1.f - native_powr(1.f - tfColor.w, refSamplingInterval);
            result.xyz = result.xyz - tfColor.xyz * opacity * (1.f - alpha);
            alpha = alpha + opacity * (1.f - alpha);

            if (t >= tfar) break;
            if (alpha > ERT_THRESHOLD)   // early ray termination check
            {
                if (useAO)  // ambient occlusion only on solid surfaces
                {
                    float3 n = -gradientCentralDiff(volData, (float4)(pos, 1.f)).xyz;
                    float ao = calcAO(n, &taus, volData, pos, length(voxLen), length(voxLen)*5.f);
                    result.xyz *= 1.f - 0.3f*ao;
                }
                break;
            }
            t += stepSize;
        }

#ifdef ESS
        if (t >= tfar || alpha > ERT_THRESHOLD) break;
        if (any(cell == exit)) break;
        t = t_exit;
    }
#endif

    // draw bounding box / empty space skipping
    if (useBox)
    {
        if (checkBoundingBox(pos, voxLen, (float2)(0.f, 1.f)))
        {
            result.xyz = fabs((float3)(1.f) - background.xyz);
            alpha = 1.f;
        }
    }

    result.w = alpha;
    write_imagef(outData, texCoords, result);
}


/**
* direct volume raycasting kernel which draws everything inside or except a specified Rectangle around the cursor position
*/
__kernel void volumeRender(  __read_only image3d_t volData
                           , __read_only image3d_t volBrickData
                           , __read_only image1d_t tffData     // constant transfer function values
                           , __write_only image2d_t outData
                           , const float samplingRate
                           , const float16 viewMat
                           , const uint orthoCam
                           , const uint illumType
                           , const uint useBox
                           , const uint useLinear
                           , const float4 background
                           , __read_only image1d_t tffPrefix
                           , const uint useAO
                           , const float3 modelScale
                           , const uint contours
                           , const uint aerial
                           , const float2 cursorPos  // cursor Position mapped to [0,1] x [0,1], is also center of ellipse 1 and ellipse 2 but then it is not normalized
                           , const float2 rectangle  // rectangle extends mapped to [0,1] x [0,1], also used for ellipse 1, contains rx and ry, not normalized
                           , const float2 ell2 // vector used for ellipse 2, contains rx and ry, not normalized
                           , const uint invert  // if true (nonzero), draws everything outside the rectangle, else everything inside, tells in mode=1 which area is being drawn (A, B or C ad 2, 1 or 0)
                           , const float resolutionfactor // is used as g for mode: discard_dc, m is derived from the mode and from the texture width, ell1 and ell2 rx.
                           , const uint mode
                           )
{
    int2 globalId = (int2)(get_global_id(0), get_global_id(1));
    if(any(globalId >= get_image_dim(outData)))
        return;
    int maxSize = max(get_global_size(0), get_global_size(1));
    float2 texCoords_nlzd = (float2)(convert_float_rtp(globalId.x) / convert_float_rtp(get_global_size(0)), convert_float_rtp(globalId.y) / convert_float_rtp(get_global_size(1)));

    int2 img_bounds = get_image_dim(outData); //(int2)(get_global_size(0), get_global_size(1));

    // check if imageCoord is in valid area according to cursoPos and rectangle (and invert)
    // shift rect that cursor is in its middle
    switch(mode){ // early discard here
        case 0: // Standard
                break;
        case 1: // distance dependent discarding
                // distance will be read from rectangle or ell1
                switch(invert){
                    case 0: // Area C
                        // img_bounds = get_image_dim(outData);
                        maxSize = max(img_bounds.x, img_bounds.y);
                        globalId = old_2d_to_new_2d_coord(globalId, img_bounds.x, round(resolutionfactor), get_global_size(0));
                        

                        // discard if inside ell2
                        if(checkPointInEllipse(cursorPos, ell2.x / 2, ell2.y / 2, convert_float2_rtz(globalId))) return;
                        break;
                    case 1: // Area B
                        maxSize = max(img_bounds.x, img_bounds.y);
                        globalId = old_2d_to_new_2d_coord(globalId, round(ell2.x), round(resolutionfactor), get_global_size(0));
                        globalId += convert_int2_rtz(cursorPos) - (int2)(0.5 * ell2.x, 0.5 * ell2.y);

                        // discard if in ell1
                        if(checkPointInEllipse(cursorPos, rectangle.x / 2, rectangle.y / 2, convert_float2_rtz(globalId))) return;

                        // discard outside ell2
                        if(!checkPointInEllipse(cursorPos, ell2.x / 2, ell2.y / 2, convert_float2_rtz(globalId))) return;
                        break;
                    case 2: // Area A
                        maxSize = max(img_bounds.x, img_bounds.y);
                        globalId = old_2d_to_new_2d_coord(globalId, round(rectangle.x), round(resolutionfactor), get_global_size(0));
                        globalId += convert_int2_rtz(cursorPos) - (int2)(0.5 * rectangle.x, 0.5 * rectangle.y);

                        // discard outside ell1
                        if(!checkPointInEllipse(cursorPos, rectangle.x / 2, rectangle.y / 2, convert_float2_rtz(globalId))) return;
                        break;
                    default:
                        break;
                }

                // discard if out of range
                if(any(globalId >= get_image_dim(outData)))
                    return;
                
                break;
        case 2: // discard with rect
                if(checkPointInRectangle(cursorPos - 0.5f * rectangle, rectangle, texCoords_nlzd)){
                    if(invert != 0){
                        write_imagef(outData, globalId, (float4)(0.0, 0.0, 0.0, 0.0));
                        return;
                      } 
                }else{
                    if(invert == 0){
                        write_imagef(outData, globalId, (float4)(0.0, 0.0, 0.0, 0.0));
                        return;
                    }
                }
                break;
        case 3: // sinus resolution
                break;
        
        default: // Standard
                break;
    }

    
    float2 imgCoords;
    imgCoords.x = native_divide((globalId.x + 0.5f), convert_float(maxSize)) * 2.f;
    imgCoords.y = native_divide((globalId.y + 0.5f), convert_float(maxSize)) * 2.f;

    uint4 taus = initRNG();
    // pseudo random number [0,1] for ray offsets to avoid moire patterns
    float iptr;
    float rand = fract(sin(dot(convert_float2(globalId),
                       (float2)(12.9898f, 78.233f))) * 43758.5453f, &iptr);

    float maxRes = (float)max(get_image_dim(volData).x, get_image_dim(volData).z);

    int2 texCoords = globalId;
    float aspectRatio = native_divide((float)(img_bounds.y), (float)(img_bounds.x));
    aspectRatio = min(aspectRatio, native_divide((float)(img_bounds.x), (float)(img_bounds.y)));
    
    // calculate correct offset based on aspect ratio
    imgCoords -= img_bounds.x > img_bounds.y ?
                        (float2)(1.0f, aspectRatio) : (float2)(aspectRatio, 1.0);
    imgCoords.y *= -1.f;   // flip y coord

    // z position of view plane is -1.0 to fit the cube to the screen quad when axes are aligned,
    // zoom is -1 and the data set is uniform in each dimension
    // (with FoV of 90° and near plane in range [-1,+1]).
    float3 nearPlanePos = fast_normalize((float3)(imgCoords, -1.0f));
    // transform nearPlane from view space to world space
    float3 rayDir = (float3)(0.f);
    rayDir.x = dot(viewMat.s012, nearPlanePos);
    rayDir.y = dot(viewMat.s456, nearPlanePos);
    rayDir.z = dot(viewMat.s89a, nearPlanePos);

    // camera position in world space (ray origin) is translation vector of view matrix
    float3 camPos = viewMat.s37b*modelScale;

    if (orthoCam)
    {
        camPos = (float3)(viewMat.s37b);
        float3 viewPlane_x = viewMat.s048;
        float3 viewPlane_y = viewMat.s159;
        float3 viewPlane_z = viewMat.s26a;
        rayDir = -viewPlane_z;
        nearPlanePos = camPos + imgCoords.x*viewPlane_x + imgCoords.y*viewPlane_y;
        nearPlanePos *= length(camPos);
        camPos = nearPlanePos * modelScale;
    }
    rayDir = fast_normalize(rayDir*modelScale);

    float tnear = FLT_MIN;
    float tfar = FLT_MAX;
    int hit = 0;
    // bbox from (-1,-1,-1) to (+1,+1,+1)
    hit = intersectBox(camPos, rayDir, &tnear, &tfar);
    if (!hit || tfar < 0)
    {
        switch(mode){ // no hit, set background
        case 0: // Standard
                write_imagef(outData, texCoords, background);
                break;
        case 1: // distance dependent
                /*if(distance(texCoords_nlzd, cursorPos) > 0.1f){ // outside the range: discard every second ray
                    if(globalId.x % 2 == 0 && globalId.y % 2 == 0){ // not blacked out, also write the other three texel
						write_imagef_with_bounds_check(img_bounds, outData, (int2)(texCoords.x, texCoords.y - 1) , background); // bottom
                        write_imagef_with_bounds_check(img_bounds, outData, texCoords - (int2)(1,1), background); // bottom right
                        write_imagef_with_bounds_check(img_bounds, outData, (int2)(texCoords.x - 1, texCoords.y), background); // right
                    }
                }else{
                    // inside the range, behave normally
                    
                }*/
				write_imagef(outData, texCoords, background);
                break;
        case 2: // discard with rect
                write_imagef(outData, texCoords, background);
                break;
        case 3: // sinus resolution
                write_imagef(outData, texCoords, background);
                break;
        
        default: // Standard
                write_imagef(outData, texCoords, background);
                break;
        }
        return;
    }

    float sampleDist = tfar - tnear;
    if (sampleDist <= 0.f)
        return;
    int3 volRes = get_image_dim(volData).xyz;
    float stepSize = min(sampleDist, sampleDist /
                            (samplingRate*length(sampleDist*rayDir*convert_float3(volRes))));
    float samples = ceil(sampleDist/stepSize);
    stepSize = sampleDist/samples;
    float offset = stepSize*rand*0.9f; // offset by 'random' distance to avoid moiré pattern

    // raycast parameters
    tnear = max(0.f, tnear);    // clamp to near plane
    float4 result = background;
    float alpha = 0.f;
    float3 pos = (float3)(0);
    float density = 0.f;
    float4 tfColor = (float4)(0);
    float opacity = 0.f;
    float t = tnear;

    float3 voxLen = (float3)(1.f) / convert_float3(volRes);
    float refSamplingInterval = 1.f / samplingRate;
    float precisionDiv = 1.f;
    if (get_image_channel_data_type(volData) == CLK_UNORM_INT16)
        precisionDiv = 8.f;

    float t_exit = tfar;

#ifdef ESS
    // 3D DDA initialization
    int3 bricksRes = get_image_dim(volBrickData).xyz;
    float3 brickLen = (float3)(1.f) / convert_float3(bricksRes);
    float3 invRay = 1.f/rayDir;
    int3 step = convert_int3(sign(rayDir));
    if (rayDir.x == 0.f)
    {
        invRay.x = FLT_MAX;
        step.x = 1;
    }
    if (rayDir.y == 0.f)
    {
        invRay.y = FLT_MAX;
        step.y = 1;
    }
    if (rayDir.z == 0.f)
    {
        invRay.z = FLT_MAX;
        step.z = 1;
    }
    float3 deltaT = convert_float3(step)*(brickLen*2.f*invRay);
    float3 voxIncr = (float3)0;

    // convert ray starting point to cell coordinates
    float3 rayOrigCell = (camPos + rayDir * tnear) - (float3)(-1.f);
    int3 cell = clamp(convert_int3(floor(rayOrigCell / (2.f*brickLen))),
                        (int3)(0), convert_int3(bricksRes.xyz) - 1);

    // add +1 to cells if ray dir component is negative: rayDir >= 0 ? (-1) : 0
    float3 tv = tnear + (convert_float3(cell - isgreaterequal(rayDir, (float3)(0)))
                            * (2.f*brickLen) - rayOrigCell) * invRay;
    int3 exit = step * bricksRes.xyz;
    if (exit.x < 0) exit.x = -1;
    if (exit.y < 0) exit.y = -1;
    if (exit.z < 0) exit.z = -1;
    // length of diagonal of a brick => longest distance through brick
    float brickDia = length(brickLen)*2.f;

    // 3D DDA loop over low-res grid for image order empty space skipping
    while (t < tfar)
    {
        float2 minMaxDensity = read_imagef(volBrickData, (int4)(cell, 0)).xy;

        // increment to next brick
        voxIncr.x = (tv.x <= tv.y) && (tv.x <= tv.z) ? 1 : 0;
        voxIncr.y = (tv.y <= tv.x) && (tv.y <= tv.z) ? 1 : 0;
        voxIncr.z = (tv.z <= tv.x) && (tv.z <= tv.y) ? 1 : 0;
        cell += convert_int3(voxIncr) * step;    // [0; res-1]

        t_exit = dot((float3)(1), tv * voxIncr);
        t_exit = clamp(t_exit, t+stepSize, t+brickDia);
        tv += voxIncr*deltaT;

        // skip bricks that contain only fully transparent voxels
        float alphaMax = read_imagef(tffData, linearSmp, minMaxDensity.y).w;
        if (alphaMax < 1e-6f)
        {
            uint prefixMin = read_imageui(tffPrefix, nearestSmp, minMaxDensity.x).x;
            uint prefixMax = read_imageui(tffPrefix, nearestSmp, minMaxDensity.y).x;
            if (prefixMin == prefixMax)
            {
                t = t_exit;
                continue;
            }
        }
#endif
        // standard raycasting loop
        while (t < t_exit)
        {
            pos = camPos + (t-offset)*rayDir;
            pos = pos * 0.5f + 0.5f;    // normalize to [0,1]

            float4 gradient = (float4)(0.f);
            if (illumType == 4)   // gradient magnitude based shading
            {
                gradient = -gradientCentralDiff(volData, (float4)(pos, 1.f));
                tfColor = read_imagef(tffData, linearSmp, -gradient.w);
            }
            else    // density based shading and optional illumination
            {
                density = useLinear ? read_imagef(volData,  linearSmp, (float4)(pos, 1.f)).x :
                                      read_imagef(volData, nearestSmp, (float4)(pos, 1.f)).x;
                density = clamp(density, 0.f, 1.f);
                tfColor = read_imagef(tffData, linearSmp, density);  // map density to color

                if (tfColor.w > 0.1f && illumType)
                {
                    if (illumType == 1)
                        gradient = -gradientCentralDiff(volData, (float4)(pos, 1.f));
                    else if (illumType == 2)
                        gradient = -gradientCentralDiffTff(volData, (float4)(pos, 1.f), tffData);
                    else if (illumType == 3)
                        gradient = -gradientSobel(volData, (float4)(pos, 1.f));

                    tfColor.xyz = illumination(volData, (float4)(pos, 1.f), tfColor.xyz, -rayDir,
                                               gradient.xyz);
                }
                if (tfColor.w > 0.1f && contours) // edge enhancement
                {
                    if (!illumType)
                        gradient = -gradientCentralDiff(volData, (float4)(pos, 1.f));
                    tfColor.xyz *= fabs(dot(rayDir, gradient.xyz));
                }
            }
            tfColor.xyz = background.xyz - tfColor.xyz;
            if (aerial) // depth cue as aerial perspective
            {
                float depthCue = 1.f - (t - tnear)/sampleDist; // [0..1]
                tfColor.w *= depthCue;
            }

            // Taylor expansion approximation
            opacity = 1.f - native_powr(1.f - tfColor.w, refSamplingInterval);
            result.xyz = result.xyz - tfColor.xyz * opacity * (1.f - alpha);
            alpha = alpha + opacity * (1.f - alpha);

            if (t >= tfar) break;
            if (alpha > ERT_THRESHOLD)   // early ray termination check
            {
                if (useAO)  // ambient occlusion only on solid surfaces
                {
                    float3 n = -gradientCentralDiff(volData, (float4)(pos, 1.f)).xyz;
                    float ao = calcAO(n, &taus, volData, pos, length(voxLen), length(voxLen)*5.f);
                    result.xyz *= 1.f - 0.3f*ao;
                }
                break;
            }
            t += stepSize;
        }

#ifdef ESS
        if (t >= tfar || alpha > ERT_THRESHOLD) break;
        if (any(cell == exit)) break;
        t = t_exit;
    }
#endif

    // draw bounding box / empty space skipping
    if (useBox)
    {
        if (checkBoundingBox(pos, voxLen, (float2)(0.f, 1.f)))
        {
            result.xyz = fabs((float3)(1.f) - background.xyz);
            alpha = 1.f;
        }
    }

    result.w = alpha;
    // if(invert == 0) result.xyz += 0.5f;
    switch(mode){ // result
        case 0: // Standard
                write_imagef(outData, texCoords, result);
                break;
        case 1: // distance dependent
                /*if(distance(texCoords_nlzd, cursorPos) > 0.1f){ // outside the range: discard every second ray
                    if(globalId.x % 2== 0 && globalId.y % 2 == 0){ // not blacked out, also write the other three texel
                        write_imagef_with_bounds_check(img_bounds, outData, (int2)(texCoords.x, texCoords.y - 1) , result); // bottom
                        write_imagef_with_bounds_check(img_bounds, outData, texCoords - (int2)(1,1), result); // bottom right
                        write_imagef_with_bounds_check(img_bounds, outData, (int2)(texCoords.x - 1, texCoords.y), result); // right
                    }
                }else{
                    // inside the range, behave normally
                    // check if still needed to write to lower, lower right and right because those are discards
                    if(texCoords.x % 2== 1 || texCoords.y - 1 % 2 == 1){ // also write the other texel
                        write_imagef_with_bounds_check(img_bounds, outData, (int2)(texCoords.x, texCoords.y - 1) , result); // bottom
                    }
                    if(texCoords.x -1 % 2== 1 || texCoords.y -1 % 2 == 1){ // also write the other texel
                        write_imagef_with_bounds_check(img_bounds, outData, texCoords - (int2)(1,1), result); // bottom right
                    }
                    if(texCoords.x - 1 % 2 == 1 || texCoords.y % 2 == 1){ // also write the other texel
                        write_imagef_with_bounds_check(img_bounds, outData, (int2)(texCoords.x - 1, texCoords.y), result); // right
                    }
                }*/
				write_imagef(outData, texCoords, result);
                break;
        case 2: // discard with rect
                write_imagef(outData, texCoords, result);
                break;
        case 3: // sinus resolution
                write_imagef(outData, texCoords, result);
                break;
        
        default: // Standard
                write_imagef(outData, texCoords, result);
                break;
        }
}

//************************** Interpolate Pixels ***************************
__kernel void interpolateTexelsFromDDC(   __read_only image2d_t inData  // data to interpolate
                                        , __write_only image2d_t outData    // interpolatet data
                                        , const uint2 g_values  // x is g for ell1, y is g for ell2
                                        , const float2 cursorPos // cursor position
                                        , const float2 ell1 // rx, ry for ell1
                                        , const float2 ell2 // rx, ry for ell2
                                        )
{
    // TODO: interpolate data. Note: to interpolate correctly: need to know the area one is in.
    return;
}

#pragma OPENCL EXTENSION cl_khr_3d_image_writes : enable

//************************** Generate brick volume ***************************

__kernel void generateBricks(  __read_only image3d_t volData
                             , __write_only image3d_t volBrickData
                            )
{
    int3 coord = (int3)(get_global_id(0), get_global_id(1), get_global_id(2));
    if(any(coord >= get_image_dim(volBrickData).xyz))
        return;

    int3 voxPerCell = convert_int3(ceil(convert_float4(get_image_dim(volData))/
                                          convert_float4(get_image_dim(volBrickData))).xyz);
    int3 volCoordLower = voxPerCell * coord;
    int3 volCoordUpper = clamp(volCoordLower + voxPerCell, (int3)(0), get_image_dim(volData).xyz);

    float maxVal = 0.f;
    float minVal = 1.f;
    float value = 0.f;

    for (int k = volCoordLower.z; k < volCoordUpper.z; ++k)
    {
        for (int j = volCoordLower.y; j < volCoordUpper.y; ++j)
        {
            for (int i = volCoordLower.x; i < volCoordUpper.x; ++i)
            {
                value = read_imagef(volData, nearestIntSmp, (int4)(i, j, k, 0)).x;  // [0; 1]
                minVal = min(minVal, value);
                maxVal = max(maxVal, value);
            }
        }
    }

    write_imagef(volBrickData, (int4)(coord, 0), (float4)(minVal, maxVal, 0, 1.f));
}


//************************** Downsample volume ***************************

__kernel void downsampling(  __read_only image3d_t volData
                           , __write_only image3d_t volDataLowRes
                          )
{
    int3 coord = (int3)(get_global_id(0), get_global_id(1), get_global_id(2));
    if(any(coord >= get_image_dim(volDataLowRes).xyz))
        return;

    int3 voxPerCell = convert_int3(ceil(convert_float4(get_image_dim(volData))/
                                          convert_float4(get_image_dim(volDataLowRes))).xyz);
    int3 volCoordLower = voxPerCell * coord;
    int3 volCoordUpper = clamp(volCoordLower + voxPerCell, (int3)(0), get_image_dim(volData).xyz);

    float value = 0.f;

    for (int k = volCoordLower.z; k < volCoordUpper.z; ++k)
    {
        for (int j = volCoordLower.y; j < volCoordUpper.y; ++j)
        {
            for (int i = volCoordLower.x; i < volCoordUpper.x; ++i)
            {
                value += read_imagef(volData, nearestIntSmp, (int4)(i, j, k, 0)).x;  // [0; 1]
            }
        }
    }
    value /= (float)(voxPerCell.x * voxPerCell.y * voxPerCell.z);

    write_imagef(volDataLowRes, (int4)(coord, 0), (float4)(value));
}
