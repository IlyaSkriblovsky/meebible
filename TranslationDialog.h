#ifndef TRANSLATIONDIALOG_H
#define TRANSLATIONDIALOG_H

#include <QDialog>
#include <QList>

class QPushButton;
class QListWidget;

class TranslationsList;
class Language;
class Translation;


class TranslationDialog: public QDialog
{
    Q_OBJECT

public:
    TranslationDialog(
        TranslationsList *list,
        const Translation* selectedTranslation,
        QWidget *parent = 0
    );

    const Language* selectedLanguage() const;
    Translation* selectedTranslation() const;

private:
    TranslationsList* _list;
    QPushButton* _okButton;
    QList<const Language*> _langs;

    const Language* _curLang;
    Translation* _curTrans;

    QListWidget* _langsList;
    QListWidget* _transList;


    void displayTrans(const Translation* selected);

private slots:
    void onCurrentLangChanged(int currentRow);
    void onCurrentTransChanged(int currentRow);
};

#endif // TRANSLATIONDIALOG_H
