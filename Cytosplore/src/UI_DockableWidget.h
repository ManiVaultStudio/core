#ifndef UI_DOCKABLE_WIDGET
#define UI_DOCKABLE_WIDGET

#include <QDockWidget>

class QWidget;
class QGridLayout;

class UI_DockableWidget : public QDockWidget
{
	Q_OBJECT
public:
	explicit UI_DockableWidget(QWidget *parent = 0);
	~UI_DockableWidget();

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

#endif // UI_DOCKABLE_WIDGET
