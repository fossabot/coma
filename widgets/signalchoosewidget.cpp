#include <QVBoxLayout>
#include "../gen/publicclass.h"
#include "../widgets/wd_func.h"
#include "signalchoosewidget.h"

SignalChooseWidget::SignalChooseWidget(QStringList &snames, QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QString tmps = "QDialog {background-color: "+QString(MAINWINCLRA1)+";}";
    setStyleSheet(tmps);
    for (int i=0; i<snames.size(); ++i)
    {
        QHBoxLayout *hlyout = new QHBoxLayout;
        QCheckBox *chb = WDFunc::NewChB(this, snames.at(i), "");
        connect(chb,SIGNAL(toggled(bool)),this,SLOT(SignalChecked(bool)));
        hlyout->addWidget(chb, 0);
        MarkSignalWidget *w = new MarkSignalWidget(snames.at(i));
        w->setObjectName(snames.at(i));
        connect(w,SIGNAL(Clicked()),this,SLOT(SignalClicked()));
        hlyout->addWidget(w);
        lyout->addLayout(hlyout);
    }
    setLayout(lyout);
}

void SignalChooseWidget::SignalClicked()
{
    QString name = sender()->objectName();
    emit SignalChoosed(name);
}

void SignalChooseWidget::SignalChecked(bool isChecked)
{
    QString name = sender()->objectName();
    emit SignalToggled(name, isChecked);
}

MarkSignalWidget::MarkSignalWidget(const QString &text, QWidget *parent) : QLabel(parent)
{
    setText(text);
    setMouseTracking(true);
}

void MarkSignalWidget::mouseMoveEvent(QMouseEvent *e)
{
    QString Hover = "* {border: 2px;}";
    QString NoHover = "* {border: none;}";
    if (e->type() == QEvent::HoverEnter)
    {
        setCursor(Qt::PointingHandCursor);
        setStyleSheet(Hover);
    }
    else if (e->type() == QEvent::HoverLeave)
    {
        setCursor(Qt::ArrowCursor);
        setStyleSheet(NoHover);
    }
    e->accept();
}

void MarkSignalWidget::mousePressEvent(QMouseEvent *e)
{
    emit Clicked();
    e->accept();
}
