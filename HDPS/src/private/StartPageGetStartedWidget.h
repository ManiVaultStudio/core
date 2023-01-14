#pragma once

#include <QWidget>

/**
 * Start page get started widget class
 *
 * Widget class which contains actions related to getting started.
 *
 * @author Thomas Kroes
 */
class StartPageGetStartedWidget : public QWidget
{
protected:

    /**
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     */
    StartPageGetStartedWidget(QWidget* parent = nullptr);

    /** Updates the actions to reflect changes */
    void updateActions();

    friend class StartPageContentWidget;
};