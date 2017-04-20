#ifndef HDPS_GUI_DOCKABLE_WIDGET
#define HDPS_GUI_DOCKABLE_WIDGET

#include <QDockWidget>

class QWidget;
class QGridLayout;

namespace hdps {

namespace gui {

class DockableWidget : public QDockWidget
{
	Q_OBJECT
public:
	explicit DockableWidget(QWidget *parent = 0);
	~DockableWidget();

	QGridLayout* mainLayout();

public slots :
	bool setVisibility(bool visible);
	bool toggleVisibility();

protected:

private:

public:

protected:
	bool _isVisible;

	QGridLayout* _mainLayout;

private:
	QWidget* _mainWidget;
};

} // namespace gui

} // namespace hdps

#endif // HDPS_GUI_DOCKABLE_WIDGET
