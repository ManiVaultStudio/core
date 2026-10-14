// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <QMenu>

#include <memory>

class GifRecorder;

/**
 * @brief Menu containing development and integration test tools.
 *
 * DeveloperMenu provides a stable host for diagnostics and experimental
 * workflows that should not be part of the regular end-user interface. Some
 * menus are populated lazily when opened so optional test features do not add
 * startup work or dependencies to the main window.
 *
 * @author Thomas Kroes
 */
class DeveloperMenu : public QMenu
{
public:
    /** Constructs the developer menu with an optional parent widget. */
    DeveloperMenu(QWidget* parent = nullptr);
    ~DeveloperMenu() override;

private:
    /** Runs the safe handled-exception reporting test. */
    void testHandledException();

    /** Runs the destructive fatal-crash reporting test after confirmation. */
    void testFatalCrash();

    /** Starts or stops the developer-only main-window GIF recorder. */
    void toggleGifRecording();

    std::unique_ptr<GifRecorder> _gifRecorder;                      /**< Developer-only GIF recorder. */
    QAction*                     _gifRecordingAction     = nullptr; /**< Start/stop recording action. */
    QMenu*                       _dataSynthesisMenu      = nullptr; /**< Runtime-populated synthetic data menu. */
    bool                         _dataSynthesisPopulated = false;
};
