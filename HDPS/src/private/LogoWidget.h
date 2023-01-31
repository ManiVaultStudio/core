#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

/**
 * Logo widget class
 *
 * Widget class for displaying the application logo
 *
 * @author Thomas Kroes
 */
class LogoWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     */
    LogoWidget(QWidget* parent = nullptr);

protected:
    QVBoxLayout     _layout;        /** Main layout */
    QLabel          _headerLabel;   /** Header label */
};

