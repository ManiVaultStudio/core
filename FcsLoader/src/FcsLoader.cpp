#include "FcsLoader.h"

#include "FcsHeader.h"
#include "PointData.h"
#include "Set.h"

#include <QtCore>
#include <QtDebug>
#include <QString>

#include <QInputDialog>

#include <vector>
#include <sstream>
#include <fstream>

#include <cassert>
#include <iostream>

Q_PLUGIN_METADATA(IID "nl.tudelft.FcsLoader")

// =============================================================================
// Loader
// =============================================================================

FcsLoader::FcsLoader()
    : LoaderPlugin("FCS Loader"),
    _fileName(),
    _textHeader(),
    _separator('\0')
{

}

FcsLoader::~FcsLoader(void)
{
    
}

void FcsLoader::init()
{

}

void FcsLoader::loadData()
{
    const QString fileName = AskForFileName(QObject::tr("FCS Files (*.fcs)"));

    // Don't try to load a file if the dialog was cancelled or the file name is empty
    if (fileName.isNull() || fileName.isEmpty())
        return;

    qDebug() << "Loading FCS file: " << fileName;

    float** rawData = new float*;
    FcsHeader header;
    _fileName = fileName.toStdString();

    std::ifstream file;
    file.open(_fileName, std::ios::binary | std::ios::in);

    if (!file.is_open())
        throw DataLoadException(fileName, "File was not found at location.");

    size_t bufferLength = 64;
    std::string buffer(bufferLength, '\0');

    // =========================================================================
    // first 64 byte
    file.read(&buffer[0], bufferLength);
    file.seekg(0);

    std::string version;
    file >> version;

    // =========================================================================
    // file version
    if (version == "FCS3.0")
    {
        header.setFcsVersion("FCS3.0");
    }
    else if (version == "FCS2.0")
    {
        header.setFcsVersion("FCS2.0");
    }
    else if (version == "FCS1.0")
    {
        throw DataLoadException(fileName, "FCS version 1.0 is not supported");
    }
    else
    {
        throw DataLoadException(fileName, "Could not find FCS version in header");
    }

    // =========================================================================
    // header info
    int headerStartByte = stoi(buffer.substr(10, 8));
    int headerStopByte = stoi(buffer.substr(18, 8));
    int dataStartByte = stoi(buffer.substr(26, 8));
    //	int dataStopByte = -1;

    //	unsigned long headerStartByte, headerStopByte, dataStartByte, dataStopByte;
    //	file >> headerStartByte >> headerStopByte >> dataStartByte >> dataStopByte;

    //std::cout << "\n\n=================================\n\n";

    //std::cout << "First 64byte: " << buffer << "\n\n";

    //std::cout << "Header startbyte: " << headerStartByte << "\n\n";
    //std::cout << "Header stopbyte: " << headerStopByte << "\n\n";
    //std::cout << "Data startbyte: " << dataStartByte << "\n\n";
    ////std::cout << "Data stopbyte: " << dataStopByte << "\n\n";

    if (dataStartByte == 0)
    {
        dataStartByte = -1;
    }

    //std::cout << "=================================\n\n";

    // =========================================================================
    // load the header
    size_t headerLength = headerStopByte - headerStartByte + 1;
    _textHeader = std::string(headerLength, '\0');

    file.seekg(headerStartByte);
    file.read(&_textHeader[0], headerLength);

    //std::cout << "Info Header:\n" << _textHeader << "\n\n";

    _separator = _textHeader[0];
    if (_separator != '\\' && _separator != '!' && _separator != '|' && _separator != '/' && _separator != '\f')
    {
        throw DataLoadException(fileName, "Unknown separator in file header");
    }

    header.setNumEvents(stoi(fetchValueForKey("$TOT")));
    header.createVariables(stoi(fetchValueForKey("$PAR")));

    std::vector<FcsVariable>* variables = header.getVariables();

    for (int i = 0; i < header.getNumVariables(); i++)
    {
        std::string idx_string = std::to_string(i + 1);

        FcsVariable var;

        var.name[1] = fetchValueForKey("$P" + idx_string + "N");
        var.name[0] = fetchValueForKey("$P" + idx_string + "S");
        var.range = stoi(fetchValueForKey("$P" + idx_string + "R"));
        var.numBits = stoi(fetchValueForKey("$P" + idx_string + "B"));
        var.exponent = stod(fetchValueForKey("$P" + idx_string + "E"));

        //		std::cout << "\nP" << i+1 << ": " << var.name[0] << "/" << var.name[1] << "/" << var.numBits << "/" << var.range << "/" << var.exponent;

        (*variables)[i] = var;
    }

    header.setStartTime(fetchValueForKey("$BTIM"));
    header.setStopTime(fetchValueForKey("$ETIM"));
    header.setCytometry(fetchValueForKey("$CYT"));
    header.setDate(fetchValueForKey("$DATE"));
    header.setByteOrder(fetchValueForKey("$BYTEORD"));
    header.setDataType(fetchValueForKey("$DATATYPE"));
    header.setSystem(fetchValueForKey("$SYS"));
    header.setProject(fetchValueForKey("$PROJ"));
    header.setExperiment(fetchValueForKey("$EXP"));
    header.setCells(fetchValueForKey("$Cells"));
    header.setCreator(fetchValueForKey("CREATOR"));

    // =========================================================================
    // load the data
    if (dataStartByte < 0)
    {
        dataStartByte = stoi(fetchValueForKey("$BEGINDATA"));
    }
    if (dataStartByte < 0)
    {
        throw DataLoadException(fileName, "Data start byte is less than 0.");
    }
    //	assert(dataStartByte == stoi(fetchValueForKey("$BEGINDATA")));
    int itemSize = (*variables)[0].numBits / 8;
    int numItems = header.getNumEvents() * header.getNumVariables();
    int dataSize = itemSize * numItems;

    char* cdata = new char[dataSize];

    file.seekg(dataStartByte);
    file.read(cdata, dataSize);

    bool swapBytes = !header.getByteOrder().compare("4,3,2,1");
    if (swapBytes)
    {
        for (int i = 0; i < numItems; i++)
        {
            char tmp[4] = { cdata[4 * i], cdata[4 * i + 1], cdata[4 * i + 2], cdata[4 * i + 3] };

            cdata[4 * i + 0] = tmp[3];
            cdata[4 * i + 1] = tmp[2];
            cdata[4 * i + 2] = tmp[1];
            cdata[4 * i + 3] = tmp[0];
        }
    }

    *rawData = (float*)cdata;

    if (!header.getFcsVersion().compare("FCS2.0"))
    {
        if (_separator == '!')
        {
            // TODO
        }
        else
        {
            // TODO
        }
    }
    else if (!header.getFcsVersion().compare("FCS3.0"))
    {
        if (_separator == '|')
        {
            // TODO
        }
        else // ordinary FCS 3.0, the only thing to test against atm
        {
            // TODO
        }
    }
    else
    {
        throw DataLoadException(fileName, "Unsupported FCS version.");
    }

    file.close();

    header.initDerivedData();

    ////////////////////////////////
    std::vector<float> data(*rawData, *rawData + numItems);
    int numDimensions = header.getNumVariables();
    std::vector<QString> dimNames;
    for (int i = 0; i < numDimensions; i++) {
        dimNames.push_back(QString(header.getVariableName(i).c_str()));
    }

    //QTextStream in(&file);
    //while (!in.atEnd())
    //{
    //    QString line = in.readLine();
    //    QStringList tokens = line.split(",");

    //    numDimensions = tokens.size();
    //    for (int i = 0; i < tokens.size(); i++)
    //    {
    //        if (tokens[i].isEmpty())
    //            continue;
    //        float f = tokens[i].toFloat();

    //        data.push_back(f);
    //    }
    //}

    bool ok;
    QString dataSetName = QInputDialog::getText(nullptr, "Add New Dataset",
        "Dataset name:", QLineEdit::Normal, "DataSet", &ok);

    if (ok && !dataSetName.isEmpty()) {
        QString name = _core->addData("Points", dataSetName);
        Points& points = _core->requestData<Points>(name);
        
        points.setData(data.data(), data.size() / numDimensions, numDimensions);
        points.setDimensionNames(dimNames);

        qDebug() << "Number of dimensions: " << points.getNumDimensions();

        _core->notifyDataAdded(name);

        qDebug() << "FCS file loaded. Num data points: " << points.getNumPoints();
    }
}

std::string FcsLoader::fetchValueForKey(const std::string key)
{
    size_t keySize = key.size();
    size_t keyOffset = _textHeader.find(key);

    if (keyOffset == std::string::npos) return "-1";

    assert(_textHeader.size() > keyOffset + keySize);
    if (_textHeader[keyOffset + keySize] != _separator) return "-1";

    size_t valueOffset = keyOffset + keySize + 1;
    size_t nextKeyOffset = _textHeader.find(_separator, valueOffset);

    size_t valueSize = nextKeyOffset - valueOffset;

    std::string result = _textHeader.substr(valueOffset, valueSize);

    assert(!result.empty());

    return result;
}

// =============================================================================
// Factory
// =============================================================================

LoaderPlugin* FcsLoaderFactory::produce()
{
    return new FcsLoader();
}
