#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

//    QWebSettings* settings =
    connect(ui->webView, SIGNAL(loadProgress(int)),this, SLOT(on_progess(int)));
    connect(ui->webView, SIGNAL(loadFinished(bool)),this, SLOT(on_load_finished(bool)));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_load_clicked()
{
    QUrl url = ui->url->text();
    ui->webView->load(url);
}

void Widget::on_progess(int progess)
{
    QString title = QString::number(progess) + "%";

    this->setWindowTitle(title);
}

void Widget::on_load_finished(bool ok)
{
    QStringList urls;
    QWebElementCollection elements = ui->webView->page()->mainFrame()->findAllElements("#imgid > div > ul > li > div > a > img");
    for(int i = 0; i < elements.count(); i++)
    {
        const QWebElement& ele = elements.at(i);
        QString url = ele.attribute("src");
        urls << url;
    }

    DownLoadImages(urls);

    while(mgrs.size() > 10)
    {

    }
    elements = ui->webView->page()->mainFrame()->findAllElements("#topRS > a:nth-child(3)");
    if(elements.count() < 0)
    {
        qDebug() << "not next page";
    }
    QString url = "https://image.baidu.com" + elements[0].attribute("href");
    ui->webView->load(url);

}


QImage Widget::DownLoadImage(const QString &url)
{
    QNetworkAccessManager mgr;
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    QNetworkReply* reply = mgr.get(request);

    // 异步改同步
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    QByteArray data = reply->readAll();
    QImage image = QImage::fromData(data);
    return image;
}

void Widget::DownLoadImages(const QStringList &urls)
{
    for(int i = 0; i < urls.size(); i++)
    {
        QNetworkAccessManager* mgr = new QNetworkAccessManager();
        mgrs << mgr;
        QNetworkRequest request;
        request.setUrl(QUrl(urls[i]));
        QNetworkReply* reply = mgr->get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(DownLoadFinished()));
        qDebug() << " 开始下载 :   " << urls[i];
    }

}


void Widget::DownLoadFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(this->sender());
    if(reply == 0)
    {
        // 转换失败
        return;
    }
    QNetworkAccessManager* src = reply->manager();

    QByteArray data = reply->readAll();
    QImage image = QImage::fromData(data);
    QFileInfo info(reply->url().toString());
    image.save("F:/Project/Qt/webkit_debug/" + info.fileName());
    mgrs.removeOne(src);
    delete src;
}
