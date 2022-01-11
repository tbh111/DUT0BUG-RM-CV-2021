#include "config.h"

Config::Config()
{
    fp_json.setFileName("../config/config.json");
}

Config::~Config()
{
    fp_json.close();
}

int Config::InitJsonFile()
{
    if(!fp_json.open(QIODevice::ReadWrite))
    {
        qDebug() << "config file open error";
    }
    else
    {
        qDebug() << "config file open success";
    }

    fp_json.resize(0);
    QJsonObject json_object;

    QJsonObject json_object_mode;
    json_object_mode.insert("source", "video");
    json_object_mode.insert("function", "full");
    json_object.insert("mode", json_object_mode);

    QJsonObject json_object_path;
    json_object_path.insert("test_video_path", "../config/1.avi");
    json_object_path.insert("camera_param_path", "../config/camera.xml");
    json_object_path.insert("energy_svm_path", "../config/SVM_MODEL.xml");
    json_object.insert("paths", json_object_path);

    QJsonObject json_object_armor;
    json_object_armor.insert("blue_binary_threshold", 200);
    json_object_armor.insert("red_binary_threshold", 150);
    json_object.insert("armor", json_object_armor);

    QJsonObject json_object_serial;
    json_object_serial.insert("port", "/dev/ttyTHS2");
    json_object_serial.insert("baud_rate", 115200);
    json_object.insert("serial", json_object_serial);

    QJsonObject json_object_energy;
    // blue
    json_object_energy.insert("hsv_blue_hmin", 80);
    json_object_energy.insert("hsv_blue_hmax", 150);
    json_object_energy.insert("hsv_blue_smin", 40);
    json_object_energy.insert("hsv_blue_smax", 255);
    json_object_energy.insert("hsv_blue_vmin", 100);
    json_object_energy.insert("hsv_blue_vmax", 255);
    // red
    json_object_energy.insert("hsv_red_hmin", 250);
    json_object_energy.insert("hsv_red_hmax", 180);
    json_object_energy.insert("hsv_red_hmin_2", 0);
    json_object_energy.insert("hsv_red_hmax_2", 40);
    json_object_energy.insert("hsv_red_smin", 40);
    json_object_energy.insert("hsv_red_smax", 255);
    json_object_energy.insert("hsv_red_vmin", 100);
    json_object_energy.insert("hsv_red_vmax", 225);
    json_object_energy.insert("hsv_red_vmax", 225);
    json_object_energy.insert("hsv_red_vmax", 225);
    // area threshold
    json_object_energy.insert("energy_armor_area_min_threshold", 3500);
    json_object_energy.insert("energy_armor_area_max_threshold", 5000);
    json_object.insert("energy",json_object_energy);

    QJsonDocument jsonDoc(json_object);
    fp_json.write(jsonDoc.toJson());
    fp_json.close();
    qDebug() << "initialize config file success";
    return 0;
}

QString Config::ReadJsonFile(QString function, QString key)
{
    if(!fp_json.open(QIODevice::ReadOnly))
    {
        qDebug() << "config file open error";
    }
    else
    {
        qDebug() << "config file open success";
    }
    QJsonParseError jsonParserError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(fp_json.readAll(), &jsonParserError);
    QJsonObject json_object, json_object_key;
    if(!jsonDocument.isNull() && jsonParserError.error == QJsonParseError::NoError)
    {
        qDebug() << "read config file success";
        json_object = jsonDocument.object();
        if(json_object.contains(function) && json_object.value(function).isObject())
        {
            json_object_key = json_object.value(function).toObject();
            if(json_object_key.contains(key) && json_object_key.value(key).isString())
            {
                qDebug() << "read: " << json_object_key.value(key).toString();
                fp_json.close();
                return json_object_key.value(key).toString();
            }
            else if(json_object_key.contains(key) && json_object_key.value(key).isDouble())
            {
                qDebug() << "read: " << json_object_key.value(key).toDouble();
                fp_json.close();
                return QString::number(json_object_key.value(key).toDouble());
            }
            else
            {
                qDebug() << "err: no key called " << key;
                fp_json.close();
                return "";
            }
        }
        else
        {
            qDebug() << "err: no function called " << function;
            fp_json.close();
            return "";
        }
    }
    qDebug() << "err: " << jsonParserError.error;
    fp_json.close();
    return "";
}
