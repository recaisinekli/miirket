/*
 * Author - Recai Sinekli 2018
 * recai@sinekyazilim.com.tr
 */

#include "serverMain.h"

serverMain::serverMain(){

        QDir::setCurrent(qApp->applicationFilePath());
	connect(&server, SIGNAL(newConnection()), this, SLOT(acceptConnectionFromUnknownSshServer()));
        server.listen(QHostAddress::Any, 7777);

        connect(&httpClientServer, SIGNAL(newConnection()), this, SLOT(acceptConnectionFromHTTPClientServer()));
        httpClientServer.listen(QHostAddress::Any, 9999);

        connect(&httpDataServer, SIGNAL(newConnection()), this, SLOT(acceptConnectionFromHTTPDataServer()));
        httpDataServer.listen(QHostAddress::Any, 7779);

        connect(&httpsClientServer, SIGNAL(newConnection()), this, SLOT(acceptConnectionFromHTTPSClientServer()));
        httpsClientServer.listen(QHostAddress::Any, 8888);

        connect(&httpsDataServer, SIGNAL(newConnection()), this, SLOT(acceptConnectionFromHTTPSDataServer()));
        httpsDataServer.listen(QHostAddress::Any, 7778);

	srand (time(NULL));
}

serverMain::~serverMain(){
	server.close();
	httpClientServer.close();
	httpDataServer.close();
	httpsClientServer.close();
        httpsDataServer.close();
}

void serverMain::acceptConnectionFromHTTPDataServer(){
        QTcpSocket *newHTTPDataServer = httpDataServer.nextPendingConnection();
        connect(newHTTPDataServer, SIGNAL(readyRead()), this, SLOT(startReadFromHTTPDataServer()));
}

void serverMain::acceptConnectionFromHTTPSDataServer(){
        QTcpSocket *newHTTPSDataServer = httpsDataServer.nextPendingConnection();
        connect(newHTTPSDataServer, SIGNAL(readyRead()), this, SLOT(startReadFromHTTPSDataServer()));
}

void serverMain::startReadFromHTTPDataServer(){
	QTcpSocket *newHTTPServer = qobject_cast<QTcpSocket *>(sender());

        QByteArray serverdata = newHTTPServer->readAll();
        QString serverdata_s = QString::fromAscii(serverdata.data());
        bool ok;
        JsonObject serverinfo = QtJson::parse(serverdata_s, ok).toMap();

        if(ok){
                if(serverinfo["command"].toString()=="register"){
			httpRelayTop *h1 = new httpRelayTop();
        		h1->httpServer = newHTTPServer;
        		h1->isRegistered = false;
                        h1->subdomain = serverinfo["subdomain"].toString();
			connect(h1, SIGNAL(mapMe(QString)), this, SLOT(mapHTTPServer(QString)));
        		connect(h1, SIGNAL(checkMap(QString)), this, SLOT(checkMapForHTTPServer(QString)));
        		connect(h1, SIGNAL(removeMeFromMap(QString)), this, SLOT(removeHTTPServerFromMap(QString)));
			connect(newHTTPServer, SIGNAL(disconnected()), h1, SLOT(disconnectedFromRegisteredHTTPServer()));
			h1->emitCheckMap();
                }
                else if(serverinfo["command"].toString()=="heartbeat"){
                        qDebug()<<"RECEIVED A HEARTBEAT FROM HTTPSERVER WITH SUBDOMAIN "<< serverinfo["subdomain"].toString();
                }
                else if(serverinfo["command"].toString()=="newchannel"){
			QString subdomain = serverinfo["subdomain"].toString();
			int randomid = serverinfo["randomid"].toInt();

			if(httpServers[subdomain]->innerMap.contains(randomid)){
				httpServers[subdomain]->innerMap[randomid]->httpServer = newHTTPServer;
				disconnect(newHTTPServer, SIGNAL(readyRead()), this, SLOT(startReadFromHTTPDataServer()));
                                connect(newHTTPServer, SIGNAL(readyRead()), httpServers[subdomain]->innerMap[randomid], SLOT(startReadFromHTTPServer()));
                                connect(newHTTPServer, SIGNAL(disconnected()), httpServers[subdomain]->innerMap[randomid], SLOT(disconnectedFromHTTPServer()));
                                httpServers[subdomain]->innerMap[randomid]->writeToServer();
			}
                }
        }
}

void serverMain::startReadFromHTTPSDataServer(){
        QTcpSocket *newHTTPSServer = qobject_cast<QTcpSocket *>(sender());

        QByteArray serverdata = newHTTPSServer->readAll();
        QString serverdata_s = QString::fromAscii(serverdata.data());
        bool ok;
        JsonObject serverinfo = QtJson::parse(serverdata_s, ok).toMap();

        if(ok){
                if(serverinfo["command"].toString()=="register"){
                        httpsRelayTop *h1 = new httpsRelayTop();
                        h1->httpsServer = newHTTPSServer;
                        h1->isRegistered = false;
                        h1->subdomain = serverinfo["subdomain"].toString();
                        connect(h1, SIGNAL(mapMe(QString)), this, SLOT(mapHTTPSServer(QString)));
                        connect(h1, SIGNAL(checkMap(QString)), this, SLOT(checkMapForHTTPSServer(QString)));
                        connect(h1, SIGNAL(removeMeFromMap(QString)), this, SLOT(removeHTTPSServerFromMap(QString)));
                        connect(newHTTPSServer, SIGNAL(disconnected()), h1, SLOT(disconnectedFromRegisteredHTTPSServer()));
                        h1->emitCheckMap();
                }
                else if(serverinfo["command"].toString()=="heartbeat"){
                        qDebug()<<"RECEIVED A HEARTBEAT FROM HTTPS SERVER WITH SUBDOMAIN "<< serverinfo["subdomain"].toString();
                }
                else if(serverinfo["command"].toString()=="newchannel"){
			QString subdomain = serverinfo["subdomain"].toString();
                        int randomid = serverinfo["randomid"].toInt();

                        if(httpsServers[subdomain]->innerMap.contains(randomid)){
                                httpsServers[subdomain]->innerMap[randomid]->httpsServer = newHTTPSServer;
                                disconnect(newHTTPSServer, SIGNAL(readyRead()), this, SLOT(startReadFromHTTPSDataServer()));
                                connect(newHTTPSServer, SIGNAL(readyRead()), httpsServers[subdomain]->innerMap[randomid], SLOT(startReadFromHTTPSServer()));
                                connect(newHTTPSServer, SIGNAL(disconnected()), httpsServers[subdomain]->innerMap[randomid], SLOT(disconnectedFromHTTPSServer()));
                                httpsServers[subdomain]->innerMap[randomid]->writeToServer();
                        }
                }
        }
}


void serverMain::checkMapForHTTPServer(QString subdomain){
	httpRelayTop *unregisteredHTTPServer = qobject_cast<httpRelayTop *>(sender());
	unregisteredHTTPServer->isSubdomainTaken(httpServers.contains(subdomain));
}

void serverMain::checkMapForHTTPSServer(QString subdomain){
        httpsRelayTop *unregisteredHTTPSServer = qobject_cast<httpsRelayTop *>(sender());
        unregisteredHTTPSServer->isSubdomainTaken(httpsServers.contains(subdomain));
}

void serverMain::mapHTTPServer(QString subdomain){
	httpRelayTop *registeredHTTPServer = qobject_cast<httpRelayTop *>(sender());
	httpServers.insertMulti(subdomain, registeredHTTPServer);
}

void serverMain::mapHTTPSServer(QString subdomain){
        httpsRelayTop *registeredHTTPSServer = qobject_cast<httpsRelayTop *>(sender());
        httpsServers.insertMulti(subdomain, registeredHTTPSServer);
}

void serverMain::removeHTTPServerFromMap(QString subdomain){
	if(httpServers.contains(subdomain)){
		httpServers.remove(subdomain);
	}
}

void serverMain::removeHTTPSServerFromMap(QString subdomain){
        if(httpsServers.contains(subdomain)){
                httpsServers.remove(subdomain);
        }
}

void serverMain::acceptConnectionFromHTTPClientServer(){
	QTcpSocket *newHTTPClient = httpClientServer.nextPendingConnection();
	connect(newHTTPClient, SIGNAL(readyRead()), this, SLOT(startReadFromHTTPClient()));
}

void serverMain::acceptConnectionFromHTTPSClientServer(){
        QTcpSocket *newHTTPSClient = httpsClientServer.nextPendingConnection();
        connect(newHTTPSClient, SIGNAL(readyRead()), this, SLOT(startReadFromHTTPSClient()));
}

void serverMain::startReadFromHTTPClient(){
	QTcpSocket *newHttpClient = qobject_cast<QTcpSocket *>(sender());

	QByteArray httpHeaders = newHttpClient->readAll();
	QByteArray httpData = httpHeaders;
	QMap<QByteArray, QByteArray> headers;

	// Discard the first line
	httpHeaders = httpHeaders.mid(httpHeaders.indexOf('\n') + 1).trimmed();

	foreach(QByteArray line, httpHeaders.split('\n')) {
    		int colon = line.indexOf(':');
    		QByteArray headerName = line.left(colon).trimmed();
    		QByteArray headerValue = line.mid(colon + 1).trimmed();

    		headers.insertMulti(headerName, headerValue);
	}

	QString s_data = QString::fromAscii(headers["Host"].data());
        QStringList subdomain = s_data.split('.');

	if(httpServers.contains(subdomain[0])){
		httpServers[subdomain[0]]->incomingClient(newHttpClient, httpData);
	}else{
		QString result = "<center><p style=\"font-size:20px;margin-top:10px;\">Tunnel <b>"+s_data+"</b> not found on miirket!</center>";
		QTextStream os(newHttpClient);
        	os.setAutoDetectUnicode(true);
        	os << "HTTP/1.0 200 Ok\r\n"
              	"Content-Type: text/html; charset=\"utf-8\"\r\n"
                "\r\n" + result;
        	newHttpClient->flush();
		newHttpClient->close();
	}
}

void serverMain::startReadFromHTTPSClient(){
        QTcpSocket *newHttpsClient = qobject_cast<QTcpSocket *>(sender());

        QByteArray httpHeaders = newHttpsClient->readAll();
        QByteArray httpData = httpHeaders;
        QMap<QByteArray, QByteArray> headers;

        // Discard the first line
        httpHeaders = httpHeaders.mid(httpHeaders.indexOf('\n') + 1).trimmed();

        foreach(QByteArray line, httpHeaders.split('\n')) {
                int colon = line.indexOf(':');
                QByteArray headerName = line.left(colon).trimmed();
                QByteArray headerValue = line.mid(colon + 1).trimmed();

                headers.insertMulti(headerName, headerValue);
        }

        QString s_data = QString::fromAscii(headers["Host"].data());
        QStringList subdomain = s_data.split('.');

        if(httpsServers.contains(subdomain[0])){
                httpsServers[subdomain[0]]->incomingClient(newHttpsClient, httpData);
        }else{
                QString result = "<center><p style=\"font-size:20px;margin-top:10px;\">Tunnel <b>"+s_data+"</b> not found on miirket!</center>";
                QTextStream os(newHttpsClient);
                os.setAutoDetectUnicode(true);
                os << "HTTP/1.0 200 Ok\r\n"
                "Content-Type: text/html; charset=\"utf-8\"\r\n"
                "\r\n" + result;
                newHttpsClient->flush();
                newHttpsClient->close();
        }
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
