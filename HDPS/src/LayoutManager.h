#pragma once

#include <QObject>

namespace hdps
{

namespace gui
{

class LayoutManager : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    LayoutManager(QObject* parent = nullptr);
};

}

}
