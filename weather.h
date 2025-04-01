#ifndef WEATHER_H
#define WEATHER_H

#include <QObject>

class Weather : public QObject
{
    Q_OBJECT
public:
    Weather();

    struct CityData {
        QString city;
        QString country;
    };

    void autoLocateCity();


signals:
    void autoLocateCityFinished(CityData);

private:
    CityData m_cityData;
};

#endif // WEATHER_H
