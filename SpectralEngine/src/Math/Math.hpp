//
//  Math.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 14.05.24.
//
#pragma once

#include "raylib.h"

namespace Spectral::Math {

    struct Matrix3 {
        float m[3][3];
    };

    struct Frustum {
        Vector4 Planes[6];
    };

    // Matrix3 functions
    Matrix3 InvertMatrix(Matrix3 mat);
    Vector3 Vector3Transform(Vector3 v, Matrix3 mat);

    // Frustum functions
    // https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
    void NormalizePlane(Vector4* plane);
    void ExtractFrustrum(Matrix projectionMatrix, Matrix viewMatrix, Frustum* frustum);

    
    Vector3 Intersection(Vector4 plane1, Vector4 plane2, Vector4 plane3);
}
