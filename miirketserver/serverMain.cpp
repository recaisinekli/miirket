
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

		QSqlQuery query;
        query.prepare("SELECT * FROM users WHERE authtoken=:authtoken");
        query.bindValue(":authtoken", serverinfo["authtoken"].toString());
        query.exec();
        query.first();
        if(query.size()>0){
                	//qDebug()<< query.value(1).toString();

			if(query.value(5).toInt()==1){
				bool createRelay=false;
				if(query.value(6).toInt()==0){
					QSqlQuery query2;
					int randomPort;

					query2.prepare("SELECT * FROM ports where email=:email");
					query2.bindValue(":email", query.value(1).toString());
					query2.exec();
					query2.first();
					if(query2.size()>0){
						//Only 1 port allowed for free accounts
						JsonObject clientinfo;
                        clientinfo["command"] = "only1portallowed";
                        QByteArray clientdata = QtJson::serialize(clientinfo);
                        newSshServer->write(clientdata, clientdata.length());
					}
					else{
						do{
							randomPort = rand() %  10000 + 30000;
							if(randomPort==33816) continue;
							query2.prepare("SELECT * FROM ports WHERE port=:port");
                			query2.bindValue(":port", randomPort);
                			query2.exec();
                			query2.first();
                			if(query2.size()==0) break;
						}while(true);

						serverinfo["remoteport"] = randomPort;
						JsonObject clientinfo;
                        clientinfo["accounttype"] = "0";
						clientinfo["port"] = serverinfo["remoteport"];
                        QByteArray clientdata = QtJson::serialize(clientinfo);
                        newSshServer->write(clientdata, clientdata.length());

						query2.prepare("INSERT INTO ports(email,port) VALUES(:email,:port)");
                        query2.bindValue(":email", query.value(1).toString());
						query2.bindValue(":port", serverinfo["remoteport"].toInt());
                        query2.exec();
						createRelay=true;
					}
				}
				else if(query.value(6).toInt()==1){
					QSqlQuery query2;
					query2.prepare("SELECT * FROM ports where email=:email AND port=:port");
                    query2.bindValue(":email", query.value(1).toString());
					query2.bindValue(":port", serverinfo["remoteport"].toInt());
                    query2.exec();
                    query2.first();
                    if(query2.size()>0){
						if(query2.value(2).toInt()==1){
							JsonObject clientinfo;
                            clientinfo["command"] = "portinuse";
                            clientinfo["port"] = serverinfo["remoteport"];
                            QByteArray clientdata = QtJson::serialize(clientinfo);
                            newSshServer->write(clientdata, clientdata.length());
						}
						else{
							JsonObject clientinfo;
                            clientinfo["accounttype"] = "1";
                            clientinfo["port"] = serverinfo["remoteport"];
                            QByteArray clientdata = QtJson::serialize(clientinfo);
                            newSshServer->write(clientdata, clientdata.length());
                            createRelay=true;

							QSqlQuery query3;
                            query3.prepare("UPDATE ports SET inuse=:inuse where email=:email AND port=:port");
							query3.bindValue(":inuse", 1);
                            query3.bindValue(":email", query.value(1).toString());
                            query3.bindValue(":port", serverinfo["remoteport"].toInt());
                            query3.exec();
						}
					}
					else{
						JsonObject clientinfo;
                        clientinfo["command"] = "wrongremoteport";
						clientinfo["port"] = serverinfo["remoteport"];
                        QByteArray clientdata = QtJson::serialize(clientinfo);
                        newSshServer->write(clientdata, clientdata.length());
					}
                }

				if(createRelay){
                	relayTop *newRelayTop = new relayTop();
					connect(newRelayTop, SIGNAL(requestNewChannel()), this, SLOT(requestNewChannel()));

                	QTcpServer *newSshRelay = new QTcpServer();
                	newRelayTop->sshRelay = newSshRelay;
					newRelayTop->sshServer = newSshServer;
                	newRelayTop->port = serverinfo["remoteport"].toInt();
					newRelayTop->userEmail = query.value(1).toString();
					newRelayTop->accountType = query.value(6).toInt();

					qDebug()<<"NEW SSH RELAY CREATED";
					connect(newRelayTop->sshRelay, SIGNAL(newConnection()), newRelayTop, SLOT(acceptConnectionFromSshClient()));
					connect(newRelayTop->sshServer, SIGNAL(disconnected()), newRelayTop, SLOT(disconnectedFromRegisteredSshServer()));
					connect(newRelayTop, SIGNAL(disconnectedFromServer(QString, int, int)), this, SLOT(disconnectedFromRegisteredSshServer(QString, int, int)));
					newRelayTop->sshRelay->listen(QHostAddress::Any, newRelayTop->port);
				}
			}
			else{
				qDebug()<< "ACCOUNT NOT VERIFIED";
                JsonObject clientinfo;
                clientinfo["command"] = "accountnotverified";
                QByteArray clientdata = QtJson::serialize(clientinfo);
                newSshServer->write(clientdata, clientdata.length());
			}
		}
		else{
			qDebug()<< "AUTHTOKEN NOT FOUND";
			JsonObject clientinfo;
        	clientinfo["command"] = "authtokennotfound";
        	QByteArray clientdata = QtJson::serialize(clientinfo);
			newSshServer->write(clientdata, clientdata.length());
		}
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

void serverMain::disconnectedFromRegisteredSshServer(QString userEmail, int accountType, int port){
	if(accountType==0){
		QSqlQuery query;
		query.prepare("DELETE FROM ports WHERE email=:email");
        query.bindValue(":email", userEmail);
        query.exec();
	}
	else if(accountType==1){
		QSqlQuery query;
        query.prepare("UPDATE ports SET inuse=:inuse WHERE email=:email AND port=:port");
        query.bindValue(":email", userEmail);
		query.bindValue(":inuse", 0);
		query.bindValue(":port", port);
        query.exec();
	}
}
