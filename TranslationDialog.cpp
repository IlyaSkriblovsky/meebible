#include "TranslationDialog.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>

#include "TranslationsList.h"


TranslationDialog::TranslationDialog(TranslationsList *list, QWidget *parent)
    : QDialog(parent)
{
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal,
        this
    );

    QVBoxLayout* vbox = new QVBoxLayout(this);
    vbox->addWidget(new QWidget);
    vbox->addWidget(buttonBox);

    setLayout(vbox);
}
