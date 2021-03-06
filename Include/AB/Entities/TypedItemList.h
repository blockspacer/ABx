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

#include <AB/Entities/Entity.h>
#include <AB/Entities/Item.h>

namespace AB {
namespace Entities {

static constexpr auto KEY_TYPED_ITEMLIST = "typed_item_list";

struct TypedListItem
{
    std::string uuid;
    ItemType belongsTo = ItemType::Unknown;
    float chance = 0.0f;
};

/// Item by type on a certain map with drop chances. UUID is the map uuid.
struct TypedItemList : Entity
{
    static constexpr const char* KEY()
    {
        return KEY_TYPED_ITEMLIST;
    }
    template<typename S>
    void serialize(S& s)
    {
        s.ext(*this, BaseClass<Entity>{});
        s.value2b(type);
        s.container(items, Limits::MAX_ITEMS, [&s](TypedListItem& c)
        {
            s.text1b(c.uuid, Limits::MAX_UUID);
            s.value2b(c.belongsTo);
            s.value4b(c.chance);
        });
    }

    ItemType type = ItemType::Unknown;
    std::vector<TypedListItem> items;
};

/// All insignias
struct TypedItemsInsignia : TypedItemList
{
    static constexpr const char* KEY()
    {
        return "insignia_item_list";
    }
    TypedItemsInsignia() :
        TypedItemList()
    {
        type = ItemType::ModifierInsignia;
    }
};

/// All runes
struct TypedItemsRunes : TypedItemList
{
    static constexpr const char* KEY()
    {
        return "runes_item_list";
    }
    TypedItemsRunes() :
        TypedItemList()
    {
        type = ItemType::ModifierRune;
    }
};

struct TypedItemsWeaponPrefix : TypedItemList
{
    static constexpr const char* KEY()
    {
        return "weapon_prefix_item_list";
    }
    TypedItemsWeaponPrefix() :
        TypedItemList()
    {
        type = ItemType::ModifierWeaponPrefix;
    }
};

struct TypedItemsWeaponSuffix : TypedItemList
{
    static constexpr const char* KEY()
    {
        return "weapon_suffix_item_list";
    }
    TypedItemsWeaponSuffix() :
        TypedItemList()
    {
        type = ItemType::ModifierWeaponSuffix;
    }
};

struct TypedItemsWeaponInscription : TypedItemList
{
    static constexpr const char* KEY()
    {
        return "weapon_inscrition_item_list";
    }
    TypedItemsWeaponInscription() :
        TypedItemList()
    {
        type = ItemType::ModifierWeaponInscription;
    }
};

}
}
