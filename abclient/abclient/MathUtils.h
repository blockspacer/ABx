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

#define _USE_MATH_DEFINES
#include <cmath>

inline void NormalizeAngle(float& angle)
{
    angle = fmodf(angle, 360.0f);
    angle = fmodf(angle + 360.0f, 360.0f);
}

inline float NormalizedAngle(float angle)
{
    float result = angle;
    NormalizeAngle(result);
    return result;
}

template <typename T>
inline T DegToRad(T deg)
{
    return -deg * (static_cast<T>(M_PI / 180.0));
}

template <typename T>
inline T RadToDeg(T rad)
{
    return (-rad / static_cast<T>(M_PI)) * (static_cast<T>(180.0));
}
