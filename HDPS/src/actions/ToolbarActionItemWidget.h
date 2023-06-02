#pragma once

#include "util/WidgetFader.h"

#include "ToolbarActionItem.h"

#include <QWidget>
#include <QPointer>

namespace hdps::gui {

/**
 * Toolbar action item widget class
 *
 * @author Thomas Kroes
 */
class ToolbarActionItemWidget final : public QWidget
{
    Q_OBJECT

protected:

    class StateWidget : public QWidget
    {
    public:
        StateWidget(QWidget* parent, WidgetAction* action, const ToolbarActionItem::State& state);
    };

protected:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param toolbarActionItem Reference to owning toolbar action item
     */
    ToolbarActionItemWidget(QWidget* parent, ToolbarActionItem& toolbarActionItem);

    /**
     * Respond to \p target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

private:

    /**
     * Synchronize widget size for \p state (updates the size in the action item)
     * @param state State to synchronize the widget size for
     */
    void synchronizeWidgetSize(const ToolbarActionItem::State& state);

private:
    ToolbarActionItem&  _toolbarActionItem;     /** Reference to owning toolbar action item */
    QPointer<QWidget>   _collapsedWidget;       /** Pointer to collapsed widget */
    QPointer<QWidget>   _expandedWidget;        /** Pointer to expanded widget */
    //util::WidgetFader   _collapsedWidgetFader;  
    //util::WidgetFader   _expandedWidgetFader;  

    friend class ToolbarActionItem;
};

}