/**
 * Copyright 2017-2020 Stefan Ascher
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include "BoundingBox.h"
#include "Shape.h"
#include <eastl.hpp>

namespace Math {

class HeightMap;

class ConvexHull : public Shape
{
private:
    void BuildHull(const ea::vector<Vector3>& vertices);
public:
    ConvexHull() = default;
    ConvexHull(const ConvexHull& other) :
        Shape(other),
        boundingBox_(other.boundingBox_)
    {}
    ConvexHull(ConvexHull&& other) noexcept :
        Shape(other),
        boundingBox_(std::move(other.boundingBox_))
    {}
    explicit ConvexHull(const ea::vector<Vector3>& vertices);
    ~ConvexHull() = default;

    ConvexHull& operator= (const ConvexHull& other)
    {
        if (this != &other)
        {
            vertexData_ = other.vertexData_;
            vertexCount_ = other.vertexCount_;
            indexData_ = other.indexData_;
            indexCount_ = other.indexCount_;
        }
        return *this;
    }
    ConvexHull& operator= (ConvexHull&& other) noexcept
    {
        vertexData_ = std::move(other.vertexData_);
        vertexCount_ = other.vertexCount_;
        indexData_ = std::move(other.indexData_);
        indexCount_ = other.indexCount_;
        boundingBox_ = std::move(other.boundingBox_);
        return *this;
    }

    BoundingBox GetBoundingBox() const
    {
        return boundingBox_;
    }
    Intersection IsInside(const Vector3& point) const;
    ConvexHull Transformed(const Matrix4& transform) const;
    bool Collides(const Sphere& b2, const Vector3& velocity, Vector3& move) const;
    bool Collides(const BoundingBox& b2, const Vector3& velocity, Vector3& move) const;
    bool Collides(const ConvexHull& b2, const Vector3& velocity, Vector3& move) const;
    bool Collides(const HeightMap& b2, const Vector3& velocity, Vector3& move) const;

    Shape GetShape() const { return *this; }

    BoundingBox boundingBox_;
};

}
