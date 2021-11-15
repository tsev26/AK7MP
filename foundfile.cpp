#include "foundfile.h"

FoundFile::FoundFile()
{

}

FoundFile::FoundFile(QFileInfo fileInfo, QString textAroundString, bool inText)
{
    this->fileInfo = fileInfo;
    this->textAroundString = textAroundString;
    this->inText = inText;
}

const QFileInfo &FoundFile::getFileInfo() const
{
    return fileInfo;
}

const QString &FoundFile::getTextAroundString() const
{
    return textAroundString;
}


bool FoundFile::getInText() const
{
    return inText;
}

/*
bool FoundFile::compareByName(FoundFile f1, FoundFile f2)
{
    return f1.getFileInfo().baseName() > f2.getFileInfo().absolutePath();
}
*/
