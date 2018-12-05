#include <QtWidgets/QApplication>
#include "Player.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	Player player;
	player.play();

	return a.exec();
}
