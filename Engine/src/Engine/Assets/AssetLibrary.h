#pragma once
#include <string>
#include <unordered_map>
#include "../EngineAssert.h"

namespace Engine
{

    template <typename T>
    class AssetLibrary
    {
    public:
        void Register(const std::string& id, T item)
        {
            ENGINE_ASSERT(!m_items.contains(id), std::format("Asset '{}' is already in library", id));
            m_items[id] = item;
        }

        T Get(const std::string& id) const
        {
            ENGINE_ASSERT(m_items.contains(id), std::format("Asset '{}' is not in library", id));
            return m_items.at(id);
        }

        bool Contains(const std::string& id) const
        {
            return m_items.contains(id);
        }

        std::unordered_map<std::string, T>& GetItems()
        {
            return m_items;
        }

    private:
        std::unordered_map<std::string, T> m_items;
    };
}

