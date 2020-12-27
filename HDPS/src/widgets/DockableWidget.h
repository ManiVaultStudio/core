#pragma once

#include <QDockWidget>

class QWidget;
class QGridLayout;

namespace hdps
{
namespace gui
{

class DockableWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit DockableWidget(QWidget *parent = 0);
    ~DockableWidget() override;

    void addWidget(QWidget* widget);
    QWidget* getWidget();
    void setMainLayout(QLayout* layout);
    QLayout* mainLayout();

    public slots :
    bool setVisibility(bool visible);
    bool toggleVisibility();

protected:
    bool _isVisible;

    QLayout* _mainLayout;

private:
    QWidget* _mainWidget;
};

} // namespace gui

} // namespace hdps
