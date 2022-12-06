#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

/**
 * Logo widget class
 *
 * Widget class for logo
 *
 * @author Thomas Kroes
 */
class LogoWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    LogoWidget(QWidget* parent = nullptr);

protected:
    QVBoxLayout     _layout;        /** Main layout */
    QLabel          _headerLabel;   /** Header label */
};

