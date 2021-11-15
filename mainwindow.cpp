#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTextEdit>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QDirIterator>
#include <QFont>
#include <QLocale>
#include <QSettings>




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->horizontalWidget_2->setVisible(false);

    startProgramWithValueFromRegister();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startProgramWithValueFromRegister()
{
    QSettings set;
    ui->PathEdit->setText(set.value("general/path","").toString());
    ui->ExpressionEdit->setText(set.value("general/expression","").toString());
    ui->SQLcheckBox->setChecked(set.value("general/suffixes/sql",true).toBool());
    ui->VBScheckBox->setChecked(set.value("general/suffixes/vbs",false).toBool());
    ui->TXTcheckBox->setChecked(set.value("general/suffixes/txt",true).toBool());
    ui->CSVcheckBox->setChecked(set.value("general/suffixes/csv",false).toBool());
    ui->JSONcheckBox->setChecked(set.value("general/suffixes/json",false).toBool());

    ui->InTextCheckBox->setChecked(set.value("general/searchIn/text",true).toBool());
    ui->InNameCheckBox->setChecked(set.value("general/searchIn/name",true).toBool());

    switch (set.value("general/orderBy","1").toInt())
    {
    case 0: ui->OrderByPathRadioButton->setChecked(true);
        break;
    case 1: ui->OrderByNameRadioButton->setChecked(true);
        break;
    case 2: ui->OrderBySearchTypeRadioButton->setChecked(true);
        break;
    case 3: ui->OrderByCreatedRadioButton->setChecked(true);
        break;
    case 4: ui->OrderByUpdatedRadioButton->setChecked(true);
        break;
    case 5: ui->OrderBySuffixRadioButton->setChecked(true);
        break;
    default: ui->OrderByNameRadioButton->setChecked(true);
        break;
    }

    if (set.value("general/lang","cz") == "cz"){
        locale  = QLocale(QLocale::Czech);
        /*
        ui->actionEnglish->setChecked(false);
        ui->actionCzech->setChecked(true);
        trans->load(":/lang/translation_cs.qm");
        */
    }else if (set.value("general/lang","cz") == "en"){
        locale  = QLocale(QLocale::English);
        /*
        ui->actionEnglish->setChecked(true);
        ui->actionCzech->setChecked(false);
        trans->load(":/lang/translation_en.qm");
        */
    }

}

void MainWindow::saveValueToRegister()
{
    QSettings set;

    set.setValue("general/path",ui->PathEdit->text());
    set.setValue("general/expression",ui->ExpressionEdit->text());

    set.setValue("general/suffixes/sql",ui->SQLcheckBox->isChecked());
    set.setValue("general/suffixes/vbs",ui->VBScheckBox->isChecked());
    set.setValue("general/suffixes/txt",ui->TXTcheckBox->isChecked());
    set.setValue("general/suffixes/csv",ui->CSVcheckBox->isChecked());
    set.setValue("general/suffixes/json",ui->JSONcheckBox->isChecked());

    set.setValue("general/searchIn/text",ui->InTextCheckBox->isChecked());
    set.setValue("general/searchIn/name",ui->InNameCheckBox->isChecked());
}


void MainWindow::on_pickFolder_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Vybrat"),
                                                "/home",
                                                QFileDialog::ShowDirsOnly
                                                );

    ui->PathEdit->setText(dir);
}


void MainWindow::on_search_clicked()
{

    QString targetStr = ui->ExpressionEdit->text();

    if (targetStr.length() < 3) {
        QMessageBox::critical(this, "TSevcu app", "Délka hledaného výrazu musím mít alespoň 3 znaky!");
        return;
    }

    QString directory = ui->PathEdit->text();

    QDir pathDir(directory);
    if (!pathDir.exists() || directory.length() < 2){
        QMessageBox::critical(this, "TSevcu app", "Zadaná cesta není validní!");
        return;
    }
    QList<QString> checkedExtensions = selectedExtensions();
    if (checkedExtensions.count() == 0){
        QMessageBox::critical(this, "TSevcu app", "Vyberte alespoň 1 příponu souboru!");
        return;
    }

    if (!ui->InNameCheckBox->isChecked() && !ui->InTextCheckBox->isChecked()){
        QMessageBox::critical(this, "TSevcu app", "Vyberte alespoň 1 možnost, kde vyhledávat!");
        return;
    }

    restartListsAndTexts();


    QDirIterator it(directory, QDirIterator::Subdirectories);

    // Iterate through the directory using the QDirIterator
    while (it.hasNext()) {
        QString filename = it.next();
        QFileInfo file(filename);

        if (file.isDir()) { // Check if it's a dir
            continue;
        }

        //podle obsahu souboru
        if(ui->InTextCheckBox->isChecked()){
            if (checkedExtensions.contains(file.suffix())) {
                QFile qfile(file.absoluteFilePath());
                if (qfile.open(QIODevice::Text | QIODevice::ReadOnly)) {

                    QString string = (QString::fromUtf8(qfile.readAll()));
                    if (string.contains(targetStr, Qt::CaseInsensitive)){
                        int value = string.indexOf(targetStr);
                        QString finalString;
                        if (value < 25){
                            finalString = string.left(50);
                        }else{
                            finalString = string.mid(value-25,50);
                        }
                        foundFileList.push_back(new FoundFile(file, finalString, true));
                        continue;
                    }
                    qfile.close();
                }
            }
        }


        //podle nazvu souboru
        if(ui->InNameCheckBox->isChecked()){
            if (file.fileName().contains(targetStr, Qt::CaseInsensitive) && checkedExtensions.contains(file.suffix())) {
                QFile qfile(file.absoluteFilePath());
                if (qfile.open(QIODevice::Text | QIODevice::ReadOnly)) {
                    QString startString = (QString::fromUtf8(qfile.readAll())).left(50);
                    qfile.close();

                    foundFileList.push_back(new FoundFile(file, startString, false));
                }
            }
        }

    }

    if(ui->OrderByPathRadioButton->isChecked()){
        on_OrderByPathRadioButton_clicked();
    }else if(ui->OrderByNameRadioButton->isChecked()){
        on_OrderByNameRadioButton_clicked();
    }else if(ui->OrderBySearchTypeRadioButton->isChecked()){
        on_OrderBySearchTypeRadioButton_clicked();
    }else if(ui->OrderByCreatedRadioButton->isChecked()){
        on_OrderByCreatedRadioButton_clicked();
    }else if(ui->OrderByUpdatedRadioButton->isChecked()){
        on_OrderByUpdatedRadioButton_clicked();
    }else if(ui->OrderBySuffixRadioButton->isChecked()){
        on_OrderBySuffixRadioButton_clicked();
    }else {
        populateListWidget();
    }

    saveValueToRegister();
}

void MainWindow::populateListWidget()
{
    foreach (auto foundFile, foundFileList){
        QListWidgetItem* itemToAdd = new QListWidgetItem();
        itemToAdd->setText(foundFile->getFileInfo().fileName());
        itemToAdd->setToolTip(foundFile->getTextAroundString());
        if (foundFile->getInText()){
            itemToAdd->setForeground(Qt::blue);
        }else{
            itemToAdd->setForeground(Qt::red);
        }
        itemToAdd->setData(12,foundFile->getFileInfo().absoluteFilePath());
        ui->listWidget->addItem(itemToAdd);
    }
}


QList<QString> MainWindow::selectedExtensions()
{
    QList<QString> extensionsList;
    QList<QCheckBox *> checkBoxes = ui->groupBox->findChildren<QCheckBox *>();
    for (auto &checkbox : checkBoxes){
        if (checkbox->isChecked()){
            extensionsList.append(checkbox->text());
        }
    }
    return extensionsList;
}



void MainWindow::restartListsAndTexts()
{
    //ui->listWidget->clear();
    ui->PickFileLabel->setText("Vyber soubor");
    ui->textEdit->setText("");
    ui->DateCreatedLabel->setText("");
    ui->DateCreatedNameLabel->setText("");
    ui->DateUpdatedLabel->setText("");
    ui->DateUpdatedNameLabel->setText("");
    ui->PathFileLabel->setText("");
    ui->horizontalWidget_2->setVisible(false);
    for (auto foundFile : foundFileList){
        delete foundFile;
    }
    foundFileList.clear();
}


void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    QString fileName = item->data(12).toString();
    ui->PickFileLabel->setText(item->text());
    QFileInfo fileInfo(fileName);

    ui->DateCreatedLabel->setText(locale.toString(fileInfo.birthTime(), "dd.MM.yy"));
    ui->DateCreatedNameLabel->setText("Vytvořeno:");
    ui->DateUpdatedLabel->setText(locale.toString(fileInfo.lastModified(), "dd.MM.yy"));
    ui->DateUpdatedNameLabel->setText("Upraveno:");

    ui->PathFileLabel->setText(fileInfo.absolutePath().right(fileInfo.absolutePath().length()-ui->PathEdit->text().length()));

    ui->horizontalWidget_2->setVisible(true);
    if (!fileName.isEmpty()){
        QFile file(fileName);
        if (file.open(QIODevice::Text | QIODevice::ReadOnly)) {
            QString text = file.readAll();
            if(fileInfo.suffix() == "sql"){
                sqlSyntaxHighlighter = new SQLSyntaxHighlighter(ui->textEdit->document());
                ui->textEdit->setText(text);
            }else{
                ui->textEdit->setText(text);
            }
            //ui->textEdit->setText(text);
            file.close();
        }else{
            //FoundFileList.removeAll(item); //nevim jestli funguje tak jak ma, mozna mi tam chybi operator porovnani (ale kdyz funguje contains, tka by mohlo fungovat i tohle)
            QMessageBox::critical(this, "TSevcu App", "Soubor se nepodařilo otevřít!");
        }
    }
}



void MainWindow::on_OrderByPathRadioButton_clicked()
{
    QSettings set;
    set.setValue("general/orderBy","0");

    ui->listWidget->clear();

    std::sort(foundFileList.begin(), foundFileList.end(), [](FoundFile *f1, FoundFile *f2){
        if (f1->getFileInfo().absoluteFilePath().toLower() == f2->getFileInfo().absoluteFilePath().toLower()) {
            return f1->getFileInfo().baseName().toLower() < f2->getFileInfo().baseName().toLower();
        }
        return f1->getFileInfo().absoluteFilePath().toLower() < f2->getFileInfo().absoluteFilePath().toLower();
    });

    populateListWidget();
}


void MainWindow::on_OrderByNameRadioButton_clicked()
{
    QSettings set;
    set.setValue("general/orderBy","1");

    ui->listWidget->clear();

    std::sort(foundFileList.begin(), foundFileList.end(), [](FoundFile *f1, FoundFile *f2){
        return f1->getFileInfo().baseName().toLower() < f2->getFileInfo().baseName().toLower();
    });

    populateListWidget();
}


void MainWindow::on_OrderBySearchTypeRadioButton_clicked()
{
    QSettings set;
    set.setValue("general/orderBy","2");

    ui->listWidget->clear();

    std::sort(foundFileList.begin(), foundFileList.end(), [](FoundFile *f1, FoundFile *f2){
        if (f1->getInText() == f2->getInText()) {
            return f1->getFileInfo().baseName().toLower() < f2->getFileInfo().baseName().toLower();
        }
        return f1->getInText() < f2->getInText();
    });

    populateListWidget();
}


void MainWindow::on_OrderByCreatedRadioButton_clicked()
{
    QSettings set;
    set.setValue("general/orderBy","3");

    ui->listWidget->clear();

    std::sort(foundFileList.begin(), foundFileList.end(), [](FoundFile *f1, FoundFile *f2){
        if (f1->getFileInfo().birthTime() == f2->getFileInfo().birthTime()) {
            return f1->getFileInfo().baseName().toLower() < f2->getFileInfo().baseName().toLower();
        }
        return f1->getFileInfo().birthTime() < f2->getFileInfo().birthTime();
    });

    populateListWidget();
}


void MainWindow::on_OrderByUpdatedRadioButton_clicked()
{
    QSettings set;
    set.setValue("general/orderBy","4");

    ui->listWidget->clear();

    std::sort(foundFileList.begin(), foundFileList.end(), [](FoundFile *f1, FoundFile *f2){
        if (f1->getFileInfo().lastModified() == f2->getFileInfo().lastModified()) {
            return f1->getFileInfo().baseName().toLower() < f2->getFileInfo().baseName().toLower();
        }
        return f1->getFileInfo().lastModified() < f2->getFileInfo().lastModified();
    });

    populateListWidget();
}


void MainWindow::on_OrderBySuffixRadioButton_clicked()
{
    QSettings set;
    set.setValue("general/orderBy","5");

    ui->listWidget->clear();

    std::sort(foundFileList.begin(), foundFileList.end(), [](FoundFile *f1, FoundFile *f2){
        if (f1->getFileInfo().suffix() == f2->getFileInfo().suffix()) {
            return f1->getFileInfo().baseName().toLower() < f2->getFileInfo().baseName().toLower();
        }
        return f1->getFileInfo().suffix() < f2->getFileInfo().suffix();
    });

    populateListWidget();
}


