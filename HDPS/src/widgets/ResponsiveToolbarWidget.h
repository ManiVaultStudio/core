#pragma once

#include "actions/WidgetAction.h"
#include "actions/WidgetActionStateWidget.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QFrame>

namespace hdps
{

namespace gui
{

class WidgetActionStateWidget;

class ResponsiveToolbarWidget : public QWidget
{
public:
    class SpacerWidget : public QWidget {
    public:
        enum class Type {
            Divider,
            Spacer
        };

    public:
        SpacerWidget(const Type& type = Type::Divider);

        static Type getType(const WidgetActionWidget::State& widgetTypeLeft, const WidgetActionWidget::State& widgetTypeRight);
        static Type getType(const WidgetActionStateWidget* stateWidgetLeft, const WidgetActionStateWidget* stateWidgetRight);

        void setType(const Type& type);
        static std::int32_t getWidth(const Type& type);

    protected:
        Type            _type;
        QHBoxLayout     _layout;
        QFrame          _verticalLine;
    };

public:
    ResponsiveToolbarWidget(QWidget* parent);

    bool eventFilter(QObject* object, QEvent* event);

protected:

    /**
     * Add a widget to the toolbar
     * @param widget Widget to add
     * @param priority Expansion priority
     */
    //void addWidget(QWidget* widget, const std::int32_t& priority = 0);

    /**
     * Add a widget action to the toolbar
     * @param widgetAction Pointer to the widget action to add
     * @param priority Expansion priority
     */
    //void addWidget(WidgetAction* widgetAction, const std::int32_t& priority = 0);

private:

    /**
     * Updates the layout
     * Distri
     */
    void updateLayout();

protected:
    QHBoxLayout                 _layout;                /** Main layout */
    QWidget                     _toolBarWidget;         /** Toolbar widget */
    QHBoxLayout                 _toolBarLayout;         /** Toolbar layout */
    QVector<QWidget*>           _widgets;               /** Widgets to display, either regular or stateful widgets */
    QVector<SpacerWidget*>      _spacerWidgets;         /** Spacer widgets */

    friend class SettingsAction;
};

}
}
