#pragma once

#include <QVBoxLayout>
#include <QLabel>

/**
 * Header widget class
 *
 * Widget class for header with logo.
 *
 * @author Thomas Kroes
 */
class StartPageHeaderWidget : public QWidget
{
public:

    /**
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     */
    StartPageHeaderWidget(QWidget* parent = nullptr);

protected:
    QVBoxLayout     _layout;        /** Main layout */
    QLabel          _headerLabel;   /** Header label */
};
