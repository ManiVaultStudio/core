#include "Accordion.h"

namespace hdps
{

namespace gui
{

Accordion::Accordion(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _layout(),
    _sections()
{
    _layout.setAlignment(Qt::AlignTop);
    _layout.setMargin(0);
    _layout.setSpacing(0);

    setLayout(&_layout);
}

void Accordion::addSection(QWidget* widget, const QString& title, const QIcon& icon /*= QIcon()*/)
{
    auto accordionSection = new AccordionSection(title);
    
    accordionSection->setWidget(widget);
    accordionSection->setIcon(icon);

    _layout.addWidget(accordionSection);
    
    _sections.append(accordionSection);
}

}

}