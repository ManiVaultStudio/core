#pragma once

class QMenu;
class QWidget;

namespace mv::pointdata::developer
{
class PointDataPhantomGenerator final
{
public:
    static void populateMenu(QMenu& menu, QWidget* owner);
};
}
