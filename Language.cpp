#include "Language.h"

#include <QListIterator>


Language::Language(const QString& code, const QString& engname, const QString& selfname)
    : _code(code), _engname(engname), _selfname(selfname)
{
}
