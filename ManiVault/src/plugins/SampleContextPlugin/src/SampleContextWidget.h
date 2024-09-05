// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QWidget>
#include <QVBoxLayout>

class SampleContextPlugin;

/**
 * Sample context widget class
 *
 * Widget class for exploring a sample context.
 *
 * @author Thomas Kroes
 */
class SampleContextWidget : public QWidget
{
public:

    /**
     * Construct with pointer to owning \p sampleContextPlugin and \p parent widget
     * @param sampleContextPlugin Pointer to parent sample context plugin
     * @param parent Pointer to parent widget
     */
    SampleContextWidget(SampleContextPlugin* sampleContextPlugin, QWidget* parent = nullptr);

protected:
    SampleContextPlugin*    _sampleContextPlugin;   /** Pointer to parent sample context plugin */
    QVBoxLayout             _layout;                /** Main layout */
};
