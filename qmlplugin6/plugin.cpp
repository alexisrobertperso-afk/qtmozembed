/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// RedNet.Web: QtMozEmbed's types under REDNET BROWSER names, registered the
// way qmlplugin5/main.cpp registers them for Sailfish.
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlExtensionPlugin>
#include <QtQml/qqml.h>

#include "qmozcontext.h"
#include "qmozenginesettings.h"
#include "qmozscrolldecorator.h"
#include "qmozsecurity.h"
#include "quickmozview.h"

template <typename T>
static QObject *singletonApiFactory(QQmlEngine *engine, QJSEngine *)
{
    return new T(engine);
}

class RedNetWebPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri) override
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("RedNet.Web"));
        qmlRegisterType<QuickMozView>(uri, 1, 0, "WebViewItem");
        qmlRegisterSingletonType<QMozContext>(uri, 1, 0, "WebEngine",
                                              singletonApiFactory<QMozContext>);
        qmlRegisterSingletonType<QMozEngineSettings>(uri, 1, 0, "WebSettings",
                                                     singletonApiFactory<QMozEngineSettings>);
        qmlRegisterUncreatableType<QMozScrollDecorator>(uri, 1, 0, "WebScrollDecorator", QString());
        qmlRegisterUncreatableType<QMozReturnValue>(uri, 1, 0, "WebReturnValue", QString());
        qmlRegisterType<QMozSecurity>(uri, 1, 0, "WebSecurity");
        qputenv("EMBED_COMPONENTS_PATH", QByteArrayLiteral(DEFAULT_COMPONENTS_PATH));
    }
};

#include "plugin.moc"
