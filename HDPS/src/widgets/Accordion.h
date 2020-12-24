#pragma once

#include "AccordionSection.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QList>

namespace hdps
{

namespace gui
{

class Accordion : public QWidget
{
    Q_OBJECT

public:
    
    explicit Accordion(QWidget* parent = nullptr);

public:

    void addSection(QWidget* widget, const QString& title, const QIcon& icon = QIcon());

public:
    QVBoxLayout                 _layout;
    QList<AccordionSection*>    _sections;
};

}
}