#include <iostream>
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcjson.h"


using namespace std;

string extractMetaDataJSON(DcmFileFormat& file)
{
    DcmMetaInfo* meta = file.getMetaInfo();

    DcmJsonFormatPretty fmt(OFTrue);
    ostringstream stream;

    meta->writeJson(stream, fmt);

    return stream.str();
}


string extractDataSetJSON(DcmFileFormat& file)
{
    DcmDataset* ds = file.getDataset();

    DcmJsonFormatPretty fmt(OFTrue);
    ostringstream stream;

    ds->writeJson(stream, fmt);

    return stream.str();
}

vector<uint8_t> extractImageBytes(DcmFileFormat& file)
{
    DcmDataset* ds = file.getDataset();

    const Uint8* pixelData = nullptr;
    unsigned long pixelSize = 0;

    OFCondition status = ds->findAndGetUint8Array(DCM_PixelData, pixelData, &pixelSize);
    if (status.bad() || !pixelData) {
        throw runtime_error("PixelData не найден");
    }

    return vector<uint8_t>(pixelData, pixelData + pixelSize);
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        cerr << "./main <input.dcm>\n";
        return 1;
    }

    const char* dcmPath = argv[1];

    DcmFileFormat file;
    if (file.loadFile(dcmPath).bad()) {
        cerr << "Ошибка загрузки DCM\n";
        return 1;
    }

    try {
        string metaJson = extractMetaDataJSON(file);
        string dataSetJson = extractDataSetJSON(file);
        vector<uint8_t> imageBytes = extractImageBytes(file);

        cout << "Meta JSON size: " << metaJson.size() << "\n";
        cout << "DataSet JSON size: " << dataSetJson.size() << "\n";
        cout << "Image bytes: " << imageBytes.size() << "\n";

    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
