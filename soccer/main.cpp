// kate: indent-mode cstyle; indent-width 4; tab-width 4; space-indent false;
// vim:ai ts=4 et

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <QApplication>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QString>
#include <QMessageBox>

#include <boost/foreach.hpp>

#include "PlayConfigTab.hpp"
#include "MainWindow.hpp"

using namespace std;

void usage(const char* prog)
{
	fprintf(stderr, "usage: %s [options...]\n", prog);
	fprintf(stderr, "\t-r <n>:     run only on specified radio channel\n");
	fprintf(stderr, "\t-y:         run as the yellow team\n");
	fprintf(stderr, "\t-b:         run as the blue team\n");
	fprintf(stderr, "\t-c <file>:  specify the configuration file\n");
	fprintf(stderr, "\t-p <file>:  load playbook\n");
	fprintf(stderr, "\t-s <seed>:  set random seed (hexadecimal)\n");
	fprintf(stderr, "\t-pp <play>: enable named play\n");
	fprintf(stderr, "\t-ng:        no goalie\n");
	fprintf(stderr, "\t-sim:       use simulator\n");
	fprintf(stderr, "\t-nolog:     don't write log files\n");
	exit(1);
}

int main (int argc, char* argv[])
{
	// Seed the large random number generator
	long int seed = 0;
	int fd = open("/dev/random", O_RDONLY);
	if (fd >= 0)
	{
		if (read(fd, &seed, sizeof(seed)) != sizeof(seed))
		{
			fprintf(stderr, "Can't read /dev/random, using zero seed: %m\n");
		}
		close(fd);
	} else {
		fprintf(stderr, "Can't open /dev/random, using zero seed: %m\n");
	}
	
	QApplication app(argc, argv);

	bool blueTeam = false;
	QString cfgFile;
	QString playbook;
	vector<const char *> playDirs;
	vector<QString> extraPlays;
	bool goalie = true;
	bool sim = false;
	bool log = true;
	int radio = -1;
	
	for (int i=1 ; i<argc; ++i)
	{
		const char* var = argv[i];

		if (strcmp(var, "--help") == 0)
		{
			usage(argv[0]);
		} else if (strcmp(var, "-y") == 0)
		{
			blueTeam = false;
		}
		else if (strcmp(var, "-b") == 0)
		{
			blueTeam = true;
		}
		else if (strcmp(var, "-ng") == 0)
		{
			goalie = false;
		}
		else if (strcmp(var, "-sim") == 0)
		{
			sim = true;
		}
		else if (strcmp(var, "-nolog") == 0)
		{
			log = false;
		}
		else if(strcmp(var, "-r") == 0)
		{
			if (i+1 >= argc)
			{
				printf("no radio channel specified after -r");
				usage(argv[0]);
			}
			
			i++;
			radio = atoi(argv[i]);
		}
		else if(strcmp(var, "-c") == 0)
		{
			if (i+1 >= argc)
			{
				printf("no config file specified after -c");
				usage(argv[0]);
			}
			
			i++;
			cfgFile = argv[i];
		}
		else if(strcmp(var, "-s") == 0)
		{
			if (i+1 >= argc)
			{
				printf("no seed specified after -s");
				usage(argv[0]);
			}
			
			i++;
			seed = strtol(argv[i], 0, 16);
		}
		else if(strcmp(var, "-pp") == 0)
		{
			if (i+1 >= argc)
			{
				printf("no play specified after -pp");
				usage(argv[0]);
			}
			
			i++;
			extraPlays.push_back(argv[i]);
		}
		else if(strcmp(var, "-p") == 0)
		{
			if (i+1 >= argc)
			{
				printf("no playbook file specified after -p");
				usage(argv[0]);
			}
			
			i++;
			playbook = argv[i];
		}
		else
		{
			printf("Not a valid flag: %s\n", argv[i]);
			usage(argv[0]);
		}
	}
	
	printf("Running on %s\n", sim ? "simulation" : "real hardware");
	
	printf("seed %016lx\n", seed);
	srand48(seed);
	
	// Default config file name
	if (cfgFile.isNull())
	{
		cfgFile = sim ? "soccer-sim.cfg" : "soccer-real.cfg";
	}
	
	Configuration config;
	
	Processor *processor = new Processor(&config, sim, radio);
	processor->blueTeam(blueTeam);
	
	// Load config file
	QString error;
	if (!config.load(cfgFile, error))
	{
		QMessageBox::critical(0, "Soccer",
			QString("Can't read initial configuration %1:\n%2").arg(cfgFile, error));
	}
	
	MainWindow *win = new MainWindow;
	win->configuration(&config);
	win->processor(processor);
	
	if (!playbook.isNull())
	{
		win->playConfigTab()->load(playbook);
	} else {
		// Try to load a default playbook
		win->playConfigTab()->load("default.pbk");
	}
	
	BOOST_FOREACH(const QString &str, extraPlays)
	{
		win->playConfigTab()->enable(str);
	}
	
	win->playConfigTab()->useGoalie(goalie);
	
	if (!QDir("logs").exists())
	{
		fprintf(stderr, "No logs directory - not writing log file\n");
	} else if (!log)
	{
		fprintf(stderr, "Not writing log file\n");
	} else {
		QString logFile = QString("logs/") + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss.log");
		if (!processor->logger.open(logFile))
		{
			printf("Failed to open %s: %m\n", (const char *)logFile.toAscii());
		}
	}
	win->logFileChanged();
	
	processor->start();
	
	win->showMaximized();

	int ret = app.exec();
	processor->stop();
	
	delete win;
	delete processor;
	
	return ret;
}
