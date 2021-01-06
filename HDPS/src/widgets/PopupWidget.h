#pragma once

#include <QVBoxLayout>
#include <QGroupBox>

namespace hdps
{

namespace gui
{

/**
 * Popup widget class
 *
 * Widget class that is configured as a popup widget. The popup widget 
 * has a border and a group box. The actual target widget is inside the group box.
 *
 * @author Thomas Kroes
 */
template<typename WidgetType>
class PopupWidget : public QWidget
{
public:

    /**
     * Constructor
     * @param parent Parent widget
     */
    PopupWidget(QWidget* parent) :
        QWidget(parent),
        _widget(new WidgetType(this)),
        _popupGroupBox(new QGroupBox()),
        _popupLayout(new QVBoxLayout())
    {
        auto mainLayout = new QVBoxLayout();

        mainLayout->setMargin(0);
        mainLayout->setSpacing(0);
        mainLayout->addWidget(_popupGroupBox);

        setLayout(mainLayout);

        _widget->setWindowFlags(Qt::Popup);
        _widget->setObjectName("PopupWidget");
        _widget->setStyleSheet("QWidget#PopupWidget { border: 1px solid grey; }");
        _widget->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));

        _popupLayout->setMargin(7);

        _popupGroupBox->setLayout(_popupLayout);
    }

private:
    WidgetType*     _widget;            /** Popup widget content */
    QGroupBox*      _popupGroupBox;     /** Group box */
    QVBoxLayout*    _popupLayout;       /** Layout for the group box */
};

}
}