/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/* SENSOR FULL SIZE */
#ifndef __CAMERA_SENSOR_PARA_H
#define __CAMERA_SENSOR_PARA_H

#define CAMERA_SENSOR_REG_DEFAULT_VALUE  \
        /* ARRAY: SENSOR.reg[11] */\
        {\
            /* STRUCT: SENSOR.reg[0] */\
            {\
                /* SENSOR.reg[0].addr */ 0x00000304, /* SENSOR.reg[0].para */ 0x00000000\
            },\
            /* STRUCT: SENSOR.reg[1] */\
            {\
                /* SENSOR.reg[1].addr */ 0x00000305, /* SENSOR.reg[1].para */ 0x0000000D\
            },\
            /* STRUCT: SENSOR.reg[2] */\
            {\
                /* SENSOR.reg[2].addr */ 0x00000306, /* SENSOR.reg[2].para */ 0x00000000\
            },\
            /* STRUCT: SENSOR.reg[3] */\
            {\
                /* SENSOR.reg[3].addr */ 0x00000307, /* SENSOR.reg[3].para */ 0x000000C0\
            },\
            /* STRUCT: SENSOR.reg[4] */\
            {\
                /* SENSOR.reg[4].addr */ 0x00000300, /* SENSOR.reg[4].para */ 0x00000000\
            },\
            /* STRUCT: SENSOR.reg[5] */\
            {\
                /* SENSOR.reg[5].addr */ 0x00000301, /* SENSOR.reg[5].para */ 0x00000004\
            },\
            /* STRUCT: SENSOR.reg[6] */\
            {\
                /* SENSOR.reg[6].addr */ 0x0000030A, /* SENSOR.reg[6].para */ 0x00000000\
            },\
            /* STRUCT: SENSOR.reg[7] */\
            {\
                /* SENSOR.reg[7].addr */ 0x0000030B, /* SENSOR.reg[7].para */ 0x00000002\
            },\
            /* STRUCT: SENSOR.reg[8] */\
            {\
                /* SENSOR.reg[8].addr */ 0x00000308, /* SENSOR.reg[8].para */ 0x00000000\
            },\
            /* STRUCT: SENSOR.reg[9] */\
            {\
                /* SENSOR.reg[9].addr */ 0x00000309, /* SENSOR.reg[9].para */ 0x00000008\
            },\
            /* STRUCT: SENSOR.reg[10] */\
            {\
                /* SENSOR.reg[10].addr */ 0xFFFFFFFF, /* SENSOR.reg[10].para */ 0x00000001\
            }\
        }

#define CAMERA_SENSOR_CCT_DEFAULT_VALUE {{ 0xFFFFFFFF, 0x08 } ,{ 0x0209, 0x0008 } ,{ 0x0207, 0x0008 } ,{ 0x020D, 0x0008 } ,{ 0x020B, 0x0008 }}
#endif /* __CAMERA_SENSOR_PARA_H */
