#include "PointDataPhantomGenerator.h"

#include "../PointData.h"

#include <CoreInterface.h>
#include <DeveloperActionRegistry.h>
#include <util/StyledIcon.h>

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>
#include <random>

namespace mv::pointdata::developer
{
namespace
{
enum class Population
{
    Deterministic,
    RandomFloat,
    RandomZeroOne,
    Zero
};

QString formatBytes(const std::uint64_t bytes)
{
    static constexpr const char* suffixes[] = { "B", "KiB", "MiB", "GiB", "TiB" };
    double value       = static_cast<double>(bytes);
    std::size_t suffix = 0;
    while (value >= 1024.0 && suffix < 4) {
        value /= 1024.0;
        ++suffix;
    }
    return QString::number(value, 'f', suffix == 0 ? 0 : 2) + " " + suffixes[suffix];
}

template <typename T>
void writeValue(char* destination, const std::size_t offset, const T value)
{
    std::memcpy(destination + offset, &value, sizeof(T));
}

QByteArray makeTile(const PointData::ElementTypeSpecifier type, const Population population,
    const std::uint64_t rowOffset, const std::uint64_t columnOffset, const std::uint64_t rows,
    const std::uint64_t columns)
{
    const auto elementSize = [&] {
        switch (type) {
            case PointData::ElementTypeSpecifier::float32: return sizeof(float);
            case PointData::ElementTypeSpecifier::bfloat16: return std::size_t(2);
            case PointData::ElementTypeSpecifier::int32:
            case PointData::ElementTypeSpecifier::uint32: return sizeof(std::int32_t);
            case PointData::ElementTypeSpecifier::int16:
            case PointData::ElementTypeSpecifier::uint16: return sizeof(std::int16_t);
            case PointData::ElementTypeSpecifier::int8:
            case PointData::ElementTypeSpecifier::uint8: return sizeof(std::int8_t);
        }
        return sizeof(float);
    }();

    QByteArray bytes(
        static_cast<qsizetype>(rows * columns * elementSize),
        Qt::Uninitialized);
    std::mt19937_64 random(rowOffset * 0x9e3779b97f4a7c15ULL + columnOffset + 17);
    std::uniform_real_distribution<float> unit(0.0f, 1.0f);

    for (std::uint64_t row = 0; row < rows; ++row) {
        for (std::uint64_t column = 0; column < columns; ++column) {
            const auto linearIndex   = row * columns + column;
            const auto offset        = static_cast<std::size_t>(linearIndex * elementSize);
            const auto deterministic = static_cast<float>((rowOffset + row) % 10000) / 10000.0f
                + static_cast<float>((columnOffset + column) % 1000) / 1000.0f;
            const auto value         = population == Population::Zero ? 0.0f
                : population == Population::RandomZeroOne ? (random() & 1ULL ? 1.0f : 0.0f)
                : population == Population::RandomFloat ? unit(random)
                : deterministic;

            switch (type) {
                case PointData::ElementTypeSpecifier::float32:
                    writeValue(bytes.data(), offset, value);
                    break;
                case PointData::ElementTypeSpecifier::bfloat16: {
                    std::uint32_t bits = 0;
                    std::memcpy(&bits, &value, sizeof(bits));
                    const auto truncated = static_cast<std::uint16_t>(bits >> 16);
                    writeValue(bytes.data(), offset, truncated);
                    break;
                }
                case PointData::ElementTypeSpecifier::int32:
                    writeValue(bytes.data(), offset, static_cast<std::int32_t>(value * 1000.0f));
                    break;
                case PointData::ElementTypeSpecifier::uint32:
                    writeValue(bytes.data(), offset, static_cast<std::uint32_t>(std::max(0.0f, value * 1000.0f)));
                    break;
                case PointData::ElementTypeSpecifier::int16:
                    writeValue(bytes.data(), offset, static_cast<std::int16_t>(value * 1000.0f));
                    break;
                case PointData::ElementTypeSpecifier::uint16:
                    writeValue(bytes.data(), offset, static_cast<std::uint16_t>(std::max(0.0f, value * 1000.0f)));
                    break;
                case PointData::ElementTypeSpecifier::int8:
                    writeValue(bytes.data(), offset, static_cast<std::int8_t>(value * 100.0f));
                    break;
                case PointData::ElementTypeSpecifier::uint8:
                    writeValue(bytes.data(), offset, static_cast<std::uint8_t>(std::max(0.0f, value * 100.0f)));
                    break;
            }
        }
    }
    return bytes;
}

class PhantomDialog final : public QDialog
{
public:
    explicit PhantomDialog(QWidget* parent) :
        QDialog(parent)
    {
        setWindowTitle(tr("Create phantom points dataset"));
        auto* form = new QFormLayout(this);

        _name         = new QLineEdit("Phantom points", this);
        _rows         = makeSpin(1, std::numeric_limits<int>::max(), 1000000);
        _columns      = makeSpin(1, 100000, 32);
        _rowTileSize = makeSpin(1, 1000000, 4096);
        _columnTileSize = makeSpin(1, 100000, 32);
        _cacheCapacity = makeSpin(1, 100000, 8);
        _type = new QComboBox(this);
        QStringList typeNames;
        for (const auto* typeName : PointData::getElementTypeNames())
        {
            typeNames.push_back(QString::fromLatin1(typeName));
        }
        _type->addItems(typeNames);
        _population = new QComboBox(this);
        _population->addItem("Deterministic", static_cast<int>(Population::Deterministic));
        _population->addItem("Random floats", static_cast<int>(Population::RandomFloat));
        _population->addItem("Random zeros and ones", static_cast<int>(Population::RandomZeroOne));
        _population->addItem("All zeros", static_cast<int>(Population::Zero));
        _estimate = new QLabel(this);
        _estimate->setWordWrap(true);

        form->addRow("Dataset name", _name);
        form->addRow("Rows", _rows);
        form->addRow("Dimensions", _columns);
        form->addRow("Element type", _type);
        form->addRow("Population", _population);
        form->addRow("Tile rows", _rowTileSize);
        form->addRow("Tile columns", _columnTileSize);
        form->addRow("Cache tiles", _cacheCapacity);
        form->addRow("Estimate", _estimate);

        auto* buttons = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
            this);
        form->addRow(buttons);
        connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
        for (auto* spin : { _rows, _columns, _rowTileSize, _columnTileSize, _cacheCapacity })
            connect(spin, qOverload<int>(&QSpinBox::valueChanged), this, [this] { updateEstimate(); });
        connect(_type, &QComboBox::currentTextChanged, this, [this] { updateEstimate(); });
        updateEstimate();
    }

    QString name() const
    {
        return _name->text().trimmed().isEmpty() ? QStringLiteral("Phantom points") : _name->text().trimmed();
    }

    std::uint64_t rows() const { return _rows->value(); }

    std::uint64_t columns() const { return _columns->value(); }

    std::size_t rowTileSize() const { return _rowTileSize->value(); }

    std::size_t columnTileSize() const { return _columnTileSize->value(); }

    std::size_t cacheCapacity() const { return _cacheCapacity->value(); }

    PointData::ElementTypeSpecifier type() const
    {
        return PointData::elementTypeSpecifier(_type->currentText());
    }

    Population population() const
    {
        return static_cast<Population>(_population->currentData().toInt());
    }

private:
    static QSpinBox* makeSpin(int minimum, int maximum, int value)
    {
        auto* spin = new QSpinBox;
        spin->setRange(minimum, maximum);
        spin->setValue(value);
        return spin;
    }

    void updateEstimate()
    {
        const auto type = PointData::elementTypeSpecifier(_type->currentText());
        const auto elementSize = type == PointData::ElementTypeSpecifier::float32
            ? 4
            : type == PointData::ElementTypeSpecifier::bfloat16
            ? 2
            : type == PointData::ElementTypeSpecifier::int32 || type == PointData::ElementTypeSpecifier::uint32
            ? 4
            : type == PointData::ElementTypeSpecifier::int16 || type == PointData::ElementTypeSpecifier::uint16
            ? 2
            : 1;

        const auto estimate = PointData::estimateTiledMemory(
            rows(),
            columns(),
            elementSize,
            rowTileSize(),
            columnTileSize(),
            cacheCapacity());

        _estimate->setText(QString("Raw size: %1\nLargest tile: %2\nEstimated cache: %3\nTiles: %4\nIndex estimate: %5")
            .arg(formatBytes(estimate.rawDataBytes))
            .arg(formatBytes(estimate.largestTileBytes))
            .arg(formatBytes(estimate.cachedTileBytes))
            .arg(estimate.tileCount)
            .arg(formatBytes(estimate.indexBytes)));
    }

    QLineEdit* _name = nullptr;
    QSpinBox* _rows = nullptr;
    QSpinBox* _columns = nullptr;
    QSpinBox* _rowTileSize = nullptr;
    QSpinBox* _columnTileSize = nullptr;
    QSpinBox* _cacheCapacity = nullptr;
    QComboBox* _type = nullptr;
    QComboBox* _population = nullptr;
    QLabel* _estimate = nullptr;
};
}

void PointDataPhantomGenerator::populateMenu(QMenu& menu, QWidget* owner)
{
    auto* action = menu.addAction(mv::util::StyledIcon("table"), QObject::tr("Phantom points…"));
    action->setToolTip(QObject::tr("Create a lazy, generator-backed tiled points dataset"));
    QObject::connect(action, &QAction::triggered, owner, [owner] {
        PhantomDialog dialog(owner);
        if (dialog.exec() != QDialog::Accepted)
            return;

        const auto type = dialog.type();
        const auto population = dialog.population();
        auto dataset = mv::data().createDataset<Points>("Points", dialog.name());
        auto* pointData = dataset->getPointData();
        pointData->makeTiledFromGenerator(dialog.rows(), dialog.columns(), type, dialog.rowTileSize(), dialog.columnTileSize(),
            [type, population](const std::uint64_t rowOffset, const std::uint64_t columnOffset,
                const std::uint64_t rows, const std::uint64_t columns) {
                return makeTile(type, population, rowOffset, columnOffset, rows, columns);
            });
        pointData->setTiledCacheCapacity(dialog.cacheCapacity());

        const auto estimate = pointData->getTiledMemoryEstimate();
        mv::help().addNotification(QObject::tr("Phantom points dataset created"),
            QObject::tr("%1 is generator-backed and occupies %2 of raw logical data. Tiles will be written when the project is saved.")
                .arg(dialog.name()).arg(formatBytes(estimate.rawDataBytes)), mv::util::StyledIcon("check"));
    });
}

namespace
{
const bool registered = [] {
    mv::developer::registerMenuPopulator(QStringLiteral("PointData.PhantomGenerator"),
        [](QMenu& menu, QWidget* owner) { PointDataPhantomGenerator::populateMenu(menu, owner); });
    return true;
}();
}
}
