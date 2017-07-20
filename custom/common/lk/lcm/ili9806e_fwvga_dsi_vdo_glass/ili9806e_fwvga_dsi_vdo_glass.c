#ifdef BUILD_LK
#else
#include <linux/string.h>
#if defined(BUILD_UBOOT)
#include <asm/arch/mt_gpio.h>
#else
#include <mach/mt_gpio.h>
#endif
#endif
#include "lcm_drv.h"
#include "lcm_adc_define.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (854)

#define REGFLAG_DELAY                                       0xAB
#define REGFLAG_END_OF_TABLE                                0xAA   // END OF REGISTERS MARKER

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif
#define ILI9806E_LCM_ID                                     (0x0604)

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)    lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)       lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)                                  lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)              lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)                                           lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)                   lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

struct LCM_setting_table
{
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_sleep_out_setting[] =
{
    // Sleep Out
    {0x11, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
    {0x29, 1, {0x00}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] =
{
    // Display off sequence
    {0x28, 1, {0x00}},
    {REGFLAG_DELAY, 10, {}},

    // Sleep Mode On
    {0x10, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    {REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
    unsigned int i;

    for(i = 0; i < count; i++)
    {
        unsigned cmd;
        cmd = table[i].cmd;

        switch (cmd)
        {
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;

            case REGFLAG_END_OF_TABLE :
                break;

            default:
                dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);

                if (cmd != 0xFF && cmd != 0x2C && cmd != 0x3C)
                {
                    //#if defined(BUILD_UBOOT)
                    //  printf("[DISP] - uboot - REG_R(0x%x) = 0x%x. \n", cmd, table[i].para_list[0]);
                    //#endif
                    while(read_reg(cmd) != table[i].para_list[0]);
                }
        }
    }
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------
static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
    memset(params, 0, sizeof(LCM_PARAMS));

    params->type   = LCM_TYPE_DSI;

    params->width  = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;

    // enable tearing-free
    params->dbi.te_mode             = LCM_DBI_TE_MODE_VSYNC_ONLY;
    params->dbi.te_edge_polarity        = LCM_POLARITY_RISING;

    params->dsi.mode   = SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE;

    // DSI
    /* Command mode setting */
    params->dsi.LANE_NUM                = LCM_TWO_LANE;

    //The following defined the fomat for data coming from LCD engine.
    params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
    params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format    = LCM_DSI_FORMAT_RGB888;

    // Video mode setting
    params->dsi.intermediat_buffer_num = 2;

    params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

    params->dsi.word_count=480*3;   //DSI CMD mode need set these two bellow params, different to 6577
    params->dsi.vertical_active_line=FRAME_HEIGHT;

    params->dsi.vertical_sync_active = 4;
    params->dsi.vertical_backporch = 16;
    params->dsi.vertical_frontporch                 = 20;
    params->dsi.vertical_active_line                = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active = 8;//10;
    params->dsi.horizontal_backporch = 60;//50;
    params->dsi.horizontal_frontporch = 60;//50;
    params->dsi.horizontal_blanking_pixel               = 60;
    params->dsi.horizontal_active_pixel             = FRAME_WIDTH;
    params->dsi.compatibility_for_nvk = 0;

    // Bit rate calculation
#ifdef CONFIG_MT6589_FPGA
    params->dsi.pll_div1=2;     // div1=0,1,2,3;div1_real=1,2,4,4
    params->dsi.pll_div2=2;     // div2=0,1,2,3;div1_real=1,2,4,4
    params->dsi.fbk_div =8;     // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)
#else
    params->dsi.pll_div1=1;     // div1=0,1,2,3;div1_real=1,2,4,4
    params->dsi.pll_div2=1;     // div2=0,1,2,3;div2_real=1,2,4,4
    params->dsi.fbk_div =30;		// fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)		
#endif
}


static void init_lcm_registers(void)
{
    unsigned int data_array[16];

    data_array[0]=0x00063902;
    data_array[1]=0x0698ffff;
    data_array[2]=0x00000104;
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00001808;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000121;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000130;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000031;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000760;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000061;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000762;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000063;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00001a40;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00003541;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000342;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000c43;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000144;
    dsi_set_cmdq(data_array, 2, 1);
    data_array[0]=0x00023902;
    data_array[1]=0x00006050;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00006051;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000052;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00005353;//4e53
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x000000a0;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x000005a1;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x000018a2;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x000011a3;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x00000ba4;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x00001ea5;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000aa6;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x000009a7;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x000002a8;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x00000ba9;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x000002aa;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x000006ab;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000dac;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x00002cad;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x000028ae;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x000000af;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x000000c0;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x000001c1;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x000006c2;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x000013c3;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x000008c4;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x000016c5;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x00000bc6;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x000009c7;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x000005c8;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x000008c9;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x000007ca;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x000006cb;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000ccc;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x00002bcd;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x000026ce;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x000000cf;
    dsi_set_cmdq(data_array, 2, 1);


    //*************gamma  *******************//
    data_array[0]=0x00063902;
    data_array[1]=0x0698ffff;
    data_array[2]=0x00000604;
    dsi_set_cmdq(data_array, 3, 1);



    data_array[0]=0x00023902;
    data_array[1]=0x00002100;
    dsi_set_cmdq(data_array, 2, 1);


    data_array[0]=0x00023902;
    data_array[1]=0x00000a01;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000002;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000003;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000104;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000105;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00008006;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000607;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000108;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00008009;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000000a;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000000b;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000060c;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000060d;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000000e;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000000f;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000f010;
    dsi_set_cmdq(data_array, 2, 1);



    data_array[0]=0x00023902;
    data_array[1]=0x0000f411;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000412;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000013;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000014;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000c015;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000816;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000017;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000018;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000019;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000001a;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000001b;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000001c;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000001d;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000120;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00002321;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00004522;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00006723;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000124;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00002325;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00004526;
    dsi_set_cmdq(data_array, 2, 1);


    data_array[0]=0x00023902;
    data_array[1]=0x00006727;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000130;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00001131;
    dsi_set_cmdq(data_array, 2, 1);


    data_array[0]=0x00023902;
    data_array[1]=0x00000032;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000ee33;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000ff34;
    dsi_set_cmdq(data_array, 2, 1);


    data_array[0]=0x00023902;
    data_array[1]=0x0000bb35;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000ca36;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000dd37;
    dsi_set_cmdq(data_array, 2, 1);


    data_array[0]=0x00023902;
    data_array[1]=0x0000ac38;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00007639;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000673a;
    dsi_set_cmdq(data_array, 2, 1);


    data_array[0]=0x00023902;
    data_array[1]=0x0000223b;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000223c;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000223d;
    dsi_set_cmdq(data_array, 2, 1);


    data_array[0]=0x00023902;
    data_array[1]=0x0000223e;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x0000223f;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00002240;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00001053;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00063902;
    data_array[1]=0x0698ffff;
    data_array[2]=0x00000704;
    dsi_set_cmdq(data_array, 3, 1);


    data_array[0]=0x00023902;
    data_array[1]=0x00002217;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00001306;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x00007702;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x00001d18;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00023902;
    data_array[1]=0x000079e1;
    dsi_set_cmdq(data_array, 2, 1);

    //*************PAGE 6  *******************//
    data_array[0]=0x00063902;
    data_array[1]=0x0698ffff;
    data_array[2]=0x00000004;
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0] = 0x00210500;
    dsi_set_cmdq(data_array, 1, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00000035;
    dsi_set_cmdq(data_array, 2, 1);


    data_array[0] = 0x00110500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(100);

    data_array[0]= 0x00290500;
    dsi_set_cmdq(data_array, 1, 1);
// opt 20140918  fyf
/*    data_array[0]=0x00063902;
    data_array[1]=0x0698ffff;
    data_array[2]=0x00000104;
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0]=0x00023902;
    data_array[1]=0x00005353;
    dsi_set_cmdq(data_array, 2, 1);

    data_array[0]=0x00063902;
    data_array[1]=0x0698ffff;
    data_array[2]=0x00000804;
    dsi_set_cmdq(data_array, 3, 1);
*/

}


static void lcm_init(void)
{
    SET_RESET_PIN(1);
    MDELAY(20);
    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(120);

    init_lcm_registers();

}

extern int disp_bls_set_backlight(unsigned int level);
static void lcm_suspend(void)
{
    unsigned int data_array[16];
#if defined(BUILD_LK)
    printf("%s\n", __func__);
#elif defined(BUILD_UBOOT)
    printf("%s\n", __func__);
#else
    printk(" 9806e lcm_suspend");
#endif

    disp_bls_set_backlight(0);


    data_array[0]=0x00063902;
    data_array[1]=0x0698ffff;
    data_array[2]=0x00000004;
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0] = 0x00280500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(20);

    data_array[0] = 0x00100500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(20);


    SET_RESET_PIN(0);
//    data_array[0] = 0x014F1500;
//    dsi_set_cmdq(data_array, 1, 1);
//   MDELAY(20);
}

static void lcm_resume(void)
{
    SET_RESET_PIN(1);
    MDELAY(5);
    SET_RESET_PIN(0);
    MDELAY(15);
    SET_RESET_PIN(1);
    MDELAY(120);  
    init_lcm_registers();
    //lcm_init();
}


static void lcm_setbacklight(unsigned int level)
{
    unsigned int data_array[16];


#if defined(BUILD_LK)
    printf("%s, %d\n", __func__, level);
#elif defined(BUILD_UBOOT)
    printf("%s, %d\n", __func__, level);
#else
    printk("lcm_setbacklight = %d\n", level);
#endif

    if(level > 255)
        level = 255;

    data_array[0]= 0x00023902;
    data_array[1] =(0x51|(level<<8));
    dsi_set_cmdq(data_array, 2, 1);
}


static void lcm_setpwm(unsigned int divider)
{
    // TBD
}


static unsigned int lcm_getpwm(unsigned int divider)
{
    // ref freq = 15MHz, B0h setting 0x80, so 80.6% * freq is pwm_clk;
    // pwm_clk / 255 / 2(lcm_setpwm() 6th params) = pwm_duration = 23706
    unsigned int pwm_clk = 23706 / (1<<divider);


    return pwm_clk;
}

////ESD start ///

static unsigned int lcm_esd_check(void)
{
#ifndef BUILD_UBOOT
    unsigned char buffer[2];
    int array[4];
    /// please notice: the max return packet size is 1
    /// if you want to change it, you can refer to the following marked code
    /// but read_reg currently only support read no more than 4 bytes....
    /// if you need to read more, please let BinHan knows.
    /*
            unsigned int data_array[16];
            unsigned int max_return_size = 1;

            data_array[0]= 0x00003700 | (max_return_size << 16);

            dsi_set_cmdq(&data_array, 1, 1);
    */
    unsigned int data_array[16];
    data_array[0]=0x00063902;
    data_array[1]=0x0698ffff;
    data_array[2]=0x00000004;
    dsi_set_cmdq(data_array, 3, 1);

    array[0]=0x00043700;//0x00023700;
    dsi_set_cmdq(array, 1, 1);

    read_reg_v2(0x0A, buffer, 4);
#if defined(BUILD_LK)
    printf("%s\n", __func__);
#elif defined(BUILD_UBOOT)
    printf("%s\n", __func__);
#else
    printk(" 9806e lcm_esd_check:%x",buffer[0]);
#endif
    //if(read_reg(0x0A) == 0x9C)
    if(buffer[0] == 0x9C)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
#endif
}

static unsigned int lcm_esd_recover(void)
{
//  unsigned char para = 0;
#if defined(BUILD_LK)
    printf("%s\n", __func__);
#elif defined(BUILD_UBOOT)
    printf("%s\n", __func__);
#else
    printk(" 9806e lcm_esd_recover");
#endif
    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(5);
    SET_RESET_PIN(1);
    MDELAY(20);

    init_lcm_registers();
    MDELAY(20);
    lcm_resume();
    MDELAY(20);
//    dsi_set_cmdq_V2(0x35, 1, &para, 1);     ///enable TE
//    MDELAY(10);
    return TRUE;
}
/////ESD end///
#if 0 //TODO shuld be use read ic id method
static unsigned int lcm_compare_id(void)
{
    int array[4];
    char buffer[5];
    char id_high=0;
    char id_midd=0;
    char id_low=0;
    int id=0;

    //Do reset here
    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(25);
    SET_RESET_PIN(1);
    MDELAY(120);
    array[0]=0x00063902;
    array[1]=0x0698ffff;
    array[2]=0x00000104;
    dsi_set_cmdq(array, 3, 1);
    MDELAY(10);

    array[0]=0x00023700;//0x00023700;
    dsi_set_cmdq(array, 1, 1);
    //read_reg_v2(0x04, buffer, 3);//if read 0x04,should get 0x008000,that is both OK.

    read_reg_v2(0x00, buffer,1);
    id_high = buffer[0]; ///////////////////////0x98

    array[0]=0x00043700;//0x00023700;
    dsi_set_cmdq(array, 1, 1);
    read_reg_v2(0x01, buffer,1);
    id_midd = buffer[0]; ///////////////////////0x06

    array[0]=0x00043700;//0x00023700;
    dsi_set_cmdq(array, 1, 1);
    read_reg_v2(0x02, buffer,1);
    id_low = buffer[0]; ////////////////////////0x04

    id = (id_midd << 8) | id_low;

#if defined(BUILD_UBOOT) || defined(BUILD_LK)
    printf("ILI9806: %s, id = 0x%x 0x%x 0x%x 0x%x 0x%x\n", __func__, id_high,id_midd,id_low,buffer[4]);//buffer[0],buffer[1],buffer[2],buffer[3],buffer[4] );
#else
    printk("ILI9806: %s, id = 0x%08x\n", __func__, id);
#endif

    return (ILI9806E_LCM_ID == id)?1:0;
}
#else
static unsigned int lcm_compare_id(void)
{
    int data[4];
    int adcVol=0;
    int res = IMM_GetOneChannelValue(AUXADC_LCM_VOLTAGE_CHANNEL, data , 0 );
    adcVol=data[0]*1000+data[1]*10;

#ifdef BUILD_LK
    printf("LINE=%d %s, res=%d,adcVol = %d \n", __LINE__,__func__,res,adcVol);
#else
    printk("LINE=%d %s, res=%d,adcVol = %d \n", __LINE__,__func__,res,adcVol);
#endif

    if(adcVol > MIN_VOLTAGE_TXD_NEW_GLASS && adcVol < MAX_VOLTAGE_TXD_NEW_GLASS)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
#endif



// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER ili9806e_fwvga_dsi_vdo_glass_drv =
{
    .name           = "ili9806e_fwvga_dsi_vdo_glass",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
    .suspend        = lcm_suspend,
    .resume         = lcm_resume,
//    .set_backlight    = lcm_setbacklight,
    //.set_pwm        = lcm_setpwm,
    //.get_pwm        = lcm_getpwm,
//ESD start ///
   // .esd_check   = lcm_esd_check,
    //.esd_recover   = lcm_esd_recover,
//ESD end ///
    .compare_id    = lcm_compare_id,
};

