#pragma once

#include <actions/WidgetAction.h>
#include <actions/ToggleAction.h>

class DataHierarchyFilterModel;

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
    DataHierarchyFilterAction(QObject* parent, DataHierarchyFilterModel& dataHierarchyFilterModel);

protected:
    ToggleAction& getShowHiddenAction() { return _showHiddenAction; }

private:
    void updateFilterModel();

private:
    DataHierarchyFilterModel&   _dataHierarchyFilterModel;      /** Reference to data hierarchy filter model owned by the data hierarchy widget */
    ToggleAction                _showHiddenAction;              /** Show hidden datasets action */
};