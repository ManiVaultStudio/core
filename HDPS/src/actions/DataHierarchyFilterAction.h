#pragma once

#include <actions/WidgetAction.h>
#include <actions/ToggleAction.h>

using namespace hdps::gui;

class DataHierarchyFilterAction : public WidgetAction
{
public:

    class Widget : public WidgetActionWidget
    {
    protected:
        Widget(QWidget* parent, DataHierarchyFilterAction* dataHierarchyFilterAction, const std::int32_t& widgetFlags = 0);

    protected:
        friend class DataHierarchyFilterAction;
    };

protected:
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    };

public:
    DataHierarchyFilterAction(QObject* parent);

protected:
    ToggleAction& getShowHiddenAction() { return _showHiddenAction; }

private:
    ToggleAction    _showHiddenAction;      /** Show hidden datasets action */
};