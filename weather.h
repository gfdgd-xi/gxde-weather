#ifndef WEATHER_H
#define WEATHER_H

#include <QObject>
#include <QPixmap>
#include <QDateTime>

class Weather : public QObject
{
    Q_OBJECT
public:

    Weather();

    struct CityData {
        QString city;
        QString country;
        double lat;
        double lon;
    };

    struct WeatherData {
        double lat;
        double lon;
        double windSpeed;
        double windDeg;
        double temp;
        int humidity;
        QString weather;
        QString name;
        QPixmap icon;

        QDateTime date;
        QDateTime time_sunrise;
        QDateTime time_sunset;
    };

    void autoLocateCity();
    QString getIconPath(const QString &iconName);
    void updateWeather();


signals:
    void autoLocateCityFinished(CityData);

private:
    QJsonObject loadCityTranslations();

    CityData m_cityData;
    QList<WeatherData> m_weatherDataList;
};

#endif // WEATHER_H
