#ifndef TRANSLATIONDIALOG_H
#define TRANSLATIONDIALOG_H

#include <QDialog>


class TranslationsList;


class TranslationDialog: public QDialog
{
    Q_OBJECT

public:
    TranslationDialog(TranslationsList *list, QWidget *parent = 0);
};

#endif // TRANSLATIONDIALOG_H
