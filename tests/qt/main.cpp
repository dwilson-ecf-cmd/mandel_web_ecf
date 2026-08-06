extern "C" {
#include "fractal/installed_modules.h"
}

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>

int main(int argc, char *argv[])
{
    QGuiApplication application(argc, argv);
    application.setApplicationName(QStringLiteral("Mandel Qt Toolchain Smoke"));
    application.setApplicationVersion(QStringLiteral("0.1.0"));

    fractal_module_registry registry{};
    const fractal_result result = fractal_installed_modules_registry(&registry);
    if (result != FRACTAL_OK || registry.count != 22
        || registry.identity != UINT64_C(0x616b9a3b92e787e5)) {
        qCritical("Portable core registry validation failed");
        return 2;
    }

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(
        QStringLiteral("registryIdentity"),
        QString::asprintf("%016llx", static_cast<unsigned long long>(registry.identity)));
    engine.rootContext()->setContextProperty(
        QStringLiteral("registryModuleCount"), static_cast<int>(registry.count));
    engine.rootContext()->setContextProperty(
        QStringLiteral("qtVersion"), QString::fromLatin1(qVersion()));
    engine.loadFromModule(QStringLiteral("Mandel.ToolchainSmoke"), QStringLiteral("Main"));
    if (engine.rootObjects().isEmpty())
        return 3;

    if (application.arguments().contains(QStringLiteral("--smoke-auto-exit")))
        QTimer::singleShot(250, &application, &QCoreApplication::quit);

    return application.exec();
}
