#ifndef GENERAL_H
#define GENERAL_H

#include <QString>

namespace common_utils
{
    // flags for transmit data
    // no detect: 0x00, detected: 0x01
    enum DETECT_STATE {DETECT_NONE = 0x00, DETECT_YES = 0x01};
    // disabled: 0x00, single: 0x01, auto: 0x02
    enum SHOOT_MODE {SHOOT_DISABLED = 0x00, SHOOT_SINGLE = 0x01, SHOOT_AUTO = 0x02};

    // function codes
    enum FUNC_CODE {FUNC_COLOR = 0x01, FUNC_TARGET = 0x02, FUNC_ENERGY = 0x03,
                    FUNC_VIDEO = 0x09, FUNC_ANGLE  = 0x11, FUNC_SPEED  = 0x12,
                    FUNC_INVALID = 0xFF};

    // flags for receive data
    // self red: 0x00, self blue: 0x01
    enum SELF_COLOR {SELF_RED = 0x00, SELF_BLUE = 0x01};
    // armor: 0x00, energy: 0x01, guard: undefined
    enum TARGET_MODE {TARGET_ARMOR = 0x00, TARGET_ENERGY = 0x01, TARGET_GUARD = 0x02};
    // small energy: 0x00, big energy: 0x01
    enum ENERGY_MODE {ENERGY_SMALL = 0x00, ENERGY_BIG = 0x01};
    // video on: 0x01, video off: 0x00
    enum VIDEO_MODE {VIDEO_OFF = 0x00, VIDEO_ON = 0x01};

    // flags for other control
    enum ARMOR_TYPE {ARMOR_SMALL = 0, ARMOR_BIG = 1, ARMOR_ENERGY = 2};
    enum PROTOCOL {CAN = 0, SERIAL = 1};
//    enum COMM_MODE {RX = 0, TX = 1};
    enum EXEC_MODE {SHOW_ON = 0, SHOW_OFF = 1};
    enum FRAME_STATE {VALID = true, INVALID = false};

    struct RecvPkg
    {
        int func_code = FUNC_INVALID;
        int recv_color = FUNC_INVALID;
        int recv_target = FUNC_INVALID;
        int recv_energy = FUNC_INVALID;
        int recv_video = FUNC_INVALID;
//        qint16 yaw = 0;
        qint16 pitch = 0;
        qint16 speed = 0;
        RecvPkg() = default;
//        RecvPkg(int t_color, int t_mode, int t_energy, int t_video,
//                qint16 t_yaw, qint16 t_pitch, qint8 t_speed):
//            recv_color(t_color), recv_mode(t_mode), recv_energy(t_energy), recv_video(t_video),
//            yaw(t_yaw), pitch(t_pitch), speed(t_speed){}
    };

    struct TranPkg
    {
        int check_mode = FUNC_INVALID;
        int check_color = FUNC_INVALID;
        int check_video = FUNC_INVALID;
        int detect_state = FUNC_INVALID;
        qint16 yaw = 0;
        qint16 pitch = 0;
        int cmd = FUNC_INVALID;
        TranPkg() = default;
        TranPkg(int t_mode, int t_color, int t_video, int t_state,
                qint16 t_yaw, qint16 t_pitch, int t_cmd):
            check_mode(t_mode), check_color(t_color), check_video(t_video), detect_state(t_state),
            yaw(t_yaw), pitch(t_pitch), cmd(t_cmd){}
    };

}

#endif // GENERAL_H
