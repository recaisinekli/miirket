#include "httpRelayTop.h"

void httpRelayTop::incomingClient(QTcpSocket *newHttpClient, QByteArray httpData){
	httpRelayInner *r1 = new httpRelayInner();
	r1->httpClient = newHttpClient;
	r1->httpData = httpData;
	r1->randomID = rand() %  1000 + 1000;

	innerMap.insertMulti(r1->randomID, r1);

	connect(r1->httpClient, SIGNAL(disconnected()), r1, SLOT(disconnectedFromClient()));
	connect(r1, SIGNAL(removeFromInnerMap(int)), this, SLOT(removeInnerRelayFromInnerMap(int)));
	QTimer::singleShot(10000, r1, SLOT(destroy()));

	JsonObject clientinfo;
        clientinfo["command"] = "createnewchannel";
	clientinfo["randomid"] = r1->randomID;
        QByteArray clientdata = QtJson::serialize(clientinfo);
        httpServer->write(clientdata, clientdata.length());
}

void httpRelayTop::disconnectedFromRegisteredHTTPServer(){
	if(isRegistered){
		qDebug()<< "DISCONNECTED FROM REGISTERED HTTP SERVER WITH SUBDOMAIN "<< this->subdomain;
		innerMap.clear();
		emit removeMeFromMap(this->subdomain);
	}
	this->deleteLater();
}

void httpRelayTop::isSubdomainTaken(bool isTaken){
	if(!isTaken){
		emit mapMe(this->subdomain);
		JsonObject clientinfo;
                clientinfo["command"] = "registered";
                QByteArray clientdata = QtJson::serialize(clientinfo);
                httpServer->write(clientdata, clientdata.length());
		isRegistered = true;
		qDebug()<< "REGISTERED NEW HTTP SERVER WITH SUBDOMAIN "<< this->subdomain;
	}
	else{
		isRegistered = false;
		JsonObject clientinfo;
                clientinfo["command"] = "subdomaintaken";
                QByteArray clientdata = QtJson::serialize(clientinfo);
                httpServer->write(clientdata, clientdata.length());
	}
}

void httpRelayTop::emitCheckMap(){
	emit checkMap(this->subdomain);
}

void httpRelayTop::removeInnerRelayFromInnerMap(int id){
	innerMap[id]->deleteLater();
	innerMap.remove(id);
}
