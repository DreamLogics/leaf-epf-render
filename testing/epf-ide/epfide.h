#ifndef EPFIDE_H
#define EPFIDE_H

#include <QMainWindow>
#include <QMap>

namespace Ui {
class EpfIDE;
}

class CProjectViewItem;
class CEPFHL;
class QTreeWidgetItem;
class QProcess;

class EpfIDE : public QMainWindow
{
    Q_OBJECT

public:
    explicit EpfIDE(QWidget *parent = 0);
    ~EpfIDE();

    void open(QString path);
    void save();
    void saveAll();

    void scanProject();

    void editFile(QString path);

    void build();
    void run();

public slots:

    void createOutline();
    void print(QString str, QColor color = QColor(0,0,0));

    void stdOut();
    void stdErr();
    void stdClose();

private slots:
    void on_action_New_triggered();

    void on_action_Open_triggered();

    void on_action_Save_triggered();

    void on_action_Build_triggered();

    void on_action_Run_triggered();

    void on_projectview_itemChanged(QTreeWidgetItem *item, int column);

    void on_projectview_customContextMenuRequested(const QPoint &pos);

    void on_projectview_itemSelectionChanged();

    void on_editsearch_textChanged(const QString &arg1);

    void on_outlinesearch_textChanged(const QString &arg1);

    void on_editsearch_returnPressed();

    void on_editview_textChanged();

private:



private:
    Ui::EpfIDE *ui;
    QString m_sPath;
    QString m_sOpenFile;
    CProjectViewItem* m_pProjectItem;
    CProjectViewItem* m_pContentItem;
    CProjectViewItem* m_pScriptsItem;
    CProjectViewItem* m_pStylesheetsItem;
    CProjectViewItem* m_pResourcesItem;
    CProjectViewItem* m_pTextItem;
    CEPFHL* m_pEPFSyntax;
    QMap<QString,QString> m_FileBuffer;
    QProcess* m_pExternal;
    bool m_bIsRunning;
    bool m_bNoChange;


    friend class CEditor;
};

#endif // EPFIDE_H
