#pragma once

#include <QObject>
#include <QMap>
#include <QString>
#include <QDebug>

namespace hdps
{

namespace gui {
    class WidgetAction;
}

class ActionsManager : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    ActionsManager(QObject* parent = nullptr);

    /**
     * Add public action
     * @param publicAction Pointer to public action
     */
    void addPublicAction(gui::WidgetAction* publicAction);

    /**
     * Remove public action
     * @param publicAction Pointer to public action
     */
    void removePublicAction(gui::WidgetAction* publicAction);

private:
    QVector<gui::WidgetAction*>  _publicActions;        /** Shared pointers to public widget actions */
};

}
