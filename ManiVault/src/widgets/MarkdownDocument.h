// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QObject>

namespace mv::util {

/**
 * Markdown document class
 * 
 * Callback object class for relaying markdown content
 * 
 * @author Thomas Kroes
 */
class MarkdownDocument : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString text MEMBER _text NOTIFY textChanged FINAL)

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    explicit MarkdownDocument(QObject* parent = nullptr);

    /**
     * Set text to \p text
     * @param text Text
     */
    void setText(const QString& text);

signals:

    /**
     * Signals that the text changed to \p text
     * @parent text New text
     */
    void textChanged(const QString& text);

private:
    QString     _text;      /** Current text */
};

}