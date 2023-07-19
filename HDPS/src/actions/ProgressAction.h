// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

#include <QProgressBar>

namespace hdps::gui {

/**
 * Progress action class
 *
 * Action class for displaying progress
 *
 * @author Thomas Kroes
 */
class ProgressAction final : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget flags */
    enum WidgetFlag {

        Horizontal  = 0x00001,  /** Widget includes a horizontal progress bar */
        Vertical    = 0x00002,  /** Widget includes a vertical progress bar */

        Default = Horizontal
    };

public:

    /** Progress bar widget for progress bar action */
    class ProgressBarWidget : public QProgressBar
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param progressAction Pointer to progress action
         * @param widgetFlags Widget flags
         */
        ProgressBarWidget(QWidget* parent, ProgressAction* progressAction, const std::int32_t& widgetFlags);

    protected:
        ProgressAction* _progressAction;      /** Pointer to progress action */

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
    Q_INVOKABLE ProgressAction(QObject* parent, const QString& title = "");

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
    int getValue() const;

    /**
     * Set progress value to \p value
     * @param value Progress range value
     */
    void setValue(int value);

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
     * @return Text format string
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
     * Signals that the progress value changed to \p value
     * @param value Updated progress value
     */
    void valueChanged(int value);

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
     * Signals that text format changed to \p textFormat
     * @param textFormat Updated text format
     */
    void textFormatChanged(const QString& textFormat);

private:
    int                 _minimum;           /** Progress range minimum */
    int                 _maximum;           /** Progress range maximum */
    int                 _value;             /** Progress value */
    bool                _textVisible;       /** Determines whether progress text is visible or not */
    Qt::AlignmentFlag   _textAlignment;     /** Determines the label placement */
    QString             _textFormat;        /** Text format string */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(hdps::gui::ProgressAction)

inline const auto progressActionMetaTypeId = qRegisterMetaType<hdps::gui::ProgressAction*>("hdps::gui::ProgressAction");
