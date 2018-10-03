/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#include "httpsRelayTop.h"

void httpsRelayTop::incomingClient(QTcpSocket *newHttpsClient, QByteArray httpsData){
	httpsRelayInner *r1 = new httpsRelayInner();
	r1->httpsClient = newHttpsClient;
	r1->httpsData = httpsData;
	r1->randomID = rand() %  1000 + 1000;

	innerMap.insertMulti(r1->randomID, r1);

	connect(r1->httpsClient, SIGNAL(disconnected()), r1, SLOT(disconnectedFromClient()));
	connect(r1, SIGNAL(removeFromInnerMap(int)), this, SLOT(removeInnerRelayFromInnerMap(int)));
	QTimer::singleShot(10000, r1, SLOT(destroy()));

	JsonObject clientinfo;
        clientinfo["command"] = "createnewchannel";
	clientinfo["randomid"] = r1->randomID;
        QByteArray clientdata = QtJson::serialize(clientinfo);
        httpsServer->write(clientdata, clientdata.length());
}

void httpsRelayTop::disconnectedFromRegisteredHTTPSServer(){
	if(isRegistered){
		qDebug()<< "DISCONNECTED FROM REGISTERED HTTPS SERVER WITH SUBDOMAIN "<< this->subdomain;
		innerMap.clear();
		emit removeMeFromMap(this->subdomain);
	}
	this->deleteLater();
}

void httpsRelayTop::isSubdomainTaken(bool isTaken){
	if(!isTaken){
		emit mapMe(this->subdomain);
		JsonObject clientinfo;
                clientinfo["command"] = "registered";
		clientinfo["type"] = "https";
                QByteArray clientdata = QtJson::serialize(clientinfo);
                httpsServer->write(clientdata, clientdata.length());
		isRegistered = true;
		qDebug()<< "REGISTERED NEW HTTPS SERVER WITH SUBDOMAIN "<< this->subdomain;
	}
	else{
		isRegistered = false;
		JsonObject clientinfo;
                clientinfo["command"] = "subdomaintaken";
                QByteArray clientdata = QtJson::serialize(clientinfo);
                httpsServer->write(clientdata, clientdata.length());
	}
}

void httpsRelayTop::emitCheckMap(){
	emit checkMap(this->subdomain);
}

void httpsRelayTop::removeInnerRelayFromInnerMap(int id){
	innerMap[id]->deleteLater();
	innerMap.remove(id);
}
