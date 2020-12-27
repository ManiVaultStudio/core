#pragma once

#include "AccordionSection.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QScrollArea>
#include <QMap>

namespace hdps
{

namespace gui
{

/**
 * Accordion class
 * 
 * Collapsible section which is part of an accordion control
 * Inspired by: https://stackoverflow.com/questions/32476006/how-to-make-an-expandable-collapsable-section-widget-in-qt
 *
 * @author Thomas Kroes
 */
class Accordion : public QWidget
{
    Q_OBJECT

public:
    
    /**
     * Constructor
     * @param parent Parent widget
     */
    explicit Accordion(QWidget* parent = nullptr);

public:

    /**
     * Add/remove section to/from the accordion
     * @param widget Widget to add/remove
     */
    void addSection(QWidget* widget);
    void removeSection(QWidget* widget);

public: // Expand/collapse

    /** Returns whether one or more sections can be expanded */
    bool canExpandAll() const;

    /** Returns whether one or more sections can be collapsed */
    bool canCollapseAll() const;

    /** Expand all sections */
    void expandAll();

    /** Collapse all sections */
    void collapseAll();

    /**
     * Go to section with \p name
     * @param name Name of the section to navigate to
     */
    void goToSection(const QString& name);

public: // Toolbar

    /**
     * Show/hide toolbar
     * @param show Show/hide
     */
    void showToolbar(const bool& show);

public: // Event handling

    /**
     * Listens to the events of \p target
     * @param target Target object to watch for events
     * @param event Event that occurred
     */
    bool eventFilter(QObject* target, QEvent* event) override;

private:

    /** Returns the number of expanded sections */
    std::uint32_t getNumExpandedSections() const;

    /** Update the expansion UI */
    void updateExpansionUI();

    /** Update the to section UI */
    void updateToSectionUI();

public:
    QVBoxLayout                 _mainLayout;                /** Main layout */
    QWidget                     _toolbar;                   /** Toolbar */
    QHBoxLayout                 _toolbarLayout;             /** Layout of the toolbar */
    QPushButton                 _expandAllPushButton;       /** Expand all section push button */
    QPushButton                 _collapseAllPushButton;     /** Collapse all section push button */
    QComboBox                   _toSectionComboBox;         /** Go to section combobox */
    QScrollArea                 _sectionsScrollArea;        /** Scroll area for sections */
    QWidget                     _sectionsWidget;            /** Widget for sections */
    QVBoxLayout                 _sectionsLayout;            /** Sections layout */
    QList<AccordionSection*>    _sections;                  /** List of sections */
    QString                     _toSection;                 /** Next section to navigate to */

    /** The cue for heading to a specific section */
    static const QString JUMP_TO_SECTION_TEXT;
};

}
}