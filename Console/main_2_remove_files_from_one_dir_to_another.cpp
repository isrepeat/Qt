//#include <QtCore/QCoreApplication>
//#include <QString>
//#include <QDebug>
//#include <QFile>
//#include <QDir>
//#include <QDirIterator>
//
//#include <filesystem>
//
//void copyAndReplaceFolderContents(const QString& fromDir, const QString& toDir, bool copyAndRemove = false) {
//    QDirIterator it(fromDir, QDirIterator::Subdirectories);
//    QDir dir(fromDir);
//    const int absSourcePathLength = dir.absoluteFilePath(fromDir).length();
//
//    while (it.hasNext()) {
//        it.next();
//        const auto fileInfo = it.fileInfo();
//        if (!fileInfo.isHidden()) { //filters dot and dotdot
//            const QString subPathStructure = fileInfo.absoluteFilePath().mid(absSourcePathLength);
//            const QString constructedAbsolutePath = toDir + subPathStructure;
//
//            if (fileInfo.isDir()) {
//                //Create directory in target folder
//                dir.mkpath(constructedAbsolutePath);
//            }
//            else if (fileInfo.isFile()) {
//                //Copy File to target directory
//
//                //Remove file at target location, if it exists, or QFile::copy will fail
//                QFile::remove(constructedAbsolutePath);
//                QFile::copy(fileInfo.absoluteFilePath(), constructedAbsolutePath);
//            }
//        }
//    }
//
//    if (copyAndRemove)
//        dir.removeRecursively();
//}
//
//
//void CopyFirstItem(const std::wstring& fromDir, const std::wstring& toDir, const std::wstring& prefix = L"") {
//    if (!std::filesystem::exists(toDir)) {
//        std::filesystem::create_directory(toDir);
//    }
//
//    auto it = std::filesystem::directory_iterator(fromDir);
//    const auto& entryPath = it->path();
//    std::filesystem::copy(entryPath, toDir + L"\\" + prefix + entryPath.filename().wstring(), std::filesystem::copy_options::overwrite_existing);
//}
//
//
//int main(int argc, char* argv[]) {
//    QCoreApplication a(argc, argv);
//
//    //QString fromDir = "d:\\WORK\\TEST\\Qt\\CopyFiles\\Logs";
//    //QString toDir = "d:\\WORK\\TEST\\Qt\\CopyFiles\\CrashReport";
//
//    //copyAndReplaceFolderContents(fromDir, toDir, true);
//
//
//    std::wstring loggerPathPrototype = L"d:\\WORK\\TEST\\Qt\\CopyFiles\\Logs_Prototype";
//    std::wstring crashReportPath = L"d:\\WORK\\TEST\\Qt\\CopyFiles\\CrashReport";
//    std::wstring loggerPathQUIC = L"d:\\WORK\\TEST\\Qt\\CopyFiles\\Logs_QUIC";
//
//    //std::filesystem::create_directory(crashReportPath);
//
//
//    //auto it = std::filesystem::directory_iterator(loggerPathPrototype);
//    //const auto& entryPath = it->path();
//    //std::filesystem::copy(entryPath, crashReportPath + L"Prototype_" + entryPath.filename().wstring(), std::filesystem::copy_options::overwrite_existing);
//
//
//    CopyFirstItem(loggerPathPrototype, crashReportPath, L"Prototype_");
//    CopyFirstItem(loggerPathQUIC, crashReportPath, L"QUIC_");
//
//    //for (const auto& entry : std::filesystem::directory_iterator(loggerPathPrototype)) {
//    //    const auto& entryPath = entry.path();
//    //    break;
//    //}
//
//
//    //std::filesystem::copy("d:\\WORK\\TEST\\Qt\\CopyFiles\\Logs", "d:\\WORK\\TEST\\Qt\\CopyFiles\\CrashReport", std::filesystem::copy_options::overwrite_existing);
//    return a.exec();
//}
