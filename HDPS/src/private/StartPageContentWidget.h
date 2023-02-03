#pragma once

#include "StartPageOpenProjectWidget.h"
#include "StartPageGetStartedWidget.h"

#include <actions/ToggleAction.h>

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

/**
 * Start page content widget class
 *
 * Widget class for showingall the actions.
 *
 * @author Thomas Kroes
 */
class StartPageContentWidget final : public QWidget
{
protected:

    /**
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     */
    StartPageContentWidget(QWidget* parent = nullptr);

    /** Updates the actions to reflect changes */
    void updateActions();

public:

    /**
     * Create header label
     * @param title Title of the header
     * @param tooltip Tooltip of the header
     * @return Pointer to newly created header label
     */
    static QLabel* createHeaderLabel(const QString& title, const QString& tooltip);

private:
    QVBoxLayout                 _mainLayout;            /** Main layout */
    QHBoxLayout                 _collumnsLayout;        /** Columns layout */
    QHBoxLayout                 _toolbarLayout;         /** Toolbar layout */
    StartPageOpenProjectWidget  _openProjectWidget;     /** Left column widget for opening existing projects */
    StartPageGetStartedWidget   _getStartedWidget;      /** Right column widget for getting started operations */
    hdps::gui::ToggleAction     _compactViewAction;     /** Toggle compact view on/off */

    friend class StartPageWidget;
};