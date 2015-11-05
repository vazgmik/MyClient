#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QWidget>
#include <QTcpSocket>

class QTextEdit;
class QTableWidget;
class QLineEdit;
class QLabel;
class QCheckBox;
class MyClient : public QWidget
{
    Q_OBJECT
private:
    QTcpSocket* m_pTcpSocket;
    QTextEdit* m_ptxtInfo;
    QLineEdit* m_ptxtInput;
    QLabel * m_plbl;
    quint64 m_nNextBlockSize;
    QTableWidget * m_ptable;
public:
    MyClient(const QString & strHost,int nPort,QWidget *parent = 0);
    ~MyClient();

private slots:
    void slot_Checkbox();
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void slotSendToServer();
    void slotConnected();
};

#endif // MYCLIENT_H
