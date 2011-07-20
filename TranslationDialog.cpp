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


TranslationDialog::TranslationDialog(
    TranslationsList *list,
    const Translation* selectedTranslation,
    QWidget *parent
)
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
    const Language* selectedLanguage = selectedTranslation->language();
    for (int i = 0; i < _langs.size(); i++)
    {
        _langsList->addItem(_langs[i]->engname());

        if (_langs[i] == selectedLanguage)
        {
            _langsList->setCurrentRow(i);
            _curLang = selectedLanguage;
            displayTrans(selectedTranslation);
        }
    }

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
    if (currentRow == -1)
        _curLang = 0;
    else
        _curLang = _langs[currentRow];

    displayTrans(0);
}

void TranslationDialog::onCurrentTransChanged(int currentRow)
{
    _okButton->setEnabled(currentRow != -1);

    if (currentRow == -1)
        _curTrans = 0;
    else
        _curTrans = _list->translationsForLang(_curLang)[currentRow];
}

void TranslationDialog::displayTrans(const Translation* selected)
{
    _transList->clear();

    if (_curLang == 0) return;

    QList<Translation*> trans = _list->translationsForLang(_curLang);
    for (int i = 0; i < trans.size(); i++)
    {
        _transList->addItem(trans[i]->name());
        if (trans[i] == selected)
        {
            _transList->setCurrentRow(i);
            onCurrentTransChanged(i);
        }
    }
}
