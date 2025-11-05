#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <iostream>

using namespace std;

class AdvancedDicomProcessor {
public:
    struct DicomMetadata {
        string patientName;
        string patientID;
        string studyDate;
        string modality;
        string studyDescription;
        int width;
        int height;
        int bitsAllocated;
        bool isMonochrome;
    };
    
    // Метод для извлечения всей метаинформации
    static DicomMetadata extractAllMetadata(const string& filename) {
        DicomMetadata metadata;
        
        // загружаем DICOM файл
        DcmFileFormat fileformat;
        OFCondition status = fileformat.loadFile(filename.c_str());
        
        if (status.bad()) {
            cerr << "Ошибка загрузки файла: " << status.text() << endl;
            return metadata; 
        }
        
        // dataset
        DcmDataset* dataset = fileformat.getDataset();
        OFString tempString;
        Sint32 tempInt;
        
        // теги
        if (dataset->findAndGetOFString(DCM_PatientName, tempString).good()) {
            metadata.patientName = tempString.c_str();
        }
        
        if (dataset->findAndGetOFString(DCM_PatientID, tempString).good()) {
            metadata.patientID = tempString.c_str();
        }
        
        if (dataset->findAndGetOFString(DCM_StudyDate, tempString).good()) {
            metadata.studyDate = tempString.c_str();
        }
        
        if (dataset->findAndGetOFString(DCM_Modality, tempString).good()) {
            metadata.modality = tempString.c_str();
        }
        
        if (dataset->findAndGetOFString(DCM_StudyDescription, tempString).good()) {
            metadata.studyDescription = tempString.c_str();
        }

        if (dataset->findAndGetSint32(DCM_BitsAllocated, tempInt).good()) {
            metadata.bitsAllocated = tempInt;
        }
        
        // изображение
        DicomImage image(filename.c_str());
        if (image.getStatus() == EIS_Normal) {
            metadata.width = image.getWidth();
            metadata.height = image.getHeight();
            metadata.isMonochrome = image.isMonochrome();
        }
        
        return metadata;
    }
    
    // вывод информации в .txt
    static void printMetadata(const DicomMetadata& metadata) {
        ofstream outFile("output.txt");
        if (outFile.is_open())
        {
            outFile << "=== DICOM МЕТАДАННЫЕ ===" << endl;
            outFile << "Пациент: " << metadata.patientName << endl;
            outFile << "ID пациента: " << metadata.patientID << endl;
            outFile << "Дата исследования: " << metadata.studyDate << endl;
            outFile << "Модальность: " << metadata.modality << endl;
            outFile << "Описание: " << metadata.studyDescription << endl;
            outFile << "Размер изображения: " << metadata.width << "x" << metadata.height << endl;
            outFile << "Бит на пиксель: " << metadata.bitsAllocated << endl;
            outFile << "Тип: " << (metadata.isMonochrome ? "Монохромное" : "Цветное") << endl;
            outFile.close();
            cout << "Данные записаны в output.txt" << endl;

        }
        else 
        {
            cout << "Ошибка записи файла!" << endl;
        }
        

    }

};

// пример использования
int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Использование: " << argv[0] << " <dicom_file>" << endl;
        return 1;
    }
    
    // извлекаем метаданные
    auto metadata = AdvancedDicomProcessor::extractAllMetadata(argv[1]);
    
    // выводим информацию
    AdvancedDicomProcessor::printMetadata(metadata);
    
    return 0;
}