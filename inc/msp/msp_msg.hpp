// MSP message definitions
// http://www.multiwii.com/wiki/index.php?title=Multiwii_Serial_Protocol

#ifndef MSP_MSG_HPP
#define MSP_MSG_HPP

#include "types.hpp"

#include <string>
#include <array>
#include <sstream>
#include <set>
#include <climits>
#include <cassert>



#include "deserialise.hpp"

namespace msp {
namespace msg {

const static size_t N_SERVO = 8;
const static size_t N_MOTOR = 8;

const static size_t BOARD_IDENTIFIER_LENGTH = 4;

const static size_t BUILD_DATE_LENGTH = 11;
const static size_t BUILD_TIME_LENGTH = 8;
const static size_t GIT_SHORT_REVISION_LENGTH = 7;

const static size_t MAX_NAME_LENGTH = 16;
const static size_t MAX_MODE_ACTIVATION_CONDITION_COUNT = 20;

const static size_t LED_CONFIGURABLE_COLOR_COUNT = 16;
const static size_t LED_MAX_STRIP_LENGTH = 32;

const static size_t MAX_ADJUSTMENT_RANGE_COUNT = 12;
const static size_t MAX_SIMULTANEOUS_ADJUSTMENT_COUNT = 6;

const static size_t OSD_ITEM_COUNT = 41; //manual count from iNav io/osd.h

const static size_t MAX_MAPPABLE_RX_INPUTS = 8;

const static size_t LED_MODE_COUNT = 6;
const static size_t LED_DIRECTION_COUNT = 6;
const static size_t LED_SPECIAL_COLOR_COUNT = 11;

enum class MultiType : uint8_t {
    TRI             = 1,
    QUADP,  		// 2
    QUADX,  		// 3
    BI,     		// 4
    GIMBAL, 		// 5
    Y6,     		// 6
    HEX6,   		// 7
    FLYING_WING,	// 8
    Y4,     		// 9
    HEX6X,  		// 10
    OCTOX8, 		// 11
    OCTOFLATP,  	// 12
    OCTOFLATX,  	// 13
    AIRPLANE,   	// 14
    HELI_120_CCPM,  // 15
    HELI_90_DEG,    // 16
    VTAIL4,     	// 17
    HEX6H,      	// 18
    DUALCOPTER      = 20,
    SINGLECOPTER,   // 21
};

enum class Capability {
    BIND,
    DYNBAL,
    FLAP,
    NAVCAP,
    EXTAUX
};

enum class Sensor {
    Accelerometer,
    Barometer,
    Magnetometer,
    GPS,
    Sonar,
    OpticalFlow,
    Pitot,
    GeneralHealth
};

const static size_t NAUX = 4;

enum class SwitchPosition : size_t {
    LOW  = 0,
    MID  = 1,
    HIGH = 2,
};

static const std::vector<std::string> FEATURES = {
    "RX_PPM", "VBAT", "INFLIGHT_ACC_CAL", "RX_SERIAL", "MOTOR_STOP",
    "SERVO_TILT", "SOFTSERIAL", "GPS", "FAILSAFE",
    "SONAR", "TELEMETRY", "AMPERAGE_METER", "3D", "RX_PARALLEL_PWM",
    "RX_MSP", "RSSI_ADC", "LED_STRIP", "DISPLAY", "ONESHOT125",
    "BLACKBOX", "CHANNEL_FORWARDING", "TRANSPONDER", "OSD"
};


enum class PID_Element: uint8_t {
    PID_ROLL,
    PID_PITCH,
    PID_YAW,
    PID_POS_Z,
    PID_POS_XY,
    PID_VEL_XY,
    PID_SURFACE,
    PID_LEVEL,
    PID_HEADING,
    PID_VEL_Z,
    PID_ITEM_COUNT
};

/////////////////////////////////////////////////////////////////////
/// Cleanflight

// MSP_API_VERSION: 1
struct ApiVersion : public Message {
    ApiVersion(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_API_VERSION; }

	value<uint8_t> protocol;
	value<uint8_t> major;
	value<uint8_t> minor;

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(protocol);
        rc &= data.unpack(major);
        rc &= data.unpack(minor);
        return rc;
    }
};

// MSP_FC_VARIANT: 2
struct FcVariant : public Message {
    FcVariant(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_FC_VARIANT; }

    value<std::string> identifier;

    bool decode(ByteVector& data) {
        return data.unpack(identifier,data.size());
    }
};

// MSP_FC_VERSION: 3
struct FcVersion : public Message {
    FcVersion(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_FC_VERSION; }

	value<uint8_t> major;
	value<uint8_t> minor;
	value<uint8_t> patch_level;

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(major);
        rc &= data.unpack(minor);
        rc &= data.unpack(patch_level);
        return rc;
    }
};

// MSP_BOARD_INFO: 4
struct BoardInfo : public Message {
    BoardInfo(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_BOARD_INFO; }

    value<std::string> identifier;
    value<uint16_t> version;
    value<uint8_t> osd_support;
    value<uint8_t> comms_capabilites;
    value<std::string> name;

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(identifier,BOARD_IDENTIFIER_LENGTH);
        rc &= data.unpack(version);
        rc &= data.unpack(osd_support);
        rc &= data.unpack(comms_capabilites);
        uint8_t name_len;
        rc &= data.unpack(name_len);
        rc &= data.unpack(name,name_len);
        return rc;
    }
};

// MSP_BUILD_INFO: 5
struct BuildInfo : public Message {
    BuildInfo(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_BUILD_INFO; }

    value<std::string> buildDate;
    value<std::string> buildTime;
    value<std::string> shortGitRevision;

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(buildDate,BUILD_DATE_LENGTH);
        rc &= data.unpack(buildTime,BUILD_TIME_LENGTH);
        rc &= data.unpack(shortGitRevision,GIT_SHORT_REVISION_LENGTH);
        return rc;
    }
};

struct PidSettings {
    value<uint8_t> async_mode;
    value<uint16_t> acc_task_frequency;
    value<uint16_t> attitude_task_frequency;
    value<uint8_t> heading_hold_rate_limit;
    value<uint8_t> heading_hold_error_lpf_freq;
    value<uint16_t> yaw_jump_prevention_limit;
    value<uint8_t> gyro_lpf;
    value<uint8_t> acc_soft_lpf_hz;
};

// MSP_INAV_PID: 6
struct InavPid : public PidSettings, public Message {
    InavPid(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_INAV_PID; }

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(async_mode);
        rc &= data.unpack(acc_task_frequency);
        rc &= data.unpack(attitude_task_frequency);
        rc &= data.unpack(heading_hold_rate_limit);
        rc &= data.unpack(heading_hold_error_lpf_freq);
        rc &= data.unpack(yaw_jump_prevention_limit);
        rc &= data.unpack(gyro_lpf);
        rc &= data.unpack(acc_soft_lpf_hz);
        //read the reserved bytes
        rc &= data.consume(4);
        return rc;
    }
};


// MSP_SET_INAV_PID: 7
struct SetInavPid : public PidSettings, public Message {
    SetInavPid(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_INAV_PID; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(async_mode);
        data.pack(acc_task_frequency);
        data.pack(attitude_task_frequency);
        data.pack(heading_hold_rate_limit);
        data.pack(heading_hold_error_lpf_freq);
        data.pack(yaw_jump_prevention_limit);
        data.pack(gyro_lpf);
        data.pack(acc_soft_lpf_hz);
        //write the reserved bytes
        data.pack(uint32_t(0));
        return data;
    }
};


// MSP_NAME: 10
struct BoardName : public Message {
    BoardName(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_NAME; }
    
    value<std::string> name;
    
    bool decode(ByteVector & data) {
        return data.unpack(name,data.size());
    }
};


// MSP_SET_NAME: 11
struct SetBoardName : public Message {
    SetBoardName(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_NAME; }
    
    value<std::string> name;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(name,MAX_NAME_LENGTH);
        return data;
    }
};

struct NavPosHoldSettings {
    value<uint8_t> user_control_mode;
    value<uint16_t> max_auto_speed;
    value<uint16_t> max_auto_climb_rate;
    value<uint16_t> max_manual_speed;
    value<uint16_t> max_manual_climb_rate;
    value<uint8_t> max_bank_angle;
    value<uint8_t> use_thr_mid_for_althold;
    value<uint16_t> hover_throttle;
};

// MSP_NAV_POSHOLD: 12
struct NavPosHold : public NavPosHoldSettings, public Message {
    NavPosHold(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_NAV_POSHOLD; }
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(user_control_mode);
        rc &= data.unpack(max_auto_speed);
        rc &= data.unpack(max_auto_climb_rate);
        rc &= data.unpack(max_manual_speed);
        rc &= data.unpack(max_manual_climb_rate);
        rc &= data.unpack(max_bank_angle);
        rc &= data.unpack(use_thr_mid_for_althold);
        rc &= data.unpack(hover_throttle);
        return rc;
    }
};

// MSP_SET_NAV_POSHOLD: 13
struct SetNavPosHold : public NavPosHoldSettings, public Message {
    SetNavPosHold(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_NAV_POSHOLD; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(user_control_mode);
        data.pack(max_auto_speed);
        data.pack(max_auto_climb_rate);
        data.pack(max_manual_speed);
        data.pack(max_manual_climb_rate);
        data.pack(max_bank_angle);
        data.pack(use_thr_mid_for_althold);
        data.pack(hover_throttle);
        return data;
    }
};

struct CalibrationDataSettings {
    value<uint16_t> acc_zero_x;
    value<uint16_t> acc_zero_y;
    value<uint16_t> acc_zero_z;
    value<uint16_t> acc_gain_x;
    value<uint16_t> acc_gain_y;
    value<uint16_t> acc_gain_z;
};
        
// MSP_CALIBRATION_DATA: 14
struct CalibrationData : public CalibrationDataSettings, public Message {
    CalibrationData(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_CALIBRATION_DATA; }
    
    value<uint8_t> axis_calibration_flags;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(axis_calibration_flags);
        rc &= data.unpack(acc_zero_x);
        rc &= data.unpack(acc_zero_y);
        rc &= data.unpack(acc_zero_z);
        rc &= data.unpack(acc_gain_x);
        rc &= data.unpack(acc_gain_y);
        rc &= data.unpack(acc_gain_z);
        return rc;
    }
};

// MSP_SET_CALIBRATION_DATA: 15
struct SetCalibrationData : public CalibrationDataSettings, public Message {
    SetCalibrationData(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_CALIBRATION_DATA; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(acc_zero_x);
        data.pack(acc_zero_y);
        data.pack(acc_zero_z);
        data.pack(acc_gain_x);
        data.pack(acc_gain_y);
        data.pack(acc_gain_z);
        return data;
    }
};

struct PositionEstimationConfigSettings {
    value<float> w_z_baro_p;
    value<float> w_z_gps_p;
    value<float> w_z_gps_v;
    value<float> w_xy_gps_p;
    value<float> w_xy_gps_v;
    value<uint8_t> gps_min_sats;
    value<bool> use_gps_vel_NED;
};


// MSP_POSITION_ESTIMATION_CONFIG: 16
struct PositionEstimationConfig : public PositionEstimationConfigSettings, public Message {
    PositionEstimationConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_POSITION_ESTIMATION_CONFIG; }
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack<uint16_t>(w_z_baro_p,0.01);
        rc &= data.unpack<uint16_t>(w_z_gps_p,0.01);
        rc &= data.unpack<uint16_t>(w_z_gps_v,0.01);
        rc &= data.unpack<uint16_t>(w_xy_gps_p,0.01);
        rc &= data.unpack<uint16_t>(w_xy_gps_v,0.01);
        rc &= data.unpack(gps_min_sats);
        rc &= data.unpack(use_gps_vel_NED);
        return rc;
    }
};

// MSP_SET_POSITION_ESTIMATION_CONFIG: 17
struct SetPositionEstimationConfig : public PositionEstimationConfigSettings, public Message {
    SetPositionEstimationConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_POSITION_ESTIMATION_CONFIG; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(static_cast<uint16_t>(w_z_baro_p()*100));
        data.pack(static_cast<uint16_t>(w_z_gps_p()*100));
        data.pack(static_cast<uint16_t>(w_z_gps_v()*100));
        data.pack(static_cast<uint16_t>(w_xy_gps_p()*100));
        data.pack(static_cast<uint16_t>(w_xy_gps_v()*100));
        data.pack(gps_min_sats);
        data.pack(use_gps_vel_NED);
        return data;
    }
};
    
// MSP_WP_MISSION_LOAD: 18
struct WpMissionLoad : public Message {
    WpMissionLoad(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_WP_MISSION_LOAD; }
    
    ByteVector encode() const {
        return ByteVector(1,0);
    }
};
    
// MSP_WP_MISSION_SAVE: 19
struct WpMissionSave : public Message {
    WpMissionSave(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_WP_MISSION_SAVE; }
    
    ByteVector encode() const {
        return ByteVector(1,0);
    }
};

// MSP_WP_GETINFO: 20
struct WpGetInfo : public Message {
    WpGetInfo(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_WP_GETINFO; }
    
    value<uint8_t> wp_capabilites;
    value<uint8_t> max_waypoints;
    value<bool> wp_list_valid;
    value<uint8_t> wp_count;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(wp_capabilites);
        rc &= data.unpack(max_waypoints);
        rc &= data.unpack(wp_list_valid);
        rc &= data.unpack(wp_count);
        return rc;
    }
};

struct RthAndLandConfigSettings {
    value<uint16_t> min_rth_distance;
    value<uint8_t> rth_climb_first;
    value<uint8_t> rth_climb_ignore_emerg;
    value<uint8_t> rth_tail_first;
    value<uint8_t> rth_allow_landing;
    value<uint8_t> rth_alt_control_mode;
    value<uint16_t> rth_abort_threshold;
    value<uint16_t> rth_altitude;
    value<uint16_t> land_descent_rate;
    value<uint16_t> land_slowdown_minalt;
    value<uint16_t> land_slowdown_maxalt;
    value<uint16_t> emerg_descent_rate;
};

// MSP_RTH_AND_LAND_CONFIG: 21
struct RthAndLandConfig : public RthAndLandConfigSettings, public Message {
    RthAndLandConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_RTH_AND_LAND_CONFIG; }
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(min_rth_distance);
        rc &= data.unpack(rth_climb_first);
        rc &= data.unpack(rth_climb_ignore_emerg);
        rc &= data.unpack(rth_tail_first);
        rc &= data.unpack(rth_allow_landing);
        rc &= data.unpack(rth_alt_control_mode);
        rc &= data.unpack(rth_abort_threshold);
        rc &= data.unpack(rth_altitude);
        rc &= data.unpack(land_descent_rate);
        rc &= data.unpack(land_slowdown_minalt);
        rc &= data.unpack(land_slowdown_maxalt);
        rc &= data.unpack(emerg_descent_rate);
        return rc;
    }
};

// MSP_SET_RTH_AND_LAND_CONFIG: 22
struct SetRthAndLandConfig : public RthAndLandConfigSettings, public Message {
    SetRthAndLandConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_RTH_AND_LAND_CONFIG; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(min_rth_distance);
        data.pack(rth_climb_first);
        data.pack(rth_climb_ignore_emerg);
        data.pack(rth_tail_first);
        data.pack(rth_allow_landing);
        data.pack(rth_alt_control_mode);
        data.pack(rth_abort_threshold);
        data.pack(rth_altitude);
        data.pack(land_descent_rate);
        data.pack(land_slowdown_minalt);
        data.pack(land_slowdown_maxalt);
        data.pack(emerg_descent_rate);
        return data;
    }
};

struct FwConfigSettings {
    value<uint16_t> cruise_throttle;
    value<uint16_t> min_throttle;
    value<uint16_t> max_throttle;
    value<uint8_t> max_bank_angle;
    value<uint8_t> max_climb_angle;
    value<uint8_t> max_dive_angle;
    value<uint8_t> pitch_to_throttle;
    value<uint16_t> loiter_radius;
};

// MSP_FW_CONFIG: 23
struct FwConfig : public FwConfigSettings, public Message {
    FwConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_FW_CONFIG; }
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(cruise_throttle);
        rc &= data.unpack(min_throttle);
        rc &= data.unpack(max_throttle);
        rc &= data.unpack(max_bank_angle);
        rc &= data.unpack(max_climb_angle);
        rc &= data.unpack(max_dive_angle);
        rc &= data.unpack(pitch_to_throttle);
        rc &= data.unpack(loiter_radius);
        return rc;
    }
};

// MSP_SET_FW_CONFIG: 24
struct SetFwConfig : public FwConfigSettings, public Message {
    SetFwConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_FW_CONFIG; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(cruise_throttle);
        data.pack(min_throttle);
        data.pack(max_throttle);
        data.pack(max_bank_angle);
        data.pack(max_climb_angle);
        data.pack(max_dive_angle);
        data.pack(pitch_to_throttle);
        data.pack(loiter_radius);
        return data;
    }
};

struct BatteryConfigSettings {
    value<uint8_t> vbatmincellvoltage;
    value<uint8_t> vbatmaxcellvoltage;
    value<uint8_t> vbatwarningcellvoltage;
    value<uint16_t> batteryCapacity;
    value<uint8_t> voltageMeterSource;
    value<uint8_t> currentMeterSource;
};

// MSP_BATTERY_CONFIG: 32
struct BatteryConfig : public BatteryConfigSettings, public Message {
    BatteryConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_BATTERY_CONFIG; }
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(vbatmincellvoltage);
        rc &= data.unpack(vbatmaxcellvoltage);
        rc &= data.unpack(vbatwarningcellvoltage);
        rc &= data.unpack(batteryCapacity);
        rc &= data.unpack(voltageMeterSource);
        rc &= data.unpack(currentMeterSource);
        return rc;
    }
};

// MSP_SET_BATTERY_CONFIG: 33
struct SetBatteryConfig : public BatteryConfigSettings, public Message {
    SetBatteryConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_BATTERY_CONFIG; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(vbatmincellvoltage);
        data.pack(vbatmaxcellvoltage);
        data.pack(vbatwarningcellvoltage);
        data.pack(batteryCapacity);
        data.pack(voltageMeterSource);
        data.pack(currentMeterSource);
        return data;
    }
};


struct box_description
{
    value<uint8_t> id;
    value<uint8_t> aux_channel_index;
    value<uint8_t> startStep;
    value<uint8_t> endStep;
};

// MSP_MODE_RANGES: 34
struct ModeRanges : public Message {
    ModeRanges(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_MODE_RANGES; }
    
    std::array<box_description,MAX_MODE_ACTIVATION_CONDITION_COUNT> boxes;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        for (uint i = 0; i < MAX_MODE_ACTIVATION_CONDITION_COUNT; i++) {
            rc &= data.unpack(boxes[i].id);
            rc &= data.unpack(boxes[i].aux_channel_index);
            rc &= data.unpack(boxes[i].startStep);
            rc &= data.unpack(boxes[i].endStep);
        }
        return rc;
    }
};

// MSP_SET_MODE_RANGE: 35
struct SetModeRange : public Message {
    SetModeRange(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_MODE_RANGE; }
    
    value<uint8_t> mode_activation_condition_idx;
    box_description box;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(mode_activation_condition_idx);
        data.pack(box.id);
        data.pack(box.aux_channel_index);
        data.pack(box.startStep);
        data.pack(box.endStep);
        return data;
    }
};



// MSP_FEATURE: 36
struct Feature : public Message {
    Feature(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_FEATURE; }

    std::set<std::string> features;
    
    bool decode(ByteVector& data) {
        uint32_t mask;
        bool rc = data.unpack(mask);
        if (!rc) return rc;
        features.clear();
        for(size_t ifeat(0); ifeat<FEATURES.size(); ifeat++) {
            if(mask & (1<<ifeat))
                features.insert(FEATURES[ifeat]);
        }
        return rc;
    }
};

// MSP_SET_FEATURE: 37
struct SetFeature : public Message {
    SetFeature(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_FEATURE; }

    std::set<std::string> features;

    ByteVector encode() const {
        ByteVector data;
        uint32_t mask = 0;
        for(size_t ifeat(0); ifeat<FEATURES.size(); ifeat++) {
            if(features.count(FEATURES[ifeat]))
                mask |= 1<<ifeat;
        }
        data.pack(mask);
        return data;
    }
};

//iNav uses decidegrees, BF/CF use degrees
struct BoardAlignmentSettings {
    value<uint16_t> roll;
    value<uint16_t> pitch;
    value<uint16_t> yaw;
};

// MSP_BOARD_ALIGNMENT: 38
struct BoardAlignment : public BoardAlignmentSettings, public Message {
    BoardAlignment(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_BOARD_ALIGNMENT; }

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(roll);
        rc &= data.unpack(pitch);
        rc &= data.unpack(yaw);
        return rc;
    }
};

// MSP_SET_BOARD_ALIGNMENT: 39
struct SetBoardAlignment : public BoardAlignmentSettings, public Message {
    SetBoardAlignment(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_BOARD_ALIGNMENT; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(roll);
        data.pack(pitch);
        data.pack(yaw);
        return data;
    }
};

struct CurrentMeterConfigSettings {
    value<uint16_t> currnet_scale;
    value<uint16_t> current_offset;
    value<uint8_t> current_type;
    value<uint16_t> capacity;
};

// MSP_CURRENT_METER_CONFIG: 40 (differs from Cleanflight/BetaFlight)
struct CurrentMeterConfig : public CurrentMeterConfigSettings, public Message {
    CurrentMeterConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_CURRENT_METER_CONFIG; }

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(currnet_scale);
        rc &= data.unpack(current_offset);
        rc &= data.unpack(current_type);
        rc &= data.unpack(capacity);
        return rc;
    }
};


// MSP_SET_CURRENT_METER_CONFIG: 41 (differs from Cleanflight/BetaFlight)
struct SetCurrentMeterConfig : public CurrentMeterConfigSettings, public Message {
    SetCurrentMeterConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_CURRENT_METER_CONFIG; }

    ByteVector encode() const {
        ByteVector data;
        data.pack(currnet_scale);
        data.pack(current_offset);
        data.pack(current_type);
        data.pack(capacity);
        return data;
    }
};


// MSP_MIXER: 42
struct Mixer : public Message {
    Mixer(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_MIXER; }

    value<uint8_t> mode;

    bool decode(ByteVector& data) {
        return data.unpack(mode);
    }
};



// MSP_SET_MIXER: 43
struct SetMixer : public Message {
    SetMixer(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_MIXER; }

    value<uint8_t> mode;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(mode);
        return data;
    }
};

struct RxConfigSettings {
    size_t valid_data_groups;
    //group1
    value<uint8_t> serialrx_provider;
    value<uint16_t> maxcheck;
    value<uint16_t> midrc;
    value<uint16_t> mincheck;
    value<uint8_t> spektrum_sat_bind;
    //group 2
    value<uint16_t> rx_min_usec;
    value<uint16_t> rx_max_usec;
    //group 3
    value<uint8_t> rcInterpolation;
    value<uint8_t> rcInterpolationInterval;
    value<uint16_t> airModeActivateThreshold;
    //group 4
    value<uint8_t> rx_spi_protocol;
    value<uint32_t> rx_spi_id;
    value<uint8_t> rx_spi_rf_channel_count;
    //group 5
    value<uint8_t> fpvCamAngleDegrees;
    //group 6 - iNav only
    value<uint8_t> receiverType;
};

// MSP_RX_CONFIG: 44
struct RxConfig : public RxConfigSettings, public Message {
    RxConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_RX_CONFIG; }

    bool decode(ByteVector& data) {
        bool rc = true;
        valid_data_groups = 1;
        rc &= data.unpack(serialrx_provider);
        rc &= data.unpack(maxcheck);
        rc &= data.unpack(midrc);
        rc &= data.unpack(mincheck);
        rc &= data.unpack(spektrum_sat_bind);
        if (data.unpacking_remaining() == 0) return rc;
        
        valid_data_groups += 1;
        rc &= data.unpack(rx_min_usec);
        rc &= data.unpack(rx_max_usec);
        if (data.unpacking_remaining() == 0) return rc;
        
        valid_data_groups += 1;
        rc &= data.unpack(rcInterpolation);
        rc &= data.unpack(rcInterpolationInterval);
        rc &= data.unpack(airModeActivateThreshold);
        if (data.unpacking_remaining() == 0) return rc;
        
        valid_data_groups += 1;
        rc &= data.unpack(rx_spi_protocol);
        rc &= data.unpack(rx_spi_id);
        rc &= data.unpack(rx_spi_rf_channel_count);
        if (data.unpacking_remaining() == 0) return rc;
        
        valid_data_groups += 1;
        rc &= data.unpack(fpvCamAngleDegrees);
        if (data.unpacking_remaining() == 0) return rc;
        
        valid_data_groups += 1;
        rc &= data.unpack(receiverType);
        return rc;
    }
};


// MSP_SET_RX_CONFIG: 45
struct SetRxConfig : public RxConfigSettings, public Message {
    SetRxConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_RX_CONFIG; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(serialrx_provider);
        data.pack(maxcheck);
        data.pack(midrc);
        data.pack(mincheck);
        data.pack(spektrum_sat_bind);
        if (valid_data_groups == 1) return data;
        data.pack(rx_min_usec);
        data.pack(rx_max_usec);
        if (valid_data_groups == 2) return data;
        data.pack(rcInterpolation);
        data.pack(rcInterpolationInterval);
        data.pack(airModeActivateThreshold);
        if (valid_data_groups == 3) return data;
        data.pack(rx_spi_protocol);
        data.pack(rx_spi_id);
        data.pack(rx_spi_rf_channel_count);
        if (valid_data_groups == 4) return data;
        data.pack(fpvCamAngleDegrees);
        if (valid_data_groups == 5) return data;
        data.pack(receiverType);
        return data;
    }
};

struct HsvColor {
    value<uint16_t> h;
    value<uint8_t> s;
    value<uint8_t> v;
};

// MSP_LED_COLORS: 46
struct LedColors : public Message {
    LedColors(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_LED_COLORS; }

    std::array<HsvColor,LED_CONFIGURABLE_COLOR_COUNT> colors;

    bool decode(ByteVector& data) {
        bool rc = true;
        for (size_t i = 0; i < LED_CONFIGURABLE_COLOR_COUNT; ++i) {
            rc &= data.unpack(colors[i].h);
            rc &= data.unpack(colors[i].s);
            rc &= data.unpack(colors[i].v);
        }
        return rc;
    }
};


// MSP_SET_LED_COLORS: 47
struct SetLedColors : public Message {
    SetLedColors(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_LED_COLORS; }

    std::array<HsvColor,LED_CONFIGURABLE_COLOR_COUNT> colors;

    ByteVector encode() const {
        ByteVector data;
        for (size_t i = 0; i < LED_CONFIGURABLE_COLOR_COUNT; ++i) {
            data.pack(colors[i].h);
            data.pack(colors[i].s);
            data.pack(colors[i].v);
        }
        return data;
    }
};

// MSP_LED_STRIP_CONFIG: 48
struct LedStripConfigs : public Message {
    LedStripConfigs(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_LED_STRIP_CONFIG; }

    std::array<uint32_t,LED_MAX_STRIP_LENGTH> configs;

    bool decode(ByteVector& data) {
        bool rc = true;
        for (size_t i = 0; i < LED_MAX_STRIP_LENGTH; ++i) {
            rc &= data.unpack(configs[i]);
        }
        return rc;
    }
};

// MSP_SET_LED_STRIP_CONFIG: 49
struct SetLedStripConfig : public Message {
    SetLedStripConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_LED_STRIP_CONFIG; }

    value<uint8_t> cfg_index;
    value<uint32_t> config;

    ByteVector encode() const {
        ByteVector data;
        data.pack(cfg_index);
        data.pack(config);
        return data;
    }
};

// MSP_RSSI_CONFIG: 50
struct RssiConfig : public Message {
    RssiConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_RSSI_CONFIG; }

    value<uint8_t> rssi_channel;

    bool decode(ByteVector& data) {
        return data.unpack(rssi_channel);
    }
};

// MSP_SET_RSSI_CONFIG: 51
struct SetRssiConfig : public Message {
    SetRssiConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_RSSI_CONFIG; }

    value<uint8_t> rssi_channel;

    ByteVector encode() const {
        ByteVector data;
        data.pack(rssi_channel);
        return data;
    }
};

struct adjustmentRange {
    value<uint8_t> adjustmentIndex;
    value<uint8_t> auxChannelIndex;
    value<uint8_t> range_startStep;
    value<uint8_t> range_endStep;
    value<uint8_t> adjustmentFunction;
    value<uint8_t> auxSwitchChannelIndex;
};

// MSP_ADJUSTMENT_RANGES: 52
struct AdjustmentRanges : public Message {
    AdjustmentRanges(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_ADJUSTMENT_RANGES; }

    std::array<adjustmentRange,MAX_ADJUSTMENT_RANGE_COUNT> ranges;

    bool decode(ByteVector& data) {
        bool rc = true;
        for (size_t i = 0; i < MAX_ADJUSTMENT_RANGE_COUNT; ++i) {
            rc &= data.unpack(ranges[i].adjustmentIndex);
            rc &= data.unpack(ranges[i].auxChannelIndex);
            rc &= data.unpack(ranges[i].range_startStep);
            rc &= data.unpack(ranges[i].range_endStep);
            rc &= data.unpack(ranges[i].adjustmentFunction);
            rc &= data.unpack(ranges[i].auxSwitchChannelIndex);
        }
        return rc;
    }
};


// MSP_SET_ADJUSTMENT_RANGE: 53
struct SetAdjustmentRange : public Message {
    SetAdjustmentRange(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_ADJUSTMENT_RANGE; }

    value<uint8_t> range_index;
    adjustmentRange range;

    ByteVector encode() const {
        ByteVector data;
        data.pack(range_index);
        data.pack(range.adjustmentIndex);
        data.pack(range.auxChannelIndex);
        data.pack(range.range_startStep);
        data.pack(range.range_endStep);
        data.pack(range.adjustmentFunction);
        data.pack(range.auxSwitchChannelIndex);
        return data;
    }
};

struct CfSerialConfigSettings {
    value<uint8_t> identifier;
    value<uint16_t> functionMask;
    value<uint8_t> mspBaudrateIndx;
    value<uint8_t> gpsBaudrateIndx;
    value<uint8_t> telemetryBaudrateIndx;
    value<uint8_t> peripheralBaudrateIndx;
};

// MSP_CF_SERIAL_CONFIG: 54
struct CfSerialConfig : public Message {
    CfSerialConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_CF_SERIAL_CONFIG; }
    
    std::vector<CfSerialConfigSettings> configs;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        do {
            CfSerialConfigSettings tmp;
            rc &= data.unpack(tmp.identifier);
            rc &= data.unpack(tmp.functionMask);
            rc &= data.unpack(tmp.mspBaudrateIndx);
            rc &= data.unpack(tmp.gpsBaudrateIndx);
            rc &= data.unpack(tmp.telemetryBaudrateIndx);
            rc &= data.unpack(tmp.peripheralBaudrateIndx);
            if (rc) configs.push_back(tmp);
        } while (rc);
        return configs.size();
    }
};

// MSP_SET_CF_SERIAL_CONFIG: 55
struct SetCfSerialConfig : public Message {
    SetCfSerialConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_CF_SERIAL_CONFIG; }
    
    std::vector<CfSerialConfigSettings> configs;
 
    ByteVector encode() const {
        ByteVector data;
        for (auto config : configs) {
            data.pack(config.identifier);
            data.pack(config.functionMask);
            data.pack(config.mspBaudrateIndx);
            data.pack(config.gpsBaudrateIndx);
            data.pack(config.telemetryBaudrateIndx);
            data.pack(config.peripheralBaudrateIndx);
        }
        return data;
    }
};

struct VoltageMeterConfigSettings {
    value<uint8_t> scale_dV;
    value<uint8_t> cell_min_dV;
    value<uint8_t> cell_max_dV;
    value<uint8_t> cell_warning_dV;
};

// MSP_VOLTAGE_METER_CONFIG: 56 (differs from Cleanflight/BetaFlight)
struct VoltageMeterConfig : public VoltageMeterConfigSettings, public Message {
    VoltageMeterConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_VOLTAGE_METER_CONFIG; }

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(scale_dV);
        rc &= data.unpack(cell_min_dV);
        rc &= data.unpack(cell_max_dV);
        rc &= data.unpack(cell_warning_dV);
        return rc;
    }
};

// MSP_SET_VOLTAGE_METER_CONFIG: 57 (differs from Cleanflight/BetaFlight)
struct SetVoltageMeterConfig : public VoltageMeterConfigSettings, public Message {
    SetVoltageMeterConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_VOLTAGE_METER_CONFIG; }

    ByteVector encode() const {
        ByteVector data;
        data.pack(scale_dV);
        data.pack(cell_min_dV);
        data.pack(cell_max_dV);
        data.pack(cell_warning_dV);
        return data;
    }
};


// MSP_SONAR_ALTITUDE: 58
struct SonarAltitude : public Message {
    SonarAltitude(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SONAR_ALTITUDE; }

    value<uint32_t> altitude_cm; // meters

    bool decode(ByteVector& data) {
        return data.unpack(altitude_cm);
    }
};

// MSP_PID_CONTROLLER: 59
struct PidController : public Message {
    PidController(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_PID_CONTROLLER; }
    
    value<uint8_t> controller_id;
    
    bool decode(ByteVector& data) {
        return data.unpack(controller_id);
    }
};

// MSP_SET_PID_CONTROLLER: 60
struct SetPidController : public Message {
    SetPidController(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_PID_CONTROLLER; }
};

struct ArmingConfigSettings {
    value<uint8_t> auto_disarm_delay;
    value<uint8_t> disarm_kill_switch;
    bool imu_small_angle_valid;
    value<uint8_t> imu_small_angle;
};

//MSP_ARMING_CONFIG: 61
struct ArmingConfig : public ArmingConfigSettings, public Message {
    ArmingConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_ARMING_CONFIG; }

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(auto_disarm_delay);
        rc &= data.unpack(disarm_kill_switch);
        if (data.unpack(imu_small_angle)) imu_small_angle_valid = true;
        return rc;
    }
};

//MSP_SET_ARMING_CONFIG: 62
struct SetArmingConfig : public ArmingConfigSettings, public Message {
    SetArmingConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_ARMING_CONFIG; }

    ByteVector encode() const {
        ByteVector data;
        data.pack(auto_disarm_delay);
        data.pack(disarm_kill_switch);
        if (imu_small_angle_valid) data.pack(imu_small_angle);
        return data;
    }
};

// MSP_RX_MAP: 64
struct RxMap : public Message {
    RxMap(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_RX_MAP; }

    std::array<uint8_t,MAX_MAPPABLE_RX_INPUTS> map;

    bool decode(ByteVector& data) {
        if (data.size() < MAX_MAPPABLE_RX_INPUTS) return false;
        bool rc = true;
        for (size_t i = 0; i < MAX_MAPPABLE_RX_INPUTS; ++i) {
            rc &= data.unpack(data[i]);
        }
        return rc;
    }
};

// MSP_SET_RX_MAP: 65
struct SetRxMap : public Message {
    SetRxMap(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_RX_MAP; }

    std::array<uint8_t,MAX_MAPPABLE_RX_INPUTS> map;

    ByteVector encode() const {
        ByteVector data;
        for (auto channel : map) {
            data.pack(channel);
        }
        return data;
    }
};

//iNav uses decidegrees, BF/CF use degrees
struct BfConfigSettings {
    value<uint8_t> mixer_mode;
    value<uint32_t> feature_mask;
    value<uint8_t> serialrx_provider;
    value<uint16_t> roll;
    value<uint16_t> pitch;
    value<uint16_t> yaw;
    value<uint16_t> current_meter_scale;
    value<uint16_t> current_meter_offset;
};

// MSP_BF_CONFIG: 66, //out message baseflight-specific settings that aren't covered elsewhere
struct BfConfig : public BfConfigSettings, public Message {
    BfConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_BF_CONFIG; }

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(mixer_mode);
        rc &= data.unpack(feature_mask);
        rc &= data.unpack(serialrx_provider);
        rc &= data.unpack(roll);
        rc &= data.unpack(pitch);
        rc &= data.unpack(yaw);
        rc &= data.unpack(current_meter_scale);
        rc &= data.unpack(current_meter_offset);
        return rc;
    }
};

// MSP_SET_BF_CONFIG: 67, //in message baseflight-specific settings save
struct SetBfConfig : public BfConfigSettings, public Message {
    SetBfConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_BF_CONFIG; }

    ByteVector encode() const {
        ByteVector data;
        data.pack(mixer_mode);
        data.pack(feature_mask);
        data.pack(serialrx_provider);
        data.pack(roll);
        data.pack(pitch);
        data.pack(yaw);
        data.pack(current_meter_scale);
        data.pack(current_meter_offset);
        return data;
    }
};


// MSP_REBOOT: 68
struct Reboot : public Message {
    Reboot(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_REBOOT; }
};


// MSP_BF_BUILD_INFO: 69
struct BfBuildInfo : public Message {
    BfBuildInfo(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_BF_BUILD_INFO; }

    value<std::string> build_date;
    value<uint32_t> reserved1;
    value<uint32_t> reserved2;

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(build_date,11);
        rc &= data.unpack(reserved1);
        rc &= data.unpack(reserved2);
        return rc;
    }
};

// MSP_DATAFLASH_SUMMARY: 70
struct DataflashSummary : public Message {
    DataflashSummary(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_DATAFLASH_SUMMARY; }

    bool flash_is_ready;
    value<uint32_t> sectors;
    value<uint32_t> total_size;
    value<uint32_t> offset;

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(flash_is_ready);
        rc &= data.unpack(sectors);
        rc &= data.unpack(total_size);
        rc &= data.unpack(offset);
        return rc;
    }
};

//message format differs between iNav and BF/CF
// MSP_DATAFLASH_READ: 71
struct DataflashRead : public Message {
    DataflashRead(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_DATAFLASH_READ; }
    
    value<uint32_t> read_address;
    value<uint16_t> read_size;
    bool allow_compression;
    ByteVector flash_data;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(read_address);
        data.pack(read_size);
        data.pack(allow_compression);
        return data;
    }
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(read_address);
        flash_data = ByteVector(data.unpacking_iterator(),data.end());
        rc &= data.consume(flash_data.size());
        return rc;
    }

};

// MSP_DATAFLASH_ERASE: 72
struct DataflashErase : public Message {
    DataflashErase(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_DATAFLASH_ERASE; }

    bool decode(ByteVector& data) {
        return true;
    }
};

// MSP_LOOP_TIME: 73
struct LoopTime : public Message {
    LoopTime(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_LOOP_TIME; }

    value<uint16_t> loop_time;
    
    bool decode(ByteVector& data) {
        return data.unpack(loop_time);
    }
};

// MSP_SET_LOOP_TIME:74
struct SetLoopTime : public Message {
    SetLoopTime(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_LOOP_TIME; }
    
    value<uint16_t> loop_time;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(loop_time);
        return data;
    }
};

struct FailsafeSettings {
    bool extended_contents;
    value<uint8_t> delay;
    value<uint8_t> off_delay;
    value<uint16_t> throttle;
    value<uint8_t> kill_switch;
    value<uint16_t> throttle_low_delay;
    value<uint8_t> procedure;
    value<uint8_t> recovery_delay;
    value<uint16_t> fw_roll_angle;
    value<uint16_t> fw_pitch_angle;
    value<uint16_t> fw_yaw_rate;
    value<uint16_t> stick_motion_threshold;
    value<uint16_t> min_distance;
    value<uint8_t> min_distance_procedure;
};

// MSP_FAILSAFE_CONFIG: 75
struct FailsafeConfig : public FailsafeSettings, public Message {
    FailsafeConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_FAILSAFE_CONFIG; }
    
    bool decode(ByteVector& data) {
        bool rc = true;
        extended_contents = false;
        rc &= data.unpack(delay);
        rc &= data.unpack(off_delay);
        rc &= data.unpack(throttle);
        rc &= data.unpack(kill_switch);
        rc &= data.unpack(throttle_low_delay);
        rc &= data.unpack(procedure);
        if (data.unpacking_remaining() == 0) return rc;
        extended_contents = true;
        rc &= data.unpack(recovery_delay);
        rc &= data.unpack(fw_roll_angle);
        rc &= data.unpack(fw_pitch_angle);
        rc &= data.unpack(fw_yaw_rate);
        rc &= data.unpack(stick_motion_threshold);
        rc &= data.unpack(min_distance);
        rc &= data.unpack(min_distance_procedure);
        return rc;
    }
};

// MSP_SET_FAILSAFE_CONFIG: 76
struct SetFailsafeConfig : public FailsafeSettings, public Message {
    SetFailsafeConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_FAILSAFE_CONFIG; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(delay);
        data.pack(off_delay);
        data.pack(throttle);
        data.pack(kill_switch);
        data.pack(throttle_low_delay);
        data.pack(procedure);
        if (!extended_contents) return data;
        data.pack(recovery_delay);
        data.pack(fw_roll_angle);
        data.pack(fw_pitch_angle);
        data.pack(fw_yaw_rate);
        data.pack(stick_motion_threshold);
        data.pack(min_distance);
        data.pack(min_distance_procedure);
        return data;
    }
};

struct RxFailChannelSettings {
    value<uint8_t> mode;
    value<uint16_t> val;
};

// MSP_RXFAIL_CONFIG: 77
struct RxFailConfigs : public Message {
    RxFailConfigs(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_RXFAIL_CONFIG; }
    
    std::vector<RxFailChannelSettings> channels;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        channels.clear();
        while (rc && data.unpacking_remaining()) {
            RxFailChannelSettings tmp;
            rc &= data.unpack(tmp.mode);
            rc &= data.unpack(tmp.val);
            channels.push_back(tmp);
        }
        return rc;
    }
};

// MSP_SET_RXFAIL_CONFIG: 78
struct SetRxFailConfigs : public RxFailChannelSettings, public Message {
    SetRxFailConfigs(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_RXFAIL_CONFIG; }
    
    value<uint8_t> channel;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(channel);
        rc &= data.unpack(mode);
        rc &= data.unpack(val);
        return rc;
    }
};

// MSP_SDCARD_SUMMARY: 79
struct SdcardSummary : public Message {
    SdcardSummary(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SDCARD_SUMMARY; }

    value<uint8_t> flags;
    value<uint8_t> state;
    value<uint8_t> last_error;
    value<uint32_t> free_space_kb;
    value<uint32_t> total_space_kb;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(flags);
        rc &= data.unpack(state);
        rc &= data.unpack(last_error);
        rc &= data.unpack(free_space_kb);
        rc &= data.unpack(total_space_kb);
        return rc;
    }
};

struct BlackboxConfigSettings {
    value<uint8_t> device;
    value<uint8_t> rate_num;
    value<uint8_t> rate_denom;
    bool p_ratio_set;
    value<uint16_t> p_ratio;
};

// MSP_BLACKBOX_CONFIG: 80
struct BlackboxConfig : public BlackboxConfigSettings, public Message {
    BlackboxConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_BLACKBOX_CONFIG; }

    value<uint8_t> supported;

    bool decode(ByteVector& data) {
        bool rc = true;
        p_ratio_set = false;
        rc &= data.unpack(supported);
        rc &= data.unpack(device);
        rc &= data.unpack(rate_num);
        rc &= data.unpack(rate_denom);
        if (data.unpacking_remaining()) {
            p_ratio_set = true;
            rc &= data.unpack(p_ratio);
        }
        return rc;
    }
};

// MSP_SET_BLACKBOX_CONFIG: 81
struct SetBlackboxConfig : public BlackboxConfigSettings, public Message {
    SetBlackboxConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_BLACKBOX_CONFIG; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(device);
        data.pack(rate_num);
        data.pack(rate_denom);
        if (p_ratio_set) data.pack(p_ratio);
        return data;
    }
};

struct TransponderConfigSettings {
    value<uint8_t> provider;
    value<uint8_t> data_length;
};

// MSP_TRANSPONDER_CONFIG: 82
struct TransponderConfig : public Message {
    TransponderConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_TRANSPONDER_CONFIG; }
    
    value<uint8_t> transponder_count;
    std::vector<TransponderConfigSettings> transponder_data;
    value<uint8_t> provider;
    ByteVector provider_data;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(transponder_count);
        if (!transponder_count()) return rc;
        for (uint8_t i = 0; i < transponder_count(); ++i) {
            TransponderConfigSettings tmp;
            rc &= data.unpack(tmp.provider);
            rc &= data.unpack(tmp.data_length);
            transponder_data.push_back(tmp);
        }
        rc &= data.unpack(provider);
        if (!provider()) return rc;
        uint8_t data_len = transponder_data[provider()-1].data_length();
        provider_data = ByteVector(data.unpacking_iterator(),data.unpacking_iterator()+data_len);
        rc &= data.consume(data_len);
        return rc;
    }
    
};

// MSP_SET_TRANSPONDER_CONFIG: 83
struct SetTransponderConfig : public Message {
    SetTransponderConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_TRANSPONDER_CONFIG; }
    
    value<uint8_t> provider;
    ByteVector provider_data;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(provider);
        data.pack(provider_data);
        return data;
    }
};

//Differences between iNav and BF/CF
// MSP_OSD_CONFIG: 84
struct OsdConfig : public Message {
    OsdConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_OSD_CONFIG; }

    value<uint8_t> osd_flags;
    value<uint8_t> video_system;
    value<uint8_t> units;
    value<uint8_t> rssi_alarm;
    value<uint16_t> battery_cap_warn;
    value<uint16_t> time_alarm;
    value<uint16_t> alt_alarm;
    value<uint16_t> dist_alarm;
    value<uint16_t> neg_alt_alarm;
    std::array<uint16_t,OSD_ITEM_COUNT> item_pos;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(osd_flags);
        if (rc && osd_flags()) {
            rc &= data.unpack(video_system);
            rc &= data.unpack(units);
            rc &= data.unpack(rssi_alarm);
            rc &= data.unpack(battery_cap_warn);
            rc &= data.unpack(time_alarm);
            rc &= data.unpack(alt_alarm);
            rc &= data.unpack(dist_alarm);
            rc &= data.unpack(neg_alt_alarm);
            for (size_t i = 0; i < OSD_ITEM_COUNT; ++i) {
                rc &= data.unpack(item_pos[i]);
            }
        }
        return rc;
    }
};

// MSP_SET_OSD_CONFIG: 85
struct SetOsdConfig : public Message {
    SetOsdConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_OSD_CONFIG; }
    
    int8_t param_idx;
    value<uint16_t> item_pos;
    value<uint8_t> video_system;
    value<uint8_t> units;
    value<uint8_t> rssi_alarm;
    value<uint16_t> battery_cap_warn;
    value<uint16_t> time_alarm;
    value<uint16_t> alt_alarm;
    value<uint16_t> dist_alarm;
    value<uint16_t> neg_alt_alarm;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(param_idx);
        if (param_idx == -1) {
            data.pack(video_system);
            data.pack(units);
            data.pack(rssi_alarm);
            data.pack(battery_cap_warn);
            data.pack(time_alarm);
            data.pack(alt_alarm);
            data.pack(dist_alarm);
            data.pack(neg_alt_alarm);
        } else {
            data.pack(item_pos);
        }
        return data;
    }
};

// MSP_OSD_CHAR_READ: 86 No reference implementation


// MSP_OSD_CHAR_WRITE: 87
struct OsdCharWrite : public Message {
    OsdCharWrite(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_OSD_CHAR_WRITE; }
    
    value<uint8_t> addr;
    std::array<uint8_t,54> data;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(addr);
        for (auto c : data) {
            data.pack(c);
        }
        return data;
    }
};


// MSP_VTX_CONFIG: 88
struct VtxConfig : public Message {
    VtxConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_VTX_CONFIG; }

    value<uint8_t> device_type;
    value<uint8_t> band;
    value<uint8_t> channel;
    value<uint8_t> power_idx;
    value<uint8_t> pit_mode;
    bool freq_set;
    value<uint16_t> frequency;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        freq_set = false;
        rc &= data.unpack(device_type);
        if (rc && (device_type() != 0xFF)) {
            rc &= data.unpack(band);
            rc &= data.unpack(channel);
            rc &= data.unpack(power_idx);
            rc &= data.unpack(pit_mode);
            if (data.unpacking_remaining()) {
                freq_set = true;
                rc &= data.unpack(frequency);
            }
        }
        return rc;
    }
};

// MSP_SET_VTX_CONFIG: 89
struct SetVtxConfig : public Message {
    SetVtxConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_VTX_CONFIG; }
    
    value<uint16_t> frequency;
    value<uint8_t> power;
    value<uint8_t> pit_mode;
    
    bool set_freq(uint8_t band, uint8_t channel) {
        if (band & 0xF8 || channel & 0xF8) {
            return false;
        }
        frequency = (band-1) & ((channel-1) << 3);
        return true;
    }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(frequency);
        data.pack(power);
        data.pack(pit_mode);
        return data;
    }
};

//Differs between iNav and BF/CF
struct AdvancedConfigSettings {
    value<uint8_t> gyro_sync_denom;
    value<uint8_t> pid_process_denom;
    value<uint8_t> use_unsynced_pwm;
    value<uint8_t> motor_pwm_protocol;
    value<uint16_t> motor_pwm_rate;
    value<uint16_t> servo_pwm_rate; //digitalIdleOffsetValue in BF/CF
    value<uint8_t> gyro_sync;
    bool pwm_inversion_set;
    value<uint8_t> pwm_inversion;
};

// Betaflight Additional Commands
// MSP_ADVANCED_CONFIG: 90
struct AdvancedConfig : public AdvancedConfigSettings, public Message {
    AdvancedConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_ADVANCED_CONFIG; }

    bool decode(ByteVector& data) {
        bool rc = true;
        pwm_inversion_set = false;
        rc &= data.unpack(gyro_sync_denom);
        rc &= data.unpack(pid_process_denom);
        rc &= data.unpack(use_unsynced_pwm);
        rc &= data.unpack(motor_pwm_protocol);
        rc &= data.unpack(motor_pwm_rate);
        rc &= data.unpack(servo_pwm_rate);
        rc &= data.unpack(gyro_sync);
        if (rc && data.unpacking_remaining()) {
            pwm_inversion_set = true;
            rc &= data.unpack(pwm_inversion);
        }
        return rc;
    }
};


// MSP_SET_ADVANCED_CONFIG: 91
struct SetAdvancedConfig : public AdvancedConfigSettings, public Message {
    SetAdvancedConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_ADVANCED_CONFIG; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(gyro_sync_denom);
        data.pack(pid_process_denom);
        data.pack(use_unsynced_pwm);
        data.pack(motor_pwm_protocol);
        data.pack(motor_pwm_rate);
        data.pack(servo_pwm_rate);
        data.pack(gyro_sync);
        if (pwm_inversion_set) data.pack(pwm_inversion);
        return data;
    }
};

struct FilterConfigSettings {
    value<uint8_t> gyro_soft_lpf_hz;
    value<uint16_t> dterm_lpf_hz;
    value<uint16_t> yaw_lpf_hz;
    value<uint16_t> gyro_soft_notch_hz_1;
    value<uint16_t> gyro_soft_notch_cutoff_1;
    value<uint16_t> dterm_soft_notch_hz;
    value<uint16_t> dterm_soft_notch_cutoff;
    value<uint16_t> gyro_soft_notch_hz_2;
    value<uint16_t> gyro_soft_notch_cutoff_2;
    bool dterm_filter_type_set;
    value<uint8_t> dterm_filter_type;
};

// MSP_FILTER_CONFIG: 92
struct FilterConfig : public FilterConfigSettings, public Message {
    FilterConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_FILTER_CONFIG; }

    bool decode(ByteVector& data) {
        bool rc = true;
        dterm_filter_type_set = false;
        rc &= data.unpack(gyro_soft_lpf_hz);
        rc &= data.unpack(dterm_lpf_hz);
        rc &= data.unpack(yaw_lpf_hz);
        rc &= data.unpack(gyro_soft_notch_hz_1);
        rc &= data.unpack(gyro_soft_notch_cutoff_1);
        rc &= data.unpack(dterm_soft_notch_hz);
        rc &= data.unpack(dterm_soft_notch_cutoff);
        rc &= data.unpack(gyro_soft_notch_hz_2);
        rc &= data.unpack(gyro_soft_notch_cutoff_2);
        if (rc && data.unpacking_remaining()) {
            dterm_filter_type_set = true;
            rc &= data.unpack(dterm_filter_type);
        }
        return rc;
    }
};

// MSP_SET_FILTER_CONFIG: 93 
struct SetFilterConfig : public FilterConfigSettings, public Message {
    SetFilterConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_FILTER_CONFIG; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(gyro_soft_lpf_hz);
        data.pack(dterm_lpf_hz);
        data.pack(yaw_lpf_hz);
        data.pack(gyro_soft_notch_hz_1);
        data.pack(gyro_soft_notch_cutoff_1);
        data.pack(dterm_soft_notch_hz);
        data.pack(dterm_soft_notch_cutoff);
        data.pack(gyro_soft_notch_hz_2);
        data.pack(gyro_soft_notch_cutoff_2);
        if (dterm_filter_type_set) data.pack(dterm_filter_type);
        return data;
    }
};


struct PidAdvancedSettings {
    value<uint16_t> rollPitchItermIgnoreRate;
    value<uint16_t> yawItermIgnoreRate;
    value<uint16_t> yaw_p_limit;
    value<uint8_t> deltaMethod;
    value<uint8_t> vbatPidCompensation;
    value<uint8_t> setpointRelaxRatio;
    value<float> dterm_setpoint_weight; //TODO scaled value
    value<uint16_t> pidSumLimit;
    value<uint8_t> itermThrottleGain;
    value<uint32_t> axisAccelerationLimitRollPitch; //TODO scaled and clamped value
    value<uint32_t> axisAccelerationLimitYaw; //TODO scaled and clamped value
    
};

//Difference between iNav and BF/CF
// MSP_PID_ADVANCED: 94
struct PidAdvanced : public PidAdvancedSettings, public Message {
    PidAdvanced(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_PID_ADVANCED; }

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(rollPitchItermIgnoreRate);
        rc &= data.unpack(yawItermIgnoreRate);
        rc &= data.unpack(yaw_p_limit);
        rc &= data.unpack(deltaMethod);
        rc &= data.unpack(vbatPidCompensation);
        rc &= data.unpack(setpointRelaxRatio);
        rc &= data.unpack<uint8_t>(dterm_setpoint_weight,0.01);
        rc &= data.unpack(pidSumLimit);
        rc &= data.unpack(itermThrottleGain);
        value<uint16_t> tmp16;
        rc &= data.unpack(tmp16);
        axisAccelerationLimitRollPitch = tmp16()*10;
        rc &= data.unpack(tmp16);
        axisAccelerationLimitYaw = tmp16()*10;
        return rc;
    }
};


// MSP_SET_PID_ADVANCED: 95
struct SetPidAdvanced : public PidAdvancedSettings, public Message {
    SetPidAdvanced(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_PID_ADVANCED; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(rollPitchItermIgnoreRate);
        data.pack(yawItermIgnoreRate);
        data.pack(yaw_p_limit);
        data.pack(deltaMethod);
        data.pack(vbatPidCompensation);
        data.pack(setpointRelaxRatio);
        data.pack(uint8_t(dterm_setpoint_weight()*100));
        data.pack(pidSumLimit);
        data.pack(itermThrottleGain);
        data.pack(axisAccelerationLimitRollPitch()/10);
        data.pack(axisAccelerationLimitYaw()/10);
        return data;
    }
};

struct SensorConfigSettings {
    value<uint8_t> acc_hardware;
    value<uint8_t> baro_hardware;
    value<uint8_t> mag_hardware;
    bool extended_contents;
    value<uint8_t> pitot_hardware;
    value<uint8_t> rangefinder_hardware;
    value<uint8_t> opflow_hardware;
};

// MSP_SENSOR_CONFIG: 96
struct SensorConfig : public SensorConfigSettings, public Message {
    SensorConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SENSOR_CONFIG; }
    
    bool decode(ByteVector& data) {
        bool rc = true;
        extended_contents = false;
        rc &= data.unpack(acc_hardware);
        rc &= data.unpack(baro_hardware);
        rc &= data.unpack(mag_hardware);
        if (data.unpacking_remaining()) {
            extended_contents = true;
            rc &= data.unpack(pitot_hardware);
            rc &= data.unpack(rangefinder_hardware);
            rc &= data.unpack(opflow_hardware);
        }
        return rc;
    }
};


// MSP_SET_SENSOR_CONFIG: 97
struct SetSensorConfig : public SensorConfigSettings, public Message {
    SetSensorConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_SENSOR_CONFIG; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(acc_hardware);
        data.pack(baro_hardware);
        data.pack(mag_hardware);
        if (!extended_contents) return data;
        data.pack(pitot_hardware);
        data.pack(rangefinder_hardware);
        data.pack(opflow_hardware);
        return data;
    }
};

// MSP_CAMERA_CONTROL: 98
struct CameraControl : public Message {
    CameraControl(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_CAMERA_CONTROL; }
    
    value<uint8_t> key;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(key);
        return data;
    }
};

// MSP_SET_ARMING_DISABLED: 99
struct SetArmingDisabled : public Message {
    SetArmingDisabled(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_ARMING_DISABLED; }
    
    value<uint8_t> command;
    value<uint8_t> disableRunawayTakeoff;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(command);
        data.pack(disableRunawayTakeoff);
        return data;
    }
};


/////////////////////////////////////////////////////////////////////
/// Requests (1xx)

// MSP_IDENT: 100
struct Ident : public Message {
    Ident(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_IDENT; }

	value<uint8_t> version;
    MultiType type;
	value<uint8_t> msp_version;
    std::set<Capability> capabilities;

    bool decode(ByteVector& data) {
        bool rc = true;
        
        rc &= data.unpack(version);
        rc &= data.unpack((uint8_t&)type);
        rc &= data.unpack(msp_version);
        uint32_t capability;
        rc &= data.unpack(capability);
        if (!rc) return false;
        capabilities.clear();
        if(capability & (1 << 0))
            capabilities.insert(Capability::BIND);
        if(capability & (1 << 2))
            capabilities.insert(Capability::DYNBAL);
        if(capability & (1 << 3))
            capabilities.insert(Capability::FLAP);
        if(capability & (1 << 4))
            capabilities.insert(Capability::NAVCAP);
        if(capability & (1 << 5))
            capabilities.insert(Capability::EXTAUX);
            
        return true;
    }

    bool has(const Capability &cap) const { return capabilities.count(cap); }

    bool hasBind() const { return has(Capability::BIND); }

    bool hasDynBal() const { return has(Capability::DYNBAL); }

    bool hasFlap() const { return has(Capability::FLAP); }
};

struct StatusBase {
    value<uint16_t>    cycle_time;   // in us
    value<uint16_t>    i2c_errors;
    std::set<Sensor> sensors;
    std::set<size_t> box_mode_flags;
	value<uint8_t>      current_profile;
    
    
    bool unpack_from(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(cycle_time);
        rc &= data.unpack(i2c_errors);
        
        // get sensors
        sensors.clear();
        uint16_t sensor;
        rc &= data.unpack(sensor);
        if(sensor & (1 << 0))
            sensors.insert(Sensor::Accelerometer);
        if(sensor & (1 << 1))
            sensors.insert(Sensor::Barometer);
        if(sensor & (1 << 2))
            sensors.insert(Sensor::Magnetometer);
        if(sensor & (1 << 3))
            sensors.insert(Sensor::GPS);
        if(sensor & (1 << 4))
            sensors.insert(Sensor::Sonar);
        if(sensor & (1 << 5))
            sensors.insert(Sensor::OpticalFlow);
        if(sensor & (1 << 6))
            sensors.insert(Sensor::Pitot);
        if(sensor & (1 << 15))
            sensors.insert(Sensor::GeneralHealth);

        // check active boxes
        box_mode_flags.clear();
        uint32_t flag;
        rc &= data.unpack(flag);
        for(size_t ibox(0); ibox < sizeof(flag)*CHAR_BIT; ibox++) {
            if(flag & (1 << ibox))
                box_mode_flags.insert(ibox);
        }

        rc &= data.unpack(current_profile);
        
        return rc;
    }
};

// MSP_STATUS: 101
struct Status : public StatusBase, public Message {
    Status(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_STATUS; }
    
    value<uint16_t>     avg_system_load_pct;
    value<uint16_t>     gyro_cycle_time;

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= StatusBase::unpack_from(data);
        
        if (fw_variant != FirmwareVariant::INAV) {
            rc &= data.unpack(avg_system_load_pct);
            rc &= data.unpack(gyro_cycle_time);
        }
        return rc;
    }

    bool hasAccelerometer() const { return sensors.count(Sensor::Accelerometer); }

    bool hasBarometer() const { return sensors.count(Sensor::Barometer); }

    bool hasMagnetometer() const { return sensors.count(Sensor::Magnetometer); }

    bool hasGPS() const { return sensors.count(Sensor::GPS); }

    bool hasSonar() const { return sensors.count(Sensor::Sonar); }
    
    bool hasOpticalFlow() const { return sensors.count(Sensor::OpticalFlow); }
    
    bool hasPitot() const { return sensors.count(Sensor::Pitot); }
    
    bool isHealthy() const { return sensors.count(Sensor::GeneralHealth); }
};

// MSP_RAW_IMU: 102
struct ImuRaw : public Message {
    ImuRaw(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_RAW_IMU; }

    std::array<int16_t, 3> acc;
    std::array<int16_t, 3> gyro;
    std::array<int16_t, 3> mag;

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(acc[0]);
        rc &= data.unpack(acc[1]);
        rc &= data.unpack(acc[2]);
        rc &= data.unpack(gyro[0]);
        rc &= data.unpack(gyro[1]);
        rc &= data.unpack(gyro[2]);
        rc &= data.unpack(mag[0]);
        rc &= data.unpack(mag[1]);
        rc &= data.unpack(mag[2]);
        return rc;
    }
};

// Imu in SI units
struct ImuSI {
    std::array<float, 3> acc;   // m/s^2
    std::array<float, 3> gyro;  // deg/s
    std::array<float, 3> mag;  // uT

    ImuSI(const ImuRaw &imu_raw,
          const float acc_1g,       // sensor value at 1g
          const float gyro_unit,    // resolution in 1/(deg/s)
          const float mag_gain,    // scale magnetic value to uT (micro Tesla)
          const float si_unit_1g    // acceleration at 1g (in m/s^2)
            )
    {
        acc = {{imu_raw.acc[0]/acc_1g*si_unit_1g,
                imu_raw.acc[1]/acc_1g*si_unit_1g,
                imu_raw.acc[2]/acc_1g*si_unit_1g}};

        gyro = {{imu_raw.gyro[0]*gyro_unit,
                 imu_raw.gyro[1]*gyro_unit,
                 imu_raw.gyro[2]*gyro_unit}};

        mag = {{imu_raw.mag[0]*mag_gain,
                imu_raw.mag[1]*mag_gain,
                imu_raw.mag[2]*mag_gain}};
    }
};

// MSP_SERVO: 103
struct Servo : public Message {
    Servo(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SERVO; }

    std::array<uint16_t,N_SERVO> servo;

    bool decode(ByteVector& data) {
        bool rc = true;
        for(auto s : servo)
            rc &= data.unpack(s);
        return rc;
    }
};

// MSP_MOTOR: 104
struct Motor : public Message {
    Motor(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_MOTOR; }

    std::array<uint16_t,N_MOTOR> motor;

    bool decode(ByteVector& data) {
        bool rc = true;
        for(auto m : motor)
            rc &= data.unpack(m);
        return rc;
    }
};

// MSP_RC: 105
struct Rc : public Message {
    Rc(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_RC; }

    std::vector<uint16_t> channels;

    bool decode(ByteVector& data) {
        channels.clear();
        bool rc = true;
        while (rc) {
            uint16_t rc_data;
            rc &= data.unpack(rc_data);
            if (rc) channels.push_back(rc_data);
        }
        return !channels.empty();
    }
};

// MSP_RAW_GPS: 106
struct RawGPS : public Message {
    RawGPS(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_RAW_GPS; }

    value<uint8_t> fix;
    value<uint8_t> numSat;
    value<uint32_t> lat;
    value<uint32_t> lon;
    value<uint16_t> altitude;
    value<uint16_t> ground_speed;
    value<uint16_t> ground_course;
    bool hdop_set;
    value<uint16_t> hdop;

    bool decode(ByteVector& data) {
        bool rc = true;
        hdop_set = false;
        rc &= data.unpack(fix);
        rc &= data.unpack(numSat);
        rc &= data.unpack(lat);
        rc &= data.unpack(lon);
        rc &= data.unpack(altitude);
        rc &= data.unpack(ground_speed);
        rc &= data.unpack(ground_course);
        if (data.unpacking_remaining()) {
            hdop_set = true;
            rc &= data.unpack(hdop);
        }
        return rc;
    }
};

// MSP_COMP_GPS: 107
struct CompGPS : public Message {
    CompGPS(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_COMP_GPS; }

    value<uint16_t> distanceToHome;    // meter
    value<uint16_t> directionToHome;   // degree
    value<uint8_t> update;

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(distanceToHome);
        rc &= data.unpack(directionToHome);
        rc &= data.unpack(update);
        return rc;
    }
};

//TODO validate units
// MSP_ATTITUDE: 108
struct Attitude : public Message {
    Attitude(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_ATTITUDE; }

    int16_t roll;        // degree
    int16_t pitch;        // degree
    int16_t yaw;    // degree

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(roll);
        rc &= data.unpack(pitch);
        rc &= data.unpack(yaw);
        return rc;
    }
};

//TODO validate units
// MSP_ALTITUDE: 109
struct Altitude : public Message {
    Altitude(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_ALTITUDE; }

    value<float> altitude; // m
    value<float> vario;    // m/s
    bool baro_altitude_set;
    value<float> baro_altitude;

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack<uint32_t>(altitude,0.01);
        rc &= data.unpack<uint16_t>(vario,0.01);
        if (data.unpacking_remaining()) {
            baro_altitude_set = true;
            rc &= data.unpack<uint32_t>(baro_altitude,0.01);
        }
        return rc;
    }
};

//TODO check amperage units
// MSP_ANALOG: 110
struct Analog : public Message {
    Analog(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_ANALOG; }

    float	vbat;           // Volt
    float	powerMeterSum;  // Ah
	uint16_t	rssi;  // Received Signal Strength Indication [0; 1023]
    float	amperage;       // Ampere

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack<uint8_t>(vbat,0.1);
        rc &= data.unpack<uint16_t>(powerMeterSum,0.001);
        rc &= data.unpack(rssi);
        rc &= data.unpack<int8_t>(amperage,0.1);
        return rc;
    }
};

struct RcTuningSettings {
    // RPY sequence
    std::array<uint8_t,3> rates;
    std::array<uint8_t,3> rcRates;
    std::array<uint8_t,3> rcExpo;
    
    value<uint8_t> dynamic_throttle_pid;
    value<uint8_t> throttle_mid;
    value<uint8_t> throttle_expo;
    value<uint16_t> tpa_breakpoint;
    
    bool extended_contents;
};

//Differences between iNav and BF/CF
// MSP_RC_TUNING: 111
struct RcTuning : public RcTuningSettings, public Message {
    RcTuning(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_RC_TUNING; }

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(rcRates[0]);
        rc &= data.unpack(rcExpo[0]);
        for (size_t i = 0; i < 3; ++i) {
            rc &= data.unpack(rates[0]);
        }
        rc &= data.unpack(dynamic_throttle_pid);
        rc &= data.unpack(throttle_mid);
        rc &= data.unpack(throttle_expo);
        rc &= data.unpack(tpa_breakpoint);
        rc &= data.unpack(rcExpo[2]);
        rc &= data.unpack(rcRates[2]);
        rc &= data.unpack(rcRates[1]);
        rc &= data.unpack(rcExpo[1]);
        return rc;
    }
};

// PID struct for messages 112 and 204
struct PidTerms {
    uint8_t P;
    uint8_t I;
    uint8_t D;

    bool unpack_from(ByteVector &data) {
        bool rc = true;
        rc &= data.unpack(P);
        rc &= data.unpack(I);
        rc &= data.unpack(D);
        return rc;
    }
    
    void pack_into(ByteVector &data) const {
        data.pack(P);
        data.pack(I);
        data.pack(D);
    }
};


//TODO: revisit
// MSP_PID: 112
struct Pid : public Message {
    Pid(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_PID; }
    
    std::array<value<PidTerms>,static_cast<uint8_t>(PID_Element::PID_ITEM_COUNT)> entry;

    bool decode(ByteVector& data) {
        bool rc = true;
        for (uint8_t i = 0; i < static_cast<uint8_t>(PID_Element::PID_ITEM_COUNT); ++i) {
            rc &= data.unpack(entry[i]);
        }
        return rc;
    }
};

// MSP_ACTIVEBOXES: 113
struct ActiveBoxes : public Message {
    ActiveBoxes(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_ACTIVEBOXES; }

    // box activation pattern
    std::vector<std::array<std::set<SwitchPosition>,NAUX>> box_pattern;

    bool decode(ByteVector& data) {
        box_pattern.clear();
        bool rc = true;
        while (rc && data.unpacking_remaining()>1) {
            value<uint16_t> box_conf;
            rc &= data.unpack(box_conf);
            std::array<std::set<SwitchPosition>,NAUX> aux_sp;
            for(size_t iaux(0); iaux<NAUX; iaux++) {
                for(size_t ip(0); ip<3; ip++) {
                    if(box_conf() & (1<<(iaux*3+ip)))
                        aux_sp[iaux].insert(SwitchPosition(ip));
                } // each position (L,M,H)
            } // each aux switch
            box_pattern.push_back(aux_sp);
        }// each box
        return rc;
    }
};

struct MiscSettings {
    value<uint16_t> mid_rc;
    value<uint16_t> min_throttle;
    value<uint16_t> max_throttle;
    value<uint16_t> min_command;
    value<uint16_t> failsafe_throttle;
    value<uint8_t> gps_provider;
    value<uint8_t> gps_baudrate;
    value<uint8_t> gps_ubx_sbas;
    value<uint8_t> multiwii_current_meter_output;
    value<uint8_t> rssi_channel;
    value<uint8_t> reserved;
    value<float> mag_declination; // degree
    value<float> voltage_scale, cell_min, cell_max, cell_warning;

};

// MSP_MISC: 114
struct Misc : public MiscSettings, public Message {
    Misc(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_MISC; }

	
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(mid_rc);
        rc &= data.unpack(min_throttle);
        rc &= data.unpack(max_throttle);
        rc &= data.unpack(min_command);
        rc &= data.unpack(failsafe_throttle);
        rc &= data.unpack(gps_provider);
        rc &= data.unpack(gps_baudrate);
        rc &= data.unpack(gps_ubx_sbas);
        rc &= data.unpack(multiwii_current_meter_output);
        rc &= data.unpack(rssi_channel);
        rc &= data.unpack(reserved);
        
        rc &= data.unpack<uint16_t>(mag_declination,0.1);
        rc &= data.unpack<uint8_t>(voltage_scale,0.1);
        rc &= data.unpack<uint8_t>(cell_min,0.1);
        rc &= data.unpack<uint8_t>(cell_max,0.1);
        rc &= data.unpack<uint8_t>(cell_warning,0.1);
        return rc;
    }
};

// MSP_MOTOR_PINS: 115
struct MotorPins : public Message {
    MotorPins(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_MOTOR_PINS; }

    value<uint8_t> pwm_pin[N_MOTOR];

    bool decode(ByteVector& data) {
        bool rc = true;
        for (auto pin : pwm_pin)
            rc &= data.unpack(pin);
        return rc;
    }
};

// MSP_BOXNAMES: 116
struct BoxNames : public Message {
    BoxNames(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_BOXNAMES; }

    std::vector<std::string> box_names;

    bool decode(ByteVector& data) {
        box_names.clear();
        bool rc = true;
        std::string str;
        rc &= data.unpack(str);
        std::stringstream ss(str);
        std::string bname;
        while(getline(ss, bname, ';')) {
            box_names.push_back(bname);
        }
        return rc;
    }
};

// MSP_PIDNAMES: 117
struct PidNames : public Message {
    PidNames(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_PIDNAMES; }

    std::vector<std::string> pid_names;

    bool decode(ByteVector& data) {
        pid_names.clear();
        bool rc = true;
        std::string str;
        rc &= data.unpack(str);
        std::stringstream ss(str);
        std::string pname;
        while(getline(ss, pname, ';')) {
            pid_names.push_back(pname);
        }
        return rc;
    }
};

// MSP_WP: 118
struct WayPoint : public Message {
    WayPoint(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_WP; }

    value<uint8_t> wp_no;
    value<uint32_t> lat;
    value<uint32_t> lon;
    value<uint32_t> altHold;
    value<uint16_t> heading;
    value<uint16_t> staytime;
    value<uint8_t> navflag;

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(wp_no);
        rc &= data.unpack(lat);
        rc &= data.unpack(lon);
        rc &= data.unpack(altHold);
        rc &= data.unpack(heading);
        rc &= data.unpack(staytime);
        rc &= data.unpack(navflag);
        return rc;
    }
};

// MSP_BOXIDS: 119
struct BoxIds : public Message {
    BoxIds(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_BOXIDS; }

    ByteVector box_ids;

    bool decode(ByteVector& data) {
        box_ids.clear();

        for(uint8_t bi : data)
            box_ids.push_back(bi);;
            
        return true;
    }
};

struct ServoConfRange {
    value<uint16_t> min;
    value<uint16_t> max;
    value<uint16_t> middle;
    value<uint8_t> rate;
};

// MSP_SERVO_CONF: 120
struct ServoConf : public Message {
    ServoConf(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SERVO_CONF; }

    ServoConfRange servo_conf[N_SERVO];

    bool decode(ByteVector& data) {
        bool rc = true;
        for(size_t i(0); i<N_SERVO; i++) {
            rc &= data.unpack(servo_conf[i].min);
            rc &= data.unpack(servo_conf[i].max);
            rc &= data.unpack(servo_conf[i].middle);
            rc &= data.unpack(servo_conf[i].rate);
        }
        return rc;
    }
};

// MSP_NAV_STATUS: 121
struct NavStatus: public Message {
    NavStatus(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_NAV_STATUS; }

    value<uint8_t> GPS_mode;
    value<uint8_t> NAV_state;
    value<uint8_t> mission_action;
    value<uint8_t> mission_number;
    value<uint8_t> NAV_error;
    int16_t target_bearing; // degrees

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(GPS_mode);
        rc &= data.unpack(NAV_state);
        rc &= data.unpack(mission_action);
        rc &= data.unpack(mission_number);
        rc &= data.unpack(NAV_error);
        rc &= data.unpack(target_bearing);
        return rc;
        
    }
};


struct GpsConf {
  uint8_t filtering;
  uint8_t lead_filter;
  uint8_t dont_reset_home_at_arm;
  uint8_t nav_controls_heading;

  uint8_t nav_tail_first;
  uint8_t nav_rth_takeoff_heading;
  uint8_t slow_nav;
  uint8_t wait_for_rth_alt;

  uint8_t ignore_throttle;
  uint8_t takeover_baro;

  uint16_t wp_radius;           // in cm
  uint16_t safe_wp_distance;    // in meter
  uint16_t nav_max_altitude;    // in meter
  uint16_t nav_speed_max;       // in cm/s
  uint16_t nav_speed_min;       // in cm/s

  uint8_t  crosstrack_gain;     // * 100 (0-2.56)
  uint16_t nav_bank_max;        // degree * 100; (3000 default)
  uint16_t rth_altitude;        // in meter
  uint8_t  land_speed;          // between 50 and 255 (100 approx = 50cm/sec)
  uint16_t fence;               // fence control in meters

  uint8_t  max_wp_number;

  uint8_t  checksum;
};

// MSP_NAV_CONFIG: 122
struct NavConfig: public Message, public GpsConf {
    NavConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_NAV_CONFIG; }


    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(filtering);
        rc &= data.unpack(lead_filter);
        rc &= data.unpack(dont_reset_home_at_arm);
        rc &= data.unpack(nav_controls_heading);
        
        rc &= data.unpack(nav_tail_first);
        rc &= data.unpack(nav_rth_takeoff_heading);
        rc &= data.unpack(slow_nav);
        rc &= data.unpack(wait_for_rth_alt);
        
        rc &= data.unpack(ignore_throttle);
        rc &= data.unpack(takeover_baro);
        
        rc &= data.unpack(wp_radius);
        rc &= data.unpack(safe_wp_distance);
        rc &= data.unpack(nav_max_altitude);
        rc &= data.unpack(nav_speed_max);
        rc &= data.unpack(nav_speed_min);
        
        rc &= data.unpack(crosstrack_gain);
        rc &= data.unpack(nav_bank_max);
        rc &= data.unpack(rth_altitude);
        rc &= data.unpack(land_speed);
        rc &= data.unpack(fence);
        
        rc &= data.unpack(max_wp_number);
        rc &= data.unpack(checksum);
        
        return rc;
    }
};


struct Motor3dConfig: public Message {
    Motor3dConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_MOTOR_3D_CONFIG; }

    value<uint16_t> deadband3d_low;
    value<uint16_t> deadband3d_high;
    value<uint16_t> neutral_3d;

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(deadband3d_low);
        rc &= data.unpack(deadband3d_high);
        rc &= data.unpack(neutral_3d);
        return rc;
    }
};

struct RcDeadbandSettings {
    value<uint8_t> deadband;
    value<uint8_t> yaw_deadband;
    value<uint8_t> alt_hold_deadband;
    value<uint16_t> deadband3d_throttle;
};

struct RcDeadband: public RcDeadbandSettings, public Message {
    RcDeadband(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_RC_DEADBAND; }

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(deadband);
        rc &= data.unpack(yaw_deadband);
        rc &= data.unpack(alt_hold_deadband);
        rc &= data.unpack(deadband3d_throttle);
        return rc;
    }
};

struct SensorAlignmentSettings {
    value<uint8_t> gyro_align;
    value<uint8_t> acc_align;
    value<uint8_t> mag_align;
};

struct SensorAlignment: public SensorAlignmentSettings, public Message {
    SensorAlignment(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SENSOR_ALIGNMENT; }

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(gyro_align);
        rc &= data.unpack(acc_align);
        rc &= data.unpack(mag_align);
        return rc;
    }
};


struct LedStripModecolor: public Message {
    LedStripModecolor(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_LED_STRIP_MODECOLOR; }

    std::array<std::array<uint8_t,LED_DIRECTION_COUNT>,LED_MODE_COUNT> mode_colors;
    std::array<uint8_t,LED_SPECIAL_COLOR_COUNT> special_colors;
    value<uint8_t> led_aux_channel;
    value<uint8_t> reserved;
    value<uint8_t> led_strip_aux_channel;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        for (auto mode : mode_colors) {
            for (auto color : mode) {
                rc &= data.unpack(color);
            }
        }
        for (auto special : special_colors) {
            rc &= data.unpack(special);
        }
        
        rc &= data.unpack(led_aux_channel);
        rc &= data.unpack(reserved);
        rc &= data.unpack(led_strip_aux_channel);
        return rc;
    }
};


struct VoltageMeter {
    value<uint8_t> id;
    value<uint8_t> val;
};

struct VoltageMeters: public Message {
    VoltageMeters(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_VOLTAGE_METERS; }

    std::vector<VoltageMeter> meters;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        for (auto meter : meters) {
            rc &= data.unpack(meter.id);
            rc &= data.unpack(meter.val);
        }
        return rc;
    }
};

struct CurrentMeter {
    value<uint8_t> id;
    value<uint16_t> mAh_drawn;
    value<uint16_t> mA;
};

struct CurrentMeters: public Message {
    CurrentMeters(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_CURRENT_METERS; }

    std::vector<CurrentMeter> meters;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        for (auto meter : meters) {
            rc &= data.unpack(meter.id);
            rc &= data.unpack(meter.mAh_drawn);
            rc &= data.unpack(meter.mA);
        }
        return rc;
    }
};


struct BatteryState: public Message {
    BatteryState(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_BATTERY_STATE; }

    value<uint8_t> cell_count;
    value<uint16_t> capacity_mAh;
    value<uint8_t> voltage;
    value<uint16_t> mAh_drawn;
    value<uint16_t> current;
    value<uint8_t> state;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(cell_count);
        rc &= data.unpack(capacity_mAh);
        rc &= data.unpack(voltage);
        rc &= data.unpack(mAh_drawn);
        rc &= data.unpack(current);
        rc &= data.unpack(state);
        return rc;
    }
};


struct MotorConfigSettings {
    value<uint16_t> min_throttle;
    value<uint16_t> max_throttle;
    value<uint16_t> min_command;
};

struct MotorConfig: public MotorConfigSettings, public Message {
    MotorConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_MOTOR_CONFIG; }

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(min_throttle);
        rc &= data.unpack(max_throttle);
        rc &= data.unpack(min_command);
        return rc;
    }
};

struct GpsConfigSettings {
    value<uint8_t> provider;
    value<uint8_t> sbas_mode;
    value<uint8_t> auto_config;
    value<uint8_t> auto_baud;
};

struct GpsConfig: public GpsConfigSettings, public Message {
    GpsConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_GPS_CONFIG; }

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(provider);
        rc &= data.unpack(sbas_mode);
        rc &= data.unpack(auto_config);
        rc &= data.unpack(auto_baud);
        return rc;
    }
};

struct CompassConfig: public Message {
    CompassConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_COMPASS_CONFIG; }

    value<uint16_t> mag_declination;
    
    bool decode(ByteVector& data) {
        return data.unpack(mag_declination);
    }
};

struct EscData {
    value<uint8_t> temperature;
    value<uint16_t> rpm;
};

struct EscSensorData: public Message {
    EscSensorData(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_ESC_SENSOR_DATA; }

    value<uint8_t> motor_count;
    std::vector<EscData> esc_data;
    
    bool decode(ByteVector& data) {
        if (data.empty()) {
            motor_count = 0;
            return true;
        }
        bool rc = true;
        rc &= data.unpack(motor_count);
        for (int i = 0; i < motor_count(); ++i) {
            EscData esc;
            rc &= data.unpack(esc.temperature);
            rc &= data.unpack(esc.rpm);
            esc_data.push_back(esc);
        }
        return rc;
    }
};


struct StatusEx: public StatusBase, public Message {
    StatusEx(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_STATUS_EX; }
    
    //bf/cf fields
    value<uint8_t> max_profiles;
    value<uint8_t> control_rate_profile;
    //iNav fields
    value<uint16_t> avg_system_load_pct;
    value<uint16_t> arming_flags;
    value<uint8_t> acc_calibration_axis_flags;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= StatusBase::unpack_from(data);
        if (fw_variant == FirmwareVariant::INAV) {
            rc &= data.unpack(avg_system_load_pct);
            rc &= data.unpack(arming_flags);
            rc &= data.unpack(acc_calibration_axis_flags);
        } else {
            
            rc &= data.unpack(max_profiles);
            rc &= data.unpack(control_rate_profile);
        }
        return rc;
    }
    
};


struct SensorStatus: public Message {
    SensorStatus(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SENSOR_STATUS; }
    
    value<uint8_t> hardware_healthy;
    value<uint8_t> hw_gyro_status;
    value<uint8_t> hw_acc_status;
    value<uint8_t> hw_compass_status;
    value<uint8_t> hw_baro_status;
    value<uint8_t> hw_gps_status;
    value<uint8_t> hw_rangefinder_status;
    value<uint8_t> hw_pitometer_status;
    value<uint8_t> hw_optical_flow_status;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(hardware_healthy);
        rc &= data.unpack(hw_gyro_status);
        rc &= data.unpack(hw_acc_status);
        rc &= data.unpack(hw_compass_status);
        rc &= data.unpack(hw_baro_status);
        rc &= data.unpack(hw_gps_status);
        rc &= data.unpack(hw_rangefinder_status);
        rc &= data.unpack(hw_pitometer_status);
        rc &= data.unpack(hw_optical_flow_status);
        return rc;
    }
};



// MSP_UID                         = 160,
struct Uid: public Message {
    Uid(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_UID; }
    
    value<uint32_t> u_id_0;
    value<uint32_t> u_id_1;
    value<uint32_t> u_id_2;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(u_id_0);
        rc &= data.unpack(u_id_1);
        rc &= data.unpack(u_id_2);
        return rc;
    }
    
};

struct GpsSvInfoSettings {
    uint8_t channel;
    uint8_t sv_id;
    uint8_t quality;
    uint8_t cno;
};

// MSP_GPSSVINFO                   = 164,
struct GpsSvInfo: public Message {
    GpsSvInfo(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_GPSSVINFO; }
    
    value<uint8_t> hdop;
    
    value<uint8_t> channel_count;
    std::vector<GpsSvInfoSettings> sv_info;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        if (fw_variant == FirmwareVariant::INAV) {
            rc &= data.consume(4);
            rc &= data.unpack(hdop);
        }
        else {
            rc &= data.unpack(channel_count);
            for (uint8_t i = 0; i < channel_count(); ++i) {
                GpsSvInfoSettings tmp;
                rc &= data.unpack(tmp.channel);
                rc &= data.unpack(tmp.sv_id);
                rc &= data.unpack(tmp.quality);
                rc &= data.unpack(tmp.cno);
            }
        }
        return rc;
    }
};

// MSP_GPSSTATISTICS               = 166,
struct GpsStatistics: public Message {
    GpsStatistics(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_GPSSTATISTICS; }
    
    value<uint16_t> last_msg_dt;
    value<uint32_t> errors;
    value<uint32_t> timeouts;
    value<uint32_t> packet_count;
    value<uint16_t> hdop;
    value<uint16_t> eph;
    value<uint16_t> epv;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(last_msg_dt);
        rc &= data.unpack(errors);
        rc &= data.unpack(timeouts);
        rc &= data.unpack(packet_count);
        rc &= data.unpack(hdop);
        rc &= data.unpack(eph);
        rc &= data.unpack(epv);
        return rc;
    }
};

//no actual implementations
// MSP_OSD_VIDEO_CONFIG            = 180,
struct OsdVideoConfig : public Message {
    OsdVideoConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_OSD_VIDEO_CONFIG; }
    
    bool decode(ByteVector& data) {
        return false;
    }
};

// MSP_SET_OSD_VIDEO_CONFIG        = 181,
struct SetOsdVideoConfig : public Message {
    SetOsdVideoConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_OSD_VIDEO_CONFIG; }
};

// MSP_DISPLAYPORT                 = 182,
struct Displayport : public Message {
    Displayport(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_DISPLAYPORT; }
    
    value<uint8_t> sub_cmd;
    value<uint8_t> row;
    value<uint8_t> col;
    value<std::string> str;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(sub_cmd);
        if (sub_cmd() == 3) {
            data.pack(row);
            data.pack(col);
            data.pack(uint8_t(0));
            data.pack(uint8_t(str().size()));
            data.pack(str);
        }
        return data;
    }
};

// Not available in iNav (183-185)
// MSP_COPY_PROFILE                = 183,
struct CopyProfile : public Message {
    CopyProfile(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_COPY_PROFILE; }
    
    value<uint8_t> profile_type;
    value<uint8_t> dest_profile_idx;
    value<uint8_t> src_profile_idx;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(profile_type);
        data.pack(dest_profile_idx);
        data.pack(src_profile_idx);
        return data;
    }
};

struct BeeperConfigSettings {
    value<uint32_t> beeper_off_mask;
    value<uint8_t> beacon_tone;
};

// MSP_BEEPER_CONFIG               = 184,
struct BeeperConfig : public BeeperConfigSettings, public Message {
    BeeperConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_BEEPER_CONFIG; }
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(beeper_off_mask);
        rc &= data.unpack(beacon_tone);
        return rc;
    }
};

// MSP_SET_BEEPER_CONFIG           = 185,
struct SetBeeperConfig : public BeeperConfigSettings, public Message {
    SetBeeperConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_BEEPER_CONFIG; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(beeper_off_mask);
        if (beacon_tone.set()) {
            data.pack(beacon_tone);
        }
        return data;
    }
};

// MSP_SET_TX_INFO                 = 186, // in message           Used to send runtime information from TX lua scripts to the firmware
struct SetTxInfo : public Message {
    SetTxInfo(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_TX_INFO; }
    
    value<uint8_t> rssi;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(rssi);
        return data;
    }
};

// MSP_TX_INFO                     = 187, // out message          Used by TX lua scripts to read information from the firmware
struct TxInfo : public Message {
    TxInfo(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_TX_INFO; }
    
    value<uint8_t> rssi_source;
    value<uint8_t> rtc_date_time_status;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(rssi_source);
        rc &= data.unpack(rtc_date_time_status);
        return rc;
    }
};

/////////////////////////////////////////////////////////////////////
/// Response (2xx)

// MSP_SET_RAW_RC: 200
// This message is accepted but ignored on betaflight 3.0.1 onwards
// if "USE_RX_MSP" is not defined for the target. In this case, you can manually
// add "#define USE_RX_MSP" to your 'target.h'.
struct SetRawRc : public Message {
    SetRawRc(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_RAW_RC; }

    std::vector<uint16_t> channels;

    ByteVector encode() const {
        ByteVector data;
        for(const uint16_t c : channels) {
            data.pack(c);
        }
        return data;
    }
};

// MSP_SET_RAW_GPS: 201
struct SetRawGPS : public Message {
    SetRawGPS(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_RAW_GPS; }

    value<uint8_t> fix;
    value<uint8_t> numSat;
    value<uint32_t> lat;
    value<uint32_t> lon;
    value<uint16_t> altitude;
    value<uint16_t> speed;

    ByteVector encode() const {
        ByteVector data;
        data.pack(fix);
        data.pack(numSat);
        data.pack(lat);
        data.pack(lon);
        data.pack(altitude);
        data.pack(speed);
        assert(data.size()==14);
        return data;
    }
};

// MSP_SET_PID: 202,
struct SetPid : public Message {
    SetPid(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_PID; }
    
    std::array<value<PidTerms>,static_cast<uint8_t>(PID_Element::PID_ITEM_COUNT)> entry;

    ByteVector encode() const {
        ByteVector data;
        for (uint8_t i = 0; i < static_cast<uint8_t>(PID_Element::PID_ITEM_COUNT); ++i) {
            data.pack(entry[i]);
        }
        return data;
    }
    
};


//Depricated - no examples
// MSP_SET_BOX: 203,
struct SetBox : public Message {
    SetBox(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_BOX; }
};


//Differences between iNav and BF/CF - this is BF/CF variant
// MSP_SET_RC_TUNING: 204
struct SetRcTuning : public RcTuningSettings, public Message {
    SetRcTuning(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_RC_TUNING; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(rcRates[0]);
        data.pack(rcExpo[0]);
        for (auto r : rates) {
            data.pack(r);
        }
        data.pack(dynamic_throttle_pid);
        data.pack(throttle_mid);
        data.pack(throttle_expo);
        data.pack(tpa_breakpoint);
        data.pack(rcExpo[2]);
        data.pack(rcRates[2]);
        data.pack(rcRates[1]);
        data.pack(rcExpo[1]);
        return data;
    }
};

// MSP_ACC_CALIBRATION: 205
struct AccCalibration : public Message {
    AccCalibration(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_ACC_CALIBRATION; }
};

// MSP_MAG_CALIBRATION: 206
struct MagCalibration : public Message {
    MagCalibration(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_MAG_CALIBRATION; }
};

//MSP_SET_MISC: 207
struct SetMisc : public MiscSettings, public Message {
    SetMisc(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_MISC; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(mid_rc);
        data.pack(min_throttle);
        data.pack(max_throttle);
        data.pack(failsafe_throttle);
        data.pack(gps_provider);
        data.pack(gps_baudrate);
        data.pack(gps_ubx_sbas);
        data.pack(multiwii_current_meter_output);
        data.pack(rssi_channel);
        data.pack(reserved);
        data.pack<uint16_t>(mag_declination,10.f);
        data.pack<uint8_t>(voltage_scale,10.f);
        data.pack<uint8_t>(cell_min,10.f);
        data.pack<uint8_t>(cell_max,10.f);
        data.pack<uint8_t>(cell_warning,10.f);
        return data;
    }
};

// MSP_RESET_CONF: 208
struct ResetConfig : public Message {
    ResetConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_RESET_CONF; }
};

//MSP_SET_WP: 209
struct SetWp : public Message {
    SetWp(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_WP; }
    
    value<uint8_t> wp_no;
    value<uint32_t> lat;
    value<uint32_t> lon;
    value<uint32_t> alt;
    
    value<uint16_t> p1;
    value<uint16_t> p2;
    value<uint16_t> p3;
    value<uint8_t> nav_flag;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(wp_no);
        data.pack(lat);
        data.pack(lon);
        data.pack(alt);
        data.pack(p1);
        data.pack(p2);
        if (fw_variant == FirmwareVariant::INAV) {
            data.pack(p3);
        }
        data.pack(nav_flag);
        return data;
    }
};


// MSP_SELECT_SETTING: 210
struct SelectSetting : public Message {
    SelectSetting(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SELECT_SETTING; }

	uint8_t current_setting;

    ByteVector encode() const {
        ByteVector data;
        data.pack(current_setting);
        return data;
    }
};

// MSP_SET_HEADING: 211
struct SetHeading : public Message {
    SetHeading(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_HEADING; }

    int16_t heading;

    ByteVector encode() const {
        ByteVector data;
        data.pack(heading);
        assert(data.size()==2);
        return data;
    }
};

//MSP_SET_SERVO_CONF: 212
struct SetServoConf : public Message {
    SetServoConf(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_SERVO_CONF; }

    value<uint8_t> servo_idx;
    value<uint16_t> min;
    value<uint16_t> max;
    value<uint16_t> middle;
    value<uint8_t> rate;
    
    value<uint8_t> forward_from_channel;
    value<uint32_t> reversed_sources;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(servo_idx);
        data.pack(min);
        data.pack(max);
        data.pack(middle);
        data.pack(rate);
        if (fw_variant == FirmwareVariant::INAV) {
            uint8_t tmp;
            data.pack(tmp);
            data.pack(tmp);
        }
        data.pack(forward_from_channel);
        data.pack(reversed_sources);
        return data;
    };
};

// MSP_SET_MOTOR: 214
struct SetMotor : public Message {
    SetMotor(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_MOTOR; }

    std::array<uint16_t,N_MOTOR> motor;

    ByteVector encode() const {
        ByteVector data;
        for(size_t i(0); i<N_MOTOR; i++)
            data.pack(motor[i]);
        assert(data.size()==N_MOTOR*2);
        return data;
    }
};


// MSP_SET_NAV_CONFIG              = 215
struct SetNavConfig : public Message {
    SetNavConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_NAV_CONFIG; }
};

// MSP_SET_MOTOR_3D_CONF           = 217
struct SetMotor3dConf : public Message {
    SetMotor3dConf(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_MOTOR_3D_CONF; }
    
    value<uint16_t> deadband3d_low;
    value<uint16_t> deadband3d_high;
    value<uint16_t> neutral_3d;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(deadband3d_low);
        data.pack(deadband3d_high);
        data.pack(neutral_3d);
        return data;
    }
};


// MSP_SET_RC_DEADBAND             = 218
struct SetRcDeadband : public RcDeadbandSettings, public Message {
    SetRcDeadband(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_RC_DEADBAND; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(deadband);
        data.pack(yaw_deadband);
        data.pack(alt_hold_deadband);
        data.pack(deadband3d_throttle);
        return data;
    }
};

// MSP_SET_RESET_CURR_PID          = 219
struct SetResetCurrPid: public Message {
    SetResetCurrPid(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_RESET_CURR_PID; }
};

// MSP_SET_SENSOR_ALIGNMENT        = 220
struct SetSensorAlignment: public SensorAlignmentSettings, public Message {
    SetSensorAlignment(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_SENSOR_ALIGNMENT; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(gyro_align);
        data.pack(acc_align);
        data.pack(mag_align);
        return data;
    }
};

// MSP_SET_LED_STRIP_MODECOLOR     = 221
struct SetLedStripModecolor: public SensorAlignmentSettings, public Message {
    SetLedStripModecolor(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_LED_STRIP_MODECOLOR; }
    
    value<uint8_t> mode_idx;
    value<uint8_t> fun_idx;
    value<uint8_t> color;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(mode_idx);
        data.pack(fun_idx);
        data.pack(color);
        return data;
    }
};

//Not available in iNav (222-224)
// MSP_SET_MOTOR_CONFIG            = 222    //out message         Motor configuration (min/max throttle, etc)
struct SetMotorConfig: public MotorConfigSettings, public Message {
    SetMotorConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_MOTOR_CONFIG; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(min_throttle);
        data.pack(max_throttle);
        data.pack(min_command);
        return data;
    }
};

// MSP_SET_GPS_CONFIG              = 223    //out message         GPS configuration
struct SetGpsConfig: public GpsConfigSettings, public Message {
    SetGpsConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_GPS_CONFIG; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(provider);
        data.pack(sbas_mode);
        data.pack(auto_config);
        data.pack(auto_baud);
        return data;
    }
};

// MSP_SET_COMPASS_CONFIG          = 224    //out message         Compass configuration
struct SetCompassConfig: public Message {
    SetCompassConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_GPS_CONFIG; }
    
    value<float> mag_declination;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack<uint16_t>(mag_declination,10);
        return data;
    }
};

struct AccTrimSettings {
    value<uint16_t> pitch;
    value<uint16_t> roll;
};

// MSP_SET_ACC_TRIM                = 239    //in message          set acc angle trim values
struct SetAccTrim: public AccTrimSettings, public Message {
    SetAccTrim(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_ACC_TRIM; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(pitch);
        data.pack(roll);
        return data;
    }
};

// MSP_ACC_TRIM                    = 240    //out message         get acc angle trim values
struct AccTrim: public AccTrimSettings, public Message {
    AccTrim(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_ACC_TRIM; }
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(pitch);
        rc &= data.unpack(roll);
        return rc;
    }
    
};

struct ServoMixRule {
    uint8_t target_channel;
    uint8_t input_source;
    uint8_t rate;
    uint8_t speed;
    uint8_t min;
    uint8_t max;
    uint8_t box;
    
    bool unpack_from(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(target_channel);
        rc &= data.unpack(input_source);
        rc &= data.unpack(rate);
        rc &= data.unpack(speed);
        rc &= data.unpack(min);
        rc &= data.unpack(max);
        rc &= data.unpack(box);
        return rc;
    }
    
    void pack_into(ByteVector& data) const {
        data.pack(target_channel);
        data.pack(input_source);
        data.pack(rate);
        data.pack(speed);
        data.pack(min);
        data.pack(max);
        data.pack(box);
    }
};

// MSP_SERVO_MIX_RULES             = 241    //out message         Returns servo mixer configuration
struct ServoMixRules: public Message {
    ServoMixRules(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SERVO_MIX_RULES; }
    
    std::vector<value<ServoMixRule>>rules;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        while (data.unpacking_remaining()) {
            value<ServoMixRule> rule;
            rc &= data.unpack(rule);
            if (rc) rules.push_back(rule);
            else break;
        }
        return rc;
    }
    
};

// MSP_SET_SERVO_MIX_RULE          = 242    //in message          Sets servo mixer configuration
struct SetServoMixRule: public Message {
    SetServoMixRule(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SERVO_MIX_RULES; }
    
    value<ServoMixRule> rule;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(rule);
        return data;
    }
};

//not used in CF, BF, iNav
// MSP_PASSTHROUGH_SERIAL          = 244
struct PassthroughSerial : public Message {
    PassthroughSerial(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_PASSTHROUGH_SERIAL; }
    
};

// MSP_SET_4WAY_IF                 = 245    //in message          Sets 4way interface
struct Set4WayIF : public Message {
    Set4WayIF(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_4WAY_IF; }
    
    value<uint8_t> esc_mode;
    value<uint8_t> esc_port_index;
    
    value<uint8_t> esc_count;
    
    ByteVector encode() const {
        ByteVector data;
        if (esc_mode.set()) {
            data.pack(esc_mode);
            data.pack(esc_port_index);
        }
        return data;
    }
    
    bool decode(ByteVector& data) {
        return data.unpack(esc_count);
    }
};

struct RtcVals {
    value<uint32_t> secs;
    value<uint16_t> millis;
};

// MSP_SET_RTC                     = 246    //in message          Sets the RTC clock
struct SetRtc: public RtcVals, public Message {
    SetRtc(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_SET_RTC; }
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(secs);
        data.pack(millis);
        return data;
    }
};

// MSP_RTC                         = 247    //out message         Gets the RTC clock
struct Rtc: public RtcVals, public Message {
    Rtc(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_RTC; }
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(secs);
        rc &= data.unpack(millis);
        return rc;
    }
};



// MSP_EEPROM_WRITE: 250
struct WriteEEPROM : public Message {
    WriteEEPROM(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_EEPROM_WRITE; }
};

// MSP_RESERVE_1: 251,    //reserved for system usage
struct Reserve1 : public Message {
    Reserve1(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_RESERVE_1; }
};

// MSP_RESERVE_2: 252,    //reserved for system usage
struct Reserve2 : public Message {
    Reserve2(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_RESERVE_2; }
};

// MSP_DEBUGMSG: 253
struct DebugMessage : public Message {
    DebugMessage(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_DEBUGMSG; }

    value<std::string> debug_msg;

    bool decode(ByteVector& data) {
        return data.unpack(debug_msg);
        
    }
};

// MSP_DEBUG: 254
struct Debug : public Message {
    Debug(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_DEBUG; }

    value<uint16_t> debug1;
    value<uint16_t> debug2;
    value<uint16_t> debug3;
    value<uint16_t> debug4;

    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(debug1);
        rc &= data.unpack(debug2);
        rc &= data.unpack(debug3);
        rc &= data.unpack(debug4);
        return rc;
    }
};

// MSP_V2_FRAME: 255
struct V2Frame : public Message {
    V2Frame(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP_V2_FRAME; }
};


//MSP2_COMMON_TZ                  = 0x1001,  //out message   Gets the TZ offset for the local time (returns: minutes(i16))
struct CommonTz : public Message {
    CommonTz(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP2_COMMON_TZ; }
    
    value<uint16_t> tz_offset;
    
    bool decode(ByteVector& data) {
        return data.unpack(tz_offset);
    }

};

//MSP2_COMMON_SET_TZ              = 0x1002,  //in message    Sets the TZ offset for the local time (args: minutes(i16))
struct CommonSetTz : public Message {
    CommonSetTz(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP2_COMMON_SET_TZ; }
    
    value<uint16_t> tz_offset;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(tz_offset);
        return data;
    }

};

//MSP2_COMMON_SETTING             = 0x1003,  //in/out message   Returns the value for a setting
struct CommonSetting : public Message {
    CommonSetting(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP2_COMMON_SETTING; }
    
    value<std::string> setting_name;
    value<uint8_t> uint8_val;
    value<int8_t> int8_val;
    value<uint16_t> uint16_val;
    value<int16_t> int16_val;
    value<uint32_t> uint32_val;
    value<float> float_val;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(setting_name);
        data.pack(uint8_t(0));
        return data;
    }
    //TODO
    bool decode(ByteVector& data) {
        return data.unpack(uint8_val);
    }

};

//MSP2_COMMON_SET_SETTING         = 0x1004,  //in message    Sets the value for a setting
struct CommonSetSetting : public Message {
    CommonSetSetting(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP2_COMMON_SET_SETTING; }
    
    value<std::string> setting_name;
    value<uint8_t> uint8_val;
    value<int8_t> int8_val;
    value<uint16_t> uint16_val;
    value<int16_t> int16_val;
    value<uint32_t> uint32_val;
    value<float> float_val;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(setting_name);
        if (uint8_val.set()) data.pack(uint8_val);
        else if (int8_val.set()) data.pack(int8_val);
        else if (uint16_val.set()) data.pack(uint16_val);
        else if (int16_val.set()) data.pack(int16_val);
        else if (uint32_val.set()) data.pack(uint32_val);
        else if (float_val.set()) data.pack(float_val);
        
        return data;
    }

};

struct MotorMixer {
    value<float> throttle;
    value<float> roll;
    value<float> pitch;
    value<float> yaw;
    
    bool unpack_from(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack<uint16_t>(throttle,1000.0);
        rc &= data.unpack<uint16_t>(roll,1000.0,1.0);
        rc &= data.unpack<uint16_t>(pitch,1000.0,1.0);
        rc &= data.unpack<uint16_t>(yaw,1000.0,1.0);
        return rc;
    }
    
    void pack_into(ByteVector& data) const {
        data.pack<uint16_t>(throttle,1000.0,1.0);
        data.pack<uint16_t>(roll,1000.0,1.0);
        data.pack<uint16_t>(pitch,1000.0,1.0);
        data.pack<uint16_t>(yaw,1000.0,1.0);
    }
};

//MSP2_COMMON_MOTOR_MIXER         = 0x1005,
struct CommonMotorMixer : public Message {
    CommonMotorMixer(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP2_COMMON_MOTOR_MIXER; }
    
    std::vector<MotorMixer> mixer;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        while (data.unpacking_remaining()) {
            MotorMixer m;
            rc &= data.unpack(m);
            mixer.push_back(m);
        }
        return rc;
    }
    
};

//MSP2_COMMON_SET_MOTOR_MIXER     = 0x1006,
struct CommonSetMotorMixer : public Message {
    CommonSetMotorMixer(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP2_COMMON_SET_MOTOR_MIXER; }
    
    value<uint8_t> index;
    MotorMixer mixer;
    
    ByteVector encode() const {
        ByteVector data;
        data.pack(index);
        data.pack(mixer);
        return data;
    }
    
};

//MSP2_INAV_STATUS                = 0x2000,
struct InavStatus : public Message {
    InavStatus(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP2_INAV_STATUS; }
    
    value<uint16_t> cycle_time;
    value<uint16_t> i2c_errors;
    value<uint16_t> sensor_status;
    value<uint16_t> avg_system_load_pct;
    value<uint8_t> config_profile;
    value<uint32_t> arming_flags;
    value<uint32_t> box_mode_flags;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(cycle_time);
        rc &= data.unpack(i2c_errors);
        rc &= data.unpack(sensor_status);
        rc &= data.unpack(avg_system_load_pct);
        rc &= data.unpack(config_profile);
        rc &= data.unpack(arming_flags);
        rc &= data.unpack(box_mode_flags);
        return rc;
    }
    
};

//MSP2_INAV_OPTICAL_FLOW          = 0x2001,
struct InavOpticalFlow : public Message {
    InavOpticalFlow(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP2_INAV_OPTICAL_FLOW; }
    
    value<uint8_t> raw_quality;
    value<uint16_t> flow_rate_x;
    value<uint16_t> flow_rate_y;
    value<uint16_t> body_rate_x;
    value<uint16_t> body_rate_y;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(raw_quality);
        rc &= data.unpack(flow_rate_x);
        rc &= data.unpack(flow_rate_y);
        rc &= data.unpack(body_rate_x);
        rc &= data.unpack(body_rate_y);
        return rc;
    }
    
};

//MSP2_INAV_ANALOG                = 0x2002,
struct InavAnalog : public Message {
    InavAnalog(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP2_INAV_ANALOG; }
    
    value<uint8_t> battery_voltage;
    value<uint16_t> mAh_drawn;
    value<uint16_t> rssi;
    value<uint16_t> amperage;
    
    bool decode(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(battery_voltage);
        rc &= data.unpack(mAh_drawn);
        rc &= data.unpack(rssi);
        rc &= data.unpack(amperage);
        return rc;
    }
    
};

struct InavMiscSettings {
    value<uint16_t> mid_rc;
    value<uint16_t> min_throttle;
    value<uint16_t> max_throttle;
    value<uint16_t> min_command;
    value<uint16_t> failsafe_throttle;
    value<uint8_t> gps_provider;
    value<uint8_t> gps_baudrate;
    value<uint8_t> gps_ubx_sbas;
    value<uint8_t> rssi_channel;
    value<uint16_t> mag_declination;
    value<uint16_t> voltage_scale;
    value<uint16_t> cell_min;
    value<uint16_t> cell_max;
    value<uint16_t> cell_warning;
    value<uint32_t> capacity;
    value<uint32_t> capacity_warning;
    value<uint32_t> capacity_critical;
    value<uint8_t> capacity_units;
    
    bool unpack_from(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(mid_rc);
        rc &= data.unpack(min_throttle);
        rc &= data.unpack(max_throttle);
        rc &= data.unpack(min_command);
        rc &= data.unpack(failsafe_throttle);
        rc &= data.unpack(gps_provider);
        rc &= data.unpack(gps_baudrate);
        rc &= data.unpack(gps_ubx_sbas);
        rc &= data.unpack(rssi_channel);
        rc &= data.unpack(mag_declination);
        rc &= data.unpack(voltage_scale);
        rc &= data.unpack(cell_min);
        rc &= data.unpack(cell_max);
        rc &= data.unpack(cell_warning);
        rc &= data.unpack(capacity);
        rc &= data.unpack(capacity_warning);
        rc &= data.unpack(capacity_critical);
        rc &= data.unpack(capacity_units);
        return rc;
    }
    
    void pack_into(ByteVector& data) const {
        data.pack(mid_rc);
        data.pack(min_throttle);
        data.pack(mid_rc);
        data.pack(max_throttle);
        data.pack(min_command);
        data.pack(failsafe_throttle);
        data.pack(gps_provider);
        data.pack(gps_baudrate);
        data.pack(gps_ubx_sbas);
        data.pack(rssi_channel);
        data.pack(mag_declination);
        data.pack(voltage_scale);
        data.pack(cell_min);
        data.pack(cell_max);
        data.pack(cell_warning);
        data.pack(capacity);
        data.pack(capacity_warning);
        data.pack(capacity_critical);
        data.pack(capacity_units);
    }
};

//MSP2_INAV_MISC                  = 0x2003,
struct InavMisc : public InavMiscSettings, public Message {
    InavMisc(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP2_INAV_MISC; }
    
    bool decode(ByteVector& data) {
        return this->unpack_from(data);
    }
    
};

//MSP2_INAV_SET_MISC              = 0x2004,
struct InavSetMisc : public InavMiscSettings, public Message {
    InavSetMisc(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP2_INAV_SET_MISC; }
    
    ByteVector encode() const {
        ByteVector data;
        this->pack_into(data);
        return data;
    }
    
};


struct InavBatteryConfigSettings {
    value<uint16_t> voltage_scale;
    value<uint16_t> cell_min;
    value<uint16_t> cell_max;
    value<uint16_t> cell_warning;
    value<uint16_t> current_offset;
    value<uint16_t> current_scale;
    value<uint32_t> capacity;
    value<uint32_t> capacity_warning;
    value<uint32_t> capacity_critical;
    value<uint8_t> capacity_units;
    
    bool unpack_from(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(voltage_scale);
        rc &= data.unpack(cell_min);
        rc &= data.unpack(cell_max);
        rc &= data.unpack(cell_warning);
        rc &= data.unpack(current_offset);
        rc &= data.unpack(current_scale);
        rc &= data.unpack(capacity);
        rc &= data.unpack(capacity_warning);
        rc &= data.unpack(capacity_critical);
        rc &= data.unpack(capacity_units);
        return rc;
    }
    
    void pack_into(ByteVector& data) const {
        data.pack(voltage_scale);
        data.pack(cell_min);
        data.pack(cell_max);
        data.pack(cell_warning);
        data.pack(current_offset);
        data.pack(current_scale);
        data.pack(capacity);
        data.pack(capacity_warning);
        data.pack(capacity_critical);
        data.pack(capacity_units);
    }
};

//MSP2_INAV_BATTERY_CONFIG        = 0x2005,
struct InavBatteryConfig : public InavBatteryConfigSettings, public Message {
    InavBatteryConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP2_INAV_BATTERY_CONFIG; }
    
    bool decode(ByteVector& data) {
        return this->unpack_from(data);
    }
    
};

//MSP2_INAV_SET_BATTERY_CONFIG    = 0x2006,
struct InavSetBatteryConfig : public InavBatteryConfigSettings, public Message {
    InavSetBatteryConfig(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP2_INAV_SET_BATTERY_CONFIG; }
    
    ByteVector encode() const {
        ByteVector data;
        this->pack_into(data);
        return data;
    }
    
};

struct InavRateProfileSettings {
    value<uint8_t> throttle_rc_mid;
    value<uint8_t> throttle_rc_expo;
    value<uint8_t> throttle_dyn_pid;
    value<uint16_t> throttle_pa_breakpoint;
    
    value<uint8_t> stabilized_rc_expo;
    value<uint8_t> stabilized_rc_yaw_expo;
    value<uint8_t> stabilized_rate_r;
    value<uint8_t> stabilized_rate_p;
    value<uint8_t> stabilized_rate_y;
    
    value<uint8_t> manual_rc_expo;
    value<uint8_t> manual_rc_yaw_expo;
    value<uint8_t> manual_rate_r;
    value<uint8_t> manual_rate_p;
    value<uint8_t> manual_rate_y;
    
    bool unpack_from(ByteVector& data) {
        bool rc = true;
        rc &= data.unpack(throttle_rc_mid);
        rc &= data.unpack(throttle_rc_expo);
        rc &= data.unpack(throttle_dyn_pid);
        rc &= data.unpack(throttle_pa_breakpoint);
        
        rc &= data.unpack(stabilized_rc_expo);
        rc &= data.unpack(stabilized_rc_yaw_expo);
        rc &= data.unpack(stabilized_rate_r);
        rc &= data.unpack(stabilized_rate_p);
        rc &= data.unpack(stabilized_rate_y);
        
        rc &= data.unpack(manual_rc_expo);
        rc &= data.unpack(manual_rc_yaw_expo);
        rc &= data.unpack(manual_rate_r);
        rc &= data.unpack(manual_rate_p);
        rc &= data.unpack(manual_rate_y);
        return rc;
    }
    
    void pack_into(ByteVector& data) const {
        data.pack(throttle_rc_mid);
        data.pack(throttle_rc_expo);
        data.pack(throttle_dyn_pid);
        data.pack(throttle_pa_breakpoint);
        
        data.pack(stabilized_rc_expo);
        data.pack(stabilized_rc_yaw_expo);
        data.pack(stabilized_rate_r);
        data.pack(stabilized_rate_p);
        data.pack(stabilized_rate_y);
        
        data.pack(manual_rc_expo);
        data.pack(manual_rc_yaw_expo);
        data.pack(manual_rate_r);
        data.pack(manual_rate_p);
        data.pack(manual_rate_y);
        
    }
};

//MSP2_INAV_RATE_PROFILE          = 0x2007,
struct InavRateProfile : public InavRateProfileSettings, public Message {
    InavRateProfile(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP2_INAV_RATE_PROFILE; }
    
    bool decode(ByteVector& data) {
        return this->unpack_from(data);
    }
    
};


//MSP2_INAV_SET_RATE_PROFILE      = 0x2008,
struct InavSetRateProfile : public InavRateProfileSettings, public Message {
    InavSetRateProfile(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP2_INAV_SET_RATE_PROFILE; }
    
    ByteVector encode() const {
        ByteVector data;
        this->pack_into(data);
        return data;
    }
    
};

//MSP2_INAV_AIR_SPEED             = 0x2009
struct InavAirSpeed : public InavMiscSettings, public Message {
    InavAirSpeed(FirmwareVariant v) : Message(v) {}
    
    ID id() const { return ID::MSP2_INAV_RATE_PROFILE; }
    
    value<uint32_t> speed;
    
    bool decode(ByteVector& data) {
        return data.unpack(speed);
    }
    
};

} // namespace msg
} // namespace msp

#endif // MSP_MSG_HPP
