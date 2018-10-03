/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#include "httpsRelayInner.h"

httpsRelayInner::~httpsRelayInner(){
	if(httpsClient!=NULL){
		httpsClient->close();
	}
	if(httpsServer!=NULL){
                httpsServer->close();
        }
}

void httpsRelayInner::writeToServer(){
	if(httpsServer->waitForConnected()){
		this->httpsServer->write(httpsData);
		this->httpsServer->waitForBytesWritten();
	}
}

void httpsRelayInner::startReadFromHTTPSServer(){
	QTcpSocket *tcpclient = qobject_cast<QTcpSocket *>(sender());
	if(httpsClient->waitForConnected()){
		this->httpsClient->write(tcpclient->readAll());
		this->httpsClient->waitForBytesWritten();
	}
}

void httpsRelayInner::disconnectedFromHTTPSServer(){
}

void httpsRelayInner::disconnectedFromClient(){
}

void httpsRelayInner::destroy(){
	emit removeFromInnerMap(this->randomID);
}
