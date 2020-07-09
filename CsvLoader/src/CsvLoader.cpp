#include "CsvLoader.h"

#include "PointData.h"
#include "Set.h"

#include <QtCore>
#include <QtDebug>

#include <QFileInfo>
#include <QInputDialog>

Q_PLUGIN_METADATA(IID "nl.tudelft.CsvLoader")

// =============================================================================
// Loader
// =============================================================================

namespace
{
    // Generic template, wrapping QString "toType(&ok)" member functions. 
    template <typename T> T ConvertString(const QString&, bool& ok);

    template <> float ConvertString(const QString& str, bool& ok)
    {
        return str.toFloat(&ok);
    }

    template <> biovault::bfloat16_t ConvertString(const QString& str, bool& ok)
    {
        return biovault::bfloat16_t{ str.toFloat(&ok) };
    }

    template <> std::int16_t ConvertString(const QString& str, bool& ok)
    {
        return str.toShort(&ok);
    }

    template <> std::uint16_t ConvertString(const QString& str, bool& ok)
    {
        return str.toUShort(&ok);
    }

    template <> std::uint8_t ConvertString(const QString& str, bool& ok)
    {
        const auto temp = str.toUShort();
        ok = temp <= std::numeric_limits<uint16_t>::max();
        return ok ? static_cast<std::uint8_t>(temp) : std::uint8_t{};
    }

    template <> std::int8_t ConvertString(const QString& str, bool& ok)
    {
        const auto temp = str.toShort();
        ok = ((temp >= std::numeric_limits<int8_t>::min()) && (temp <= std::numeric_limits<int8_t>::max()));
        return ok ? static_cast<std::int8_t>(temp) : std::int8_t{};
    }


    // Converts the strings from the string lists to the PointData, converting
    // each string to data element type T.
    template <typename T>
    void convertStringsToPointData(
        Points& points,
        const std::vector<QStringList>& stringLists,
        const bool hasHeaders)
    {
        std::vector<T> data;
        std::size_t numDimensions = 1;
        const auto numberOfStringLists = stringLists.size();
        const std::size_t startIndex{ hasHeaders ? 1U : 0U };

        if (numberOfStringLists > startIndex)
        {
            const auto expectedNumberOfDataElements = (numberOfStringLists - startIndex) * stringLists.front().size();
            data.reserve(expectedNumberOfDataElements);

            for (auto i = startIndex; i < numberOfStringLists; ++i)
            {
                const QStringList& items = stringLists[i];
                numDimensions = static_cast<std::size_t>(items.size());

                for (const auto& item : items)
                {
                    if (!item.isEmpty())
                    {
                        auto ok = false;
                        data.push_back(ConvertString<T>(item, ok));
                        if (!ok)
                        {
                            qDebug() << "Conversion error at row" << i << "! Input value:" << item << "Output value: zero";
                        }
                    }
                }
            }
        }
        points.setData(std::move(data), numDimensions);
    }


    // Recursively searches for the data element type that is specified by the selectedDataElementType parameter. 
    template <unsigned N = 0>
    void recursiveConvertStringsToPointData(
        const QString& selectedDataElementType,
        Points& points,
        const std::vector<QStringList>& stringLists,
        const bool hasHeaders)
    {
        const QLatin1String nthDataElementTypeName(std::get<N>(PointData::getElementTypeNames()));

        if (selectedDataElementType == nthDataElementTypeName)
        {
            convertStringsToPointData<PointData::ElementTypeAt<N>>(points, stringLists, hasHeaders);
        }
        else
        {
            recursiveConvertStringsToPointData<N + 1>(selectedDataElementType, points, stringLists, hasHeaders);
        }
    }

    template <>
    void recursiveConvertStringsToPointData<PointData::getNumberOfSupportedElementTypes()>(const QString&, Points&, const std::vector<QStringList>&, bool)
    {
        // This specialization does nothing, intensionally! 
    }

}   // End of unnamed namespace


CsvLoader::~CsvLoader(void)
{
    
}

void CsvLoader::init()
{

}

void CsvLoader::loadData()
{
    const auto fileName = AskForFileName(QObject::tr("CSV Files (*.csv)"));

    if (fileName.isEmpty())
    {
        return;
    }

    qDebug() << "Loading CSV file: " << fileName;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw DataLoadException(fileName, "File was not found at location.");

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList tokens = line.split(",");

        _loadedData.push_back(tokens);
    }

    // Gather some knowledge about the data from the user
    InputDialog inputDialog(nullptr);
    inputDialog.setModal(true);

    connect(&inputDialog, &InputDialog::closeDialog, this, &CsvLoader::dialogClosed);

    int inputOk = inputDialog.exec();
}

void CsvLoader::dialogClosed(QString dataSetName, bool hasHeaders, QString selectedDataElementType)
{
    QStringList headers;

    if (hasHeaders)
    {
        headers = _loadedData[0];
    }

    QString name = _core->addData("Points", dataSetName);
    Points& points = _core->requestData<Points>(name);

    recursiveConvertStringsToPointData(selectedDataElementType, points, _loadedData, hasHeaders);

    qDebug() << "Number of dimensions: " << points.getNumDimensions();

    _core->notifyDataAdded(name);

    qDebug() << "CSV file loaded. Num data points: " << points.getNumPoints();

    qDebug() << dataSetName << hasHeaders;
}

// =============================================================================
// Factory
// =============================================================================

LoaderPlugin* CsvLoaderFactory::produce()
{
    return new CsvLoader();
}
