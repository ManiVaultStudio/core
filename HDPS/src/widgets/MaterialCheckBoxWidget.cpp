#include "MaterialCheckBoxWidget.h"

#include <QWebEngineView>
#include <QWebChannel>
#include <QLabel>
#include <QLayout>

#include "../util/FileUtil.h"

MaterialCheckBoxWidget::MaterialCheckBoxWidget(QString label, QWidget *parent, QColor *baseColor) :
	QWidget(parent)
{
	_checked = false;

	_layout = new QGridLayout(this);
    _layout->setSpacing(0);
    _layout->setMargin(0);
	_label = new QLabel(" " + label, this);

	setStyleSheet("background:transparent");
	setAttribute(Qt::WA_TranslucentBackground);

	_switch = new QWebEngineView(this);

	_switch->setFixedSize(36, 24);

    QWebChannel* channel = new QWebChannel(_switch->page());
    channel->registerObject("Qt", this);
	//QWebFrame* frame = _switch->page()->mainFrame();
	//frame->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
	//frame->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

    //QObject::connect(frame, &QWebFrame::javaScriptWindowObjectCleared, this, &MaterialCheckBoxWidget::connectJs);

    QString html = hdps::util::loadFileContents(":/html_ui_elements/checkbox.html");

	if (baseColor)
	{
		QString injectCss = QString("<style>") +
			".mdl-switch.is-checked .mdl-switch__thumb { background: rgba(" +
			QString::number(baseColor->red()) + ", " + QString::number(baseColor->green()) + ", " + QString::number(baseColor->blue()) + ", " + "1.0); }" +
			".mdl-switch.is-checked .mdl-switch__track { background: rgba(" +
			QString::number(baseColor->red()) + ", " + QString::number(baseColor->green()) + ", " + QString::number(baseColor->blue()) + ", " + "0.5) }" +
			"</style>";

		int insertIdx = html.indexOf("</head>");

		html.insert(insertIdx, injectCss);
	}

	_switch->setHtml(html, QUrl("qrc:/html_ui_elements/"));

    _layout->addWidget(_switch, 0, 0, Qt::AlignHCenter);
	_layout->addWidget(_label, 0, 1, Qt::AlignHCenter);
}

//void MaterialCheckBoxWidget::connectJs()
//{
//	_switch->page()->mainFrame()->addToJavaScriptWindowObject("Qt", this);
//}

void MaterialCheckBoxWidget::js_clicked() {
	
	_checked = !_checked;

	emit clicked();
	emit stateChanged(_checked);
}

void MaterialCheckBoxWidget::setChecked(bool checked)
{
	_checked = checked;

	qt_setState(_checked);
}


void MaterialCheckBoxWidget::setLabel(QString label)
{
	_label->setText(label);
}

MaterialCheckBoxWidget::~MaterialCheckBoxWidget()
{
	delete _switch;
	delete _label;
	delete _layout;
}
