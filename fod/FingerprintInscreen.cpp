/*
 * Copyright (C) 2019-2020 The LineageOS Project
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

#define LOG_TAG "FingerprintInscreenService"

#include "FingerprintInscreen.h"

#include <android-base/logging.h>
#include <hardware_legacy/power.h>

#include <fstream>
#include <cmath>

#define FINGERPRINT_ERROR_VENDOR 8

#define COMMAND_NIT 10
#define PARAM_NIT_630_FOD 1
#define PARAM_NIT_NONE 0

#define FOD_SENSOR_X 445
#define FOD_SENSOR_Y 1931
#define FOD_SENSOR_SIZE 190

namespace vendor {
namespace lineage {
namespace biometrics {
namespace fingerprint {
namespace inscreen {
namespace V1_1 {
namespace implementation {

template <typename T>
static void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value;
}

FingerprintInscreen::FingerprintInscreen() {
    TouchFeatureService = ITouchFeature::getService();
    xiaomiDisplayFeatureService = IDisplayFeature::getService();
    xiaomiFingerprintService = IXiaomiFingerprint::getService();
}

Return<int32_t> FingerprintInscreen::getPositionX() {
    return FOD_SENSOR_X;
}

Return<int32_t> FingerprintInscreen::getPositionY() {
    return FOD_SENSOR_Y;
}

Return<int32_t> FingerprintInscreen::getSize() {
    return FOD_SENSOR_SIZE;
}

Return<void> FingerprintInscreen::onStartEnroll() {
    return Void();
}

Return<void> FingerprintInscreen::onFinishEnroll() {
    return Void();
}

Return<void> FingerprintInscreen::switchHbm(bool enabled) {
    if (enabled) {
        xiaomiDisplayFeatureService->setFeature(0, 11, 1, 3);
    } else {
        xiaomiDisplayFeatureService->setFeature(0, 11, 0, 3);
    }
    return Void();
}

Return<void> FingerprintInscreen::onPress() {
    acquire_wake_lock(PARTIAL_WAKE_LOCK, LOG_TAG);
    TouchFeatureService->setTouchMode(10, 1);
    xiaomiFingerprintService->extCmd(COMMAND_NIT, PARAM_NIT_630_FOD);
    return Void();
}

Return<void> FingerprintInscreen::onRelease() {
    TouchFeatureService->setTouchMode(10, 0);
    xiaomiFingerprintService->extCmd(COMMAND_NIT, PARAM_NIT_NONE);
    release_wake_lock(LOG_TAG);
    return Void();
}

Return<void> FingerprintInscreen::onShowFODView() {
    xiaomiDisplayFeatureService->setFeature(0, 17, 2, 1);
    xiaomiDisplayFeatureService->setFeature(0, 17, 1, 1);
    xiaomiDisplayFeatureService->setFeature(0, 11, 1, 4);
    return Void();
}

Return<void> FingerprintInscreen::onHideFODView() {
    xiaomiDisplayFeatureService->setFeature(0, 17, 0, 255);
    xiaomiDisplayFeatureService->setFeature(0, 17, 0, 1);
    return Void();
}

Return<bool> FingerprintInscreen::handleAcquired(int32_t acquiredInfo, int32_t vendorCode) {
    LOG(ERROR) << "acquiredInfo: " << acquiredInfo << ", vendorCode: " << vendorCode << "\n";
    return false;
}

Return<bool> FingerprintInscreen::handleError(int32_t error, int32_t vendorCode) {
    LOG(ERROR) << "error: " << error << ", vendorCode: " << vendorCode << "\n";
    return error == FINGERPRINT_ERROR_VENDOR && vendorCode == 6;
}

Return<void> FingerprintInscreen::setLongPressEnabled(bool) {
    return Void();
}

Return<int32_t> FingerprintInscreen::getDimAmount(int32_t brightness) {
    float alpha;
    int realBrightness = brightness * 2047 / 255;

    if (realBrightness > 500) {
        alpha = 1.0 - pow(realBrightness / 2047.0 * 430.0 / 600.0, 0.455);
    } else {
        alpha = 1.0 - pow(realBrightness / 1680.0, 0.455);
    }

    return 255 * alpha;
}

Return<bool> FingerprintInscreen::shouldBoostBrightness() {
    return false;
}

Return<void> FingerprintInscreen::setCallback(const sp<IFingerprintInscreenCallback>&) {
    return Void();
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace inscreen
}  // namespace fingerprint
}  // namespace biometrics
}  // namespace lineage
}  // namespace vendor
