#ifndef FOUNDFILE_H
#define FOUNDFILE_H

#include <QString>
#include <QFileInfo>

class FoundFile
{
public:
    FoundFile();
    FoundFile(QFileInfo fileInfo, QString textAroundString, bool inText);

    const QFileInfo &getFileInfo() const;

    const QString &getTextAroundString() const;

    bool getInText() const;


    /*
    bool compareByName(FoundFile f1, FoundFile f2);

    bool compareByPath();

    bool compareBySearchType();

    bool compareByCreated();

    bool compareByUpdated();

    bool compareBySuffix();
    */

private:
    QFileInfo fileInfo;
    QString textAroundString;
    bool inText;

};

#endif // FOUNDFILE_H
