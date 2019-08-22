#include "BinLoader.h"

#include "PointsPlugin.h"
#include "Set.h"

#include <QtCore>
#include <QtDebug>

#include <QFileDialog>
#include <vector>
#include <QInputDialog>

#include <fstream>
#include <iterator>
#include <vector>

Q_PLUGIN_METADATA(IID "nl.tudelft.BinLoader")

// =============================================================================
// View
// =============================================================================

BinLoader::~BinLoader(void)
{
    
}

void BinLoader::init()
{

}

void BinLoader::loadData()
{
    QString fileName = QFileDialog::getOpenFileName(Q_NULLPTR, "Load File", "", "BIN Files (*.bin *)");
    
    // Don't try to load a file if the dialog was cancelled or the file name is empty
    if (fileName.isNull() || fileName.isEmpty())
        return;

    qDebug() << "Loading BIN file: " << fileName;

    std::vector<char> contents;
    std::ifstream in(fileName.toStdString(), std::ios::in | std::ios::binary);
    if (in)
    {
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
    }
    else
    {
        throw DataLoadException(fileName, "File was not found at location.");
    }

    std::vector<float> data;

    BinLoadingInputDialog inputDialog(nullptr);
    inputDialog.setModal(true);

    connect(&inputDialog, &BinLoadingInputDialog::closeDialog, this, &BinLoader::dialogClosed);

    int ok = inputDialog.exec();

    if (ok == QDialog::Accepted) {
        if (_dataType == BinaryDataType::FLOAT)
        {
            for (int i = 0; i < contents.size() / 4; i++)
            {
                float f = ((float*) contents.data())[i];

                data.push_back(f);
            }
        }
        else if (_dataType == BinaryDataType::UBYTE)
        {
            for (int i = 0; i < contents.size(); i++)
            {
                unsigned char c = (unsigned char) contents[i];

                float f = (float)(c);
                data.push_back(f);
            }
        }
    }

    if (ok && !_dataSetName.isEmpty()) {
        QString name = _core->addData("Points", _dataSetName);
        const IndexSet& set = dynamic_cast<const IndexSet&>(_core->requestSet(name));
        PointsPlugin& points = set.getData();

        points.setData(data.data(), data.size() / _numDimensions, _numDimensions);

        qDebug() << "Number of dimensions: " << points.getNumDimensions();

        _core->notifyDataAdded(name);

        qDebug() << "BIN file loaded. Num data points: " << points.getNumPoints();
    }
}

void BinLoader::dialogClosed(unsigned int numDimensions, BinaryDataType dataType, QString dataSetName)
{
    _numDimensions = numDimensions;
    _dataType = dataType;
    _dataSetName = dataSetName;
    qDebug() << _numDimensions << _dataType << _dataSetName;
}

// =============================================================================
// Factory
// =============================================================================

LoaderPlugin* BinLoaderFactory::produce()
{
    return new BinLoader();
}
