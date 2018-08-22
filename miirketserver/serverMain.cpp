
/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

 #include "serverMain.h"

serverMain::serverMain(){
	QDir::setCurrent(qApp->applicationFilePath());
	connect(&server, SIGNAL(newConnection()), this, SLOT(acceptConnectionFromUnknownSshServer()));
	server.listen(QHostAddress::Any, 7777);

	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName(".....");
	db.setDatabaseName(".....");
	db.setUserName(".....");
	db.setPassword(".....");
	db.setConnectOptions("MYSQL_OPT_RECONNECT=1");
	db.open();

	srand (time(NULL));

	QSqlQuery query;
	query.prepare("UPDATE ports SET inuse=:inuse");
	query.bindValue(":inuse", "0");
	query.exec();
}

serverMain::~serverMain(){
	server.close();
}

void serverMain::acceptConnectionFromUnknownSshServer(){
	QTcpSocket *newSshServer = server.nextPendingConnection();
	connect(newSshServer, SIGNAL(readyRead()), this, SLOT(startReadFromUnknownSshServer()));
}

void serverMain::startReadFromUnknownSshServer(){
	QTcpSocket *newSshServer = qobject_cast<QTcpSocket *>(sender());

    QString serverdata = newSshServer->readAll();
    bool ok;
    JsonObject serverinfo = QtJson::parse(serverdata, ok).toMap();

    if(serverinfo["command"].toString()=="register"){

    		relayTop *newRelayTop = new relayTop();
			connect(newRelayTop, SIGNAL(requestNewChannel()), this, SLOT(requestNewChannel()));

            QTcpServer *newSshRelay = new QTcpServer();
            newRelayTop->sshRelay = newSshRelay;
			newRelayTop->sshServer = newSshServer;
            newRelayTop->port = serverinfo["remoteport"].toInt();

			qDebug()<<"NEW SSH RELAY CREATED";
			connect(newRelayTop->sshRelay, SIGNAL(newConnection()), newRelayTop, SLOT(acceptConnectionFromSshClient()));
			connect(newRelayTop->sshServer, SIGNAL(disconnected()), newRelayTop, SLOT(disconnectedFromRegisteredSshServer()));
			newRelayTop->sshRelay->listen(QHostAddress::Any, newRelayTop->port);
	}
	else if(serverinfo["command"].toString()=="newchannel"){
		disconnect(newSshServer, SIGNAL(readyRead()), this, SLOT(startReadFromUnknownSshServer()));
		bindNewChannelToWaitingRelay(newSshServer);
	}
	else if(serverinfo["command"].toString()=="heartbeat"){
		qDebug()<<"RECEIVED A HEART BEAT";
	}
}

void serverMain::requestNewChannel(){
	relayTop *waitingRelayTop = qobject_cast<relayTop *>(sender());
	waitingRelay = waitingRelayTop;

	JsonObject clientinfo;
	clientinfo["command"] = "createnewchannel";
	QByteArray clientdata = QtJson::serialize(clientinfo);
	waitingRelay->sshServer->write(clientdata, clientdata.length());
}

void serverMain::bindNewChannelToWaitingRelay(QTcpSocket *newChannel){
	waitingRelay->incomingChannel(newChannel);
}
