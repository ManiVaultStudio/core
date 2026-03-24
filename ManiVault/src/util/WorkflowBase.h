// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QtTaskTree>
#include <QObject>

namespace mv::util
{

template<typename Context>
class WorkflowBase : public QObject
{
public:
    explicit WorkflowBase(QObject* parent = nullptr)
        : QObject(parent)
    {
    }

    void start()
    {
        _runner.start(
            makeRecipe(),
            [this](QtTaskTree::QTaskTree& tree) {
                tree.onStorageSetup(_contextStorage, [this](Context& storage) {
                    initializeContext(storage);
                    });

                registerStorageDoneHandler(tree);
                setupTree(tree);
            },
            [this](QtTaskTree::DoneWith doneWith) {
                handleDone(doneWith);
            });
    }

    void cancel()
    {
        _runner.cancel();
    }

protected:
    virtual QtTaskTree::Group makeRecipe() = 0;

    virtual void initializeContext(Context& storage) = 0;

    virtual void setupTree(QtTaskTree::QTaskTree&)
    {
    }

    virtual void handleDone(QtTaskTree::DoneWith doneWith) = 0;

    virtual void onStorageDone(const Context&)
    {
    }

    QtTaskTree::Storage<Context>& contextStorage()
    {
        return _contextStorage;
    }

    const QtTaskTree::Storage<Context>& contextStorage() const
    {
        return _contextStorage;
    }

private:
    void registerStorageDoneHandler(QtTaskTree::QTaskTree& tree)
    {
        tree.onStorageDone(_contextStorage, [this](const Context& storage) {
            onStorageDone(storage);
            });
    }

private:
    QtTaskTree::QSingleTaskTreeRunner   _runner;
    QtTaskTree::Storage<Context>        _contextStorage;
};

} // namespace mv::util