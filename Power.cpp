/*
 * Copyright (C) 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Power.h"

#include <aidl/android/hardware/power/Boost.h>

#include <android-base/logging.h>
#include <cutils/properties.h>
#include <string>
#include <log/log.h>


#define CPU_CLUST0_GOV_PATH "/sys/devices/system/cpu/cpufreq/policy0/scaling_governor"
#define CPU_CLUST1_GOV_PATH "/sys/devices/system/cpu/cpufreq/policy4/scaling_governor"



namespace aidl {
namespace android {
namespace hardware {
namespace power {
namespace impl {
namespace rockchip {

void Power::getSupportedPlatform() {
    if (_mode_support_int < 0) {
        char platform[PROPERTY_VALUE_MAX] = {0};
        property_get("ro.boot.hardware", platform, "");
        if (strncmp(platform, "rk3399", PROPERTY_VALUE_MAX) == 0) {
            _boost_support_int = 0x003F;
            _mode_support_int = 0x3FFF;
        }
    }
}

static void sysfs_write(const char *path, const char *s) {
    if (access(path, F_OK) < 0) return;
    char buf[80];
    int len;
    int fd = open(path, O_WRONLY);

    if (fd < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error opening %s: %s\n", path, buf);
        return;
    }

    len = write(fd, s, strlen(s));
    if (len < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error writing to %s: %s\n", path, buf);
    }
    close(fd);
}

static void performance_boost(bool on) {
    ALOGV("RK performance_boost Entered!");
    sysfs_write(CPU_CLUST0_GOV_PATH, on?"performance":"interactive");
    sysfs_write(CPU_CLUST1_GOV_PATH, on?"performance":"interactive");
}

static void powersave(bool on) {
    ALOGV("RK powersave Entered!");
    sysfs_write(CPU_CLUST0_GOV_PATH, on?"powersave":"interactive");
    sysfs_write(CPU_CLUST1_GOV_PATH, on?"powersave":"interactive");
}

static void interactive() {
    ALOGV("RK interactive Entered!");
    sysfs_write(CPU_CLUST0_GOV_PATH, "interactive");
    sysfs_write(CPU_CLUST1_GOV_PATH, "interactive");
}

ndk::ScopedAStatus Power::setMode(Mode type, bool enabled) {
    LOG(VERBOSE) << "Power setMode: " << static_cast<int32_t>(type) << " to: " << enabled;
    switch (type) {
        case Mode::DOUBLE_TAP_TO_WAKE:
        break;
        case Mode::LOW_POWER:
            powersave(enabled);
        break;
        case Mode::SUSTAINED_PERFORMANCE:
        break;
        case Mode::FIXED_PERFORMANCE:
            performance_boost(enabled);
        break;
        case Mode::VR:
        break;
        case Mode::LAUNCH:
            performance_boost(enabled);
        break;
        case Mode::EXPENSIVE_RENDERING:
        break;
        case Mode::INTERACTIVE:
            if (enabled) interactive();
        break;
        case Mode::DEVICE_IDLE:
            powersave(enabled);
        break;
        case Mode::DISPLAY_INACTIVE:
        break;
        case Mode::AUDIO_STREAMING_LOW_LATENCY:
        break;
        case Mode::CAMERA_STREAMING_SECURE:
        break;
        case Mode::CAMERA_STREAMING_LOW:
        break;
        case Mode::CAMERA_STREAMING_MID:
        break;
        case Mode::CAMERA_STREAMING_HIGH:
        break;
        default:
        break;
    }
    return ndk::ScopedAStatus::ok();
}


/**
 * _PLACEHOLDER_,           DOUBLE_TAP_TO_WAKE,     LOW_POWER,              SUSTAINED_PERFORMANCE,
 * FIXED_PERFORMANCE,       VR,                     LAUNCH,                 EXPENSIVE_RENDERING,
 * INTERACTIVE,             DEVICE_IDLE,            DISPLAY_INACTIVE,       AUDIO_STREAMING_LOW_LATENCY,
 * CAMERA_STREAMING_SECURE, CAMERA_STREAMING_LOW,   CAMERA_STREAMING_MID,   CAMERA_STREAMING_HIGH
 */
ndk::ScopedAStatus Power::isModeSupported(Mode type, bool* _aidl_return) {
    LOG(INFO) << "Power isModeSupported: " << static_cast<int32_t>(type);
    getSupportedPlatform();
    switch (type) {
        case Mode::DOUBLE_TAP_TO_WAKE:
            *_aidl_return = 0x4000 & _mode_support_int;
        break;
        case Mode::LOW_POWER:
            *_aidl_return = 0x2000 & _mode_support_int;
        break;
        case Mode::SUSTAINED_PERFORMANCE:
            *_aidl_return = 0x1000 & _mode_support_int;
        break;
        case Mode::FIXED_PERFORMANCE:
            *_aidl_return = 0x0800 & _mode_support_int;
        break;
        case Mode::VR:
            *_aidl_return = 0x0400 & _mode_support_int;
        break;
        case Mode::LAUNCH:
            *_aidl_return = 0x0200 & _mode_support_int;
        break;
        case Mode::EXPENSIVE_RENDERING:
            *_aidl_return = 0x0100 & _mode_support_int;
        break;
        case Mode::INTERACTIVE:
            *_aidl_return = 0x0080 & _mode_support_int;
        break;
        case Mode::DEVICE_IDLE:
            *_aidl_return = 0x0040 & _mode_support_int;
        break;
        case Mode::DISPLAY_INACTIVE:
            *_aidl_return = 0x0020 & _mode_support_int;
        break;
        case Mode::AUDIO_STREAMING_LOW_LATENCY:
            *_aidl_return = 0x0010 & _mode_support_int;
        break;
        case Mode::CAMERA_STREAMING_SECURE:
            *_aidl_return = 0x0008 & _mode_support_int;
        break;
        case Mode::CAMERA_STREAMING_LOW:
            *_aidl_return = 0x0004 & _mode_support_int;
        break;
        case Mode::CAMERA_STREAMING_MID:
            *_aidl_return = 0x0002 & _mode_support_int;
        break;
        case Mode::CAMERA_STREAMING_HIGH:
            *_aidl_return = 0x0001 & _mode_support_int;
        break;
        default:
            *_aidl_return = false;
        break;
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Power::setBoost(Boost type, int32_t durationMs) {
    LOG(VERBOSE) << "Power setBoost: " << static_cast<int32_t>(type)
                 << ", duration: " << durationMs;
    switch (type) {
        // Touch screen
        case Boost::INTERACTION:
        case Boost::DISPLAY_UPDATE_IMMINENT:
        case Boost::ML_ACC:
        case Boost::AUDIO_LAUNCH:
        case Boost::CAMERA_LAUNCH:
        case Boost::CAMERA_SHOT:
        default:
        break;
    }
    return ndk::ScopedAStatus::ok();
}

/**
 * Boost type defined from:
 * hardware/interfaces/power/aidl/android/hardware/power/Boost.aidl
 *
 * platform : _PLACEHOLDER_, _PLACEHOLDER_, INTERACTION,  DISPLAY_UPDATE_IMMINENT,
 *            ML_AAC,        AUDIO_LAUNCH,  CAMERA_LUNCH, CAMERA_SHOT
 *
 * rk3399 : 0x003F
 * rk3326 : 0x003F
 * ...
 */
ndk::ScopedAStatus Power::isBoostSupported(Boost type, bool* _aidl_return) {
    LOG(INFO) << "Power isBoostSupported: " << static_cast<int32_t>(type);
    getSupportedPlatform();
    switch (type) {
        // Touch screen
        case Boost::INTERACTION:
            *_aidl_return = 0x0020 & _boost_support_int;
        break;
        // Refresh screen
        case Boost::DISPLAY_UPDATE_IMMINENT:
            *_aidl_return = 0x0010 & _boost_support_int;
        break;
        // ML accelerator
        case Boost::ML_ACC:
            *_aidl_return = 0x0008 & _boost_support_int;
        break;
        case Boost::AUDIO_LAUNCH:
            *_aidl_return = 0x0004 & _boost_support_int;
        break;
        case Boost::CAMERA_LAUNCH:
            *_aidl_return = 0x0002 & _boost_support_int;
        break;
        case Boost::CAMERA_SHOT:
            *_aidl_return = 0x0001 & _boost_support_int;
        break;
        default:
            *_aidl_return = false;
        break;
    }
    return ndk::ScopedAStatus::ok();
}

}  // namespace rockchip
}  // namespace impl
}  // namespace power
}  // namespace hardware
}  // namespace android
}  // namespace aidl
