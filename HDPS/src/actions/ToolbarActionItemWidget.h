#pragma once

#include "util/WidgetFader.h"

#include <QWidget>
#include <QPointer>

namespace hdps::gui {

class ToolbarActionItem;

/**
 * Toolbar action item widget class
 *
 * @author Thomas Kroes
 */
//class ToolbarActionItemWidget final : public QWidget
//{
//    Q_OBJECT
//
//protected:
//
//    /**
//     * Constructor
//     * @param parent Pointer to parent widget
//     * @param toolbarActionItem Reference to owning toolbar action item
//     */
//    ToolbarActionItemWidget(QWidget* parent, ToolbarActionItem& toolbarActionItem);
//
//private:
//    ToolbarActionItem&  _toolbarActionItem;     /** Reference to owning toolbar action item */
//    QPointer<QWidget>   _collapsedWidget;       /** Pointer to collapsed widget */
//    QPointer<QWidget>   _expandedWidget;        /** Pointer to expanded widget */
//    //util::WidgetFader   _collapsedWidgetFader;  
//    //util::WidgetFader   _expandedWidgetFader;  
//
//    friend class ToolbarActionItem;
//};

}