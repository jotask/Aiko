#pragma once

#include <aiko_types.h>

namespace aiko::editor
{
    class EditorDocument
    {
    public:
        const string& path() const { return m_path; }
        bool hasPath() const { return m_path.empty() == false; }

        bool isDirty() const { return m_dirty; }

        void setPath(string path) { m_path = std::move(path); }

        void markDirty() { m_dirty = true; }

        void markSaved() { m_dirty = false; }

        void reset()
        {
            m_path.clear();
            m_dirty = false;
        }

    private:
        string m_path;
        bool m_dirty = false;
    };
}
