// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "MarkdownDocument.h"

#ifdef _DEBUG
    #define MARKDOWN_DOCUMENT_VERBOSE
#endif

namespace mv::util {

MarkdownDocument::MarkdownDocument(QObject* parent /*= nullptr*/) :
    QObject(parent)
{
}

void MarkdownDocument::setText(const QString& text)
{
    if (text == _text)
        return;

    _text = text;

    emit textChanged(_text);
}

}