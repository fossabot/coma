#ifndef CHECKDIALOGA1_H
#define CHECKDIALOGA1_H

#include "abstractcheckdialog.h"
#include "checka1.h"

#define A1_BDA_IN_BN        1
#define A1_BDA_IN_AN_BN     2
#define A1_BDA_H_BN         3
#define A1_BDA_OUT_BN       4
#define A1_BDA_OUT_AN_BN    5

class CheckDialogA1 : public AbstractCheckDialog
{
    Q_OBJECT
public:
    explicit CheckDialogA1(QWidget *parent = 0);

signals:

public slots:

private:
    CheckA1 *ChA1;
    const QString ValuesFormat = "QLabel {border: 1px solid red; border-radius: 4px; padding: 1px; color: red; font: bold 10px;}";

    QWidget *AutoCheckUI(); // UI для автоматической проверки модуля
    QWidget *BdUI(int bdnum); // визуализация наборов текущих данных от модуля
    void SetupUI();
    void RefreshAnalogValues(int bdnum); // обновление полей в GUI из полученного соответствующего Bd_block
    void PrepareHeadersForFile(int row); // row - строка для записи заголовков
    void WriteToFile(int row, int bdnum); // row - номер строки для записи в файл xlsx, bdnum - номер блока данных
    void ChooseValuesToWrite();
    void SetDefaultValuesToWrite();
};

#endif // CHECKDIALOGA1_H
