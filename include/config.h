#ifndef CONFIG_H
#define CONFIG_H
#include <QString>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QFile>
#include <QDebug>

class Config
{
public:
    Config();
    ~Config();
    QString ReadJsonFile(QString function, QString key);
    QString ReadYamlFile();
    int InitJsonFile();
    int WriteJsonFile();
    int WriteYamlFile();

private:
    QFile fp_json;
};

#endif // CONFIG_H
