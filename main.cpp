#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcjson.h"
#include "json.hpp"

using namespace std;
using namespace nlohmann;


static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

std::string base64Encode(const uint8_t* data, size_t len)
{
    std::string out;
    int i = 0;
    uint32_t val = 0;

    for (size_t pos = 0; pos < len; ++pos) {
        val = (val << 8) + data[pos];
        i += 8;

        while (i >= 6) {
            out.push_back(base64_chars[(val >> (i - 6)) & 0x3F]);
            i -= 6;
        }
    }

    if (i > 0) {
        val <<= (6 - i);
        out.push_back(base64_chars[val & 0x3F]);
    }

    while (out.size() % 4)
        out.push_back('=');

    return out;
}


int main(int argc, char* argv[])
{
    if (argc < 3) {
        cerr << "./main <input.dcm> <output.json>\n";
        return 1;
    }

    const char* dcmPath  = argv[1];
    const char* jsonPath = argv[2];

    // дайком 
    DcmFileFormat file;
    OFCondition status = file.loadFile(dcmPath);
    if (status.bad()) {
        cerr << "Ошибка загрузки DCM: " << status.text() << "\n";
        return 1;
    }

    DcmMetaInfo* meta = file.getMetaInfo();
    DcmDataset*  ds   = file.getDataset();

    // метадата
    DcmJsonFormatPretty fmt(OFTrue);
    ostringstream metaStream;
    meta->writeJson(metaStream, fmt);

    json metaRoot = json::parse("{ " + metaStream.str() + " }");

    json metaArray = json::array();
    for (auto& el : metaRoot.items()) {
        json obj;
        obj[el.key()] = el.value();
        metaArray.push_back(obj);
    }

    // датасет
    ostringstream dataStream;
    ds->writeJson(dataStream, fmt);

    json dataRoot = json::parse("{ " + dataStream.str() + " }");

    json dataArray = json::array();
    for (auto& el : dataRoot.items()) {
        json obj;
        obj[el.key()] = el.value();
        dataArray.push_back(obj);
    }

    // картинка
    Uint16 rows = 0, cols = 0;
    Uint16 bitsAllocated = 0, samplesPerPixel = 1;
    OFString photoMetric;
    double wc = 0.0, ww = 0.0;

    ds->findAndGetUint16(DCM_Rows, rows);
    ds->findAndGetUint16(DCM_Columns, cols);
    ds->findAndGetUint16(DCM_BitsAllocated, bitsAllocated);
    ds->findAndGetUint16(DCM_SamplesPerPixel, samplesPerPixel);
    ds->findAndGetOFString(DCM_PhotometricInterpretation, photoMetric);
    ds->findAndGetFloat64(DCM_WindowCenter, wc);
    ds->findAndGetFloat64(DCM_WindowWidth, ww);

    const Uint8* pixelData = nullptr;
    unsigned long pixelSize = 0;

    status = ds->findAndGetUint8Array(DCM_PixelData, pixelData, &pixelSize);
    if (status.bad() || !pixelData) {
        cerr << "PixelData не найден\n";
        return 1;
    }

    json imageData;
    imageData["rows"] = rows;
    imageData["columns"] = cols;
    imageData["samplesPerPixel"] = samplesPerPixel;
    imageData["bitsAllocated"] = bitsAllocated;
    imageData["photometricInterpretation"] = photoMetric.c_str();
    imageData["windowCenter"] = wc;
    imageData["windowWidth"]  = ww;
    imageData["pixelDataBase64"] = base64Encode(pixelData, pixelSize);

    // json сборка
    json root;
    root["filename"]  = dcmPath;
    root["metaData"]  = metaArray;
    root["dataSet"]   = dataArray;
    root["imageData"] = imageData;

    // созранинение json 
    ofstream out(jsonPath);
    if (!out.is_open()) {
        cerr << "Ошибка открытия JSON файла\n";
        return 1;
    }

    out << root.dump(4);

    cout << "Готово: " << jsonPath << "\n";
    return 0;
}
