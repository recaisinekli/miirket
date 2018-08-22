/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#include "connectorTop.h"

void connectorTop::startTransfer(){
	this->timer1->stop();
	this->clientinfo["command"] = "register";

	QByteArray clientdata = QtJson::serialize(this->clientinfo);
	this->client->write(clientdata, clientdata.length());
}

void connectorTop::startReadFromRemote(){
	QTcpSocket *tcpclient = qobject_cast<QTcpSocket *>(sender());

	QString remotedata = tcpclient->readAll();
	bool ok;
 	JsonObject remoteinfo = QtJson::parse(remotedata, ok).toMap();

	if(remoteinfo["command"].toString()=="createnewchannel"){

		qDebug()<< "SERVER REQUESTED NEW CHANNEL";

		connectorInner *newConnectorInner = new connectorInner();
		connectorInnerList.push_back(newConnectorInner);
		connect(newConnectorInner, SIGNAL(deleteMe()), this, SLOT(deleteConnectorInner()));
		newConnectorInner->clientinfo["server_address"] = this->clientinfo["server_address"];
		newConnectorInner->clientinfo["server_port"] = this->clientinfo["server_port"];
		newConnectorInner->clientinfo["authtoken"] = this->clientinfo["authtoken"];
		newConnectorInner->clientinfo["localport"] = this->clientinfo["localport"];
		newConnectorInner->clientinfo["remoteport"] = this->clientinfo["remoteport"];
		newConnectorInner->tryToConnect();
	}

}

void connectorTop::tryToConnectToServer(){
	this->client->connectToHostEncrypted(this->clientinfo["server_address"].toString(), this->clientinfo["server_port"].toInt());
}

void connectorTop::disconnectedFromServer(){

	for(int i=0;i<connectorInnerList.size();i++){
		connectorInnerList.at(i)->deleteLater();
	}
	connectorInnerList.empty();
	qDebug()<<"DISCONNECTED FROM SERVER";
	this->timer1->start();
	this->heartbeatTimer->deleteLater();
}

void connectorTop::deleteConnectorInner(){
	connectorInner *innerConnector = qobject_cast<connectorInner *>(sender());
	int id=connectorInnerList.indexOf(innerConnector);
	if(id>-1){
		connectorInnerList.at(id)->deleteLater();
		connectorInnerList.removeAt(id);
	}
}


void connectorTop::sslErrors(const QList<QSslError> &errors)
{
	qDebug()<<"ERROR";
    	foreach (const QSslError &error, errors)
    		qDebug() << error.errorString();
}

void connectorTop::socketEncrypted(){
	qDebug()<<"SOCKET ENCRYPTED";
	this->heartbeatTimer = new QTimer();
	connect(this->heartbeatTimer, SIGNAL(timeout()), this, SLOT(heartbeat()));;
	this->heartbeatTimer->start(60000*3);
}

void connectorTop::heartbeat(){
	JsonObject clientinfo;
	clientinfo["command"] = "heartbeat";
	QByteArray clientdata = QtJson::serialize(clientinfo);
	this->client->write(clientdata, clientdata.length());
}
