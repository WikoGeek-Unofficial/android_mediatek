#ifndef TOUCHPANEL_H__
#define TOUCHPANEL_H__

/* Pre-defined definition */
#define TPD_TYPE_CAPACITIVE
#define TPD_TYPE_RESISTIVE
#define TPD_POWER_SOURCE         MT6575_POWER_VGP2
#define TPD_I2C_NUMBER           0
#define TPD_WAKEUP_TRIAL         60
#define TPD_WAKEUP_DELAY         100

#define TPD_DELAY                (2*HZ/100)

#define TPD_VELOCITY_CUSTOM_X 10
#define TPD_VELOCITY_CUSTOM_Y 12

//#define TPD_RES_X                480
//#define TPD_RES_Y                800
#define TPD_CALIBRATION_MATRIX  {962,0,0,0,1600,0,0,0};

#define TPD_HAVE_CALIBRATION
//#define TPD_HAVE_BUTTON
#define TPD_HAVE_BUTTON
#define TPD_BUTTON_HEIGH        (100)
#define TPD_KEY_COUNT           4
#define TPD_KEYS                { KEY_MENU, KEY_HOMEPAGE ,KEY_BACK, KEY_SEARCH}
#define TPD_KEYS_DIM            {{60,850,120,TPD_BUTTON_HEIGH},{180,850,120,TPD_BUTTON_HEIGH},{300,850,120,TPD_BUTTON_HEIGH}, {420,850,120,TPD_BUTTON_HEIGH}}

#define TPD_HAVE_TREMBLE_ELIMINATION

#define TPD_NO_GPIO
#define TPD_RESET_PIN_ADDR   (PERICFG_BASE + 0xC000)
#define TPD_DITO_SENSOR
#define TPD_CONDITION_SWITCH
#define TPD_RESET_ISSUE_WORKAROUND
#define TPD_MAX_RESET_COUNT 3
#define TPD_X_RES (480)
#define TPD_Y_RES (800)
#define TPD_WARP_Y(y) (TPD_Y_RES - y )
#define TPD_WARP_X(x) ( x )


#define TPD_GPIO_GPO_ADDR (0xc100c000)
#define TPD_GPIO_OE_ADDR (0xc100c008)
//#define RES_AUTO_CONFIG

#ifdef TPD_DITO_SENSOR

static u8 cfg_data_version_d[] =
{
    0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,
0x10,0x12,0x82,0x22,0x92,0x22,0xA2,0x22,
0xB2,0x22,0xC2,0x22,0xD2,0x22,0xE2,0x22,
0xF2,0x22,0x72,0x22,0x62,0x22,0x52,0x22,
0x42,0x22,0x32,0x22,0x22,0x22,0x12,0x22,
0x02,0x22,0x37,0x03,0xC8,0xC8,0xC8,0x21,
0x21,0x21,0x10,0x10,0x0A,0x47,0x37,0x49,
0x03,0x00,0x05,0xE0,0x01,0x55,0x03,0x00,
0x00,0x56,0x54,0x5A,0x58,0x00,0x00,0x04,
0x20,0x05,0x07,0x00,0x00,0x00,0x00,0x00,
0x32,0x16,0xAC,0x03,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,
0x30,0x20,0x14,0x00,0x00,0x00,0x00,0x00,
0x00,0x01

};


static u8 cfg_data_with_charger_version_d[] =
{
    0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,
0x10,0x12,0x82,0x22,0x92,0x22,0xA2,0x22,
0xB2,0x22,0xC2,0x22,0xD2,0x22,0xE2,0x22,
0xF2,0x22,0x72,0x22,0x62,0x22,0x52,0x22,
0x42,0x22,0x32,0x22,0x22,0x22,0x12,0x22,
0x02,0x22,0x37,0x03,0xC8,0xC8,0xC8,0x21,
0x21,0x21,0x10,0x10,0x0A,0x47,0x37,0x49,
0x03,0x00,0x05,0xE0,0x01,0x55,0x03,0x00,
0x00,0x56,0x54,0x5A,0x58,0x00,0x00,0x04,
0x20,0x05,0x07,0x00,0x00,0x00,0x00,0x00,
0x32,0x16,0xAC,0x03,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,
0x30,0x20,0x14,0x00,0x00,0x00,0x00,0x00,
0x00,0x01

};

//GT818_6573_DITO_config.gtx
static u8 cfg_data_version_b[] =
{
    0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E,
    0x10, 0x12, 0x60, 0x00, 0x50, 0x00, 0x40, 0x00,
    0x30, 0x00, 0x20, 0x00, 0x10, 0x00, 0x00, 0x00,
    0x70, 0x00, 0x80, 0x00, 0x90, 0x00, 0xA0, 0x00,
    0xB0, 0x00, 0xC0, 0x00, 0xD0, 0x00, 0xE0, 0x00,
    0x00, 0x00, 0x11, 0x13, 0x90, 0x90, 0x90, 0x40,
    0x40, 0x40, 0x0F, 0x0F, 0x0A, 0x50, 0x3C, 0x49,
    0x03, 0x00, 0x05, 0xE0, 0x01, 0x20, 0x03, 0x00,
    0x00, 0x64, 0x5A, 0x40, 0x40, 0x00, 0x00, 0x03,
    0x19, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x20, 0x10, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38,
    0x00, 0x3C, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01
};

// GT818_6573_DITO_charger_577K.gtx
static u8 cfg_data_with_charger_version_b[] =
{
    0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E,
    0x10, 0x12, 0x60, 0x00, 0x50, 0x00, 0x40, 0x00,
    0x30, 0x00, 0x20, 0x00, 0x10, 0x00, 0x00, 0x00,
    0x70, 0x00, 0x80, 0x00, 0x90, 0x00, 0xA0, 0x00,
    0xB0, 0x00, 0xC0, 0x00, 0xD0, 0x00, 0xE0, 0x00,
    0x00, 0x00, 0x2F, 0x13, 0x10, 0x10, 0x10, 0x60,
    0x60, 0x60, 0x0F, 0x0F, 0x0A, 0x78, 0x5A, 0x49,
    0x03, 0x64, 0x05, 0xE0, 0x01, 0x20, 0x03, 0x00,
    0x00, 0x64, 0x5A, 0x40, 0x40, 0x00, 0x00, 0x03,
    0x19, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x20, 0x10, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38,
    0x00, 0x3C, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01
};

#else //SITO
// 6573_SITO_config.gtx
static u8 cfg_data_version_b[] =
{
    0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E,
    0x10, 0x12, 0x80, 0x00, 0x90, 0x00, 0xA0, 0x00,
    0xB0, 0x00, 0xC0, 0x00, 0xD0, 0x00, 0xE0, 0x00,
    0xF0, 0x00, 0x70, 0x00, 0x60, 0x00, 0x50, 0x00,
    0x40, 0x00, 0x30, 0x00, 0x20, 0x00, 0x10, 0x00,
    0x00, 0x00, 0x27, 0x13, 0xE0, 0xE0, 0xE0, 0x1A,
    0x1A, 0x1A, 0x10, 0x10, 0x0A, 0x50, 0x3C, 0x0D,
    0x00, 0x00, 0x05, 0xE0, 0x01, 0x20, 0x03, 0x00,
    0x00, 0x64, 0x5A, 0x40, 0x40, 0x00, 0x00, 0x03,
    0x19, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x20, 0x10, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38,
    0x00, 0x3C, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01
};

//6573_SITO_charger.gtx
static u8 cfg_data_with_charger_version_b[] =
{
    0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E,
    0x10, 0x12, 0x80, 0x00, 0x90, 0x00, 0xA0, 0x00,
    0xB0, 0x00, 0xC0, 0x00, 0xD0, 0x00, 0xE0, 0x00,
    0xF0, 0x00, 0x70, 0x00, 0x60, 0x00, 0x50, 0x00,
    0x40, 0x00, 0x30, 0x00, 0x20, 0x00, 0x10, 0x00,
    0x00, 0x00, 0x2B, 0x13, 0x50, 0x50, 0x50, 0x51,
    0x61, 0x71, 0x10, 0x10, 0x0A, 0x64, 0x50, 0x4D,
    0x00, 0x00, 0x05, 0xE0, 0x01, 0x20, 0x03, 0x00,
    0x00, 0x64, 0x5A, 0x40, 0x40, 0x00, 0x00, 0x03,
    0x19, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x20, 0x10, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38,
    0x00, 0x3C, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01
};
#endif
#endif /* TOUCHPANEL_H__ */
