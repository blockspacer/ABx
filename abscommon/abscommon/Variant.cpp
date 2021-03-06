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


#include "Variant.h"

namespace Utils {

const Variant Variant::Empty;

Variant& Variant::operator =(const Variant& other)
{
    SetType(other.GetType());

    switch (type_)
    {
    case VariantType::String:
        stringValue_.assign(other.stringValue_);
        break;
    default:
        value_ = other.value_;
        break;
    }
    return *this;
}

bool Variant::operator ==(const Variant& other) const
{
    if (type_ != other.type_)
        return false;
    switch (type_)
    {
    case VariantType::Int:
        return value_.intValue == other.value_.intValue;
    case VariantType::Boolean:
        return value_.boolValue == other.value_.boolValue;
    case VariantType::Float:
        return value_.floatValue + std::numeric_limits<float>::epsilon() >= other.value_.floatValue &&
            value_.floatValue - std::numeric_limits<float>::epsilon() <= other.value_.floatValue;
    case VariantType::Int64:
        return value_.int64Value == other.value_.int64Value;
    case VariantType::String:
        return stringValue_.compare(other.stringValue_) == 0;
    case VariantType::VoidPtr:
        return value_.ptrValue == other.value_.ptrValue;
    default:
        return true;
    }
}

std::string Variant::ToString() const
{
    switch (type_)
    {
    case VariantType::Int:
        return std::to_string(value_.intValue);
    case VariantType::Boolean:
        return std::to_string(value_.boolValue);
    case VariantType::Float:
        return std::to_string(value_.floatValue);
    case VariantType::Int64:
        return std::to_string(value_.int64Value);
    case VariantType::String:
        return stringValue_;
    case VariantType::VoidPtr:
    {
        char buff[100];
        int len = snprintf(buff, 100, "0x%p", value_.ptrValue);
        return std::string(buff, static_cast<size_t>(len));
    }
    default:
        return std::string();
    }
}

bool VariantMapRead(VariantMap& vMap, sa::PropReadStream& stream)
{
    vMap.clear();
    if (stream.GetSize() == 0)
        // Empty but OK
        return true;

    uint16_t count = 0;
    if (!stream.Read<uint16_t>(count))
        return false;

    for (uint16_t i = 0; i < count; ++i)
    {
        uint64_t stat = 0;
        if (!stream.Read<uint64_t>(stat))
            return false;

        uint8_t bt = 0;
        if (!stream.Read<uint8_t>(bt))
            return false;
        VariantType t = static_cast<VariantType>(bt);

        if (t == VariantType::None || t == VariantType::VoidPtr)
            continue;

        switch (t)
        {
        case VariantType::Int:
        {
            int value = 0;
            if (stream.Read<int>(value))
                vMap[static_cast<size_t>(stat)] = value;
            break;
        }
        case VariantType::Int64:
        {
            long long value = 0;
            if (stream.Read<long long>(value))
                vMap[static_cast<size_t>(stat)] = value;
            break;
        }
        case VariantType::Boolean:
        {
            uint8_t value = 0;
            if (stream.Read<uint8_t>(value))
                vMap[static_cast<size_t>(stat)] = value == 0 ? false : true;
            break;
        }
        case VariantType::Float:
        {
            float value = 0.0f;
            if (stream.Read<float>(value))
                vMap[static_cast<size_t>(stat)] = value;
            break;
        }
        case VariantType::String:
        {
            std::string value;
            if (stream.ReadString(value))
                vMap[static_cast<size_t>(stat)] = value;
            break;
        }
        default:
            break;
        }
    }
    return true;
}

void VariantMapWrite(const VariantMap& vMap, sa::PropWriteStream& stream)
{
    stream.Write<uint16_t>(static_cast<uint16_t>(vMap.size()));
    for (const auto& s : vMap)
    {
        Utils::VariantType t = s.second.GetType();
        if (t == VariantType::None || t == VariantType::VoidPtr)
            continue;

        stream.Write<uint64_t>(static_cast<uint64_t>(s.first));
        uint8_t bt = static_cast<uint8_t>(t);
        stream.Write<uint8_t>(bt);
        switch (t)
        {
        case VariantType::Int:
            stream.Write<int>(s.second.GetInt());
            break;
        case VariantType::Int64:
            stream.Write<long long>(s.second.GetInt64());
            break;
        case VariantType::Boolean:
            stream.Write<uint8_t>(s.second.GetBool() ? 1 : 0);
            break;
        case VariantType::Float:
            stream.Write<float>(s.second.GetFloat());
            break;
        case VariantType::String:
            stream.WriteString(s.second.GetString());
            break;
        default:
            break;
        }
    }
}

}
