// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "../PointData.h"

#include <CoreInterface.h>
#include <DeveloperActionRegistry.h>
#include <Application.h>
#include <util/StyledIcon.h>
#include <workflow/WorkflowOptions.h>
#include <workflow/WorkflowPlan.h>

#include <QMessageBox>
#include <QDialog>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QToolButton>
#include <QVBoxLayout>

#include <cstdint>
#include <exception>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <QStringList>
#include <vector>

namespace mv::pointdata::developer
{
namespace
{
QString runDeterministicRowFetchTest(const bool thorough, const Points::StorageBackendType backend)
{
    const std::uint64_t rows    = thorough ? 10001 : 1025;
    constexpr std::uint64_t columns = 33;
    constexpr std::size_t rowTileSize = 256;
    constexpr std::size_t columnTileSize = 8;
    std::vector<float> source(rows * columns);

    for (std::uint64_t row = 0; row < rows; ++row)
    {
        for (std::uint64_t column = 0; column < columns; ++column)
            source[row * columns + column] = row == 0 || column == 0 || row == 6 || column == 4 ? 0.0f : static_cast<float>(static_cast<std::int32_t>(row * 17 + column * 5) - 50) / 4.0f;
    }

    const auto backendName = pointdata::storage::getStorageBackendTypeName(backend);
    auto dataset = mv::data().createDataset<Points>("Points", QString("Point-data %1 row access test").arg(backendName));
    dataset->setData(source, columns);

    try
    {
        if (backend == Points::StorageBackendType::Tiled)
        {
            pointdata::storage::StorageBackendConfiguration configuration;
            configuration.rowTileSize = rowTileSize;
            configuration.columnTileSize = columnTileSize;
            dataset->convertToStorageBackend(backend, configuration);
        }
        else if (backend == Points::StorageBackendType::Sparse)
            dataset->convertToStorageBackend(backend, std::make_shared<const pointdata::storage::SparseStorageBackendConfiguration>());

        for (std::uint64_t row = 0; row < rows; ++row)
        {
            const auto view = dataset->getRow(row);
            if (!view || view.rows != 1 || view.columns != columns || view.elementSizeBytes != sizeof(float))
                throw std::runtime_error(QString("Unexpected row view at row %1").arg(row).toStdString());

            const auto* actual = static_cast<const float*>(view.data);
            for (std::uint64_t column = 0; column < columns; ++column)
            {
                if (actual[column] != source[row * columns + column])
                    throw std::runtime_error(QString("Value mismatch at row %1, column %2").arg(row).arg(column).toStdString());
            }

        }

        mv::data().removeDataset(dataset);
        return QObject::tr("%1: passed (%2 rows checked)").arg(backendName).arg(rows);
    }
    catch (...)
    {
        mv::data().removeDataset(dataset);
        throw;
    }
}

void showReport(QWidget* owner, const bool thorough, const QStringList& results)
{
    const auto failures = std::count_if(results.cbegin(), results.cend(), [](const QString& result) { return result.contains(QStringLiteral(": FAILED")); });
    const auto passed = results.size() - failures;
    QDialog report(owner);
    report.setWindowTitle(QObject::tr("Point-data test report"));
    report.setWindowIcon(mv::util::StyledIcon("clipboard-check"));
    report.setMinimumWidth(680);

    auto* layout = new QVBoxLayout(&report);
    auto* summary = new QLabel(QObject::tr("%1 run: %2 passed, %3 failed").arg(thorough ? QObject::tr("Thorough") : QObject::tr("Quick")).arg(passed).arg(failures), &report);
    auto* toggleButton = new QToolButton(&report);
    toggleButton->setText(QObject::tr("Show test results"));
    toggleButton->setCheckable(true);
    toggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toggleButton->setArrowType(Qt::RightArrow);
    auto* table = new QTableWidget(static_cast<int>(results.size()), 4, &report);
    table->setHorizontalHeaderLabels({ QObject::tr("Test"), QObject::tr("Backend"), QObject::tr("Result"), QObject::tr("Details") });
    table->verticalHeader()->hide();
    table->setAlternatingRowColors(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

    for (qsizetype row = 0; row < results.size(); ++row)
    {
        const auto result = results.at(row);
        const auto separator = result.indexOf(": ");
        const auto backend = separator >= 0 ? result.left(separator) : QObject::tr("Unknown");
        const auto details = separator >= 0 ? result.mid(separator + 2) : result;
        const auto failed = details.startsWith(QStringLiteral("FAILED"));
        auto* testItem = new QTableWidgetItem(QObject::tr("Fetch deterministic rows"));
        testItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        testItem->setIcon(mv::util::StyledIcon(failed ? "square-xmark" : "square-check"));
        table->setItem(static_cast<int>(row), 0, testItem);
        auto* backendItem = new QTableWidgetItem(backend);
        backendItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        table->setItem(static_cast<int>(row), 1, backendItem);
        auto* resultItem = new QTableWidgetItem(failed ? QObject::tr("Failed") : QObject::tr("Passed"));
        resultItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        table->setItem(static_cast<int>(row), 2, resultItem);
        auto* detailsItem = new QTableWidgetItem(failed ? details.mid(QStringLiteral("FAILED — ").size()) : details.mid(QStringLiteral("passed ").size()).remove(QLatin1Char('(')).remove(QLatin1Char(')')));
        detailsItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        table->setItem(static_cast<int>(row), 3, detailsItem);
    }

    auto* closeButton = new QPushButton(QObject::tr("Close"), &report);
    auto* resultsContainer = new QWidget(&report);
    auto* resultsLayout = new QVBoxLayout(resultsContainer);
    resultsLayout->setContentsMargins(0, 0, 0, 0);
    resultsLayout->addWidget(table);
    resultsContainer->setMaximumHeight(0);
    resultsContainer->hide();
    QObject::connect(closeButton, &QPushButton::clicked, &report, &QDialog::accept);
    layout->addWidget(summary);
    layout->addWidget(toggleButton);
    layout->addWidget(resultsContainer);
    layout->addWidget(closeButton, 0, Qt::AlignRight);
    QObject::connect(toggleButton, &QToolButton::toggled, &report, [toggleButton, resultsContainer, layout, &report](const bool expanded) {
        toggleButton->setArrowType(expanded ? Qt::DownArrow : Qt::RightArrow);
        toggleButton->setText(expanded ? QObject::tr("Hide test results") : QObject::tr("Show test results"));
        if (expanded)
        {
            resultsContainer->setMaximumHeight(QWIDGETSIZE_MAX);
            resultsContainer->show();
        }
        else
        {
            resultsContainer->setMaximumHeight(0);
            resultsContainer->hide();
            report.setMinimumWidth(680);
        }
        layout->invalidate();
        layout->activate();
        report.adjustSize();
    });
    report.exec();
}

void startWorkflow(QWidget* owner, const bool thorough, const std::vector<Points::StorageBackendType>& backends)
{
    auto results = std::make_shared<QStringList>();
    auto plan = std::make_unique<workflow::WorkflowPlan>(QObject::tr("Point-data %1 tests").arg(thorough ? QObject::tr("thorough") : QObject::tr("quick")));

    for (const auto backend : backends)
    {
        const auto backendName = pointdata::storage::getStorageBackendTypeName(backend);
        plan->addSequentialStage(QObject::tr("Check %1 row access").arg(backendName), [results, backend, thorough, backendName](const workflow::WorkflowPlan::Job&, const workflow::SharedWorkflowExecutionContext&) {
            try
            {
                *results << runDeterministicRowFetchTest(thorough, backend);
            }
            catch (const std::exception& exception)
            {
                *results << QStringLiteral("%1: FAILED — %2").arg(backendName, QString::fromUtf8(exception.what()));
            }
        }, workflow::WorkflowPlan::JobThreadAffinity::GuiThread);
    }

    workflow::WorkflowOptions options;
    options.cancellation.enabled = true;
    options.reporting.progress = true;
    options.reporting.finishedNotification = true;
    auto future = Application::getWorkflowPlanExecutor().execute(std::move(plan), nullptr, options);
    future.onFinished(owner, [owner, thorough, results](const workflow::SharedWorkflowResult&) {
        showReport(owner, thorough, *results);
    });
}

const bool registered = [] {
    mv::developer::registerMenuPopulator(QStringLiteral("PointData.Testing.RowFetch"), [](QMenu& menu, QWidget* owner) {
        auto populateMode = [owner](QMenu& modeMenu, const bool thorough) {
            constexpr Points::StorageBackendType backends[] = { Points::StorageBackendType::Eager, Points::StorageBackendType::Tiled, Points::StorageBackendType::Sparse };
            auto runAll = [owner, thorough, backends] {
                startWorkflow(owner, thorough, { backends[0], backends[1], backends[2] });
            };

            auto* runAllAction = modeMenu.addAction(mv::util::StyledIcon("forward-fast"), QObject::tr("Run all scenarios"));
            QObject::connect(runAllAction, &QAction::triggered, owner, runAll);
            modeMenu.addSeparator();
            for (const auto backend : backends)
            {
                auto* action = modeMenu.addAction(mv::util::StyledIcon("list-check"), QObject::tr("Fetch rows · %1").arg(pointdata::storage::getStorageBackendTypeName(backend)));
                QObject::connect(action, &QAction::triggered, owner, [owner, thorough, backend] {
                    startWorkflow(owner, thorough, { backend });
                });
            }
        };

        auto* quickMenu = menu.addMenu(mv::util::StyledIcon("bolt"), QObject::tr("Quick"));
        populateMode(*quickMenu, false);

        auto* thoroughMenu = menu.addMenu(mv::util::StyledIcon("list-check"), QObject::tr("Thorough"));
        populateMode(*thoroughMenu, true);
    });
    return true;
}();
}
}
