/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/


#ifdef BUILD_LK
#else
#include <linux/string.h>
#if defined(BUILD_UBOOT)
#include <asm/arch/mt_gpio.h>
#else
//#include <mach/mt_gpio.h>
#endif
#endif
#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1280)

#define LCM_RM68200_ID 										(0x6820)

#define REGFLAG_DELAY             								(0XFFFE)
#define REGFLAG_END_OF_TABLE      								(0xFFFF)	// END OF REGISTERS MARKER

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    									(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 										(lcm_util.udelay(n))
#define MDELAY(n) 										(lcm_util.mdelay(n))

//static unsigned int lcm_esd_test = FALSE;      ///only for ESD test

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)						lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)			lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)						lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)			lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

 struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {
/*Note :
Data ID will depends on the following rule.
count of parameters > 1      => Data ID = 0x39
count of parameters = 1      => Data ID = 0x15
count of parameters = 0      => Data ID = 0x05
Struclcm_deep_sleep_mode_in_settingture Format :
{DCS command, count of parameters, {parameter list}}
{REGFLAG_DELAY, milliseconds of time, {}},
...
Setting ending by predefined flag
{REGFLAG_END_OF_TABLE, 0x00, {}}
*/

{0xFE, 1,{0x01}},
{0x24, 1,{0xC0}},
{0x25, 1,{0x53}},
{0x26, 1,{0x00}},
{0x27, 1,{0x0A}},
{0x29, 1,{0x0A}},
{0x2B, 1,{0xE5}},
{0x2F, 1,{0x44}},
{0x34, 1,{0x55}},
{0x1B, 1,{0x00}},
{0x12, 1,{0x08}},
{0x46, 1,{0x85}},
{0x52, 1,{0xA8}},
{0x53, 1,{0x00}},
{0x54, 1,{0xA8}},
{0x55, 1,{0x00}},
{0x5F, 1,{0x13}}, //13 4 LANE /12 3lane
//{REGFLAG_DELAY, 200, {}},
{0xFE, 1,{0x03}},
{0x00, 1,{0x04}},
{0x01, 1,{0x14}},
{0x02, 1,{0x01}},
{0x03, 1,{0x00}},
{0x04, 1,{0x00}},
{0x05, 1,{0x00}},
{0x06, 1,{0x00}},
{0x07, 1,{0x00}},
{0x08, 1,{0x00}},
{0x09, 1,{0x00}},
{0x0A, 1,{0x00}},
{0x0B, 1,{0x00}},
{0x0C, 1,{0x00}},
{0x0D, 1,{0x00}},
{0x0E, 1,{0x05}},
{0x0F, 1,{0x06}},
{0x10, 1,{0x07}},
{0x11, 1,{0x08}},
{0x12, 1,{0x00}},
{0x13, 1,{0x40}},
{0x14, 1,{0x00}},
{0x15, 1,{0xC4}},
{0x16, 1,{0x08}},
{0x17, 1,{0x09}},
{0x18, 1,{0x0A}},
{0x19, 1,{0x0B}},
{0x1A, 1,{0x0C}},
{0x1B, 1,{0x00}},
{0x1C, 1,{0x40}},
{0x1D, 1,{0x00}},
{0x1E, 1,{0x84}},
{0x1F, 1,{0x08}},
{0x20, 1,{0x00}},
{0x21, 1,{0x00}},
{0x22, 1,{0x05}},
{0x23, 1,{0x0C}},
{0x24, 1,{0x00}},
{0x25, 1,{0x28}},
{0x26, 1,{0x00}},
{0x27, 1,{0x10}},
{0x28, 1,{0x00}},
{0x29, 1,{0x28}},
{0x2A, 1,{0x00}},
{0x2B, 1,{0x00}},
{0x2D, 1,{0x00}},
{0x2F, 1,{0x00}},
{0x30, 1,{0x00}},
{0x31, 1,{0x00}},
{0x32, 1,{0x00}},
{0x33, 1,{0x00}},
{0x34, 1,{0x00}},
{0x35, 1,{0x00}},
{0x36, 1,{0x00}},
{0x37, 1,{0x00}},
{0x38, 1,{0x00}},
{0x39, 1,{0x00}},
{0x3A, 1,{0x00}},
{0x3B, 1,{0x00}},
{0x3D, 1,{0x00}},
{0x3F, 1,{0x00}},
{0x40, 1,{0x00}},
{0x41, 1,{0x00}},
{0x42, 1,{0x00}},
{0x43, 1,{0x00}},
{0x44, 1,{0x00}},
{0x45, 1,{0x00}},
{0x46, 1,{0x00}},
{0x47, 1,{0x00}},
{0x48, 1,{0x00}},
{0x49, 1,{0x00}},
{0x4A, 1,{0x00}},
{0x4B, 1,{0x00}},
{0x4C, 1,{0x00}},
{0x4D, 1,{0x00}},
{0x4E, 1,{0x00}},
{0x4F, 1,{0x00}},
{0x50, 1,{0x00}},
{0x51, 1,{0x00}},
{0x52, 1,{0x00}},
{0x53, 1,{0x00}},
{0x54, 1,{0x00}},
{0x55, 1,{0x00}},
{0x56, 1,{0x00}},
{0x58, 1,{0x00}},
{0x59, 1,{0x00}},
{0x5A, 1,{0x00}},
{0x5B, 1,{0x00}},
{0x5C, 1,{0x00}},
{0x5D, 1,{0x00}},
{0x5E, 1,{0x00}},
{0x5F, 1,{0x00}},
{0x60, 1,{0x00}},
{0x61, 1,{0x00}},
{0x62, 1,{0x00}},
{0x63, 1,{0x00}},
{0x64, 1,{0x00}},
{0x65, 1,{0x00}},
{0x66, 1,{0x00}},
{0x67, 1,{0x00}},
{0x68, 1,{0x00}},
{0x69, 1,{0x00}},
{0x6A, 1,{0x00}},
{0x6B, 1,{0x00}},
{0x6C, 1,{0x00}},
{0x6D, 1,{0xBC}},
{0x6E, 1,{0x20}},
{0x6F, 1,{0x00}},
{0x70, 1,{0x00}},
{0x71, 1,{0x88}},
{0x72, 1,{0x00}},
{0x73, 1,{0x00}},
{0x74, 1,{0x01}},
{0x75, 1,{0x01}},
{0x76, 1,{0x01}},
{0x77, 1,{0x01}},
{0x78, 1,{0x00}},
{0x79, 1,{0x00}},
{0x7A, 1,{0x00}},
{0x7B, 1,{0x00}},
{0x7C, 1,{0x00}},
{0x7D, 1,{0x00}},
{0x7E, 1,{0x1D}},
{0x7F, 1,{0x1C}},
{0x80, 1,{0x1A}},
{0x81, 1,{0x1B}},
{0x82, 1,{0x0D}},
{0x83, 1,{0x0F}},
{0x84, 1,{0x09}},
{0x85, 1,{0x0B}},
{0x86, 1,{0x01}},
{0x87, 1,{0x3F}},
{0x88, 1,{0x03}},
{0x89, 1,{0x1C}},
{0x8A, 1,{0x3F}},
{0x8B, 1,{0x1C}},
{0x8C, 1,{0x1C}},
{0x8D, 1,{0x3F}},
{0x8E, 1,{0x3F}},
{0x8F, 1,{0x3F}},
{0x90, 1,{0x3F}},
{0x91, 1,{0x3F}},
{0x92, 1,{0x3F}},
{0x93, 1,{0x3F}},
{0x94, 1,{0x3F}},
{0x95, 1,{0x3F}},
{0x96, 1,{0x3F}},
{0x97, 1,{0x3F}},
{0x98, 1,{0x3F}},
{0x99, 1,{0x3F}},
{0x9A, 1,{0x3F}},
{0x9B, 1,{0x1C}},
{0x9C, 1,{0x1C}},
{0x9D, 1,{0x3F}},
{0x9E, 1,{0x1C}},
{0x9F, 1,{0x02}},
{0xA0, 1,{0x3F}},
{0xA2, 1,{0x00}},
{0xA3, 1,{0x0A}},
{0xA4, 1,{0x08}},
{0xA5, 1,{0x0E}},
{0xA6, 1,{0x0C}},
{0xA7, 1,{0x19}},
{0xA9, 1,{0x18}},
{0xAA, 1,{0x1C}},
{0xAB, 1,{0x1D}},
{0xAC, 1,{0x1C}},
{0xAD, 1,{0x1D}},
{0xAE, 1,{0x1A}},
{0xAF, 1,{0x1B}},
{0xB0, 1,{0x0A}},
{0xB1, 1,{0x08}},
{0xB2, 1,{0x0E}},
{0xB3, 1,{0x0C}},
{0xB4, 1,{0x02}},
{0xB5, 1,{0x3F}},
{0xB6, 1,{0x00}},
{0xB7, 1,{0x1C}},
{0xB8, 1,{0x3F}},
{0xB9, 1,{0x1C}},
{0xBA, 1,{0x1C}},
{0xBB, 1,{0x3F}},
{0xBC, 1,{0x3F}},
{0xBD, 1,{0x3F}},
{0xBE, 1,{0x3F}},
{0xBF, 1,{0x3F}},
{0xC0, 1,{0x3F}},
{0xC1, 1,{0x3F}},
{0xC2, 1,{0x3F}},
{0xC3, 1,{0x3F}},
{0xC4, 1,{0x3F}},
{0xC5, 1,{0x3F}},
{0xC6, 1,{0x3F}},
{0xC7, 1,{0x3F}},
{0xC8, 1,{0x3F}},
{0xC9, 1,{0x1C}},
{0xCA, 1,{0x1C}},
{0xCB, 1,{0x3F}},
{0xCC, 1,{0x1C}},
{0xCD, 1,{0x01}},
{0xCE, 1,{0x3F}},
{0xCF, 1,{0x03}},
{0xD0, 1,{0x0D}},
{0xD1, 1,{0x0F}},
{0xD2, 1,{0x09}},
{0xD3, 1,{0x0B}},
{0xD4, 1,{0x19}},
{0xD5, 1,{0x18}},
{0xD6, 1,{0x1D}},
{0xD7, 1,{0x1C}},
{0xDC, 1,{0x02}},
{0xDE, 1,{0x0C}},
{0xDF, 1,{0x00}},
//{REGFLAG_DELAY, 200, {}},
{0xFE, 1,{0x0E}},
{0x01, 1,{0x75}},
{0xFE, 1,{0x04}},
{0x60, 1,{0x00}},
{0x61, 1,{0x0A}},
{0x62, 1,{0x10}},
{0x63, 1,{0x0E}},
{0x64, 1,{0x06}},
{0x65, 1,{0x12}},
{0x66, 1,{0x0E}},
{0x67, 1,{0x0A}},
{0x68, 1,{0x17}},
{0x69, 1,{0x0D}},
{0x6A, 1,{0x11}},
{0x6B, 1,{0x08}},
{0x6C, 1,{0x0F}},
{0x6D, 1,{0x13}},
{0x6E, 1,{0x0B}},
{0x6F, 1,{0x00}},
{0x70, 1,{0x00}},
{0x71, 1,{0x0A}},
{0x72, 1,{0x10}},
{0x73, 1,{0x0E}},
{0x74, 1,{0x06}},
{0x75, 1,{0x12}},
{0x76, 1,{0x0E}},
{0x77, 1,{0x0A}},
{0x78, 1,{0x17}},
{0x79, 1,{0x0D}},
{0x7A, 1,{0x11}},
{0x7B, 1,{0x08}},
{0x7C, 1,{0x0F}},
{0x7D, 1,{0x13}},
{0x7E, 1,{0x0B}},
{0x7F, 1,{0x00}},
//{REGFLAG_DELAY, 200, {}},
{0xFE, 1,{0x00}},
{0x58, 1,{0xA9}},
{0x11, 0,{0x00}},
{REGFLAG_DELAY, 150, {}},
{0x29, 0,{0x00}},
{REGFLAG_DELAY, 100, {}},
{REGFLAG_END_OF_TABLE, 0x00, {}}
};
	//{0x35, 0,{0x00}},
	//{REGFLAG_DELAY, 200, {}},
	//{REGFLAG_END_OF_TABLE, 0x00, {}}
//};

static struct LCM_setting_table lcm_sleep_in_setting[] = {
	// Display off sequence
	{0x01, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},
	
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},

	// Sleep Mode On
	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count,
		unsigned char force_update)
{
	unsigned int i;

	for (i = 0; i < count; i++) {

		unsigned cmd;
		cmd = table[i].cmd;

		switch (cmd) {

		case REGFLAG_DELAY:
			MDELAY(table[i].count);
			break;

		case REGFLAG_END_OF_TABLE:
			break;

		default:
			dsi_set_cmdq_V2(cmd, table[i].count,
					table[i].para_list, force_update);
		}
	}

}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS * util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS * params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type = LCM_TYPE_DSI;

	params->width = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	// enable tearing-free
	params->dbi.te_mode = LCM_DBI_TE_MODE_DISABLED;
	params->dbi.te_edge_polarity = LCM_POLARITY_RISING;

	params->dsi.mode   = SYNC_PULSE_VDO_MODE;//SYNC_EVENT_VDO_MODE;//BURST_VDO_MODE;////

	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM				= LCM_FOUR_LANE;
	
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;
	
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	
	params->dsi.intermediat_buffer_num = 0;	//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

	// Video mode setting
	params->dsi.packet_size=256;

	params->dsi.vertical_sync_active				=  2;//2
	params->dsi.vertical_backporch					= 14;//50;
	params->dsi.vertical_frontporch					= 16;//50;
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active				= 8;//10
	params->dsi.horizontal_backporch				= 48;//34; 
	params->dsi.horizontal_frontporch				= 44;//24;
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

	// Bit rate calculation
	//1 Every lane speed
	params->dsi.PLL_CLOCK=220;
	params->dsi.noncont_clock = 1;
	params->dsi.noncont_clock_period = 1;

/*	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
	params->dsi.lcm_esd_check_table[0].cmd = 0x0a;
	params->dsi.lcm_esd_check_table[0].count = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;
*/
}

static void lcm_init(void)
{

	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(120);

	push_table(lcm_initialization_setting,sizeof(lcm_initialization_setting) /sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{
     push_table(lcm_sleep_in_setting,sizeof(lcm_sleep_in_setting) /sizeof(struct LCM_setting_table), 1);
}


static void lcm_resume(void)
{   
	lcm_init();
}


static unsigned int lcm_compare_id(void)
{
	int array[4];
	char buffer[5];
	char id_high=0;
	char id_low=0;
	int id1=0;
	int id2=0;

	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(200);
	array[0]=0x01FE1500;
	dsi_set_cmdq(array,1, 1);

	array[0] = 0x00013700;
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0xde, buffer, 1);

	id_high = buffer[0];
	read_reg_v2(0xdf, buffer, 1);
	id_low = buffer[0];
	id1 = (id_high<<8) | id_low;

	#if defined(BUILD_LK)
	printf("rm68200a %s id1 = 0x%04x, id2 = 0x%04x\n", __func__, id1,id2);
	#else
	printk("rm68200a %s id1 = 0x%04x, id2 = 0x%04x\n", __func__, id1,id2);
	#endif
	return (LCM_RM68200_ID == id1)?1:0;
}

/*
模组厂:新世界
*/

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER hct_rm68200_dsi_vdo_hd_hsd_50_xsj = 
{
	.name			= "hct_rm68200_dsi_vdo_hd_hsd_50_xsj",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,	
	.compare_id     = lcm_compare_id,	
};
