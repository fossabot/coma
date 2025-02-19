#include "module.h"

#include "../comaversion/comaversion.h"
#include "../config/configdialog.h"
#include "../ctti/type_id.hpp"
#include "../dialogs/fwuploaddialog.h"
#include "../dialogs/infodialog.h"
#include "../dialogs/journalsdialog.h"
#include "../dialogs/timedialog.h"
#include "../gen/board.h"
#include "../gen/s2.h"
#include "../gen/stdfunc.h"
#include "../gen/xmlparser.h"
#include "../widgets/udialog.h"
//#define XML_DEBUG

#include <QDir>
#include <QtXml>

constexpr char versionFile[] = "coma.ver";

Module::Module(QObject *parent) : QObject(parent)
{
}
void Module::createAlarm(AlarmWidget *aw)
{
    using namespace Modules;

    if (m_settings)
        BaseInterface::iface()->setSettings(settings()->ifaceSettings);

    Q_ASSERT(aw->count() == 0);
    AlarmStateAll *alarmStateAll = new AlarmStateAll;
    alarmStateAll->setupUI(AVM::HthToolTip);
    aw->addAlarm(alarmStateAll);
    if (m_settings)
    {
        if (settings()->alarms.contains((AlarmType::Warning)))
        {
            auto *warnAlarm = new ModuleAlarm(settings()->alarms.value(AlarmType::Warning), settings()->alarmCount());
            aw->addAlarm(warnAlarm, tr("Предупредительная сигнализация"));
        }
        if (settings()->alarms.contains((AlarmType::Critical)))
        {
            auto *critAlarm = new ModuleAlarm(settings()->alarms.value(AlarmType::Critical), settings()->alarmCount());
            aw->addAlarm(critAlarm, tr("Аварийная сигнализация"));
        }
    }
}

QList<UDialog *> Module::dialogs()
{
    QList<UDialog *> list = m_dialogs;
    return list;
}

QList<UDialog *> Module::confDialogs()
{
    QList<UDialog *> list;
    for (UDialog *dlg : qAsConst(m_dialogs))
    {
        if (dlg->objectName().contains("conf"))
            list.append(dlg);
    }
    return list;
}

void Module::addDialogToList(UDialog *dlg, const QString &caption, const QString &name)
{
    dlg->setObjectName(name);
    dlg->setCaption(caption);
    m_dialogs.append(dlg);
}

void Module::insertDialogToList(UDialog *dlg, int pos, const QString &caption, const QString &name)
{
    dlg->setObjectName(name);
    dlg->setCaption(caption);
    m_dialogs.insert(pos, dlg);
}

void Module::parentTWTabChanged(int index)
{
    if (index == -1 || m_dialogs.isEmpty())
        return;

    for (auto &dialog : m_dialogs)
    {
        if (dialog->updatesEnabled())
            dialog->setUpdatesDisabled();
    }

    UDialog *udlg = m_dialogs.at(index);

    udlg->setEnabled(true);
    udlg->setUpdatesEnabled();
    udlg->reqUpdate();
}

void Module::closeDialogs()
{
    while (!m_dialogs.isEmpty())
        m_dialogs.takeFirst()->close();
}

ModuleSettings *Module::settings() const
{
    return m_settings.get();
}

bool Module::loadSettings()
{
    if (!loadS2Settings())
        return false;
    auto moduleName = Board::GetInstance().moduleName();
    if (moduleName.isEmpty())
        return false;
    if (moduleName.contains("-"))
    {
        QRegularExpression regex("(?<=[-])\\d+");
        QRegularExpressionMatch match = regex.match(moduleName);
        if (!match.hasMatch())
            return false;

        moduleName = /*moduleName.split("-").last();*/ match.captured(0);
    }
    QDir directory(StdFunc::GetSystemHomeDir());
    qDebug() << directory;
    auto allFiles = directory.entryList(QDir::Files);
    auto xmlFiles = allFiles.filter(".xml");
    qDebug() << xmlFiles;
    QDomDocument domDoc;
    QFile file;
    for (const auto &xmlFile : xmlFiles)
    {
        if (xmlFile.contains(moduleName, Qt::CaseInsensitive))
            file.setFileName(directory.filePath(xmlFile));
    }
    if (file.fileName().isEmpty())
    {
        directory = QDir(":/module");
        allFiles = directory.entryList(QDir::Files);
        xmlFiles = allFiles.filter(".xml");
        qDebug() << xmlFiles;
        for (const auto &xmlFile : qAsConst(xmlFiles))
        {
            if (!xmlFile.contains(moduleName, Qt::CaseInsensitive))
                continue;
            if (!QFile::copy(directory.filePath(xmlFile), StdFunc::GetSystemHomeDir() + xmlFile))
            {
                qCritical() << Error::DescError;
                qInfo() << directory.filePath(xmlFile) << StdFunc::GetSystemHomeDir() + xmlFile;
                return false;
            }

            return loadSettings();
        }
    }
    if (!file.open(QIODevice::ReadOnly))
    {
        qCritical() << Error::FileOpenError << file.fileName();
        return false;
    }
    if (domDoc.setContent(&file))
    {
        m_settings = std::unique_ptr<ModuleSettings>(new ModuleSettings);
        QDomElement domElement = domDoc.documentElement();
        GlobalSettings settings { &DataTypes::DataRecV::map, &WidgetFactory::m_widgetMap,
            &WidgetFactory::m_categoryMap };
        XmlParser::traverseNode(domElement, m_settings.get(), settings);
        file.close();
        return true;
    }
    else
    {
        file.close();
        qInfo() << Error::WrongFileError << file.fileName();
        return false;
    }
}

bool Module::loadS2Settings()
{
    const auto name = "s2files";

    QDir directory(StdFunc::GetSystemHomeDir());
    qDebug() << directory;
    auto allFiles = directory.entryList(QDir::Files);
    auto xmlFiles = allFiles.filter(".xml");
    qDebug() << xmlFiles;
    QDomDocument domDoc;
    QFile file;
    for (const auto &xmlFile : xmlFiles)
    {
        if (xmlFile.contains(name, Qt::CaseInsensitive))
            file.setFileName(directory.filePath(xmlFile));
    }
    if (file.fileName().isEmpty())
    {
        directory = QDir(":/module");
        allFiles = directory.entryList(QDir::Files);
        xmlFiles = allFiles.filter(".xml");
        qDebug() << xmlFiles;
        for (const auto &xmlFile : qAsConst(xmlFiles))
        {
            if (!xmlFile.contains(name, Qt::CaseInsensitive))
                continue;
            if (!QFile::copy(directory.filePath(xmlFile), StdFunc::GetSystemHomeDir() + xmlFile))
            {
                qCritical() << Error::DescError;
                qInfo() << directory.filePath(xmlFile) << StdFunc::GetSystemHomeDir() + xmlFile;
                return false;
            }

            return loadS2Settings();
        }
    }
    if (file.open(QIODevice::ReadOnly))
    {
        if (domDoc.setContent(&file))
        {
            QDomElement domElement = domDoc.documentElement();
            GlobalSettings settings { &DataTypes::DataRecV::map, &WidgetFactory::m_widgetMap,
                &WidgetFactory::m_categoryMap };
            XmlParser::traverseNode(domElement, settings);
            file.close();
            return true;
        }
        file.close();
        qInfo() << Error::WrongFileError << file.fileName();
        return false;
    }
    else
    {
        qCritical() << Error::FileOpenError << file.fileName();
        return false;
    }
}

quint64 Module::configVersion() const
{
    QDir directory(StdFunc::GetSystemHomeDir());
    QFile file;
    file.setFileName(directory.filePath(versionFile));
    if (!file.open(QIODevice::ReadOnly))
        return 0;
    bool status = false;
    quint64 counter = file.readAll().toULongLong(&status);
    if (!status)
        return 0;
    return counter;
}

bool Module::isConfigOutdated() const
{
    GitVersion version;

    return configVersion() < version.getGitCounter();
    return false;
}

void Module::eraseSettings() const
{
    QDir directory(StdFunc::GetSystemHomeDir());
    auto files = directory.entryList(QDir::Files).filter(".xml");

    files.push_back(versionFile);
    // qDebug() << files;
    for (const auto &filename : files)
    {
        QFile file(directory.filePath(filename));
        file.setPermissions(QFile::ReadOther | QFile::WriteOther);
        if (!file.remove())
            qDebug() << file;
    }
}

void Module::putConfigVersion() const
{
    QDir directory(StdFunc::GetSystemHomeDir());
    QFile file;
    file.setFileName(directory.filePath(versionFile));
    if (!file.open(QIODevice::WriteOnly))
    {
        qCritical() << Error::FileWriteError << versionFile;
        return;
    }
    QTextStream out(&file);
    GitVersion version;
    out << version.getGitCounter();
    file.close();
}

void Module::create(UniquePointer<Journals> jour)
{
    if (Board::GetInstance().interfaceType() != Board::InterfaceType::RS485)
    {
        //     addDialogToList(new AbstractConfDialog(settings()->defaultConfig), "Конфигурирование", "conf1");
        Q_ASSERT(jour != nullptr);
        addDialogToList(new JournalDialog(std::move(jour)), "Журналы");
    }
}

void Module::createCommon()
{
    const auto &board = Board::GetInstance();
    //    TimeDialog *tdlg = new TimeDialog;
    //    addDialogToList(tdlg, "Время", "time");

    if (board.interfaceType() != Board::InterfaceType::RS485)
        addDialogToList(new FWUploadDialog, "Загрузка ВПО");

    addDialogToList(new InfoDialog, "О приборе", "info");
}
