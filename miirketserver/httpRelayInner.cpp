#include "httpRelayInner.h"

httpRelayInner::~httpRelayInner(){
	if(httpClient!=NULL){
		httpClient->close();
	}
	if(httpServer!=NULL){
                httpServer->close();
        }
}

void httpRelayInner::writeToServer(){
	if(httpServer->waitForConnected()){
		this->httpServer->write(httpData);
		this->httpServer->waitForBytesWritten();
	}
}

void httpRelayInner::startReadFromHTTPServer(){
	QTcpSocket *tcpclient = qobject_cast<QTcpSocket *>(sender());
	if(httpClient->waitForConnected()){
		this->httpClient->write(tcpclient->readAll());
		this->httpClient->waitForBytesWritten();
	}
}

void httpRelayInner::disconnectedFromHTTPServer(){
}

void httpRelayInner::disconnectedFromClient(){
}

void httpRelayInner::destroy(){
	emit removeFromInnerMap(this->randomID);
}
