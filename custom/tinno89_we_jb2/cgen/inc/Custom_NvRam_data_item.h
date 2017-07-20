#ifndef CUSTOM_NVRAM_DATA_ITEM_H
#define CUSTOM_NVRAM_DATA_ITEM_H

#include "custom_cfg_module_file.h"
#include "Custom_NvRam_LID.h"



/*****************************************************************************
* NVRAM database define
*****************************************************************************/
BEGIN_NVRAM_DATA

LID_BIT VER_LID(AP_CFG_RDCL_FILE_AUDIO_LID)
AUDIO_CUSTOM_PARAM_STRUCT *CFG_FILE_SPEECH_REC_TOTAL
{

};

LID_BIT VER_LID(AP_CFG_CUSTOM_FILE_GPS_LID)
ap_nvram_gps_config_struct *CFG_FILE_GPS_CONFIG_TOTAL
{

};

LID_BIT VER_LID(AP_CFG_RDCL_FILE_AUDIO_COMPFLT_LID)
AUDIO_ACF_CUSTOM_PARAM_STRUCT *CFG_FILE_AUDIO_COMPFLT_REC_TOTAL
{

};

LID_BIT VER_LID(AP_CFG_RDCL_FILE_HEADPHONE_COMPFLT_LID)
AUDIO_ACF_CUSTOM_PARAM_STRUCT *CFG_FILE_HEADPHONE_COMPFLT_REC_TOTAL
{

};

LID_BIT VER_LID(AP_CFG_RDCL_FILE_AUDIO_EFFECT_LID)
AUDIO_EFFECT_CUSTOM_PARAM_STRUCT *CFG_FILE_AUDIO_EFFECT_REC_TOTAL
{

};


LID_BIT VER_LID(AP_CFG_RDEB_FILE_WIFI_LID)
WIFI_CFG_PARAM_STRUCT *CFG_FILE_WIFI_REC_TOTAL
{

};

LID_BIT VER_LID(AP_CFG_RDEB_WIFI_CUSTOM_LID)
WIFI_CUSTOM_PARAM_STRUCT *CFG_FILE_WIFI_CUSTOM_REC_TOTAL
{

};
LID_BIT VER_LID(AP_CFG_RDCL_FILE_AUDIO_PARAM_MED_LID)
AUDIO_PARAM_MED_STRUCT *CFG_FILE_AUDIO_PARAM_MED_REC_TOTAL
{

};

LID_BIT VER_LID(AP_CFG_RDCL_FILE_AUDIO_VOLUME_CUSTOM_LID)
AUDIO_VOLUME_CUSTOM_STRUCT *CFG_FILE_AUDIO_VOLUME_CUSTOM_REC_TOTAL
{

};

LID_BIT VER_LID(AP_CFG_RDCL_FILE_DUAL_MIC_CUSTOM_LID)
AUDIO_CUSTOM_EXTRA_PARAM_STRUCT *CFG_FILE_SPEECH_DUAL_MIC_TOTAL
{

};

LID_BIT VER_LID(AP_CFG_RDCL_FILE_AUDIO_WB_PARAM_LID)
AUDIO_CUSTOM_WB_PARAM_STRUCT *CFG_FILE_WB_SPEECH_REC_TOTAL
{

};

LID_BIT VER_LID(AP_CFG_REEB_PRODUCT_INFO_LID)
PRODUCT_INFO *CFG_FILE_PRODUCT_INFO_TOTAL
{

};

LID_BIT VER_LID(AP_CFG_RDCL_FILE_AUDIO_GAIN_TABLE_LID)
AUDIO_GAIN_TABLE_STRUCT *CFG_FILE_AUDIO_GAIN_TABLE_CUSTOM_REC_TOTAL
{

};

LID_BIT VER_LID(AP_CFG_RDCL_FILE_AUDIO_VER1_VOLUME_CUSTOM_LID)
AUDIO_VER1_CUSTOM_VOLUME_STRUCT *CFG_FILE_AUDIO_VER1_VOLUME_CUSTOM_REC_TOTAL
{

};

LID_BIT VER_LID(AP_CFG_RDCL_FILE_AUDIO_HD_REC_PAR_LID)
AUDIO_HD_RECORD_PARAM_STRUCT *CFG_FILE_AUDIO_HD_REC_PAR_TOTAL
{

};

LID_BIT VER_LID(AP_CFG_RDCL_FILE_AUDIO_HD_REC_SCENE_LID)
AUDIO_HD_RECORD_SCENE_TABLE_STRUCT *CFG_FILE_AUDIO_HD_REC_SCENE_TABLE_TOTAL
{

};

LID_BIT VER_LID(AP_CFG_RDCL_FILE_AUDIO_HD_REC_48K_PAR_LID)
AUDIO_HD_RECORD_48K_PARAM_STRUCT *CFG_FILE_AUDIO_HD_REC_48K_PAR_TOTAL
{
	
};

LID_BIT VER_LID(AP_CFG_RDCL_FILE_VOICE_RECOGNIZE_PARAM_LID)
VOICE_RECOGNITION_PARAM_STRUCT *CFG_FILE_VOICE_RECOGNIZE_PAR_TOTAL
{
	
};

LID_BIT VER_LID(AP_CFG_RDCL_FILE_AUDIO_AUDENH_CONTROL_OPTION_PAR_LID)
AUDIO_AUDENH_CONTROL_OPTION_STRUCT *CFG_FILE_AUDIO_AUDENH_CONTROL_OPTION_PAR_TOTAL
{
	
};

END_NVRAM_DATA

#endif /* CUSTOM_NVRAM_DATA_ITEM_H */

