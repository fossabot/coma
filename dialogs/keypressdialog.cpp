#include "keypressdialog.h"

#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "../widgets/wd_func.h"

#include <QEventLoop>
#include <QMessageBox>
#include <QVBoxLayout>

KeyPressDialog::KeyPressDialog(QWidget *parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
    SetPhrase("Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc");
}

Error::Msg KeyPressDialog::CheckPassword(const QString &psw)
{
    //    m_pswValid = false;
    StdFunc::clearCancel();
    QEventLoop PasswordLoop;
    connect(this, &KeyPressDialog::PasswordChecked, &PasswordLoop, &QEventLoop::quit);
    show();
    PasswordLoop.exec();
    if (StdFunc::isCancelled())
    {
        ERMSG("Отмена ввода пароля");
        return Error::Msg::GeneralError;
    }
    if (m_pswEntered != psw)
    {
        ERMSG("Пароль введён неверно");
        QMessageBox::critical(this, "Неправильно", "Пароль введён неверно", QMessageBox::Ok);
        return Error::Msg::GeneralError;
    }
    return Error::Msg::NoError;
}

void KeyPressDialog::SetupUI()
{
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addWidget(
        WDFunc::NewLBL(this, "Введите пароль\nПодтверждение: клавиша Enter\nОтмена: клавиша Esc", "", "pswlbl"));
    vlyout->addWidget(WDFunc::NewPswLE(this, "pswle", QLineEdit::Password));
    setLayout(vlyout);
}

void KeyPressDialog::SetPhrase(const QString &Phrase)
{
    WDFunc::LE_write_data(this, Phrase, "pswlbl");
}

void KeyPressDialog::keyPressEvent(QKeyEvent *e)
{
    QString str;
    if ((e->modifiers() == Qt::AltModifier) || (e->modifiers() == Qt::ControlModifier))
    {
        ERMSG("Ошибка при обработке пароля");
        return;
    }
    if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
    {
        if (WDFunc::LE_read_data(this, "pswle", m_pswEntered))
            emit PasswordChecked();
    }
    if (e->key() == Qt::Key_Escape)
    {
        StdFunc::cancel();
        emit PasswordChecked();
    }
    QDialog::keyPressEvent(e);
}

void KeyPressDialog::closeEvent(QCloseEvent *e)
{
    QString str;
    WDFunc::LE_read_data(this, "pswle", m_pswEntered);
    emit PasswordChecked();
    //    else
    //        emit Finished(QString());
    e->accept();
}
