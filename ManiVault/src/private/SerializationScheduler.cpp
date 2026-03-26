// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "SerializationScheduler.h"

#ifdef _DEBUG
	#define ZSTD_CODEC_VERBOSE
#endif

using namespace mv::util;

SerializationScheduler::SerializationScheduler(QObject* parent) :
    QObject(parent)
{
    _pool.setObjectName(QStringLiteral("SerializationSchedulerPool"));
    _pool.setMaxThreadCount(std::max(1, QThread::idealThreadCount() - 1));
}

SerializationScheduler& SerializationScheduler::instance()
{
    static SerializationScheduler executor;
    return executor;
}

DecodeRequestHandle SerializationScheduler::submitDecode(
    const QVariantMap& variantMap,
    char* destination,
    mv::util::ConcurrencyMode concurrencyMode)
{
    auto handle = DecodeRequestHandle::create();

    try {
        variantMapMustContain(variantMap, "BlockSize");
        variantMapMustContain(variantMap, "Blocks");

        const auto blocks = variantMap.value("Blocks").toList();
        const bool hasCodec = variantMap.contains("Codec");
        const QString codecName = hasCodec ? variantMap.value("Codec").toString()
            : QStringLiteral("none");

        if (hasCodec && !codecRegistry().isRegistered(codecName)) {
            throw std::runtime_error(
                QStringLiteral("Unable to load raw data, codec %1 is not registered")
                .arg(codecName)
                .toStdString()
            );
        }

        auto createCodec = [hasCodec, codecName]() -> std::shared_ptr<BlobCodec> {
            if (hasCodec)
                return codecRegistry().createCodec(nullptr, codecName);

            return codecRegistry().createCodec(nullptr, BlobCodec::Type::None);
            };

        QVector<DecodeBlockJob> decodeBlockJobs;
        decodeBlockJobs.reserve(blocks.size());

        for (const auto& block : blocks) {
            const auto blockMap = block.toMap();

            variantMapMustContain(blockMap, "Offset");
            variantMapMustContain(blockMap, "Size");

            DecodeBlockJob job;
            job._offset = blockMap.value("Offset").value<quint64>();
            job._size = blockMap.value("Size").value<quint64>();

            if (blockMap.contains("URI"))
                job._uri = blockMap.value("URI").toString();

            if (blockMap.contains("Data"))
                job._encodedData = blockMap.value("Data").toString();

            decodeBlockJobs.push_back(std::move(job));
        }

        {
            QMutexLocker lock(&handle->_mutex);

            handle->_totalJobs = decodeBlockJobs.size();
            handle->_completedJobs = 0;
            handle->_finished = false;
            handle->_error.clear();
        }

        QFuture<QVector<DecodeBlockResult>> future;

        if (concurrencyMode == mv::util::ConcurrencyMode::Parallel) {
            future = QtConcurrent::mappedReduced<QVector<DecodeBlockResult>>(
                &_pool,
                decodeBlockJobs,
                [destination, createCodec](const DecodeBlockJob& job) -> DecodeBlockResult {
                    return decodeBlock(job, destination, createCodec);
                },
                [](QVector<DecodeBlockResult>& results, const DecodeBlockResult& result) {
                    results.push_back(result);
                },
                QtConcurrent::OrderedReduce | QtConcurrent::SequentialReduce
            );
        }
        else {
            future = QtConcurrent::run(
                &_pool,
                [decodeBlockJobs, destination, createCodec]() -> QVector<DecodeBlockResult> {
                    QVector<DecodeBlockResult> results;
                    results.reserve(decodeBlockJobs.size());

                    for (const auto& job : decodeBlockJobs)
                        results.push_back(decodeBlock(job, destination, createCodec));

                    return results;
                }
            );
        }

        QObject::connect(
            &handle->_watcher,
            &QFutureWatcher<QVector<DecodeBlockResult>>::progressValueChanged,
            handle.data(),
            [handle](int progressValue) {
                int total = 0;

                {
                    QMutexLocker lock(&handle->_mutex);
                    handle->_completedJobs = progressValue;
                    total = handle->_totalJobs;
                }

                emit handle->progressChanged(progressValue, total);
            }
        );

        QObject::connect(
            &handle->_watcher,
            &QFutureWatcher<QVector<DecodeBlockResult>>::finished,
            handle.data(),
            [handle]() {
                QString firstError;

                try {
                    const auto results = handle->_watcher.result();

                    for (const auto& result : results) {
                        if (!result._error.isEmpty()) {
                            firstError = result._error;
                            break;
                        }
                    }
                }
                catch (const std::exception& e) {
                    firstError = QString::fromUtf8(e.what());
                }
                catch (...) {
                    firstError = QStringLiteral("Unknown decode failure");
                }

                {
                    QMutexLocker lock(&handle->_mutex);
                    handle->_finished = true;
                    handle->_completedJobs = handle->_totalJobs;

                    if (!firstError.isEmpty())
                        handle->_error = firstError;
                }

                emit handle->progressChanged(handle->completedJobs(), handle->totalJobs());

                if (!firstError.isEmpty())
                    emit handle->failed(firstError);

                emit handle->finished();
            }
        );

        handle->_watcher.setFuture(future);
    }
    catch (const std::exception& e) {
        {
            QMutexLocker lock(&handle->_mutex);
            handle->_finished = true;
            handle->_error = QString::fromUtf8(e.what());
            handle->_completedJobs = handle->_totalJobs;
        }

        QMetaObject::invokeMethod(
            handle.data(),
            [handle]() {
                emit handle->failed(handle->errorString());
                emit handle->finished();
            },
            Qt::QueuedConnection
        );
    }
    catch (...) {
        {
            QMutexLocker lock(&handle->_mutex);
            handle->_finished = true;
            handle->_error = QStringLiteral("Unknown decode setup failure");
            handle->_completedJobs = handle->_totalJobs;
        }

        QMetaObject::invokeMethod(
            handle.data(),
            [handle]() {
                emit handle->failed(handle->errorString());
                emit handle->finished();
            },
            Qt::QueuedConnection
        );
    }

    return handle;
}