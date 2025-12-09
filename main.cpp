#include <iostream>
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcjson.h"
#include "dcmtk/dcmimgle/dcmimage.h"


using namespace std;

string extractMetaDataJSON(DcmFileFormat& file)
{
    DcmMetaInfo* meta = file.getMetaInfo();

    DcmJsonFormatCompact fmt(OFTrue);
    ostringstream stream;

    meta->writeJson(stream, fmt);

    return stream.str();
}


string extractDataSetJSON(DcmFileFormat& file)
{
    DcmDataset* ds = file.getDataset();

    DcmJsonFormatCompact fmt(OFTrue);
    ostringstream stream;

    ds->writeJson(stream, fmt);

    return stream.str();
}

void extractImageBytes(const char* dcmFile, const char* bmpFile)
{
    DicomImage image(dcmFile);

    int status = image.writeBMP(bmpFile);
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        cerr << "./main <input.dcm>\n";
        return 1;
    }

    const char* dcmPath = argv[1];
    const char* bmpPath = "output.bmp";

    DcmFileFormat file;
    if (file.loadFile(dcmPath).bad()) {
        cerr << "Ошибка загрузки DCM\n";
        return 1;
    }

    try {
        string metaJson = extractMetaDataJSON(file);
        string dataSetJson = extractDataSetJSON(file);
        
        extractImageBytes(dcmPath, bmpPath);

        cout << "Meta JSON size: " << metaJson.size() << "\n";
        cout << "DataSet JSON size: " << dataSetJson.size() << "\n";

    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << "\n";
        return 1;
    }

    return 0;
}