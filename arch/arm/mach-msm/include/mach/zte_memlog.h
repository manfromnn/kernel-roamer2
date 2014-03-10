/* ========================================================================
Copyright (c) 2001-2009 by ZTE Corporation.  All Rights Reserved.        

-------------------------------------------------------------------------------------------------
   Modify History
-------------------------------------------------------------------------------------------------
When           Who        What 
20110211	JIANGFENG			关机充电动画移到appsboot, PM_JIANGFENG_20110211_01
20100611	JIANGFENG			Add ZTE_PLATFORM Micron, BOOT_JIANGFENG_20100611_01
20100413	JIANGFENG			Add ZTE_PLATFORM Micron, BOOT_JIANGFENG_20100413_01
=========================================================================== */
#ifndef ZTE_MEMLOG_H
#define ZTE_MEMLOG_H
#include <mach/msm_iomap.h>

#define SMEM_LOG_INFO_BASE    MSM_SMEM_RAM_PHYS
#define SMEM_LOG_GLOBAL_BASE  (MSM_SMEM_RAM_PHYS + PAGE_SIZE)

#define SMEM_LOG_ENTRY_OFFSET (64*PAGE_SIZE)
#define SMEM_LOG_ENTRY_BASE   (MSM_SMEM_RAM_PHYS + SMEM_LOG_ENTRY_OFFSET)

//Define Vol Up and Down Key magic, ZTE_BOOT_LIWEI_20110815
#define MAGIC_VOLUME_DOWN_KEY 0x75898668 //"KYVD"
#define MAGIC_VOLUME_UP_KEY 0x75898680  //"KYVP"

#define ERR_DATA_MAX_SIZE 0x4000


//PM_JIANGFENG_20110211_01,start
typedef struct
{
	unsigned char display_flag;
	unsigned char img_id;
	unsigned char chg_fulled;
	unsigned char battery_capacity;
	unsigned char battery_valid;
} power_off_supply_status;
//PM_JIANGFENG_20110211_01,end

typedef struct {
  unsigned int ftm;
  unsigned int boot_reason;
  unsigned int reset_reason;
  unsigned int chg_count;
  unsigned int f3log;
  unsigned int err_fatal;
  unsigned int err_dload;
  char err_log[ERR_DATA_MAX_SIZE];
  unsigned char flash_id[2];//BOOT_JIANGFENG_20100413_01
  unsigned char sdrem_length;//0:128M; 1:256M; 2:384M; 3:512M; ...		//BOOT_JIANGFENG_20100612_01
//PM_JIANGFENG_20110211_01,start
  unsigned char power_off_charge;	//0:正常开机;   1:正常的关机充电; 2:关机充电并显示动画
  power_off_supply_status power_off_charge_info;
//PM_JIANGFENG_20110211_01,end

  unsigned int boot_pressed_keys[2];//0-upkey, 1-downkey;ZTE_BOOT_LIWEI_20110815
  unsigned int usb_charge_disable;//added by zhang.yu_1 for usb charge ena/dis @110907
  unsigned int mboard_id;
  unsigned int lcd_id;
   
  unsigned int rtc_alarm;
  unsigned int secboot_enable; 

} smem_global;

#endif
