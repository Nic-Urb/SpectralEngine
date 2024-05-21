//
//  Math.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 14.05.24.
//

#include "Math.hpp"

#include "raymath.h"

namespace Spectral::Math {

    Matrix3 InvertMatrix(Matrix3 mat)
    {
        Matrix3 inv = { 0 };
        
        // Cache matrix values (speed optimization)
        float a00 = mat.m[0][0], a01 = mat.m[0][1], a02 = mat.m[0][2];
        float a10 = mat.m[1][0], a11 = mat.m[1][1], a12 = mat.m[1][2];
        float a20 = mat.m[2][0], a21 = mat.m[2][1], a22 = mat.m[2][2];
        
        // Calculate the cofactors (determinants of 2x2 submatrices)
        float b00 = a11 * a22 - a12 * a21;
        float b01 = a02 * a21 - a01 * a22;
        float b02 = a01 * a12 - a02 * a11;
        float b10 = a12 * a20 - a10 * a22;
        float b11 = a00 * a22 - a02 * a20;
        float b12 = a02 * a10 - a00 * a12;
        float b20 = a10 * a21 - a11 * a20;
        float b21 = a01 * a20 - a00 * a21;
        float b22 = a00 * a11 - a01 * a10;
        
        float det = a00 * b00 + a10 * b01 + a20 * b02;
        
        if (det == 0) {
            return inv; // Inversion not possible
        }

        float invDet = 1.0f / det;

        inv.m[0][0] = b00 * invDet;
        inv.m[0][1] = b01 * invDet;
        inv.m[0][2] = b02 * invDet;
        inv.m[1][0] = b10 * invDet;
        inv.m[1][1] = b11 * invDet;
        inv.m[1][2] = b12 * invDet;
        inv.m[2][0] = b20 * invDet;
        inv.m[2][1] = b21 * invDet;
        inv.m[2][2] = b22 * invDet;

        return inv;
    }

    Vector3 Vector3Transform(Vector3 v, Matrix3 mat)
    {
        Vector3 result = { 0 };
        
        float x = v.x;
        float y = v.y;
        float z = v.z;
        
        result.x = mat.m[0][0]*x + mat.m[0][1]*y + mat.m[0][2]*z;
        result.y = mat.m[1][0]*x + mat.m[1][1]*y + mat.m[1][2]*z;
        result.z = mat.m[2][0]*x + mat.m[2][1]*y + mat.m[2][2]*z;
        
        return result;
    }

    void NormalizePlane(Vector4* plane)
    {
        if (!plane) {
            return;
        }
        
        float magnitude = sqrtf(plane->x * plane->x + plane->y * plane->y + plane->z * plane->z);
        
        plane->x /= magnitude;
        plane->y /= magnitude;
        plane->z /= magnitude;
        plane->w /= magnitude;
    }

    void ExtractFrustrum(Matrix projectionMatrix, Matrix viewMatrix, Frustum* frustum)
    {
        if (!frustum) {
            return;
        }
        
        Matrix comboMatrix = MatrixMultiply(viewMatrix, projectionMatrix);

        // Right clipping plane
        frustum->Planes[4] = (Vector4){
            comboMatrix.m3 - comboMatrix.m0,
            comboMatrix.m7 - comboMatrix.m4,
            comboMatrix.m11 - comboMatrix.m8,
            comboMatrix.m15 - comboMatrix.m12
        };
        
        // Left clipping plane
        frustum->Planes[5] = (Vector4){ 
            comboMatrix.m3 + comboMatrix.m0,
            comboMatrix.m7 + comboMatrix.m4,
            comboMatrix.m11 + comboMatrix.m8,
            comboMatrix.m15 + comboMatrix.m12
        };
        
        // Top clipping plane
        frustum->Planes[3] = (Vector4){ 
            comboMatrix.m3 - comboMatrix.m1,
            comboMatrix.m7 - comboMatrix.m5,
            comboMatrix.m11 - comboMatrix.m9,
            comboMatrix.m15 - comboMatrix.m13
        };
        
        // Bottom clipping plane
        frustum->Planes[2] = (Vector4){
            comboMatrix.m3 + comboMatrix.m1,
            comboMatrix.m7 + comboMatrix.m5,
            comboMatrix.m11 + comboMatrix.m9,
            comboMatrix.m15 + comboMatrix.m13
        };
        
        // Back clipping plane
        frustum->Planes[0] = (Vector4){
            comboMatrix.m3 - comboMatrix.m2,
            comboMatrix.m7 - comboMatrix.m6,
            comboMatrix.m11 - comboMatrix.m10,
            comboMatrix.m15 - comboMatrix.m14
        };
        
        // Front clipping plane
        frustum->Planes[1] = (Vector4){
            comboMatrix.m3 + comboMatrix.m2,
            comboMatrix.m7 + comboMatrix.m6,
            comboMatrix.m11 + comboMatrix.m10,
            comboMatrix.m15 + comboMatrix.m14
        };
        
        
        for (int i = 0; i < 6; i++) {
            NormalizePlane(&frustum->Planes[i]);
        }
        
    }
    
    Vector3 Intersection(Vector4 plane1, Vector4 plane2, Vector4 plane3)
    {
        Matrix3 mat;
        
        mat.m[0][0] = plane1.x; mat.m[0][1] = plane1.y; mat.m[0][2] = plane1.z;
        mat.m[1][0] = plane2.x; mat.m[1][1] = plane2.y; mat.m[1][2] = plane2.z;
        mat.m[2][0] = plane3.x; mat.m[2][1] = plane3.y; mat.m[2][2] = plane3.z;
        
        Vector3 v = (Vector3){ -plane1.w, -plane2.w, -plane3.w };
        
        Matrix3 invMat = InvertMatrix(mat);
        
        return Vector3Transform(v, invMat);
    }
    
}
