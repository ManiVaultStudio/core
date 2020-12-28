#pragma once

#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QPushButton>
#include <QWidget>

namespace hdps
{

namespace gui
{

/**
 * Accordion section class
 * 
 * Collapsible section which is part of an accordion control
 * At the top is a toggle button for expanding/collapsing the section. It displays (from left to right):
 * 1. An icon which show the expansion state
 * 2. The title of the widget (synchronized with `Title` dynamic QObject property, uses windowTitle() if empty)
 * 3. The subtitle of the widget (synchronized with `Subtitle` dynamic QObject property)
 * 4. Section icon (synchronized with `Icon` dynamic QObject property)
 *
 * Below the toggle button it displays the contents of the target widget
 *
 * Inspired by: https://stackoverflow.com/questions/32476006/how-to-make-an-expandable-collapsable-section-widget-in-qt
 *
 * @author Thomas Kroes
 */
class AccordionSection : public QWidget
{
    Q_OBJECT

    friend class Accordion;

protected:

    /**
     * Constructor
     * @param parent Parent widget
     */
    explicit AccordionSection(QWidget* parent = nullptr);

    /** Get section name (target widget object name) */
    QString getName() const;

    /** Get/set contained widget */
    QWidget* getWidget();
    void setWidget(QWidget* widget);

public: // Events

    /**
     * Tap into events from another object (in this case to handle changes to dynamic properties)
     * @param object Object to listen to
     * @param event Event that occurred
     */
    bool eventFilter(QObject* object, QEvent* event);

public: // Expand collapse

    /** Expand/collapse section */
    void expand();
    void collapse();
    void setExpanded(const bool& expanded);
    
    /** Returns whether the section is expanded */
    bool isExpanded() const;

private: // Header content

    /** Update the expansion icon on the left */
    void updateLeftIcon();

    /** Update the title label */
    void updateTitleLabel();

    /** Update the subtitle label */
    void updateSubtitleLabel();
    
    /** Update the miscellaneous icon on the right */
    void updateRightIcon();
    
signals:

    /** Signals that the expansion has changed */
    void expandedChanged(const bool& expanded);

private:
    QVBoxLayout     _mainLayout;            /** Main layout */
    QFrame          _frame;                 /** Frame for the toggle button */
    QHBoxLayout     _frameLayout;           /** Frame layout */
    QLabel          _leftIconLabel;         /** Expansion icon label */
    QLabel          _titleLabel;            /** Title label */
    QLabel          _subtitleLabel;         /** Subtitle label */
    QLabel          _rightIconLabel;        /** Miscellaneous icon label */
    QVBoxLayout     _contentLayout;         /** Context layout */
    QPushButton     _toggleButton;          /** Toggle button for expansion */
    QWidget*        _widget;                /** Target widget */

    /** Toggle button icon size */
    static const std::uint32_t ICON_SIZE = 10;
};

}
}