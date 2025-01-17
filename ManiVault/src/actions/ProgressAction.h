// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

#include <QProgressBar>
#include <QLabel>
#include <QLineEdit>

namespace mv::gui {

/**
 * Progress action class
 *
 * Action class for displaying progress
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ProgressAction final : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget flags */
    enum WidgetFlag {
        HorizontalBar   = 0x00001,  /** Widget includes a horizontal bar */
        VerticalBar     = 0x00002,  /** Widget includes a vertical bar */
        Label           = 0x00004,  /** Widget includes a label widget */
        LineEdit        = 0x00008,  /** Widget includes a line edit widget */

        Default = HorizontalBar
    };

public:

    /** Bar widget for progress action */
    class CORE_EXPORT BarWidget : public QProgressBar
    {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param progressAction Pointer to progress action
         * @param widgetFlags Widget flags
         */
        BarWidget(QWidget* parent, ProgressAction* progressAction, const std::int32_t& widgetFlags);

        /**
         * Override the paint event to add text elidation
         * @param paintEvent Pointer to the paint event
         */
        void paintEvent(QPaintEvent* paintEvent) override;

    protected:
        ProgressAction*     _progressAction;    /** Pointer to owning progress action */

        friend class ProgressAction;
    };

    /** Label widget for progress action */
    class CORE_EXPORT LabelWidget : public QLabel
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param progressAction Pointer to progress action
         * @param widgetFlags Widget flags
         */
        LabelWidget(QWidget* parent, ProgressAction* progressAction, const std::int32_t& widgetFlags);

    protected:
        ProgressAction*     _progressAction;    /** Pointer to owning progress action */

        friend class ProgressAction;
    };

    /** Line edit widget for progress action */
    class CORE_EXPORT LineEditWidget : public QLineEdit
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param progressAction Pointer to progress action
         * @param widgetFlags Widget flags
         */
        LineEditWidget(QWidget* parent, ProgressAction* progressAction, const std::int32_t& widgetFlags);

    protected:
        ProgressAction*     _progressAction;    /** Pointer to owning progress action */

        friend class ProgressAction;
    };

protected:

    /**
     * Get widget representation of the toggle action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE ProgressAction(QObject* parent, const QString& title);

    /**
     * Get progress range minimum
     * @return Progress range minimum
     */
    int getMinimum() const;

    /**
     * Set progress range minimum to \p minimum
     * @param minimum Progress range minimum
     */
    void setMinimum(int minimum);

    /**
     * Get progress range maximum
     * @return Progress range maximum
     */
    int getMaximum() const;

    /**
     * Set progress range maximum to \p maximum
     * @param maximum Progress range maximum
     */
    void setMaximum(int maximum);

    /**
     * Set progress range from \p minimum to \p maximum
     * @param minimum Progress range minimum
     * @param maximum Progress range maximum
     */
    void setRange(int minimum, int maximum);

    /**
     * Get progress value
     * @return Progress value
     */
    int getProgress() const;

    /**
     * Set progress value to \p value
     * @param value Progress value
     */
    void setProgress(int value);

    /**
     * Get whether text is visible
     * @return Boolean determining whether text is visible
     */
    bool getTextVisible() const;

    /**
     * Set whether text is visible to \p textVisible
     * @param textVisible Boolean determining whether text is visible
     */
    void setTextVisible(bool textVisible);

    /**
     * Get text alignment
     * @return Text alignment flag
     */
    Qt::AlignmentFlag getTextAlignment() const;

    /**
     * Set text alignment to \p textAlignment
     * @param textAlignment Text alignment flag
     */
    void setTextAlignment(Qt::AlignmentFlag textAlignment);

    /**
     * Get text format
     * @return ProgressAction#_textFormat when ProgressAction#_overrideTextFormat is empty, otherwise ProgressAction#_overrideTextFormat
     */
    QString getTextFormat() const;

    /**
     * Set text format to \p textFormat
     * %p - is replaced by the percentage completed
     * %v - is replaced by the current value
     * %m - is replaced by the total number of steps
     * The default value is "%p%"
     * @param textFormat Text format string
     */
    void setTextFormat(const QString& textFormat);

    /**
     * Get override text format
     * @return Override text format string
     */
    QString getOverrideTextFormat() const;

    /**
     * Set override text format to \p overrideTextFormat
     * %p - is replaced by the percentage completed
     * %v - is replaced by the current value
     * %m - is replaced by the total number of steps
     * The default value is "%p%"
     * @param overrideTextFormat Override text format string
     */
    void setOverrideTextFormat(const QString& overrideTextFormat);

    /**
     * Get number of steps
     * @return Number of steps
     */
    int getNumberOfSteps() const;

    /**
     * Get percentage completed
     * @return Percentage
     */
    float getPercentage() const;

    /**
     * Get text representation of the progress (using the text format)
     * @return Text representation of the progress
     */
    QString getText() const;

signals:

    /**
     * Signals that the progress range minimum changed to \p minimum
     * @param minimum Updated progress range minimum
     */
    void minimumChanged(int minimum);

    /**
     * Signals that the progress range maximum changed to \p maximum
     * @param maximum Updated progress range maximum
     */
    void maximumChanged(int maximum);

    /**
     * Signals that the number of steps changed to \p numberOfSteps
     * @param numberOfSteps Number of steps
     */
    void numberOfStepsChanged(int numberOfSteps);

    /**
     * Signals that the progress value changed to \p value
     * @param value Updated progress value
     */
    void valueChanged(int value);

    /**
     * Signals that the progress changed to \p percentage
     * @param progress Updated progress
     */
    void progressChanged(float progress);

    /**
     * Signals that text visibility changed to \p textVisible
     * @param textVisible Updated text visibility
     */
    void textVisibleChanged(bool textVisible);

    /**
     * Signals that text alignment changed to \p textAlignment
     * @param textAlignment Updated text alignment
     */
    void textAlignmentChanged(bool textAlignment);

    /**
     * Signals that the text format changed from \p previousTextFormat to \p currentTextFormat
     * @param previousTextFormat Previous text format
     * @param currentTextFormat Current text format
     */
    void textFormatChanged(const QString& previousTextFormat, const QString& currentTextFormat);

    /**
     * Signals that the override text format changed from \p previousOverrideTextFormat to \p currentOverrideTextFormat
     * @param previousOverrideTextFormat Previous override text format
     * @param currentOverrideTextFormat Current override text format
     */
    void overrideTextFormatChanged(const QString& previousOverrideTextFormat, const QString& currentOverrideTextFormat);

private:
    int                 _minimum;               /** Progress range minimum */
    int                 _maximum;               /** Progress range maximum */
    int                 _value;                 /** Progress value */
    bool                _textVisible;           /** Determines whether progress text is visible or not */
    Qt::AlignmentFlag   _textAlignment;         /** Determines the label placement */
    QString             _textFormat;            /** Text format string */
    QString             _overrideTextFormat;    /** Override text format string (when set, it overrides ProgressAction#_textFormat) */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::ProgressAction)

inline const auto progressActionMetaTypeId = qRegisterMetaType<mv::gui::ProgressAction*>("mv::gui::ProgressAction");
