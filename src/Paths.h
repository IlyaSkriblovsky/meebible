#ifndef PATHS_H
#define PATHS_H

#include <QString>
#include <QDir>


class Paths
{
public:
    static void init();

    static QString cacheDB();

    static QString nwtDB();
    static QString boDB();
    static QString langsDB();

    static QString qmlMain();

    static QString script_js();
    static QString style_css();

    static QString nwt_xslt();

    static QString welcome_html();


private:
    static QDir _cacheDir;
    static QDir _shareDir;
    static QDir _qmlDir;
};

#endif // PATHS_H
