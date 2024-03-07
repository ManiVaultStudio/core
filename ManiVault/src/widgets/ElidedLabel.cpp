// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ElidedLabel.h"

#include <QDebug>

namespace mv::gui {

ElidedLabel::ElidedLabel(const QString& text, QWidget* parent /*= nullptr*/, const Qt::TextElideMode& textElideMode /*= Qt::ElideMiddle*/) :
    QLabel(parent),
    _fullText(text),
    _textElideMode(textElideMode),
    _elide(true)
{
    elide();
}

void ElidedLabel::setText(const QString& fullText)
{
    _fullText = fullText;

    elide();
}

void ElidedLabel::resizeEvent(QResizeEvent* resizeEvent)
{
    QLabel::resizeEvent(resizeEvent);
    
    if (!getElide())
        return;

    elide();
}

Qt::TextElideMode ElidedLabel::getTextElideMode() const
{
    return _textElideMode;
}

void ElidedLabel::setTextElideMode(const Qt::TextElideMode& textElideMode)
{
    if (textElideMode == _textElideMode)
        return;

    _textElideMode = textElideMode;

    elide();
}

bool ElidedLabel::getElide() const
{
    return _elide;
}

void ElidedLabel::setElide(bool elide)
{
    if (elide == _elide)
        return;

    _elide = elide;
}

void ElidedLabel::elide()
{
    if (!getElide()) {
        QLabel::setText(_fullText);
    }
    else {
        QFontMetrics metrics(font());

        QLabel::setText(metrics.elidedText(_fullText, _textElideMode, width()));
    }    
}

}
