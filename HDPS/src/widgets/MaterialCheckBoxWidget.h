#pragma once

#include <QWidget>
#include <QLabel>

class QWebEngineView;
class QGridLayout;

class MaterialCheckBoxWidget : public QWidget
{
	Q_OBJECT
public:
    explicit MaterialCheckBoxWidget(QString label = "", QWidget *parent = 0, QColor *basecolor = 0);
    ~MaterialCheckBoxWidget() override;

	inline bool checked() { return _checked; }
	void setChecked(bool checked);

	inline QString label() { return _label->text(); }
	void setLabel(QString label);

protected:

private:

signals :
	void clicked();
	void stateChanged(int state);

	void qt_setState(bool state);
	
public slots:
	//void connectJs();
	void js_clicked();

private:

	QGridLayout* _layout;
	QWebEngineView* _switch;
	QLabel* _label;

	bool _checked;
};
