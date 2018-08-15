/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#include <QDebug>
#include <QtCore>
#include <QtNetwork>
#include <iostream>
#include "serverMain.h"

using namespace std;

int main(int argc, char *argv[]){

	QCoreApplication a(argc, argv);

	serverMain n1;

	return a.exec();
}
