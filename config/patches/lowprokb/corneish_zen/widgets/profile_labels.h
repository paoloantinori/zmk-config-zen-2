/*
 *
 * Copyright (c) 2021 Darryl deHaan
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#define PROFILE_LABEL_0 "LINUX"
#define PROFILE_LABEL_1 "MAC"
#define PROFILE_LABEL_2 "PHONE"
#define PROFILE_LABEL_3 "EBOOK"
#define PROFILE_LABEL_4 NULL

#define PROFILE_COUNT 5

static const char *const profile_labels[PROFILE_COUNT] = {
    PROFILE_LABEL_0,
    PROFILE_LABEL_1,
    PROFILE_LABEL_2,
    PROFILE_LABEL_3,
    PROFILE_LABEL_4,
};

static inline const char *profile_label_get(int index) {
    if (index < 0 || index >= PROFILE_COUNT) {
        return NULL;
    }
    return profile_labels[index];
}
