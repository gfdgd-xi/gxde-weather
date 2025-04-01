#include "weather.h"
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QPixmap>

Weather::Weather()
{

}

QJsonObject Weather::loadCityTranslations()
{
    // Open the file from Qt resource
    QFile file(":/city.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file:" << file.errorString();
        return QJsonObject();
    }

    // Read the file content
    QByteArray jsonData = file.readAll();
    file.close();

    // Parse JSON
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return QJsonObject();
    }

    // Return the JSON object
    return jsonDoc.object();
}


// 更新天气信息函数
void Weather::updateWeather()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();  // 获取当前时间
    QString stemp = "", stip = "", surl="";
    QString log = currentDateTime.toString("yyyy/MM/dd HH:mm:ss") + "\n";  // 记录日志信息
    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *reply;

    // 从设置中读取城市和国家信息
    /*QString city = m_settings.value("city","").toString();
    QString country = m_settings.value("country","").toString();*/
    QString city = m_cityData.city;
    QString country = m_cityData.country;

    if(city != "" && country != ""){
        // 设置默认图标路径
        QString icon_path; //= ":icon/Default/na.png";
        QString iconTheme = "Flat"; //m_settings.value("IconTheme","").toString();
        if(iconTheme != ""){
            if(!iconTheme.startsWith("/")){
                icon_path = ":icon/" + iconTheme + "/na.png";
            }else{
                QString icon_path1 = iconTheme + "/na.png";
                QFile file(icon_path1);
                if(file.exists()){
                    icon_path = icon_path1;
                }
            }
        }

        // 发送信号显示当前天气信息
        //emit weatherNow("Weather", "Temp", currentDateTime.toString("yyyy/MM/dd HH:mm:ss") + "\nGetting weather of " + city + "," + country, QPixmap(icon_path));

        // 构建OpenWeatherMap API的URL
        QString appid = "8f3c852b69f0417fac76cd52c894ba63";
        surl = "https://api.openweathermap.org/data/2.5/forecast?q=" + city + "," + country + "&appid=" + appid + "&lang=zh_cn";

        // 发送网络请求
        reply = manager.get(QNetworkRequest(QUrl(surl)));
        QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();  // 等待请求完成

        // 读取回复数据
        QByteArray BA = reply->readAll();
        log += surl + "\n";
        log += BA + "\n";

        QJsonParseError JPE;
        QJsonDocument JD = QJsonDocument::fromJson(BA, &JPE);  // 解析JSON数据
        QJsonObject cityTranslations = loadCityTranslations();

        if (JPE.error == QJsonParseError::NoError) {  // 确保解析无错误
            QString cod = JD.object().value("cod").toString();
            if(cod == "200"){
                m_weatherDataList.clear();

                // 提取城市和天气数据
                QJsonObject JO_city = JD.object().value("city").toObject();
                QJsonObject coord = JO_city.value("coord").toObject();
                double lat = coord.value("lat").toDouble();
                double lon = coord.value("lon").toDouble();
                //m_settings.setValue("lat", lat);
                //m_settings.setValue("lon", lon);
                //weatherData.lat = lat;
                //weatherData.lon = lon;

                // 提取日出和日落时间
                QDateTime time_sunrise = QDateTime::fromMSecsSinceEpoch(JO_city.value("sunrise").toInt()*1000L, Qt::LocalTime);
                QDateTime time_sunset = QDateTime::fromMSecsSinceEpoch(JO_city.value("sunset").toInt()*1000L, Qt::LocalTime);

                QJsonArray list = JD.object().value("list").toArray();  // 获取天气列表数据
                //int r = 0;

                // 遍历天气预报数据
                for (int i=0; i<list.size(); i++) {
                    WeatherData weatherData;
                    QDateTime date = QDateTime::fromMSecsSinceEpoch(list[i].toObject().value("dt").toInt()*1000L, Qt::UTC);
                    QString sdate = date.toString("MM-dd ddd");  // 格式化日期
                    QString today = date.toString("MM-dd");  // 格式化日期
                    QString dt_txt = list[i].toObject().value("dt_txt").toString();
                    double temp = list[i].toObject().value("main").toObject().value("temp").toDouble() - 273.15;  // 转换温度为摄氏度
                    stemp = QString::number(qRound(temp)) + "°C";


                    // 检查是否需要将温度单位转换为华氏度
                    /*if(m_settings.value("TemperatureUnit","°C").toString() == "°F"){
                        stemp = QString::number(qRound(temp*1.8 + 32)) + "°F";
                    }*/

                    // 提取其他天气信息
                    //QString humidity = "湿度: " + QString::number() + "%";
                    QString weather = list[i].toObject().value("weather").toArray().at(0).toObject().value("description").toString();
                    QString icon_name = list[i].toObject().value("weather").toArray().at(0).toObject().value("icon").toString() + ".png";
                    QString icon_path = ":icon/Default/" + icon_name;

                    // 根据图标主题设置图标路径
                    QString iconTheme = "Flat"; //m_settings.value("IconTheme","").toString();
                    if(iconTheme != ""){
                        if(!iconTheme.startsWith("/")){
                            icon_path = ":icon/" + iconTheme + "/" + icon_name;
                        }else{
                            QString icon_path1 = iconTheme + "/" + icon_name;
                            QFile file(icon_path1);
                            if(file.exists()){
                                icon_path = icon_path1;
                            }
                        }
                    }

                    // 提取风速和风向
                    //QString wind = "风: " + QString::number() + "m/s, " + QString::number() + "°";

                    weatherData.date = date;
                    weatherData.weather = weather;
                    weatherData.humidity = list[i].toObject().value("main").toObject().value("humidity").toInt();
                    weatherData.icon = QPixmap(icon_path);
                    weatherData.windSpeed = list[i].toObject().value("wind").toObject().value("speed").toDouble();
                    weatherData.windDeg = qRound(list[i].toObject().value("wind").toObject().value("deg").toDouble());
                    weatherData.name = cityTranslations[JO_city.value("name").toString()].toString();
                    weatherData.time_sunrise = time_sunrise;
                    weatherData.time_sunset = time_sunset;
                    weatherData.temp = temp;

                    // 仅更新每天中午的天气数据
                    if(date.time() == QTime(12,0,0)){
                        m_weatherDataList << weatherData;
                    }
                }
            } else {
                // 如果返回的状态码不是200，显示错误信息
                //emit weatherNow("Weather", "Temp", city + ", " + country + "\n" + cod + "\n" + JD.object().value("message").toString(), QPixmap(":icon/na.png"));
            }
        }else{
            // 如果JSON解析出错，显示原始响应内容
            //emit weatherNow("Weather", "Temp", QString(BA), QPixmap(":icon/na.png"));
        }

    }
}


QString Weather::getIconPath(const QString &iconName)
{
    QString icon_path; // = ":icon/Default/" + iconName + ".png";
    QString iconTheme = "Flat";
    if (!iconTheme.isEmpty()) {
        if (!iconTheme.startsWith("/")) {
            icon_path = ":icon/" + iconTheme + "/" + iconName + ".png";
        } else {
            QString icon_path1 = iconTheme + "/" + iconName + ".png";
            QFile file(icon_path1);
            if (file.exists()) {
                icon_path = icon_path1;
            }
        }
    }
    return icon_path;
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
