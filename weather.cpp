#include "weather.h"
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>

Weather::Weather()
{

}


// 自动IP定位函数
void Weather::autoLocateCity()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString log = "IP:" + currentDateTime.toString("yyyy/MM/dd HH:mm:ss") + "\n";

    // 这里需要实现IP定位逻辑
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, [&log, manager, this](QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            log += "NoError";
            QString result = reply->readAll();
            // 解析结果，获取城市和国家信息
            // 这里假设返回的是JSON格式，包含city和country字段
            QJsonDocument doc = QJsonDocument::fromJson(result.toUtf8());
            QJsonObject obj = doc.object();
            QString city = obj["city"].toString();
            QString country = obj["countryCode"].toString();

            m_cityData.city = city;
            m_cityData.country = country;
            //m_settings.setValue("city", city);
            //m_settings.setValue("country", country);
            //log += "IP::city: " + city + " country: " + country;
            //forcastApplet->updateWeather();
            emit autoLocateCityFinished(m_cityData);
        }
        else{
            QString result = reply->readAll();
            log += "IP::error: " + result;

        }
        reply->deleteLater();
        manager->deleteLater();
    });
    QEventLoop loop;
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl("http://ip-api.com/json/")));
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    //loop.exec();  // 等待请求完成
}
