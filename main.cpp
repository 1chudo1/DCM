
#include <fstream>
#include <iostream>

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcjson.h"
#include "json.hpp"

using namespace std;
using namespace nlohmann;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "./main <input.dcm> <output.json>\n";
        return 1;
    }

    const char* dcmPath = argv[1];
    const char* jsonPath = argv[2];

    // Загружаем DICOM файл
    DcmFileFormat file;
    OFCondition status = file.loadFile(dcmPath);
    if (status.bad()) {
        cerr << "Ошибка загрузки DCM: " << status.text() << "\n";
        return 1;
    }

    // Настройка JSON формата
    DcmJsonFormatPretty jsonFormat(/*writeMetaHeader*/ OFTrue);

    // Открытие файла для записи
    ofstream out(jsonPath);
    if (!out.is_open()) {
        cerr << "Ошибка открытия файла для зайписи\n";
        return 1;
    }
    
    // Запись JSON в файл
    status = file.writeJson(out, jsonFormat);
    if (status.bad()) {
        cerr << "Ошибка записи JSON: " << status.text() << "\n";
        return 1;
    }

    cout << "Успешно" << dcmPath 
              << " → " << jsonPath << "\n";

    return 0;
}