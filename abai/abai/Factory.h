#pragma once

#include <memory>
#include <map>
#include "AiDefines.h"

namespace AI {

template <typename T>
class AbstractFactory
{
public:
    virtual ~AbstractFactory() = default;
    virtual std::shared_ptr<T> Create(const ArgumentsType& arguments) const = 0;
};

template <typename Key, typename T>
class FactoryRegistry
{
protected:
    using FactoryMap = std::map<const Key, const AbstractFactory<T>*>;
    FactoryMap factories_;
public:
    FactoryRegistry() = default;
    FactoryRegistry(const FactoryRegistry&) = delete;
    FactoryRegistry& operator= (const FactoryRegistry&) = delete;

    bool RegisterFactory(const Key& key, const AbstractFactory<T>& factory)
    {
        const auto it = factories_.find(key);
        if (it != factories_.end())
            return false;

        factories_[key] = &factory;
        return true;
    }

    bool UnregisterFactory(const Key& key)
    {
        auto it = factories_.find(key);
        if (it == factories_.end())
            return false;
        factories_.erase(it);
        return true;
    }

    std::shared_ptr<T> Create(const Key& key, const ArgumentsType& arguments) const
    {
        const auto it = factories_.find(key);
        if (it == factories_.end())
            return std::shared_ptr<T>();

        const auto* factory = (*it).second;
        return factory->Create(arguments);
    }
};

}
