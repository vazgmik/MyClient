#include "myclient.h"
#include <QtWidgets>
#include <QStringList>

MyClient::MyClient(const QString &strHost, int nPort, QWidget *parent):
     QWidget(parent),m_nNextBlockSize(0)
{
    m_pTcpSocket = new QTcpSocket(this);
    m_pTcpSocket->connectToHost(strHost,nPort);
    connect(m_pTcpSocket,SIGNAL(connected()),SLOT(slotConnected()));
    connect(m_pTcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),SLOT(slotError(QAbstractSocket::SocketError)));
    connect(m_pTcpSocket,SIGNAL(readyRead()),SLOT(slotReadyRead()));

    m_ptxtInput = new QLineEdit;
    m_ptxtInfo = new QTextEdit;
    m_ptable = new QTableWidget;
    m_plbl = new QLabel;
    //m_plbl->setScaledContents(true);
    m_ptxtInfo->setReadOnly(true);


    //prepare table to show
    QStringList labels;
    labels <<"Author" <<"Description" <<"Date" <<"Path";
    m_ptable->setColumnCount(4);
    m_ptable->setHorizontalHeaderLabels(labels);
    int vwidth = m_ptable->verticalHeader()->width();
    int hwidth = m_ptable->horizontalHeader()->length();
    m_ptable->setMinimumWidth(vwidth + hwidth);

    //add scrollarea for image
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(m_plbl);
    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumHeight(200);
    scrollArea->setMinimumWidth(vwidth + hwidth);

    QPushButton * pcmd = new QPushButton("&Send");
    connect(pcmd,SIGNAL(clicked(bool)),SLOT(slotSendToServer()));
    connect(m_ptxtInput,SIGNAL(returnPressed()),SLOT(slotSendToServer()));

    QVBoxLayout * pvbxLayout = new QVBoxLayout;
    pvbxLayout->addWidget(new QLabel("<h1>Client</h1>"));
    pvbxLayout->addWidget(m_ptxtInfo);
    pvbxLayout->addWidget(m_ptable);
    pvbxLayout->addWidget(m_ptxtInput);
    pvbxLayout->addWidget(scrollArea);
    pvbxLayout->addWidget(pcmd);
    setLayout(pvbxLayout);
}


void MyClient::slotReadyRead()
{
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_5_5);

    for(;;)
    {
        if(!m_nNextBlockSize){
            if(m_pTcpSocket->bytesAvailable() < (qint64)sizeof(qint64))
                break;
            in >> m_nNextBlockSize;
        }
        if(m_pTcpSocket->bytesAvailable() < (qint64)m_nNextBlockSize)
        {
            break;
        }
        QString typemsg;
        in >> typemsg;
        QTime time;
        in >> time;
        if(typemsg == "data")
        {
            QString msg;
            QVector<QStringList> v_rows;
            //QImage img;
            in>> msg >> v_rows;

            if(!v_rows.isEmpty())
            {
                m_ptable->setRowCount(v_rows.size());
                int index = 0;
                foreach(QStringList str,v_rows)
                {
                    m_ptable->setItem(index, 0, new QTableWidgetItem(str.at(0)));
                    m_ptable->setItem(index, 1, new QTableWidgetItem(str.at(1)));
                    m_ptable->setItem(index, 2, new QTableWidgetItem(str.at(2)));
                    m_ptable->setItem(index, 3, new QTableWidgetItem(str.at(3)));
                    ++index;
                }



            }
        }
        else if(typemsg == "image")
        {
            QImage img;
            in >> img;
            m_plbl->setPixmap(QPixmap::fromImage(img));
           // m_plbl->adjustSize();
        }
        m_ptxtInfo->append(time.toString());
        m_nNextBlockSize = 0;
    }
}

void MyClient::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
            "Error:" +(err == QAbstractSocket::HostNotFoundError?
                       "The host was not found.":
                       err == QAbstractSocket::RemoteHostClosedError?
                       "The remote host is closed.":
                       err == QAbstractSocket::ConnectionRefusedError?
                        "The connection was refused.":
                       QString(m_pTcpSocket->errorString()));
    m_ptxtInfo->append(strError);
}

void MyClient::slotSendToServer()
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_5);
    out << quint64(0) << m_ptxtInput->text();
    out.device()->seek(0);
    qint64 size = arrBlock.size()-sizeof(quint64);
    out<< quint64(size);
    if(m_pTcpSocket->write(arrBlock) < size)
        qWarning("transmit error");

    m_ptxtInput->setText("");
}

void MyClient::slotConnected()
{
    m_ptxtInfo->append("Received the connected signal()");
}

MyClient::~MyClient()
{

}


