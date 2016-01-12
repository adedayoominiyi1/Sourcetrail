#include <memory>

#include <QApplication>

#include "utility/logging/ConsoleLogger.h"
#include "utility/logging/FileLogger.h"
#include "utility/logging/LogManager.h"
#include "utility/Version.h"

#include "Application.h"
#include "includes.h" // defines 'void setup(int argc, char *argv[])'
#include "qt/commandline/QtCommandLineParser.h"
#include "qt/window/QtMainWindow.h"
#include "qt/window/QtSplashScreen.h"
#include "qt/network/QtNetworkFactory.h"
#include "qt/utility/utilityQt.h"
#include "qt/view/QtViewFactory.h"
#include "version.h"

#include "component/controller/helper/NetworkProtocolHelper.h" // remove when done

void init()
{
	std::shared_ptr<ConsoleLogger> consoleLogger = std::make_shared<ConsoleLogger>();
	consoleLogger->setLogLevel(Logger::LOG_WARNINGS | Logger::LOG_ERRORS);
	LogManager::getInstance()->addLogger(consoleLogger);

	std::shared_ptr<FileLogger> fileLogger = std::make_shared<FileLogger>();
	fileLogger->setLogLevel(Logger::LOG_ALL);
	FileLogger::setFilePath(logPath);
	LogManager::getInstance()->addLogger(fileLogger);

	utility::loadFontsFromDirectory("data/fonts", ".otf");
}

int main(int argc, char *argv[])
{
	Version version = Version::fromString(GIT_VERSION_NUMBER);

	setup(argc, argv);
	QApplication qtApp(argc, argv);
	QApplication::setApplicationName("Coati");
	QApplication::setApplicationVersion(version.toDisplayString().c_str());

	QtCommandLineParser commandLineParser;
	commandLineParser.setup();
	commandLineParser.process(qtApp);

	qtApp.setAttribute(Qt::AA_UseHighDpiPixmaps);

	QPixmap whitePixmap(500, 500);
	whitePixmap.fill(Qt::white);

	QtSplashScreen* splash = new QtSplashScreen(whitePixmap, Qt::WindowStaysOnTopHint);
	splash->setMessage("Loading UI");
	splash->setVersion(version.toDisplayString().c_str());
	splash->exec(qtApp);

	init();

	QtViewFactory viewFactory;
	QtNetworkFactory networkFactory;

	QtMainWindow::setWindowSettingsPath(windowSettingsPath);
	Application::setAppSettingsPath(appSettingsPath);
	std::shared_ptr<Application> app = Application::create(version, &viewFactory, &networkFactory);

	if (splash)
	{
		delete splash;
	}

	commandLineParser.parseCommandline();

	return qtApp.exec();
}
