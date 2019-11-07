#pragma once

#include <QSplitter>

namespace hdps
{
    namespace plugin
    {
        class ViewPlugin;
    }
    namespace gui
    {
        class CentralWidget : public QSplitter
        {
        public:
            void addView(plugin::ViewPlugin* plugin);
        private:
        };
    }
}
