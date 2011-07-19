#include "TranslationDialog.h"

#include <QDebug>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>

#include "TranslationsList.h"
#include "Translation.h"
#include "Language.h"


TranslationDialog::TranslationDialog(TranslationsList *list, QWidget *parent)
    : QDialog(parent), _list(list), _curLang(0), _curTrans(0)
{
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Cancel,
        Qt::Horizontal,
        this
    );

    _okButton = new QPushButton("OK");
    _okButton->setEnabled(false);
    buttonBox->addButton(_okButton, QDialogButtonBox::AcceptRole);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    _langsList = new QListWidget;
    _transList = new QListWidget;


    _langs = Language::allLanguages();
    for (int i = 0; i < _langs.size(); i++)
        _langsList->addItem(_langs[i]->engname());

    connect(_langsList, SIGNAL(currentRowChanged(int)), this, SLOT(onCurrentLangChanged(int)));
    connect(_transList, SIGNAL(currentRowChanged(int)), this, SLOT(onCurrentTransChanged(int)));


    QVBoxLayout* langsVBox = new QVBoxLayout;
    langsVBox->addWidget(new QLabel("Languages:"));
    langsVBox->addWidget(_langsList);

    QVBoxLayout* transVBox = new QVBoxLayout;
    transVBox->addWidget(new QLabel("Translations:"));
    transVBox->addWidget(_transList);

    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->addLayout(langsVBox);
    hbox->addLayout(transVBox);

    QVBoxLayout* vbox = new QVBoxLayout(this);
    vbox->addLayout(hbox);
    vbox->addWidget(buttonBox);
}


const Language* TranslationDialog::selectedLanguage() const
{
    return _curLang;
}

Translation* TranslationDialog::selectedTranslation() const
{
    return _curTrans;
}


void TranslationDialog::onCurrentLangChanged(int currentRow)
{
    _curLang = _langs[currentRow];

    _transList->clear();
    if (currentRow == -1) return;

    QList<Translation*> trans = _list->translationsForLang(_curLang);
    for (int i = 0; i < trans.size(); i++)
        _transList->addItem(trans[i]->name());
}

void TranslationDialog::onCurrentTransChanged(int currentRow)
{
    _okButton->setEnabled(currentRow != -1);

    if (currentRow == -1)
        _curTrans = 0;
    else
        _curTrans = _list->translationsForLang(_curLang)[currentRow];
}
