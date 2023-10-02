// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QLabel>

namespace hdps::gui {

/**
 * Elided label class
 *
 * Truncates label text when text does not fit in the label
 * 
 * @author Thomas Kroes
 */
class ElidedLabel : public QLabel
{
public:

    /**
     * Construct with initial \text, pointer to \p parent widget and \p textElideMode
     * @param parent Pointer to parent widget
     * @param textElideMode Where to truncate the text; left, middle or right
     */
    explicit ElidedLabel(const QString& text, QWidget* parent = nullptr, const Qt::TextElideMode& textElideMode = Qt::ElideMiddle);

    /**
     * Set text to \p fullText
     * @param fullText Non-elided text
     */
    void setText(const QString& fullText);

    /**
     * Invoked when the label is resized
     * @param resizeEvent Pointer to the resize event
     */
    void resizeEvent(QResizeEvent* resizeEvent) override;

    /** Gets the text elide mode */
    Qt::TextElideMode getTextElideMode() const;

    /**
     * Set whether label elide is enabled or not
     * @param textElideMode Where to truncate the text; left, middle or right
     */
    void setTextElideMode(const Qt::TextElideMode& textElideMode);

    /** Gets whether label elide is enabled or not */
    bool getElide() const;

    /**
     * Set whether label elide is enabled or not
     * @param labelElide Whether label elide is enabled or not
     */
    void setElide(bool elide);

    

private:

    /** Truncates the label text */
    void elide();

private:
    QString             _fullText;          /** Text which is not truncated */
    Qt::TextElideMode   _textElideMode;     /** Where to truncate the text; left, middle or right */
    bool                _elide;             /** Whether elidation is enabled or not */
};

}
