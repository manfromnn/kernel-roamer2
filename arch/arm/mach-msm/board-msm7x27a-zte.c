/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio_event.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <mach/board.h>
#include <mach/msm_iomap.h>
#include <mach/msm_hsusb.h>
#include <mach/rpc_hsusb.h>
#include <mach/rpc_pmapp.h>
#include <mach/usbdiag.h>
#include <mach/msm_memtypes.h>
#include <mach/msm_serial_hs.h>
#include <linux/usb/android.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <mach/vreg.h>
#include <mach/pmic.h>
#include <mach/socinfo.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <asm/mach/mmc.h>
#include <linux/i2c.h>
#include <linux/i2c/sx150x.h>
#include <linux/gpio.h>
#include <linux/android_pmem.h>
#include <linux/bootmem.h>
#include <linux/mfd/marimba.h>
#include <mach/vreg.h>
#include <linux/power_supply.h>
#include <linux/regulator/consumer.h>
#include <mach/rpc_pmapp.h>
#include "proc_comm.h"

#include <mach/msm_battery.h>
#include <linux/smsc911x.h>
#include "devices.h"
#include "timer.h"
#include "board-msm7x27a-regulator.h"
#include "devices-msm7x2xa.h"
#include "pm.h"
#include <mach/rpc_server_handset.h>
#include <mach/socinfo.h>
#include <asm/setup.h>
#include "pm-boot.h"
#include "board-msm7627a.h"


#ifdef CONFIG_SENSORS_AKM8962C //used by Freescale && Kionix  
#include <linux/akm8962_new.h>
#endif
#ifdef CONFIG_PN544_NFC
#include <linux/nfc/pn544.h>
#endif
#include <mach/camera.h>

#ifdef CONFIG_ZTE_PLATFORM  // merge log system
#include <mach/zte_memlog.h>
#include <linux/proc_fs.h>
#endif
#ifdef CONFIG_SENSORS_AKM8962C //used by Freescale && Kionix  
#include <linux/akm8962_new.h>
#endif
#ifdef CONFIG_SENSORS_ST_LIS3DHTR 
#include <linux/lis3dh.h>
#endif
#ifdef CONFIG_MXC_MMA8452
#include <linux/mma854x.h> 
#endif

#ifdef CONFIG_SENSORS_KXTIK 
#include <linux/kxtik.h> 
#endif

#define PMEM_KERNEL_EBI1_SIZE	0x3A000
#define MSM_PMEM_AUDIO_SIZE	0x5B000
#if  0
#define BAHAMA_SLAVE_ID_FM_ADDR         0x2A
#define BAHAMA_SLAVE_ID_QMEMBIST_ADDR   0x7B
#define BAHAMA_SLAVE_ID_FM_REG 0x02
#define FM_GPIO	83
#define BT_PCM_BCLK_MODE  0x88
#define BT_PCM_DIN_MODE   0x89
#define BT_PCM_DOUT_MODE  0x8A
#define BT_PCM_SYNC_MODE  0x8B
#define FM_I2S_SD_MODE    0x8E
#define FM_I2S_WS_MODE    0x8F
#define FM_I2S_SCK_MODE   0x90
#define I2C_PIN_CTL       0x15
#define I2C_NORMAL        0x40
#endif

#ifdef CONFIG_ZTE_PLATFORM
#ifdef CONFIG_ANDROID_RAM_CONSOLE
#define MSM_RAM_CONSOLE_PHYS  0xFF00000
#define MSM_RAM_CONSOLE_SIZE  SZ_1M
#endif
//static smem_global *global;
// end
// start
static int g_zte_board_id_type;
static int g_zte_secboot_mode;
// end
// start
int get_zte_board_id_type(void);
extern void sync_sysfs_board_id(int flag);
// end
#endif


#if defined(CONFIG_GPIO_SX150X)
enum {
	SX150X_CORE,
	SX150X_CAM,
};

static struct sx150x_platform_data sx150x_data[] __initdata = {
	[SX150X_CORE]	= {
		.gpio_base		= GPIO_CORE_EXPANDER_BASE,
		.oscio_is_gpo		= false,
		.io_pullup_ena		= 0,
		.io_pulldn_ena		= 0x02,
		.io_open_drain_ena	= 0xfef8,
		.irq_summary		= -1,
	},
	[SX150X_CAM]	= {
		.gpio_base		= GPIO_CAM_EXPANDER_BASE,
		.oscio_is_gpo		= false,
		.io_pullup_ena		= 0,
		.io_pulldn_ena		= 0,
		.io_open_drain_ena	= 0x23,
		.irq_summary		= -1,
	},
};
#endif

#ifdef CONFIG_ZTE_PLATFORM
//compatible of qualcomm and broadcomm bluetooth chip     
#define ATAG_ZTEFTM  0x5d53cd73
static int g_zte_ftm_flag_fixup;

static int  parse_tag_zte_ftm(const struct tag *tags)
{
        int flag = 0, find = 0;
        struct tag *t = (struct tag *)tags;

        for (; t->hdr.size; t = tag_next(t)) {
                if (t->hdr.tag == ATAG_ZTEFTM) {
                        printk(KERN_DEBUG "find the zte ftm tag\n");
                        find = 1;
                        break;
                }
        }

        if (find)
                flag = t->u.revision.rev;
        printk(KERN_INFO "[ZYF@FTM]parse_tag_zteftm: zte FTM %s !\n",
	       flag?"enable":"disable");
        return flag;
}


int get_ftm_from_tag(void)
{
	return g_zte_ftm_flag_fixup;
}
EXPORT_SYMBOL(get_ftm_from_tag);

static void __init zte_fixup(struct machine_desc *desc, struct tag *tags,
			     char **cmdline, struct meminfo *mi)
{
#ifdef CONFIG_ZTE_FTM_FLAG_SUPPORT
        g_zte_ftm_flag_fixup = parse_tag_zte_ftm((const struct tag *)tags);
#endif
}
#endif //CONFIG_ZTE_PLATFORM


#ifdef CONFIG_LEDS_MSM_PMIC//added by zhang.yu_1 for enabling CONFIG_LEDS_MSM_PMIC @110823
static struct platform_device msm_device_pmic_leds = {
	.name   = "pmic-leds",
	.id = -1,
};
#endif

#if 0
#if defined(CONFIG_BT) && defined(CONFIG_MARIMBA_CORE)

	/* FM Platform power and shutdown routines */
#define FPGA_MSM_CNTRL_REG2 0x90008010
static int switch_pcm_i2s_reg_mode(int mode)
{
	unsigned char reg = 0;
	int rc = -1;
	unsigned char set = I2C_PIN_CTL; /*SET PIN CTL mode*/
	unsigned char unset = I2C_NORMAL; /* UNSET PIN CTL MODE*/
	struct marimba config = { .mod_id =  SLAVE_ID_BAHAMA};

	if (mode == 0) {
		/* as we need to switch path to FM we need to move
		 BT AUX PCM lines to PIN CONTROL mode then move
		 FM to normal mode.*/
		for (reg = BT_PCM_BCLK_MODE; reg <= BT_PCM_SYNC_MODE; reg++) {
			rc = marimba_write(&config, reg, &set, 1);
			if (rc < 0) {
				pr_err("pcm pinctl failed = %d", rc);
				goto err_all;
			}
		}
		for (reg = FM_I2S_SD_MODE; reg <= FM_I2S_SCK_MODE; reg++) {
			rc = marimba_write(&config, reg, &unset, 1);
			if (rc < 0) {
				pr_err("i2s normal failed = %d", rc);
				goto err_all;
			}
		}
	} else {
		/* as we need to switch path to AUXPCM we need to move
		 FM I2S lines to PIN CONTROL mode then move
		 BT AUX_PCM to normal mode.*/
		for (reg = FM_I2S_SD_MODE; reg <= FM_I2S_SCK_MODE; reg++) {
			rc = marimba_write(&config, reg, &set, 1);
			if (rc < 0) {
				pr_err("i2s pinctl failed = %d", rc);
				goto err_all;
			}
		}
		for (reg = BT_PCM_BCLK_MODE; reg <= BT_PCM_SYNC_MODE; reg++) {
			rc = marimba_write(&config, reg, &unset, 1);
			if (rc < 0) {
				pr_err("pcm normal failed = %d", rc);
				goto err_all;
			}
		}
	}

	return 0;

err_all:
	return rc;
}

static void config_pcm_i2s_mode(int mode)
{
	void __iomem *cfg_ptr;
	u8 reg2;

	cfg_ptr = ioremap_nocache(FPGA_MSM_CNTRL_REG2, sizeof(char));

	if (!cfg_ptr)
		return;
	if (mode) {
		/*enable the pcm mode in FPGA*/
		reg2 = readb_relaxed(cfg_ptr);
		if (reg2 == 0) {
			reg2 = 1;
			writeb_relaxed(reg2, cfg_ptr);
		}
	} else {
		/*enable i2s mode in FPGA*/
		reg2 = readb_relaxed(cfg_ptr);
		if (reg2 == 1) {
			reg2 = 0;
			writeb_relaxed(reg2, cfg_ptr);
		}
	}
	iounmap(cfg_ptr);
}

static unsigned fm_i2s_config_power_on[] = {
	/*FM_I2S_SD*/
	GPIO_CFG(68, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	/*FM_I2S_WS*/
	GPIO_CFG(70, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	/*FM_I2S_SCK*/
	GPIO_CFG(71, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};

static unsigned fm_i2s_config_power_off[] = {
	/*FM_I2S_SD*/
	GPIO_CFG(68, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	/*FM_I2S_WS*/
	GPIO_CFG(70, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	/*FM_I2S_SCK*/
	GPIO_CFG(71, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
};

static unsigned bt_config_power_on[] = {
	/*RFR*/
	GPIO_CFG(43, 2, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	/*CTS*/
	GPIO_CFG(44, 2, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	/*RX*/
	GPIO_CFG(45, 2, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	/*TX*/
	GPIO_CFG(46, 2, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
#if defined(CONFIG_BCM_BT_SLEEP_GPIO)
    /*ext_wake*/
	GPIO_CFG(116, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL,   GPIO_CFG_2MA),
	/*host_wake*/
	GPIO_CFG(83, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN,  GPIO_CFG_2MA),
#endif			
};
static unsigned bt_config_pcm_on[] = {
	/*PCM_DOUT*/
	GPIO_CFG(68, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	/*PCM_DIN*/
	GPIO_CFG(69, 1, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	/*PCM_SYNC*/
	GPIO_CFG(70, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	/*PCM_CLK*/
	GPIO_CFG(71, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};
static unsigned bt_config_power_off[] = {
	/*RFR*/
	GPIO_CFG(43, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	/*CTS*/
	GPIO_CFG(44, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	/*RX*/
	GPIO_CFG(45, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	/*TX*/
	GPIO_CFG(46, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
#if defined(CONFIG_BCM_BT_SLEEP_GPIO)
    /*ext_wake*/
	GPIO_CFG(116, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN,   GPIO_CFG_2MA),	
	/*host_wake*/
    GPIO_CFG(83, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN,   GPIO_CFG_2MA),
#endif
};
static unsigned bt_config_pcm_off[] = {
	/*PCM_DOUT*/
	GPIO_CFG(68, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	/*PCM_DIN*/
	GPIO_CFG(69, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	/*PCM_SYNC*/
	GPIO_CFG(70, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
	/*PCM_CLK*/
	GPIO_CFG(71, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
};

static int config_i2s(int mode)
{
	int pin, rc = 0;

	if (mode == FM_I2S_ON) {
		if (1 || machine_is_msm7625a_surf())
			config_pcm_i2s_mode(0);
		pr_err("%s mode = FM_I2S_ON", __func__);

		rc = switch_pcm_i2s_reg_mode(0);
		if (rc) {
			pr_err("switch mode failed");
			return rc;
		}
		for (pin = 0; pin < ARRAY_SIZE(fm_i2s_config_power_on);
			pin++) {
				rc = gpio_tlmm_config(
					fm_i2s_config_power_on[pin],
					GPIO_CFG_ENABLE
					);
				if (rc < 0)
					return rc;
			}
	} else if (mode == FM_I2S_OFF) {
		pr_err("%s mode = FM_I2S_OFF", __func__);
		rc = switch_pcm_i2s_reg_mode(1);
		if (rc) {
			pr_err("switch mode failed");
			return rc;
		}
		for (pin = 0; pin < ARRAY_SIZE(fm_i2s_config_power_off);
			pin++) {
				rc = gpio_tlmm_config(
					fm_i2s_config_power_off[pin],
					GPIO_CFG_ENABLE
					);
				if (rc < 0)
					return rc;
			}
	}
	return rc;
}
static int config_pcm(int mode)
{
	int pin, rc = 0;

	if (mode == BT_PCM_ON) {
		if (1 || machine_is_msm7625a_surf())
			config_pcm_i2s_mode(1);
		pr_err("%s mode =BT_PCM_ON", __func__);
		rc = switch_pcm_i2s_reg_mode(1);
		if (rc) {
			pr_err("switch mode failed");
			return rc;
		}
		for (pin = 0; pin < ARRAY_SIZE(bt_config_pcm_on);
			pin++) {
				rc = gpio_tlmm_config(bt_config_pcm_on[pin],
					GPIO_CFG_ENABLE);
				if (rc < 0)
					return rc;
			}
	} else if (mode == BT_PCM_OFF) {
		pr_err("%s mode =BT_PCM_OFF", __func__);
		rc = switch_pcm_i2s_reg_mode(0);
		if (rc) {
			pr_err("switch mode failed");
			return rc;
		}
		for (pin = 0; pin < ARRAY_SIZE(bt_config_pcm_off);
			pin++) {
				rc = gpio_tlmm_config(bt_config_pcm_off[pin],
					GPIO_CFG_ENABLE);
				if (rc < 0)
					return rc;
			}

	}

	return rc;
}

static int msm_bahama_setup_pcm_i2s(int mode)
{
	int fm_state = 0, bt_state = 0;
	int rc = 0;
	struct marimba config = { .mod_id =  SLAVE_ID_BAHAMA};

	fm_state = marimba_get_fm_status(&config);
	bt_state = marimba_get_bt_status(&config);

	switch (mode) {
	case BT_PCM_ON:
	case BT_PCM_OFF:
		if (!fm_state)
			rc = config_pcm(mode);
		break;
	case FM_I2S_ON:
		rc = config_i2s(mode);
		break;
	case FM_I2S_OFF:
		if (bt_state)
			rc = config_pcm(BT_PCM_ON);
		else
			rc = config_i2s(mode);
		break;
	default:
		rc = -EIO;
		pr_err("%s:Unsupported mode", __func__);
	}
	return rc;
}

static int bt_set_gpio(int on)
{
	int rc = 0;
	struct marimba config = { .mod_id =  SLAVE_ID_BAHAMA};

	if (on) {
		rc = gpio_direction_output(GPIO_BT_SYS_REST_EN, 1);
		msleep(100);
	} else {
		if (!marimba_get_fm_status(&config) &&
				!marimba_get_bt_status(&config)) {
			gpio_set_value_cansleep(GPIO_BT_SYS_REST_EN, 0);
			rc = gpio_direction_input(GPIO_BT_SYS_REST_EN);
			msleep(100);
		}
	}
	if (rc)
		pr_err("%s: BT sys_reset_en GPIO : Error", __func__);

	return rc;
}
static struct regulator *fm_regulator;
static int fm_radio_setup(struct marimba_fm_platform_data *pdata)
{
	int rc = 0;
	const char *id = "FMPW";
	uint32_t irqcfg;
	struct marimba config = { .mod_id =  SLAVE_ID_BAHAMA};
	u8 value;

	/* Voting for 1.8V Regulator */
	fm_regulator = regulator_get(NULL, "msme1");
	if (IS_ERR(fm_regulator)) {
		rc = PTR_ERR(fm_regulator);
		pr_err("%s: could not get regulator: %d\n", __func__, rc);
		goto out;
	}

	/* Set the voltage level to 1.8V */
	rc = regulator_set_voltage(fm_regulator, 1800000, 1800000);
	if (rc < 0) {
		pr_err("%s: could not set voltage: %d\n", __func__, rc);
		goto reg_free;
	}

	/* Enabling the 1.8V regulator */
	rc = regulator_enable(fm_regulator);
	if (rc) {
		pr_err("%s: could not enable regulator: %d\n", __func__, rc);
		goto reg_free;
	}

	/* Voting for 19.2MHz clock */
	rc = pmapp_clock_vote(id, PMAPP_CLOCK_ID_D1,
			PMAPP_CLOCK_VOTE_ON);
	if (rc < 0) {
		pr_err("%s: clock vote failed with :(%d)\n",
			 __func__, rc);
		goto reg_disable;
	}

	rc = bt_set_gpio(1);
	if (rc) {
		pr_err("%s: bt_set_gpio = %d", __func__, rc);
		goto gpio_deconfig;
	}
	/*re-write FM Slave Id, after reset*/
	value = BAHAMA_SLAVE_ID_FM_ADDR;
	rc = marimba_write_bit_mask(&config,
			BAHAMA_SLAVE_ID_FM_REG, &value, 1, 0xFF);
	if (rc < 0) {
		pr_err("%s: FM Slave ID rewrite Failed = %d", __func__, rc);
		goto gpio_deconfig;
	}
	/* Configuring the FM GPIO */
	irqcfg = GPIO_CFG(FM_GPIO, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL,
			GPIO_CFG_2MA);

	rc = gpio_tlmm_config(irqcfg, GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("%s: gpio_tlmm_config(%#x)=%d\n",
			 __func__, irqcfg, rc);
		goto gpio_deconfig;
	}

	return 0;

gpio_deconfig:
	pmapp_clock_vote(id, PMAPP_CLOCK_ID_D1,
		PMAPP_CLOCK_VOTE_OFF);
	bt_set_gpio(0);
reg_disable:
	regulator_disable(fm_regulator);
reg_free:
	regulator_put(fm_regulator);
	fm_regulator = NULL;
out:
	return rc;
};

static void fm_radio_shutdown(struct marimba_fm_platform_data *pdata)
{
	int rc;
	const char *id = "FMPW";

	/* Releasing the GPIO line used by FM */
	uint32_t irqcfg = GPIO_CFG(FM_GPIO, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP,
		GPIO_CFG_2MA);

	rc = gpio_tlmm_config(irqcfg, GPIO_CFG_ENABLE);
	if (rc)
		pr_err("%s: gpio_tlmm_config(%#x)=%d\n",
			 __func__, irqcfg, rc);

	/* Releasing the 1.8V Regulator */
	if (!IS_ERR_OR_NULL(fm_regulator)) {
		rc = regulator_disable(fm_regulator);
		if (rc)
			pr_err("%s: could not disable regulator: %d\n",
					__func__, rc);
		regulator_put(fm_regulator);
		fm_regulator = NULL;
	}

	/* Voting off the clock */
	rc = pmapp_clock_vote(id, PMAPP_CLOCK_ID_D1,
		PMAPP_CLOCK_VOTE_OFF);
	if (rc < 0)
		pr_err("%s: voting off failed with :(%d)\n",
			__func__, rc);
	rc = bt_set_gpio(0);
	if (rc)
		pr_err("%s: bt_set_gpio = %d", __func__, rc);
}

static struct marimba_fm_platform_data marimba_fm_pdata = {
	.fm_setup = fm_radio_setup,
	.fm_shutdown = fm_radio_shutdown,
	.irq = MSM_GPIO_TO_INT(FM_GPIO),
	.vreg_s2 = NULL,
	.vreg_xo_out = NULL,
	/* Configuring the FM SoC as I2S Master */
	.is_fm_soc_i2s_master = true,
	.config_i2s_gpio = msm_bahama_setup_pcm_i2s,
};




static struct platform_device msm_bt_power_device = {
	.name = "bt_power",
};
struct bahama_config_register {
		u8 reg;
		u8 value;
		u8 mask;
};
struct bt_vreg_info {
	const char *name;
	unsigned int pmapp_id;
	unsigned int min_level;
	unsigned int max_level;
	unsigned int is_pin_controlled;
	struct regulator *reg;
};
static struct bt_vreg_info bt_vregs[] = {
	{"msme1", 2, 1800000, 1800000, 0, NULL},
	{"bt", 21, 2600000, 3300000, 1, NULL}
};

static int bahama_bt(int on)
{
	int rc = 0;
	int i;

	struct marimba config = { .mod_id =  SLAVE_ID_BAHAMA};

	struct bahama_variant_register {
		const size_t size;
		const struct bahama_config_register *set;
	};

	const struct bahama_config_register *p;

	u8 version;

	const struct bahama_config_register v10_bt_on[] = {
		{ 0xE9, 0x00, 0xFF },
		{ 0xF4, 0x80, 0xFF },
		{ 0xE4, 0x00, 0xFF },
		{ 0xE5, 0x00, 0x0F },
#ifdef CONFIG_WLAN
		{ 0xE6, 0x38, 0x7F },
		{ 0xE7, 0x06, 0xFF },
#endif
		{ 0xE9, 0x21, 0xFF },
		{ 0x01, 0x0C, 0x1F },
		{ 0x01, 0x08, 0x1F },
	};

	const struct bahama_config_register v20_bt_on_fm_off[] = {
		{ 0x11, 0x0C, 0xFF },
		{ 0x13, 0x01, 0xFF },
		{ 0xF4, 0x80, 0xFF },
		{ 0xF0, 0x00, 0xFF },
		{ 0xE9, 0x00, 0xFF },
#ifdef CONFIG_WLAN
		{ 0x81, 0x00, 0x7F },
		{ 0x82, 0x00, 0xFF },
		{ 0xE6, 0x38, 0x7F },
		{ 0xE7, 0x06, 0xFF },
#endif
		{ 0x8E, 0x15, 0xFF },
		{ 0x8F, 0x15, 0xFF },
		{ 0x90, 0x15, 0xFF },

		{ 0xE9, 0x21, 0xFF },
	};

	const struct bahama_config_register v20_bt_on_fm_on[] = {
		{ 0x11, 0x0C, 0xFF },
		{ 0x13, 0x01, 0xFF },
		{ 0xF4, 0x86, 0xFF },
		{ 0xF0, 0x06, 0xFF },
		{ 0xE9, 0x00, 0xFF },
#ifdef CONFIG_WLAN
		{ 0x81, 0x00, 0x7F },
		{ 0x82, 0x00, 0xFF },
		{ 0xE6, 0x38, 0x7F },
		{ 0xE7, 0x06, 0xFF },
#endif
		{ 0xE9, 0x21, 0xFF },
	};

	const struct bahama_config_register v10_bt_off[] = {
		{ 0xE9, 0x00, 0xFF },
	};

	const struct bahama_config_register v20_bt_off_fm_off[] = {
		{ 0xF4, 0x84, 0xFF },
		{ 0xF0, 0x04, 0xFF },
		{ 0xE9, 0x00, 0xFF }
	};

	const struct bahama_config_register v20_bt_off_fm_on[] = {
		{ 0xF4, 0x86, 0xFF },
		{ 0xF0, 0x06, 0xFF },
		{ 0xE9, 0x00, 0xFF }
	};
	const struct bahama_variant_register bt_bahama[2][3] = {
	{
		{ ARRAY_SIZE(v10_bt_off), v10_bt_off },
		{ ARRAY_SIZE(v20_bt_off_fm_off), v20_bt_off_fm_off },
		{ ARRAY_SIZE(v20_bt_off_fm_on), v20_bt_off_fm_on }
	},
	{
		{ ARRAY_SIZE(v10_bt_on), v10_bt_on },
		{ ARRAY_SIZE(v20_bt_on_fm_off), v20_bt_on_fm_off },
		{ ARRAY_SIZE(v20_bt_on_fm_on), v20_bt_on_fm_on }
	}
	};

	u8 offset = 0; /* index into bahama configs */
	on = on ? 1 : 0;
	version = marimba_read_bahama_ver(&config);
	if ((int)version < 0 || version == BAHAMA_VER_UNSUPPORTED) {
		dev_err(&msm_bt_power_device.dev, "%s: Bahama \
				version read Error, version = %d \n",
				__func__, version);
		return -EIO;
	}

	if (version == BAHAMA_VER_2_0) {
		if (marimba_get_fm_status(&config))
			offset = 0x01;
	}

	p = bt_bahama[on][version + offset].set;

	dev_info(&msm_bt_power_device.dev,
		"%s: found version %d\n", __func__, version);

	for (i = 0; i < bt_bahama[on][version + offset].size; i++) {
		u8 value = (p+i)->value;
		rc = marimba_write_bit_mask(&config,
			(p+i)->reg,
			&value,
			sizeof((p+i)->value),
			(p+i)->mask);
		if (rc < 0) {
			dev_err(&msm_bt_power_device.dev,
				"%s: reg %x write failed: %d\n",
				__func__, (p+i)->reg, rc);
			return rc;
		}
		dev_dbg(&msm_bt_power_device.dev,
			"%s: reg 0x%02x write value 0x%02x mask 0x%02x\n",
				__func__, (p+i)->reg,
				value, (p+i)->mask);
		value = 0;
		rc = marimba_read_bit_mask(&config,
				(p+i)->reg, &value,
				sizeof((p+i)->value), (p+i)->mask);
		if (rc < 0)
			dev_err(&msm_bt_power_device.dev, "%s marimba_read_bit_mask- error",
					__func__);
		dev_dbg(&msm_bt_power_device.dev,
			"%s: reg 0x%02x read value 0x%02x mask 0x%02x\n",
				__func__, (p+i)->reg,
				value, (p+i)->mask);
	}
	/* Update BT Status */
	if (on)
		marimba_set_bt_status(&config, true);
	else
		marimba_set_bt_status(&config, false);
	return rc;
}
static int bluetooth_switch_regulators(int on)
{
	int i, rc = 0;
	const char *id = "BTPW";

	for (i = 0; i < ARRAY_SIZE(bt_vregs); i++) {
		if (IS_ERR_OR_NULL(bt_vregs[i].reg)) {
			rc = bt_vregs[i].reg ?
				PTR_ERR(bt_vregs[i].reg) :
				-ENODEV;
			dev_err(&msm_bt_power_device.dev,
				"%s: invalid regulator handle for %s: %d\n",
					__func__, bt_vregs[i].name, rc);
			goto reg_disable;
		}

		rc = on ? regulator_set_voltage(bt_vregs[i].reg,
				bt_vregs[i].min_level,
					bt_vregs[i].max_level) : 0;
		if (rc) {
			dev_err(&msm_bt_power_device.dev,
				"%s: could not set voltage for %s: %d\n",
					__func__, bt_vregs[i].name, rc);
			goto reg_disable;
		}

		rc = on ? regulator_enable(bt_vregs[i].reg) : 0;
		if (rc) {
			dev_err(&msm_bt_power_device.dev,
				"%s: could not %sable regulator %s: %d\n",
					__func__, "en", bt_vregs[i].name, rc);
			goto reg_disable;
		}

		if (bt_vregs[i].is_pin_controlled) {
			rc = pmapp_vreg_lpm_pincntrl_vote(id,
					bt_vregs[i].pmapp_id,
					PMAPP_CLOCK_ID_D1,
					on ? PMAPP_CLOCK_VOTE_ON :
						PMAPP_CLOCK_VOTE_OFF);
			if (rc) {
				dev_err(&msm_bt_power_device.dev,
					"%s: pin control failed for %s: %d\n",
					__func__, bt_vregs[i].name, rc);
				goto pin_cnt_fail;
			}
		}
		rc = on ? 0 : regulator_disable(bt_vregs[i].reg);

		if (rc) {
			dev_err(&msm_bt_power_device.dev,
				"%s: could not %sable regulator %s: %d\n",
				__func__, "dis", bt_vregs[i].name, rc);
			goto reg_disable;
		}
	}

	return rc;
pin_cnt_fail:
	if (on)
		regulator_disable(bt_vregs[i].reg);
reg_disable:
	while (i) {
		if (on) {
			i--;
			regulator_disable(bt_vregs[i].reg);
			regulator_put(bt_vregs[i].reg);
		}
	}
	return rc;
}

static struct regulator *reg_s3;
static unsigned int msm_bahama_setup_power(void)
{
	int rc = 0;

	reg_s3 = regulator_get(NULL, "msme1");
	if (IS_ERR(reg_s3)) {
		rc = PTR_ERR(reg_s3);
		pr_err("%s: could not get regulator: %d\n", __func__, rc);
		goto out;
	}

	rc = regulator_set_voltage(reg_s3, 1800000, 1800000);
	if (rc) {
		pr_err("%s: could not set voltage: %d\n", __func__, rc);
		goto reg_fail;
	}

	rc = regulator_enable(reg_s3);
	if (rc < 0) {
		pr_err("%s: could not enable regulator: %d\n", __func__, rc);
		goto reg_fail;
	}

	/*setup Bahama_sys_reset_n*/
	rc = gpio_request(GPIO_BT_SYS_REST_EN, "bahama sys_rst_n");
	if (rc) {
		pr_err("%s: gpio_request %d = %d\n", __func__,
			GPIO_BT_SYS_REST_EN, rc);
		goto reg_disable;
	}

	rc = bt_set_gpio(1);
	if (rc) {
		pr_err("%s: bt_set_gpio %d = %d\n", __func__,
			GPIO_BT_SYS_REST_EN, rc);
		goto gpio_fail;
	}

	return rc;

gpio_fail:
	gpio_free(GPIO_BT_SYS_REST_EN);
reg_disable:
	regulator_disable(reg_s3);
reg_fail:
	regulator_put(reg_s3);
out:
	reg_s3 = NULL;
	return rc;
}

static unsigned int msm_bahama_shutdown_power(int value)
{
	int rc = 0;

	if (IS_ERR_OR_NULL(reg_s3)) {
		rc = reg_s3 ? PTR_ERR(reg_s3) : -ENODEV;
		goto out;
	}

	rc = regulator_disable(reg_s3);
	if (rc) {
		pr_err("%s: could not disable regulator: %d\n", __func__, rc);
		goto out;
	}

	if (value == BAHAMA_ID) {
		rc = bt_set_gpio(0);
		if (rc) {
			pr_err("%s: bt_set_gpio = %d\n",
					__func__, rc);
		}
		gpio_free(GPIO_BT_SYS_REST_EN);
	}

	regulator_put(reg_s3);
	reg_s3 = NULL;

	return 0;

out:
	return rc;
}

static unsigned int msm_bahama_core_config(int type)
{
	int rc = 0;

	if (type == BAHAMA_ID) {
		int i;
		struct marimba config = { .mod_id =  SLAVE_ID_BAHAMA};
		const struct bahama_config_register v20_init[] = {
			/* reg, value, mask */
			{ 0xF4, 0x84, 0xFF }, /* AREG */
			{ 0xF0, 0x04, 0xFF } /* DREG */
		};
		if (marimba_read_bahama_ver(&config) == BAHAMA_VER_2_0) {
			for (i = 0; i < ARRAY_SIZE(v20_init); i++) {
				u8 value = v20_init[i].value;
				rc = marimba_write_bit_mask(&config,
					v20_init[i].reg,
					&value,
					sizeof(v20_init[i].value),
					v20_init[i].mask);
				if (rc < 0) {
					pr_err("%s: reg %d write failed: %d\n",
						__func__, v20_init[i].reg, rc);
					return rc;
				}
				pr_debug("%s: reg 0x%02x value 0x%02x"
					" mask 0x%02x\n",
					__func__, v20_init[i].reg,
					v20_init[i].value, v20_init[i].mask);
			}
		}
	}
	rc = bt_set_gpio(0);
	if (rc) {
		pr_err("%s: bt_set_gpio = %d\n",
		       __func__, rc);
	}
	pr_debug("core type: %d\n", type);
	return rc;
}

static int bluetooth_power(int on)
{
	int pin, rc = 0;
	const char *id = "BTPW";


	int cid = 0;

	cid = adie_get_detected_connectivity_type();
	if (cid != BAHAMA_ID) {
		pr_err("%s: unexpected adie connectivity type: %d\n",
					__func__, cid);
		return -ENODEV;
	}

	if (on) {
		/*setup power for BT SOC*/
		rc = bt_set_gpio(on);
		if (rc) {
			pr_err("%s: bt_set_gpio = %d\n",
					__func__, rc);
			goto exit;
		}
		rc = bluetooth_switch_regulators(on);
		if (rc < 0) {
			pr_err("%s: bluetooth_switch_regulators rc = %d",
					__func__, rc);
			goto exit;
		}
		/*setup BT GPIO lines*/
		for (pin = 0; pin < ARRAY_SIZE(bt_config_power_on);
			pin++) {
			rc = gpio_tlmm_config(bt_config_power_on[pin],
					GPIO_CFG_ENABLE);
			if (rc < 0) {
				pr_err("%s: gpio_tlmm_config(%#x)=%d\n",
						__func__,
						bt_config_power_on[pin],
						rc);
				goto fail_power;
			}
		}
		/*Setup BT clocks*/
		rc = pmapp_clock_vote(id, PMAPP_CLOCK_ID_D1,
			PMAPP_CLOCK_VOTE_ON);
		if (rc < 0) {
			pr_err("Failed to vote for TCXO_D1 ON\n");
			goto fail_clock;
		}
		msleep(20);

		/*I2C config for Bahama*/
		rc = bahama_bt(1);
		if (rc < 0) {
			pr_err("%s: bahama_bt rc = %d", __func__, rc);
			goto fail_i2c;
		}
		msleep(20);

		/*setup BT PCM lines*/
		rc = msm_bahama_setup_pcm_i2s(BT_PCM_ON);
		if (rc < 0) {
			pr_err("%s: msm_bahama_setup_pcm_i2s , rc =%d\n",
				__func__, rc);
				goto fail_power;
			}
		rc = pmapp_clock_vote(id, PMAPP_CLOCK_ID_D1,
				  PMAPP_CLOCK_VOTE_PIN_CTRL);
		if (rc < 0)
			pr_err("%s:Pin Control Failed, rc = %d",
					__func__, rc);

	} else {
		rc = bahama_bt(0);
		if (rc < 0)
			pr_err("%s: bahama_bt rc = %d", __func__, rc);

		rc = bt_set_gpio(on);
		if (rc) {
			pr_err("%s: bt_set_gpio = %d\n",
					__func__, rc);
		}
fail_i2c:
		rc = pmapp_clock_vote(id, PMAPP_CLOCK_ID_D1,
				  PMAPP_CLOCK_VOTE_OFF);
		if (rc < 0)
			pr_err("%s: Failed to vote Off D1\n", __func__);
fail_clock:
		for (pin = 0; pin < ARRAY_SIZE(bt_config_power_off);
			pin++) {
				rc = gpio_tlmm_config(bt_config_power_off[pin],
					GPIO_CFG_ENABLE);
				if (rc < 0) {
					pr_err("%s: gpio_tlmm_config(%#x)=%d\n",
					__func__, bt_config_power_off[pin], rc);
				}
			}
		rc = msm_bahama_setup_pcm_i2s(BT_PCM_OFF);
		if (rc < 0) {
			pr_err("%s: msm_bahama_setup_pcm_i2s, rc =%d\n",
					__func__, rc);
				}
fail_power:
		rc = bluetooth_switch_regulators(0);
		if (rc < 0) {
			pr_err("%s: switch_regulators : rc = %d",\
					__func__, rc);
			goto exit;
		}
	}
	return rc;
exit:
	pr_err("%s: failed with rc = %d", __func__, rc);
	return rc;
}

static int bcm_power(int on)
{

  int rc;
	
	if (on){		
	   rc =gpio_request(115, "bt_reset");
	   if(!rc){
       gpio_direction_output(115, 0);
		   mdelay(1);
		   gpio_direction_output(115, 1);		   
		   }
		else{
				printk(KERN_ERR "gpio_request: 115 failed!\n");			
		}  
		
		gpio_free(115);
			
		
	
	}else {
  	
		rc = gpio_request(115, "bt_reset");
		if(!rc){
		   gpio_direction_output(115, 0);
		 }
		else
			printk(KERN_ERR "gpio_request: 115 failed!\n");
		gpio_free(115);		
		

	}
	return 0;
}
//compatible of qualcomm and broadcomm bluetooth chip     .08.24 end

//compatible of qualcomm and broadcomm bluetooth chip      begin
static struct platform_device msm_bcm_power_device = {
	.name = "bcm_power",
};

//compatible of qualcomm and broadcomm bluetooth chip      end

static struct resource bluesleep_resources[] = {
	{
		.name	= "gpio_host_wake",
		.start	= 83,
		.end	= 83,
		.flags	= IORESOURCE_IO,
	},
	{
		.name	= "gpio_ext_wake",
		.start	= 116,   // emmc mipi to 116
		.end	= 116,     //emmc mipi to 116
		.flags	= IORESOURCE_IO,
	},
	{
		.name	= "host_wake",
		.start	= MSM_GPIO_TO_INT(83),
		.end	= MSM_GPIO_TO_INT(83),
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device msm_bcmsleep_device = {
	.name = "bcmsleep",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(bluesleep_resources),
	.resource	= bluesleep_resources,
};
static int __init bt_power_init(void)
{
	int i, rc = 0;
	struct device *dev = &msm_bt_power_device.dev;

	for (i = 0; i < ARRAY_SIZE(bt_vregs); i++) {
		bt_vregs[i].reg = regulator_get(dev, bt_vregs[i].name);
		if (IS_ERR(bt_vregs[i].reg)) {
			rc = PTR_ERR(bt_vregs[i].reg);
			dev_err(dev, "%s: could not get regulator %s: %d\n",
					__func__, bt_vregs[i].name, rc);
			goto reg_get_fail;
		}
	}

	dev->platform_data = &bluetooth_power;

msm_bcm_power_device.dev.platform_data = &bcm_power;
	return rc;

reg_get_fail:
	while (i--) {
		regulator_put(bt_vregs[i].reg);
		bt_vregs[i].reg = NULL;
	}
	return rc;
}

static struct marimba_platform_data marimba_pdata = {
	.slave_id[SLAVE_ID_BAHAMA_FM]        = BAHAMA_SLAVE_ID_FM_ADDR,
	.slave_id[SLAVE_ID_BAHAMA_QMEMBIST]  = BAHAMA_SLAVE_ID_QMEMBIST_ADDR,
	.bahama_setup                        = msm_bahama_setup_power,
	.bahama_shutdown                     = msm_bahama_shutdown_power,
	.bahama_core_config                  = msm_bahama_core_config,
	.fm				     = &marimba_fm_pdata,
};

#endif   //end  config_bt  && config_marimba_core
#endif   //remove bt module for qcm
#ifdef CONFIG_PN544_NFC

struct pn544_i2c_platform_data pn544_data={
       .irq_gpio=41,
       .ven_gpio=26,
       .firm_gpio=40,
       .dcdc_gpio=0,
       .clock_gpio=0,
       .int_active_low=0,
};
#endif
#if defined(CONFIG_I2C) && defined(CONFIG_GPIO_SX150X)
static struct i2c_board_info core_exp_i2c_info[] __initdata = {
/* 0x3e address conflict with focaltech touchsreen driver
	{
		I2C_BOARD_INFO("sx1509q", 0x3e),
	},
*/
	// start
	{
		.type         = "taos",
		.addr         = 0x39,
	},
	// end
};

#ifdef CONFIG_SENSORS_AKM8962C //used by Freesca;e && Kionix   
struct akm8962_platform_data akm_platform_data_8962 ={
	.layout			 = 5,
	.gpio_DRDY		 = 18,
};
#endif

#ifdef CONFIG_MXC_MMA8452
struct mma8452_platform_data mma8452_pdata = {
   .axis_map_cordination = 5,
};
#endif
#ifdef CONFIG_SENSORS_KXTIK 

#ifdef CONFIG_GSENSORS_NEW_POSITION
#define KXTIK_DEVICE_MAP 4 
#elif defined (CONFIG_MACH_SEANPLUS)
#define KXTIK_DEVICE_MAP 4 
#else
#define KXTIK_DEVICE_MAP 1 
#endif

#define KXTIK_MAP_X (KXTIK_DEVICE_MAP-1)%2 
#define KXTIK_MAP_Y KXTIK_DEVICE_MAP%2 
#define KXTIK_NEG_X (KXTIK_DEVICE_MAP/2)%2 
#define KXTIK_NEG_Y (KXTIK_DEVICE_MAP+1)/4 
#define KXTIK_NEG_Z (KXTIK_DEVICE_MAP-1)/4

struct kxtik_platform_data kxtik_pdata = { 
  .min_interval = 100, 
  .poll_interval  = 200,
 
  .axis_map_x  = KXTIK_MAP_X, 
  .axis_map_y  = KXTIK_MAP_Y, 
  .axis_map_z  = 2, 
 
  .negate_x  = KXTIK_NEG_X, 
  .negate_y  = KXTIK_NEG_Y, 
  .negate_z  = KXTIK_NEG_Z, 
 
  .res_12bit  = RES_12BIT, 
  .g_range  = KXTIK_G_2G, 
 
  //.data_odr_init    = ODR12_5F, //modify by fya
}; 
#endif /* CONFIG_SENSORS_KXTIK */ 

#ifdef CONFIG_SENSORS_ST_LIS3DHTR
struct lis3dh_acc_platform_data lis3dhtr_pdata = {
	.poll_interval = 100,
	.min_interval = 10, 
    /* measure range */ 
	.g_range = LIS3DH_ACC_G_2G,
    /*layout of lis3dhtr*/
	.axis_map_x = 0,
	.axis_map_y = 1,
	.axis_map_z = 2,

	.negate_x = 1,
	.negate_y = 0,
	.negate_z = 1,
	//end of layout 
	.gpio_int1 = -1,
	.gpio_int2 = -1,//we do not use interrupt in android phone
	.power_on = NULL,
	.power_off = NULL,
}; 
#endif

static struct i2c_board_info cam_exp_i2c_info[] __initdata = {
	{
		I2C_BOARD_INFO("sx1508q", 0x22),
		.platform_data	= &sx150x_data[SX150X_CAM],
	},
#ifdef CONFIG_SENSORS_AKM8962C //used by Freesca;e && Kionix   
	{
		.type = "akm8962",
		.addr = 0x0c,
		//.flag = I2C_CLIENT_WAKE,
		.platform_data = &akm_platform_data_8962,
		.irq = MSM_GPIO_TO_INT(18),
	},
#endif
#ifdef CONFIG_MXC_MMA8452
    { 
    	.type = "mma8452", 
	    .platform_data = &mma8452_pdata,  
    	.addr = 0x1C,     // MMA8452 i2c slave address 
    }, 
#endif
#ifdef CONFIG_SENSORS_KXTIK
	{ 
		I2C_BOARD_INFO("kxtik", KXTIK_I2C_ADDR), 
    	.platform_data = &kxtik_pdata,  
    	.irq = MSM_GPIO_TO_INT(28), // Replace with appropriate GPIO setup 
  },
#endif
#ifdef CONFIG_SENSORS_ST_LIS3DHTR
	{ 
		.type = "lis3dhtr",
		.platform_data = &lis3dhtr_pdata,
		.addr = 0x18,
  },
#endif

#ifdef CONFIG_PN544_NFC 
        { 
                I2C_BOARD_INFO("pn544", 0x50>>1 ), 
		   .platform_data = &pn544_data,			
                .irq = MSM_GPIO_TO_INT(41),   
	},
#endif

};
#endif

#if 0
#if defined(CONFIG_BT) && defined(CONFIG_MARIMBA_CORE)
static struct i2c_board_info bahama_devices[] = {
{
	I2C_BOARD_INFO("marimba", 0xc),
	.platform_data = &marimba_pdata,
},
};
#endif
#endif  // remove  bt  module  for qcm

#if defined(CONFIG_I2C) && defined(CONFIG_GPIO_SX150X)
static void __init register_i2c_devices(void)
{

	i2c_register_board_info(MSM_GSBI0_QUP_I2C_BUS_ID,
				cam_exp_i2c_info,
				ARRAY_SIZE(cam_exp_i2c_info));

	if (1 || machine_is_msm7625a_surf())
		sx150x_data[SX150X_CORE].io_open_drain_ena = 0xe0f0;

	core_exp_i2c_info[0].platform_data =
			&sx150x_data[SX150X_CORE];

	i2c_register_board_info(MSM_GSBI1_QUP_I2C_BUS_ID,
				core_exp_i2c_info,
				ARRAY_SIZE(core_exp_i2c_info));
#if 0
#if defined(CONFIG_BT) && defined(CONFIG_MARIMBA_CORE)
	i2c_register_board_info(MSM_GSBI1_QUP_I2C_BUS_ID,
				bahama_devices,
				ARRAY_SIZE(bahama_devices));
#endif
#endif // remove bt module  for  qcm
}
#endif

#if 0
static struct msm_gpio qup_i2c_gpios_io[] = {
	{ GPIO_CFG(60, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_scl" },
	{ GPIO_CFG(61, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_sda" },
	{ GPIO_CFG(131, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_scl" },
	{ GPIO_CFG(132, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_sda" },
};

static struct msm_gpio qup_i2c_gpios_hw[] = {
	{ GPIO_CFG(60, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_scl" },
	{ GPIO_CFG(61, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_sda" },
	{ GPIO_CFG(131, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_scl" },
	{ GPIO_CFG(132, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_sda" },
};
#endif  //remove  for  bt  chip wcn2243



#if defined (CONFIG_MACH_ATLAS40) 
static struct msm_gpio qup_i2c_gpios_io[] = {
      { GPIO_CFG(60, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),		
	      "qup_scl" },	
      { GPIO_CFG(61, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),		
            "qup_sda" },
      { GPIO_CFG(131, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),		
             "qup_scl" },	
      { GPIO_CFG(132, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),		
            "qup_sda" },
};
static struct msm_gpio qup_i2c_gpios_hw[] = {	
	{ GPIO_CFG(60, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),		
		"qup_scl" },	
	{ GPIO_CFG(61, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),		
	       "qup_sda" },	
	{ GPIO_CFG(131, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),	
	        "qup_scl" },	
	{ GPIO_CFG(132, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),		
	     "qup_sda" },
};


#elif defined (CONFIG_MACH_NICE)
static struct msm_gpio qup_i2c_gpios_io[] = {
      { GPIO_CFG(60, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),		
	      "qup_scl" },	
      { GPIO_CFG(61, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),		
            "qup_sda" },
      { GPIO_CFG(131, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA),		
             "qup_scl" },	
      { GPIO_CFG(132, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA),		
            "qup_sda" },
};
static struct msm_gpio qup_i2c_gpios_hw[] = {	
	{ GPIO_CFG(60, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),		
		"qup_scl" },	
	{ GPIO_CFG(61, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA),		
	       "qup_sda" },	
	{ GPIO_CFG(131, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA),	
	        "qup_scl" },	
	{ GPIO_CFG(132, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA),		
	     "qup_sda" },
};
#elif defined(CONFIG_MACH_SEANPLUS)
static struct msm_gpio qup_i2c_gpios_io[] = {
      { GPIO_CFG(60, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),		
	      "qup_scl" },	
      { GPIO_CFG(61, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),		
            "qup_sda" },
      { GPIO_CFG(131, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA),		
             "qup_scl" },	
      { GPIO_CFG(132, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA),		
            "qup_sda" },
};
static struct msm_gpio qup_i2c_gpios_hw[] = {	
	{ GPIO_CFG(60, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),		
		"qup_scl" },	
	{ GPIO_CFG(61, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),		
	       "qup_sda" },	
	{ GPIO_CFG(131, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA),	
	        "qup_scl" },	
	{ GPIO_CFG(132, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_4MA),		
	     "qup_sda" },
};
#elif defined(CONFIG_MACH_ROAMER2)
static struct msm_gpio qup_i2c_gpios_io[] = {
	{ GPIO_CFG(60, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_scl" },
	{ GPIO_CFG(61, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_sda" },
	{ GPIO_CFG(131, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_scl" },
	{ GPIO_CFG(132, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_sda" },
};

static struct msm_gpio qup_i2c_gpios_hw[] = {
	{ GPIO_CFG(60, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_scl" },
	{ GPIO_CFG(61, 1, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_sda" },
	{ GPIO_CFG(131, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_scl" },
	{ GPIO_CFG(132, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA),
		"qup_sda" },
};
#endif


static void gsbi_qup_i2c_gpio_config(int adap_id, int config_type)
{
	int rc;

	if (adap_id < 0 || adap_id > 1)
		return;

	/* Each adapter gets 2 lines from the table */
	if (config_type)
		rc = msm_gpios_request_enable(&qup_i2c_gpios_hw[adap_id*2], 2);
	else
		rc = msm_gpios_request_enable(&qup_i2c_gpios_io[adap_id*2], 2);
	if (rc < 0)
		pr_err("QUP GPIO request/enable failed: %d\n", rc);
}

static struct msm_i2c_platform_data msm_gsbi0_qup_i2c_pdata = {
#if defined(CONFIG_MACH_SEANPLUS)
	.clk_freq		= 100000,
#else
	.clk_freq		= 300000,
#endif
	.msm_i2c_config_gpio	= gsbi_qup_i2c_gpio_config,
};

static struct msm_i2c_platform_data msm_gsbi1_qup_i2c_pdata = {
	.clk_freq		= 300000,
	.msm_i2c_config_gpio	= gsbi_qup_i2c_gpio_config,
};

#ifdef CONFIG_ARCH_MSM7X27A

#if defined (CONFIG_MACH_ATLAS40)
#define LCD_XY  (480*800)
#elif defined (CONFIG_MACH_NICE) || defined (CONFIG_MACH_SEANPLUS)|| defined (CONFIG_MACH_ROAMER2)
#define LCD_XY  (320*480)
#else
#define LCD_XY  (0)
#endif

#if (LCD_XY == 480*800)
//#define MSM_PMEM_MDP_SIZE       (26*1024*1024)
//#define MSM_PMEM_MDP_SIZE       0x1DD1000
//35M
#define MSM_PMEM_MDP_SIZE       0x2300000
#elif (LCD_XY == 320*480)
//#define MSM_PMEM_MDP_SIZE       (16*1024*1024)
#define MSM_PMEM_MDP_SIZE       (20*1024*1024)
#else
#define MSM_PMEM_MDP_SIZE       0x1DD1000
#endif

#define MSM7x25A_MSM_PMEM_MDP_SIZE       0x1500000

#define MSM_PMEM_ADSP_SIZE      0x1000000
#define MSM7x25A_MSM_PMEM_ADSP_SIZE      0xB91000


#ifdef CONFIG_FB_MSM_TRIPLE_BUFFER
#define MSM_FB_SIZE		0x500000
#define MSM7x25A_MSM_FB_SIZE	0xE1000
#else
#define MSM_FB_SIZE		0x196000
#define MSM7x25A_MSM_FB_SIZE	0x96000

#endif

#endif

static struct android_usb_platform_data android_usb_pdata = {
	.update_pid_and_serial_num = usb_diag_update_pid_and_serial_num,
};

static struct platform_device android_usb_device = {
	.name	= "android_usb",
	.id	= -1,
	.dev	= {
		.platform_data = &android_usb_pdata,
	},
};

#ifdef CONFIG_USB_EHCI_MSM_72K
static void msm_hsusb_vbus_power(unsigned phy_info, int on)
{
	int rc = 0;
	unsigned gpio;

	gpio = GPIO_HOST_VBUS_EN;

	rc = gpio_request(gpio, "i2c_host_vbus_en");
	if (rc < 0) {
		pr_err("failed to request %d GPIO\n", gpio);
		return;
	}
	gpio_direction_output(gpio, !!on);
	gpio_set_value_cansleep(gpio, !!on);
	gpio_free(gpio);
}

static struct msm_usb_host_platform_data msm_usb_host_pdata = {
	.phy_info       = (USB_PHY_INTEGRATED | USB_PHY_MODEL_45NM),
};

static void __init msm7x2x_init_host(void)
{
	msm_add_host(0, &msm_usb_host_pdata);
}
#endif

#ifdef CONFIG_USB_MSM_OTG_72K
static int hsusb_rpc_connect(int connect)
{
	if (connect)
		return msm_hsusb_rpc_connect();
	else
		return msm_hsusb_rpc_close();
}

static struct regulator *reg_hsusb;
static int msm_hsusb_ldo_init(int init)
{
	int rc = 0;

	if (init) {
		reg_hsusb = regulator_get(NULL, "usb");
		if (IS_ERR(reg_hsusb)) {
			rc = PTR_ERR(reg_hsusb);
			pr_err("%s: could not get regulator: %d\n",
					__func__, rc);
			goto out;
		}

		rc = regulator_set_voltage(reg_hsusb, 3300000, 3300000);
		if (rc) {
			pr_err("%s: could not set voltage: %d\n",
					__func__, rc);
			goto reg_free;
		}

		return 0;
	}
	/* else fall through */
reg_free:
	regulator_put(reg_hsusb);
out:
	reg_hsusb = NULL;
	return rc;
}

static int msm_hsusb_ldo_enable(int enable)
{
	static int ldo_status;

	if (IS_ERR_OR_NULL(reg_hsusb))
		return reg_hsusb ? PTR_ERR(reg_hsusb) : -ENODEV;

	if (ldo_status == enable)
		return 0;

	ldo_status = enable;

	return enable ?
		regulator_enable(reg_hsusb) :
		regulator_disable(reg_hsusb);
}

#ifndef CONFIG_USB_EHCI_MSM_72K
static int msm_hsusb_pmic_notif_init(void (*callback)(int online), int init)
{
	int ret = 0;
	printk("msm_hsusb_pmic_notif_init enter\n");

	if (init)
		ret = msm_pm_app_rpc_init(callback);
	else
		msm_pm_app_rpc_deinit(callback);

	printk("msm_hsusb_pmic_notif_init exit\n");
	return ret;
}
#endif

static int zte_phy_init_seq_override[] = {
	-1,
	-1,
	-1,
	-1
        /* 0x00, */
        /* 0x42, */
        /* 0x30, */
        /* 0x94 */
};/*for usb eye diagram test*/

static struct msm_otg_platform_data msm_otg_pdata = {
#ifndef CONFIG_USB_EHCI_MSM_72K
	.pmic_vbus_notif_init	 = msm_hsusb_pmic_notif_init,
#else
	.vbus_power		 = msm_hsusb_vbus_power,
#endif
	.rpc_connect		 = hsusb_rpc_connect,
	.pemp_level		 = PRE_EMPHASIS_WITH_20_PERCENT,
	.cdr_autoreset		 = CDR_AUTO_RESET_DISABLE,
	.drv_ampl		 = HS_DRV_AMPLITUDE_DEFAULT,
	.se1_gating		 = SE1_GATING_DISABLE,
	.ldo_init		 = msm_hsusb_ldo_init,
	.ldo_enable		 = msm_hsusb_ldo_enable,
	.chg_init		 = hsusb_chg_init,
	.chg_connected		 = hsusb_chg_connected,
	.chg_vbus_draw		 = hsusb_chg_vbus_draw,
};
#endif

static struct msm_hsusb_gadget_platform_data msm_gadget_pdata = {
	.is_phy_status_timer_on = 1,
};

static struct resource smc91x_resources[] = {
	[0] = {
		.start = 0x90000300,
		.end   = 0x900003ff,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = MSM_GPIO_TO_INT(4),
		.end   = MSM_GPIO_TO_INT(4),
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device smc91x_device = {
	.name           = "smc91x",
	.id             = 0,
	.num_resources  = ARRAY_SIZE(smc91x_resources),
	.resource       = smc91x_resources,
};


#ifdef CONFIG_SERIAL_MSM_HS
static struct msm_serial_hs_platform_data msm_uart_dm1_pdata = {
	.inject_rx_on_wakeup	= 1,
	.rx_to_inject		= 0xFD,
};
#endif
static struct msm_pm_platform_data msm7x27a_pm_data[MSM_PM_SLEEP_MODE_NR] = {
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 1,
					.suspend_enabled = 1,
					.latency = 16000,
					.residency = 20000,
	},
	[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 1,
					.suspend_enabled = 1,
					.latency = 12000,
					.residency = 20000,
	},
	[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 0,
					.suspend_enabled = 1,
					.latency = 2000,
					.residency = 0,
	},
	[MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT] = {
					.idle_supported = 1,
					.suspend_supported = 1,
					.idle_enabled = 1,
					.suspend_enabled = 1,
					.latency = 2,
					.residency = 0,
	},
};

u32 msm7x27a_power_collapse_latency(enum msm_pm_sleep_mode mode)
{
	switch (mode) {
	case MSM_PM_SLEEP_MODE_POWER_COLLAPSE:
		return msm7x27a_pm_data
		[MSM_PM_SLEEP_MODE_POWER_COLLAPSE].latency;
	case MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN:
		return msm7x27a_pm_data
		[MSM_PM_SLEEP_MODE_POWER_COLLAPSE_NO_XO_SHUTDOWN].latency;
	case MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT:
		return msm7x27a_pm_data
		[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].latency;
	case MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT:
		return msm7x27a_pm_data
		[MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT].latency;
	default:
		return 0;
	}
}

static struct msm_pm_boot_platform_data msm_pm_boot_pdata __initdata = {
	.mode = MSM_PM_BOOT_CONFIG_RESET_VECTOR_PHYS,
	.p_addr = 0,
};

static struct android_pmem_platform_data android_pmem_adsp_pdata = {
	.name = "pmem_adsp",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 1,
	.memory_type = MEMTYPE_EBI1,
};

static struct platform_device android_pmem_adsp_device = {
	.name = "android_pmem",
	.id = 1,
	.dev = { .platform_data = &android_pmem_adsp_pdata },
};

static unsigned pmem_mdp_size = MSM_PMEM_MDP_SIZE;
static int __init pmem_mdp_size_setup(char *p)
{
	pmem_mdp_size = memparse(p, NULL);
	return 0;
}

early_param("pmem_mdp_size", pmem_mdp_size_setup);

static unsigned pmem_adsp_size = MSM_PMEM_ADSP_SIZE;
static int __init pmem_adsp_size_setup(char *p)
{
	pmem_adsp_size = memparse(p, NULL);
	return 0;
}

early_param("pmem_adsp_size", pmem_adsp_size_setup);

static unsigned fb_size = MSM_FB_SIZE;
static int __init fb_size_setup(char *p)
{
	fb_size = memparse(p, NULL);
	return 0;
}

early_param("fb_size", fb_size_setup);

static struct regulator_bulk_data regs_lcdc[] = {
	{ .supply = "bt",   .min_uV = 2600000, .max_uV = 2600000 },
};

static uint32_t lcdc_gpio_initialized;

static void lcdc_toshiba_gpio_init(void)
{
	int rc = 0;
	if (!lcdc_gpio_initialized) {
		if (gpio_request(124, "spi_clk")) {
			pr_err("failed to request gpio spi_clk\n");
			return;
		}
		if (gpio_request(122, "spi_cs")) {
			pr_err("failed to request gpio spi_cs0_N\n");
			goto fail_gpio6;
		}
		if (gpio_request(109, "spi_mosi")) {
			pr_err("failed to request gpio spi_mosi\n");
			goto fail_gpio5;
		}
		if (gpio_request(123, "spi_miso")) {
			pr_err("failed to request gpio spi_miso\n");
			goto fail_gpio4;
		}
		/* if (gpio_request(89, "gpio_disp_rst")) { */
		/* 	pr_err("failed to request gpio_disp_pwr\n"); */
		/* 	goto fail_gpio3; */
		/* } */
		//if (gpio_request(GPIO_BACKLIGHT_EN, "gpio_bkl_en")) {
		//	pr_err("failed to request gpio_bkl_en\n");
		//	goto fail_gpio2;
		//}
		//pmapp_disp_backlight_init();

		rc = regulator_bulk_get(NULL, ARRAY_SIZE(regs_lcdc), regs_lcdc);
		if (rc) {
			pr_err("%s: could not get regulators: %d\n",
					__func__, rc);
			goto fail_gpio1;
		}

		rc = regulator_bulk_set_voltage(ARRAY_SIZE(regs_lcdc),
				regs_lcdc);
		if (rc) {
			pr_err("%s: could not set voltages: %d\n",
					__func__, rc);
			goto fail_vreg;
		}
		lcdc_gpio_initialized = 1;
	}
	return;
fail_vreg:
	regulator_bulk_free(ARRAY_SIZE(regs_lcdc), regs_lcdc);
fail_gpio1:


//	gpio_free(GPIO_BACKLIGHT_EN);
//fail_gpio2:
	//gpio_free(89);
	//fail_gpio3:
	gpio_free(123);
fail_gpio4:
	gpio_free(109);
fail_gpio5:
	gpio_free(122);
fail_gpio6:
	gpio_free(124);
	lcdc_gpio_initialized = 0;
}

static uint32_t lcdc_gpio_table[] = {
	124,
	122,
	109,
	0xff,//	89,
	//GPIO_BACKLIGHT_EN,
	123,
};

static void config_lcdc_gpio_table(uint32_t *table, int len, unsigned enable)
{
	int n;

	if (lcdc_gpio_initialized) {
		/* All are IO Expander GPIOs */
		for (n = 0; n < (len - 1); n++)
			gpio_direction_output(table[n], 1);
	}
}

static void lcdc_toshiba_config_gpios(int enable)
{
	config_lcdc_gpio_table(lcdc_gpio_table,
		ARRAY_SIZE(lcdc_gpio_table), enable);
}

static int msm_fb_lcdc_power_save(int on)
{
	int rc = 0;
	/* Doing the init of the LCDC GPIOs very late as they are from
		an I2C-controlled IO Expander */
	lcdc_toshiba_gpio_init();

	if (lcdc_gpio_initialized) {
		//gpio_set_value_cansleep(GPIO_DISPLAY_PWR_EN, on);
		//gpio_set_value_cansleep(GPIO_BACKLIGHT_EN, on);

		rc = on ? regulator_bulk_enable(
				ARRAY_SIZE(regs_lcdc), regs_lcdc) :
			  regulator_bulk_disable(
				ARRAY_SIZE(regs_lcdc), regs_lcdc);

		if (rc)
			pr_err("%s: could not %sable regulators: %d\n",
					__func__, on ? "en" : "dis", rc);
	}

	return rc;
}


static int lcdc_toshiba_set_bl(int level)
{
	int ret;

	return 0;

	ret = pmapp_disp_backlight_set_brightness(level);
	if (ret)
		pr_err("%s: can't set lcd backlight!\n", __func__);

	return ret;
}


static struct lcdc_platform_data lcdc_pdata = {
	.lcdc_gpio_config = NULL,
	.lcdc_power_save   = msm_fb_lcdc_power_save,
};

static int lcd_panel_spi_gpio_num[] = {
		124,   /* spi_clk */
		122, /* spi_cs  */
		109,  /* spi_sdoi */
		123,  /* spi_sdi */
		0xff, /* rst */
};

static struct msm_panel_common_pdata lcdc_toshiba_panel_data = {
	.panel_config_gpio = lcdc_toshiba_config_gpios,
	.pmic_backlight = lcdc_toshiba_set_bl,
	.gpio_num	  = lcd_panel_spi_gpio_num,
};

static struct platform_device lcdc_toshiba_panel_device = {
	.name   = "lcdc_toshiba_fwvga_pt",
	.id     = 0,
	.dev    = {
		.platform_data = &lcdc_toshiba_panel_data,
	}
};

static struct resource msm_fb_resources[] = {
	{
		.flags  = IORESOURCE_DMA,
	}
};

#define LCDC_TOSHIBA_FWVGA_PANEL_NAME	"lcdc_toshiba_fwvga_pt"
#define MIPI_CMD_RENESAS_FWVGA_PANEL_NAME	"mipi_cmd_renesas_fwvga"

static int msm_fb_detect_panel(const char *name)
{
	int ret = -ENODEV;

	if (1|| machine_is_msm7625a_surf()) {
		if (!strncmp(name, "lcdc_toshiba_fwvga_pt", 21) ||
				!strncmp(name, "mipi_cmd_renesas_fwvga", 22))
			ret = 0;
	} else if (machine_is_msm7x27a_ffa() || machine_is_msm7625a_ffa()) {
		if (!strncmp(name, "mipi_cmd_renesas_fwvga", 22))
			ret = 0;
	}

#if !defined(CONFIG_FB_MSM_LCDC_AUTO_DETECT) && \
	!defined(CONFIG_FB_MSM_MIPI_PANEL_AUTO_DETECT) && \
	!defined(CONFIG_FB_MSM_LCDC_MIPI_PANEL_AUTO_DETECT)
		if (1 ||
			machine_is_msm7625a_surf()) {
			if (!strncmp(name, LCDC_TOSHIBA_FWVGA_PANEL_NAME,
				strnlen(LCDC_TOSHIBA_FWVGA_PANEL_NAME,
					PANEL_NAME_MAX_LEN)))
				return 0;
		}
#endif
	return ret;
}

static struct msm_fb_platform_data msm_fb_pdata = {
	.detect_client = msm_fb_detect_panel,
};

static struct platform_device msm_fb_device = {
	.name   = "msm_fb",
	.id     = 0,
	.num_resources  = ARRAY_SIZE(msm_fb_resources),
	.resource       = msm_fb_resources,
	.dev    = {
		.platform_data = &msm_fb_pdata,
	}
};

#ifdef CONFIG_FB_MSM_MIPI_DSI
#if 0
static int mipi_renesas_set_bl(int level)
{
	int ret;

	ret = pmapp_disp_backlight_set_brightness(level);

	if (ret)
		pr_err("%s: can't set lcd backlight!\n", __func__);

	return ret;
}
#endif
static struct msm_panel_common_pdata mipi_renesas_pdata = {
	//.pmic_backlight = mipi_renesas_set_bl,
		.pmic_backlight = NULL,
};


static struct platform_device mipi_dsi_renesas_panel_device = {
	.name = "mipi_lead",
	.id = 0,
	.dev    = {
		.platform_data = &mipi_renesas_pdata,
	}
};
#endif

#define SND(desc, num) { .name = #desc, .id = num }
static struct snd_endpoint snd_endpoints_list[] = {
	SND(HANDSET, 0),
	SND(MONO_HEADSET, 2),
	SND(HEADSET, 3),
/* start */
	SND(HAC, 4),
/* end */
	SND(SPEAKER, 6),
	SND(TTY_HEADSET, 8),
	SND(TTY_VCO, 9),
	SND(TTY_HCO, 10),
	SND(BT, 12),
	SND(AUXMIC_SPEAKER, 14),
	SND(IN_S_SADC_OUT_HANDSET, 16),
	SND(IN_S_SADC_OUT_SPEAKER_PHONE, 25),
	SND(FM_DIGITAL_STEREO_HEADSET, 26),
	SND(FM_DIGITAL_SPEAKER_PHONE, 27),
	SND(FM_DIGITAL_BT_A2DP_HEADSET, 28),
	SND(STEREO_HEADSET_AND_SPEAKER, 31),
	SND(CURRENT, 38), /* SND(CURRENT, 0x7FFFFFFE), */
	SND(FM_ANALOG_STEREO_HEADSET, 35),
	SND(FM_ANALOG_STEREO_HEADSET_CODEC, 36),
};
#undef SND

static struct msm_snd_endpoints msm_device_snd_endpoints = {
	.endpoints = snd_endpoints_list,
	.num = sizeof(snd_endpoints_list) / sizeof(struct snd_endpoint)
};

static struct platform_device msm_device_snd = {
	.name = "msm_snd",
	.id = -1,
	.dev    = {
		.platform_data = &msm_device_snd_endpoints
	},
};

#define DEC0_FORMAT ((1<<MSM_ADSP_CODEC_MP3)| \
	(1<<MSM_ADSP_CODEC_AAC)|(1<<MSM_ADSP_CODEC_WMA)| \
	(1<<MSM_ADSP_CODEC_WMAPRO)|(1<<MSM_ADSP_CODEC_AMRWB)| \
	(1<<MSM_ADSP_CODEC_AMRNB)|(1<<MSM_ADSP_CODEC_WAV)| \
	(1<<MSM_ADSP_CODEC_ADPCM)|(1<<MSM_ADSP_CODEC_YADPCM)| \
	(1<<MSM_ADSP_CODEC_EVRC)|(1<<MSM_ADSP_CODEC_QCELP))
#define DEC1_FORMAT ((1<<MSM_ADSP_CODEC_MP3)| \
	(1<<MSM_ADSP_CODEC_AAC)|(1<<MSM_ADSP_CODEC_WMA)| \
	(1<<MSM_ADSP_CODEC_WMAPRO)|(1<<MSM_ADSP_CODEC_AMRWB)| \
	(1<<MSM_ADSP_CODEC_AMRNB)|(1<<MSM_ADSP_CODEC_WAV)| \
	(1<<MSM_ADSP_CODEC_ADPCM)|(1<<MSM_ADSP_CODEC_YADPCM)| \
	(1<<MSM_ADSP_CODEC_EVRC)|(1<<MSM_ADSP_CODEC_QCELP))
#define DEC2_FORMAT ((1<<MSM_ADSP_CODEC_MP3)| \
	(1<<MSM_ADSP_CODEC_AAC)|(1<<MSM_ADSP_CODEC_WMA)| \
	(1<<MSM_ADSP_CODEC_WMAPRO)|(1<<MSM_ADSP_CODEC_AMRWB)| \
	(1<<MSM_ADSP_CODEC_AMRNB)|(1<<MSM_ADSP_CODEC_WAV)| \
	(1<<MSM_ADSP_CODEC_ADPCM)|(1<<MSM_ADSP_CODEC_YADPCM)| \
	(1<<MSM_ADSP_CODEC_EVRC)|(1<<MSM_ADSP_CODEC_QCELP))
#define DEC3_FORMAT ((1<<MSM_ADSP_CODEC_MP3)| \
	(1<<MSM_ADSP_CODEC_AAC)|(1<<MSM_ADSP_CODEC_WMA)| \
	(1<<MSM_ADSP_CODEC_WMAPRO)|(1<<MSM_ADSP_CODEC_AMRWB)| \
	(1<<MSM_ADSP_CODEC_AMRNB)|(1<<MSM_ADSP_CODEC_WAV)| \
	(1<<MSM_ADSP_CODEC_ADPCM)|(1<<MSM_ADSP_CODEC_YADPCM)| \
	(1<<MSM_ADSP_CODEC_EVRC)|(1<<MSM_ADSP_CODEC_QCELP))
#define DEC4_FORMAT (1<<MSM_ADSP_CODEC_MIDI)

static unsigned int dec_concurrency_table[] = {
	/* Audio LP */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DMA)), 0,
	0, 0, 0,

	/* Concurrency 1 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	 /* Concurrency 2 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	/* Concurrency 3 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	/* Concurrency 4 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	/* Concurrency 5 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_TUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),

	/* Concurrency 6 */
#if 0
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	0, 0, 0, 0,
#else
#if 1 
	/*  Too many concurrency will exhaust PMEM_AUDIO memory, and cause silent problem */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	0,
	0,
	0,
#else
// chenjun:support decoder Concurrency
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),
#endif
#endif

	/* Concurrency 7 */
	(DEC0_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC1_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC2_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC3_FORMAT|(1<<MSM_ADSP_MODE_NONTUNNEL)|(1<<MSM_ADSP_OP_DM)),
	(DEC4_FORMAT),
};

#define DEC_INFO(name, queueid, decid, nr_codec) { .module_name = name, \
	.module_queueid = queueid, .module_decid = decid, \
	.nr_codec_support = nr_codec}

static struct msm_adspdec_info dec_info_list[] = {
	DEC_INFO("AUDPLAY0TASK", 13, 0, 11), /* AudPlay0BitStreamCtrlQueue */
	DEC_INFO("AUDPLAY1TASK", 14, 1, 11),  /* AudPlay1BitStreamCtrlQueue */
	DEC_INFO("AUDPLAY2TASK", 15, 2, 11),  /* AudPlay2BitStreamCtrlQueue */
	DEC_INFO("AUDPLAY3TASK", 16, 3, 11),  /* AudPlay3BitStreamCtrlQueue */
	DEC_INFO("AUDPLAY4TASK", 17, 4, 1),  /* AudPlay4BitStreamCtrlQueue */
};

static struct msm_adspdec_database msm_device_adspdec_database = {
	.num_dec = ARRAY_SIZE(dec_info_list),
	.num_concurrency_support = (ARRAY_SIZE(dec_concurrency_table) / \
					ARRAY_SIZE(dec_info_list)),
	.dec_concurrency_table = dec_concurrency_table,
	.dec_info_list = dec_info_list,
};

static struct platform_device msm_device_adspdec = {
	.name = "msm_adspdec",
	.id = -1,
	.dev    = {
		.platform_data = &msm_device_adspdec_database
	},
};

static struct android_pmem_platform_data android_pmem_audio_pdata = {
	.name = "pmem_audio",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
	.memory_type = MEMTYPE_EBI1,
};

static struct platform_device android_pmem_audio_device = {
	.name = "android_pmem",
	.id = 2,
	.dev = { .platform_data = &android_pmem_audio_pdata },
};

static struct android_pmem_platform_data android_pmem_pdata = {
	.name = "pmem",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 1,
	.memory_type = MEMTYPE_EBI1,
};
static struct platform_device android_pmem_device = {
	.name = "android_pmem",
	.id = 0,
	.dev = { .platform_data = &android_pmem_pdata },
};

static u32 msm_calculate_batt_capacity(u32 current_voltage);

static struct msm_psy_batt_pdata msm_psy_batt_data = {
	.voltage_min_design     = 2800,
	.voltage_max_design     = 4300,
	.avail_chg_sources      = AC_CHG | USB_CHG ,
	.batt_technology        = POWER_SUPPLY_TECHNOLOGY_LION,
	.calculate_capacity     = &msm_calculate_batt_capacity,
};

static u32 msm_calculate_batt_capacity(u32 current_voltage)
{
	u32 low_voltage	 = msm_psy_batt_data.voltage_min_design;
	u32 high_voltage = msm_psy_batt_data.voltage_max_design;

	return (current_voltage - low_voltage) * 100
			/ (high_voltage - low_voltage);
}

static struct platform_device msm_batt_device = {
	.name               = "msm-battery",
	.id                 = -1,
	.dev.platform_data  = &msm_psy_batt_data,
};

static struct smsc911x_platform_config smsc911x_config = {
	.irq_polarity	= SMSC911X_IRQ_POLARITY_ACTIVE_HIGH,
	.irq_type	= SMSC911X_IRQ_TYPE_PUSH_PULL,
	.flags		= SMSC911X_USE_16BIT,
};

static struct resource smsc911x_resources[] = {
	[0] = {
		.start	= 0x90000000,
		.end	= 0x90007fff,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= MSM_GPIO_TO_INT(48),
		.end	= MSM_GPIO_TO_INT(48),
		.flags	= IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHLEVEL,
	},
};

static struct platform_device smsc911x_device = {
	.name		= "smsc911x",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(smsc911x_resources),
	.resource	= smsc911x_resources,
	.dev		= {
		.platform_data	= &smsc911x_config,
	},
};

static struct msm_gpio smsc911x_gpios[] = {
	{ GPIO_CFG(48, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_6MA),
							 "smsc911x_irq"  },
	{ GPIO_CFG(49, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_6MA),
							 "eth_fifo_sel" },
};

#define ETH_FIFO_SEL_GPIO	49
static void msm7x27a_cfg_smsc911x(void)
{
	int res;

	res = msm_gpios_request_enable(smsc911x_gpios,
				 ARRAY_SIZE(smsc911x_gpios));
	if (res) {
		pr_err("%s: unable to enable gpios for SMSC911x\n", __func__);
		return;
	}

	/* ETH_FIFO_SEL */
	res = gpio_direction_output(ETH_FIFO_SEL_GPIO, 0);
	if (res) {
		pr_err("%s: unable to get direction for gpio %d\n", __func__,
							 ETH_FIFO_SEL_GPIO);
		msm_gpios_disable_free(smsc911x_gpios,
						 ARRAY_SIZE(smsc911x_gpios));
		return;
	}
	gpio_set_value(ETH_FIFO_SEL_GPIO, 0);
}

#if defined(CONFIG_SERIAL_MSM_HSL_CONSOLE) \
		&& defined(CONFIG_MSM_SHARED_GPIO_FOR_UART2DM)
static struct msm_gpio uart2dm_gpios[] = {
	{GPIO_CFG(19, 2, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
							"uart2dm_rfr_n" },
	{GPIO_CFG(20, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
							"uart2dm_cts_n" },
	{GPIO_CFG(21, 2, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
							"uart2dm_rx"    },
	{GPIO_CFG(108, 2, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
							"uart2dm_tx"    },
};

static void msm7x27a_cfg_uart2dm_serial(void)
{
	int ret;
	ret = msm_gpios_request_enable(uart2dm_gpios,
					ARRAY_SIZE(uart2dm_gpios));
	if (ret)
		pr_err("%s: unable to enable gpios for uart2dm\n", __func__);
}
#else
static void msm7x27a_cfg_uart2dm_serial(void) { }
#endif

static struct platform_device *rumi_sim_devices[] __initdata = {
	&msm_device_dmov,
	&msm_device_smd,
	&smc91x_device,
	&msm_device_uart1,
	&msm_device_nand,
	&msm_device_uart_dm1,
	&msm_gsbi0_qup_i2c_device,
	&msm_gsbi1_qup_i2c_device,
};

#ifdef CONFIG_ZTE_PLATFORM
#ifdef CONFIG_ANDROID_RAM_CONSOLE
static struct resource ram_console_resource[] = {
	{
		.start	= MSM_RAM_CONSOLE_PHYS,
		.end	= MSM_RAM_CONSOLE_PHYS + MSM_RAM_CONSOLE_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device ram_console_device = {
	.name = "ram_console",
	.id = -1,
	.num_resources  = ARRAY_SIZE(ram_console_resource),
	.resource       = ram_console_resource,
};
#endif
#endif
#ifdef CONFIG_ZTE_PLATFORM // begin
static struct gpio_led android_led_list[] = {
	{
		.name = "button-backlight",
		.gpio = 85,
	},
};

static struct gpio_led_platform_data android_leds_data = {
	.num_leds	= ARRAY_SIZE(android_led_list),
	.leds		= android_led_list,
};

static struct platform_device android_leds = {
	.name		= "leds-gpio",
	.id		= -1,
	.dev		= {
		.platform_data = &android_leds_data,
	},
};
#endif// end

static struct platform_device *surf_ffa_devices[] __initdata = {
	&msm_device_dmov,
	&msm_device_smd,
	&msm_device_uart1,
	&msm_device_uart_dm1,
	&msm_device_uart_dm2,
	&msm_device_nand,
	&msm_gsbi0_qup_i2c_device,
	&msm_gsbi1_qup_i2c_device,
	&msm_device_otg,
	&msm_device_gadget_peripheral,
	&android_usb_device,
	&android_pmem_device,
	&android_pmem_adsp_device,
	&android_pmem_audio_device,
	&msm_device_snd,
	&msm_device_adspdec,
	&msm_fb_device,
	&lcdc_toshiba_panel_device,
	&msm_batt_device,
	&smsc911x_device,
#if 0	
#ifdef CONFIG_S5K4E1
	&msm_camera_sensor_s5k4e1,
#endif
#ifdef CONFIG_IMX072
	&msm_camera_sensor_imx072,
#endif
#ifdef CONFIG_WEBCAM_OV9726
	&msm_camera_sensor_ov9726,
#endif
#ifdef CONFIG_MT9E013
	&msm_camera_sensor_mt9e013,
#endif
#ifdef CONFIG_OV5640
	&msm_camera_sensor_ov5640,	
#endif
#ifdef CONFIG_S5K5CAGX
	&msm_camera_sensor_s5k5cagx,
#endif
#ifdef CONFIG_MT9D115
	&msm_camera_sensor_mt9d115,	
#endif
#endif
#ifdef CONFIG_FB_MSM_MIPI_DSI
	&mipi_dsi_renesas_panel_device,
#endif
	&msm_kgsl_3d0,
#ifdef CONFIG_BT 
#ifdef CONFIG_MARIMBA_CORE
	&msm_bt_power_device,
#endif
#endif  //end  config_bt
#ifdef CONFIG_BCM_BT
	&msm_bcm_power_device,  //compatible of qualcomm and broadcomm bluetooth chip     
	&msm_bcmsleep_device,
#endif
	&asoc_msm_pcm,
	&asoc_msm_dai0,
	&asoc_msm_dai1,
#ifdef CONFIG_LEDS_MSM_PMIC//added by zhang.yu_1 for enabling CONFIG_LEDS_MSM_PMIC @110823
        &msm_device_pmic_leds,
#endif
#ifdef CONFIG_ZTE_PLATFORM
#ifdef CONFIG_ANDROID_RAM_CONSOLE
	&ram_console_device,
#endif
	&android_leds, //button-backlight  
#endif
	
};

static unsigned pmem_kernel_ebi1_size = PMEM_KERNEL_EBI1_SIZE;
static int __init pmem_kernel_ebi1_size_setup(char *p)
{
	pmem_kernel_ebi1_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_kernel_ebi1_size", pmem_kernel_ebi1_size_setup);

static unsigned pmem_audio_size = MSM_PMEM_AUDIO_SIZE;
static int __init pmem_audio_size_setup(char *p)
{
	pmem_audio_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_audio_size", pmem_audio_size_setup);

static void __init msm_msm7x2x_allocate_memory_regions(void)
{
	void *addr;
	unsigned long size;
#if defined(CONFIG_ZTE_PLATFORM) && defined(CONFIG_F3_LOG)
  /*  begin */
      unsigned int len;
      smem_global *global_tmp = (smem_global *)(MSM_RAM_LOG_BASE + PAGE_SIZE) ;
  
      len = global_tmp->f3log;
  /*  end */
#endif
	
	if (machine_is_msm7625a_surf() || machine_is_msm7625a_ffa())
		fb_size = MSM7x25A_MSM_FB_SIZE;
	else
		fb_size = MSM_FB_SIZE;

	size = fb_size;
	addr = alloc_bootmem_align(size, 0x1000);
	msm_fb_resources[0].start = __pa(addr);
	msm_fb_resources[0].end = msm_fb_resources[0].start + size - 1;
	pr_info("allocating %lu bytes at %p (%lx physical) for fb\n",
		size, addr, __pa(addr));
#if defined(CONFIG_ZTE_PLATFORM) && defined(CONFIG_F3_LOG)
/*  begin */
	pr_info("length = %d ++ \n", len);

	if (len > 12)
		len = 12;
	else
		len = len/2*2;
    
    pr_info("length = %d -- \n", len);
    size = len;
    
	if (size)
		reserve_bootmem(MSM_SDLOG_RAM_PHYS, size*MSM_SDLOG_RAM_SIZE, BOOTMEM_DEFAULT);

	addr = phys_to_virt(MSM_SDLOG_RAM_PHYS);
	pr_info("allocating %lu M at %p (%lx physical) for F3\n",size, addr, __pa(addr));
/*  end */
#endif 	
}

static struct memtype_reserve msm7x27a_reserve_table[] __initdata = {
	[MEMTYPE_SMI] = {
	},
	[MEMTYPE_EBI0] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
	[MEMTYPE_EBI1] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
};

static void __init size_pmem_devices(void)
{

	if (machine_is_msm7625a_surf() || machine_is_msm7625a_ffa()) {
		pmem_mdp_size = MSM7x25A_MSM_PMEM_MDP_SIZE;
		pmem_adsp_size = MSM7x25A_MSM_PMEM_ADSP_SIZE;
	} else {
		pmem_mdp_size = MSM_PMEM_MDP_SIZE;
		pmem_adsp_size = MSM_PMEM_ADSP_SIZE;
	}

#ifdef CONFIG_ANDROID_PMEM
	android_pmem_adsp_pdata.size = pmem_adsp_size;
	android_pmem_pdata.size = pmem_mdp_size;
	android_pmem_audio_pdata.size = pmem_audio_size;
#endif
}

static void __init reserve_memory_for(struct android_pmem_platform_data *p)
{
	msm7x27a_reserve_table[p->memory_type].size += p->size;
}

static void __init reserve_pmem_memory(void)
{
#ifdef CONFIG_ANDROID_PMEM
	reserve_memory_for(&android_pmem_adsp_pdata);
	reserve_memory_for(&android_pmem_pdata);
	reserve_memory_for(&android_pmem_audio_pdata);
	msm7x27a_reserve_table[MEMTYPE_EBI1].size += pmem_kernel_ebi1_size;
#endif
}

static void __init msm7x27a_calculate_reserve_sizes(void)
{
	size_pmem_devices();
	reserve_pmem_memory();
}

static int msm7x27a_paddr_to_memtype(unsigned int paddr)
{
	return MEMTYPE_EBI1;
}

static struct reserve_info msm7x27a_reserve_info __initdata = {
	.memtype_reserve_table = msm7x27a_reserve_table,
	.calculate_reserve_sizes = msm7x27a_calculate_reserve_sizes,
	.paddr_to_memtype = msm7x27a_paddr_to_memtype,
};

static void __init msm7x27a_reserve(void)
{
	reserve_info = &msm7x27a_reserve_info;
	msm_reserve();
}

static void __init msm_device_i2c_init(void)
{
	msm_gsbi0_qup_i2c_device.dev.platform_data = &msm_gsbi0_qup_i2c_pdata;
	msm_gsbi1_qup_i2c_device.dev.platform_data = &msm_gsbi1_qup_i2c_pdata;
}

static struct msm_panel_common_pdata mdp_pdata = {
	.gpio = 97,
	.mdp_rev = MDP_REV_303,
};


#ifdef CONFIG_FB_MSM
#define GPIO_LCDC_BRDG_PD	121
#define GPIO_LCD_BACKLIGHT_EN	97
#ifdef CONFIG_MACH_ATLAS40
#ifdef CONFIG_FB_MSM_GPIO
#define GPIO_LCDC_BRDG_RESET_N 129
#else
#define GPIO_LCDC_BRDG_RESET_N	84
#endif
#endif
#if defined (CONFIG_MACH_NICE) || defined (CONFIG_MACH_SEANPLUS)
#define GPIO_LCDC_BRDG_RESET_N 129
#endif

#ifdef CONFIG_MACH_ROAMER2
#define GPIO_LCDC_BRDG_RESET_N	89
#endif

#define LCDC_RESET_PHYS		0x90008014

static	void __iomem *lcdc_reset_ptr;

static unsigned mipi_dsi_gpio[] = {
	GPIO_CFG(GPIO_LCDC_BRDG_RESET_N, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL,
		GPIO_CFG_4MA),       /* LCDC_BRDG_RESET_N */
	GPIO_CFG(GPIO_LCDC_BRDG_PD, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL,
		GPIO_CFG_4MA),       /* LCDC_BRDG_RESET_N */
	GPIO_CFG(GPIO_LCD_BACKLIGHT_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL,
		GPIO_CFG_2MA),       /* LCDC_BRDG_RESET_N */
};

enum {
	DSI_SINGLE_LANE = 1,
	DSI_TWO_LANES,
};

static int msm_fb_get_lane_config(void)
{
	int rc = DSI_TWO_LANES;

	if (machine_is_msm7625a_surf() || machine_is_msm7625a_ffa()) {
		rc = DSI_SINGLE_LANE;
		pr_info("DSI Single Lane\n");
	} else {
		pr_info("DSI Two Lanes\n");
	}
	return rc;
}

static int msm_fb_dsi_client_reset(void)
{
	int rc = 0;

	rc = gpio_request(GPIO_LCDC_BRDG_RESET_N, "lcdc_brdg_reset_n");
	if (rc < 0) {
		pr_err("failed to request lcd brdg reset_n\n");
		return rc;
	}

	rc = gpio_request(GPIO_LCDC_BRDG_PD, "lcdc_brdg_pd");
	if (rc < 0) {
		pr_err("failed to request lcd brdg pd\n");
		return rc;
	}

	rc = gpio_request(GPIO_LCD_BACKLIGHT_EN, "lcdc_brdg_pd");
	if (rc < 0) {
		pr_err("failed to request lcd backlight\n");
		//return rc;
	}

	rc = gpio_tlmm_config(mipi_dsi_gpio[0], GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("Failed to enable LCDC Bridge reset enable\n");
		goto gpio_error;
	}

	rc = gpio_tlmm_config(mipi_dsi_gpio[1], GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("Failed to enable LCDC Bridge pd enable\n");
		goto gpio_error2;
	}

	rc = gpio_tlmm_config(mipi_dsi_gpio[2], GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("Failed to enable lcd backlight\n");
		//goto gpio_error2;
	}

	rc = gpio_direction_output(GPIO_LCDC_BRDG_RESET_N, 1);
	//rc |= gpio_direction_output(GPIO_LCDC_BRDG_PD, 1);
	//gpio_set_value_cansleep(GPIO_LCDC_BRDG_PD, 0);
	
	if (!rc) {
		if (1/*machine_is_msm7x27a_surf()*/) {
			lcdc_reset_ptr = ioremap_nocache(LCDC_RESET_PHYS,
				sizeof(uint32_t));

			if (!lcdc_reset_ptr)
				return 0;
		}
		return rc;
	} else {
		goto gpio_error;
	}

gpio_error2:
	pr_err("Failed GPIO bridge pd\n");
	//gpio_free(GPIO_LCDC_BRDG_PD);

gpio_error:
	pr_err("Failed GPIO bridge reset\n");
	gpio_free(GPIO_LCDC_BRDG_RESET_N);
	return rc;
}

static struct regulator_bulk_data regs_dsi[] = {
	{ .supply = "bt",   .min_uV = 2800000, .max_uV = 2800000 },
};

static int dsi_gpio_initialized;

static int mipi_dsi_panel_power(int on)
{
	int rc = 0;
	uint32_t lcdc_reset_cfg;

	/* I2C-controlled GPIO Expander -init of the GPIOs very late */
	if (unlikely(!dsi_gpio_initialized)) {


		rc = regulator_bulk_get(NULL, ARRAY_SIZE(regs_dsi), regs_dsi);
		if (rc) {
			pr_err("%s: could not get regulators: %d\n",
					__func__, rc);
			goto fail_gpio2;
		}

		rc = regulator_bulk_set_voltage(ARRAY_SIZE(regs_dsi), regs_dsi);
		if (rc) {
			pr_err("%s: could not set voltages: %d\n",
					__func__, rc);
			goto fail_vreg;
		}

		dsi_gpio_initialized = 1;
	}

	if (on) {
			//gpio_set_value_cansleep(GPIO_LCDC_BRDG_PD, 0);

			if (1/*machine_is_msm7x27a_surf()*/) {
				lcdc_reset_cfg = readl_relaxed(lcdc_reset_ptr);
				rmb();
				lcdc_reset_cfg &= ~1;

			writel_relaxed(lcdc_reset_cfg, lcdc_reset_ptr);
			msleep(20);
			wmb();
			lcdc_reset_cfg |= 1;
			writel_relaxed(lcdc_reset_cfg, lcdc_reset_ptr);
			} else {
				gpio_set_value_cansleep(GPIO_LCDC_BRDG_RESET_N,
					0);
				msleep(20);
				gpio_set_value_cansleep(GPIO_LCDC_BRDG_RESET_N,
					1);
			}

			//if (pmapp_disp_backlight_set_brightness(100))
				//pr_err("backlight set brightness failed\n");
		} else {
			//gpio_set_value_cansleep(GPIO_LCDC_BRDG_PD, 1);

			//if (pmapp_disp_backlight_set_brightness(0))
				//pr_err("backlight set brightness failed\n");
		}
/* sensor && lcd used together, do not to shut it down alone.we need to check it later!
*/
#if 0
	rc = on ? regulator_bulk_enable(ARRAY_SIZE(regs_dsi), regs_dsi) :
		  regulator_bulk_disable(ARRAY_SIZE(regs_dsi), regs_dsi);
#else
	rc = on ? regulator_bulk_enable(ARRAY_SIZE(regs_dsi), regs_dsi) :
		  0;
#endif
	if (rc)
		pr_err("%s: could not %sable regulators: %d\n",
				__func__, on ? "en" : "dis", rc);

	return rc;

fail_vreg:
	regulator_bulk_free(ARRAY_SIZE(regs_dsi), regs_dsi);
fail_gpio2:
	dsi_gpio_initialized = 0;
	return rc;
}
#endif

#define MDP_303_VSYNC_GPIO 121

#ifdef CONFIG_FB_MSM_MDP303
static struct mipi_dsi_platform_data mipi_dsi_pdata = {
	.vsync_gpio = MDP_303_VSYNC_GPIO,
	.dsi_power_save   = mipi_dsi_panel_power,
	.dsi_client_reset = msm_fb_dsi_client_reset,
	.get_lane_config = msm_fb_get_lane_config,
};
#endif

static void __init msm_fb_add_devices(void)
{
	msm_fb_register_device("mdp", &mdp_pdata);
	msm_fb_register_device("lcdc", &lcdc_pdata);
	msm_fb_register_device("mipi_dsi", &mipi_dsi_pdata);
}

#define MSM_EBI2_PHYS			0xa0d00000
#define MSM_EBI2_XMEM_CS2_CFG1		0xa0d10030

static void __init msm7x27a_init_ebi2(void)
{
	uint32_t ebi2_cfg;
	void __iomem *ebi2_cfg_ptr;

	ebi2_cfg_ptr = ioremap_nocache(MSM_EBI2_PHYS, sizeof(uint32_t));
	if (!ebi2_cfg_ptr)
		return;

	ebi2_cfg = readl(ebi2_cfg_ptr);
	if (machine_is_msm7x27a_rumi3() || 1||
			machine_is_msm7625a_surf())
		ebi2_cfg |= (1 << 4); /* CS2 */

	writel(ebi2_cfg, ebi2_cfg_ptr);
	iounmap(ebi2_cfg_ptr);

	/* Enable A/D MUX[bit 31] from EBI2_XMEM_CS2_CFG1 */
	ebi2_cfg_ptr = ioremap_nocache(MSM_EBI2_XMEM_CS2_CFG1,
							 sizeof(uint32_t));
	if (!ebi2_cfg_ptr)
		return;

	ebi2_cfg = readl(ebi2_cfg_ptr);
	if (1 || machine_is_msm7625a_surf())
		ebi2_cfg |= (1 << 31);

	writel(ebi2_cfg, ebi2_cfg_ptr);
	iounmap(ebi2_cfg_ptr);
}

#if 0
#define ATMEL_TS_I2C_NAME "maXTouch"

static struct regulator_bulk_data regs_atmel[] = {
	{ .supply = "ldo2",  .min_uV = 2850000, .max_uV = 2850000 },
	{ .supply = "smps3", .min_uV = 1800000, .max_uV = 1800000 },
};

#define ATMEL_TS_GPIO_IRQ 82

static int atmel_ts_power_on(bool on)
{
	int rc = on ?
		regulator_bulk_enable(ARRAY_SIZE(regs_atmel), regs_atmel) :
		regulator_bulk_disable(ARRAY_SIZE(regs_atmel), regs_atmel);

	if (rc)
		pr_err("%s: could not %sable regulators: %d\n",
				__func__, on ? "en" : "dis", rc);
	else
		msleep(50);

	return rc;
}

static int atmel_ts_platform_init(struct i2c_client *client)
{
	int rc;
	struct device *dev = &client->dev;

	rc = regulator_bulk_get(dev, ARRAY_SIZE(regs_atmel), regs_atmel);
	if (rc) {
		dev_err(dev, "%s: could not get regulators: %d\n",
				__func__, rc);
		goto out;
	}

	rc = regulator_bulk_set_voltage(ARRAY_SIZE(regs_atmel), regs_atmel);
	if (rc) {
		dev_err(dev, "%s: could not set voltages: %d\n",
				__func__, rc);
		goto reg_free;
	}

	rc = gpio_tlmm_config(GPIO_CFG(ATMEL_TS_GPIO_IRQ, 0,
				GPIO_CFG_INPUT, GPIO_CFG_PULL_UP,
				GPIO_CFG_8MA), GPIO_CFG_ENABLE);
	if (rc) {
		dev_err(dev, "%s: gpio_tlmm_config for %d failed\n",
			__func__, ATMEL_TS_GPIO_IRQ);
		goto reg_free;
	}

	/* configure touchscreen interrupt gpio */
	rc = gpio_request(ATMEL_TS_GPIO_IRQ, "atmel_maxtouch_gpio");
	if (rc) {
		dev_err(dev, "%s: unable to request gpio %d\n",
			__func__, ATMEL_TS_GPIO_IRQ);
		goto ts_gpio_tlmm_unconfig;
	}

	rc = gpio_direction_input(ATMEL_TS_GPIO_IRQ);
	if (rc < 0) {
		dev_err(dev, "%s: unable to set the direction of gpio %d\n",
			__func__, ATMEL_TS_GPIO_IRQ);
		goto free_ts_gpio;
	}
	return 0;

free_ts_gpio:
	gpio_free(ATMEL_TS_GPIO_IRQ);
ts_gpio_tlmm_unconfig:
	gpio_tlmm_config(GPIO_CFG(ATMEL_TS_GPIO_IRQ, 0,
				GPIO_CFG_INPUT, GPIO_CFG_NO_PULL,
				GPIO_CFG_2MA), GPIO_CFG_DISABLE);
reg_free:
	regulator_bulk_free(ARRAY_SIZE(regs_atmel), regs_atmel);
out:
	return rc;
}

static int atmel_ts_platform_exit(struct i2c_client *client)
{
	gpio_free(ATMEL_TS_GPIO_IRQ);
	gpio_tlmm_config(GPIO_CFG(ATMEL_TS_GPIO_IRQ, 0,
				GPIO_CFG_INPUT, GPIO_CFG_NO_PULL,
				GPIO_CFG_2MA), GPIO_CFG_DISABLE);
	regulator_bulk_free(ARRAY_SIZE(regs_atmel), regs_atmel);
	return 0;
}

static u8 atmel_ts_read_chg(void)
{
	return gpio_get_value(ATMEL_TS_GPIO_IRQ);
}

static u8 atmel_ts_valid_interrupt(void)
{
	return !atmel_ts_read_chg();
}

#define ATMEL_X_OFFSET 13
#define ATMEL_Y_OFFSET 0

static struct maxtouch_platform_data atmel_ts_pdata = {
	.numtouch = 4,
	.init_platform_hw = atmel_ts_platform_init,
	.exit_platform_hw = atmel_ts_platform_exit,
	.power_on = atmel_ts_power_on,
	.display_res_x = 480,
	.display_res_y = 864,
	.min_x = ATMEL_X_OFFSET,
	.max_x = (505 - ATMEL_X_OFFSET),
	.min_y = ATMEL_Y_OFFSET,
	.max_y = (863 - ATMEL_Y_OFFSET),
	.valid_interrupt = atmel_ts_valid_interrupt,
	.read_chg = atmel_ts_read_chg,
};

static struct i2c_board_info atmel_ts_i2c_info[] __initdata = {
	{
		I2C_BOARD_INFO(ATMEL_TS_I2C_NAME, 0x4a),
		.platform_data = &atmel_ts_pdata,
		.irq = MSM_GPIO_TO_INT(ATMEL_TS_GPIO_IRQ),
	},
};
#endif

#define KP_INDEX(row, col) ((row)*ARRAY_SIZE(kp_col_gpios) + (col))

#if defined (CONFIG_MACH_SEANPLUS)//P765U10(N850L) add camera key
static unsigned int kp_row_gpios[] = {31,32};
static unsigned int kp_col_gpios[] = {36, 37, 38};

static const unsigned short keymap[ARRAY_SIZE(kp_col_gpios) *
					  ARRAY_SIZE(kp_row_gpios)] = {
	[KP_INDEX(0, 0)] = KEY_VOLUMEUP,
	[KP_INDEX(0, 1)] = KEY_VOLUMEDOWN,
	[KP_INDEX(0, 2)] = KEY_RESERVED,
	
	[KP_INDEX(1, 0)] = KEY_CAMERA_FOCUS,
	[KP_INDEX(1, 1)] = KEY_RESERVED,
	[KP_INDEX(1, 2)] = KEY_CAMERA,
};
#else
static unsigned int kp_row_gpios[] = {31,32};
static unsigned int kp_col_gpios[] = {36, 37};

static const unsigned short keymap[ARRAY_SIZE(kp_col_gpios) *
					  ARRAY_SIZE(kp_row_gpios)] = {
	[KP_INDEX(0, 0)] = KEY_VOLUMEUP,
	[KP_INDEX(0, 1)] = KEY_VOLUMEDOWN,
	
	[KP_INDEX(1, 0)] = KEY_F18,
	[KP_INDEX(1, 1)] = KEY_RESERVED,
};
#endif

/* SURF keypad platform device information */
static struct gpio_event_matrix_info kp_matrix_info = {
	.info.func	= gpio_event_matrix_func,
	.keymap		= keymap,
	.output_gpios	= kp_row_gpios,
	.input_gpios	= kp_col_gpios,
	.noutputs	= ARRAY_SIZE(kp_row_gpios),
	.ninputs	= ARRAY_SIZE(kp_col_gpios),
	.settle_time.tv_nsec = 40 * NSEC_PER_USEC,
	.poll_time.tv_nsec = 20 * NSEC_PER_MSEC,
	.debounce_delay.tv_nsec = 10 * NSEC_PER_MSEC,
	.flags		= GPIOKPF_LEVEL_TRIGGERED_IRQ | GPIOKPF_DRIVE_INACTIVE |
			  GPIOKPF_PRINT_UNMAPPED_KEYS|GPIOKPF_DEBOUNCE,
};

static struct gpio_event_info *kp_info[] = {
	&kp_matrix_info.info
};

static struct gpio_event_platform_data kp_pdata = {
#if defined (CONFIG_MACH_ATLAS40)
	.name		= "atlas40_keypad",//"7x27a_kp",
#elif defined (CONFIG_MACH_NICE)
	.name		= "nice_keypad",//"7x27a_kp",
#elif defined (CONFIG_MACH_SEANPLUS)
	.name		= "seanplus_keypad",
#elif defined (CONFIG_MACH_ROAMER2)
	.name		= "roamer2_keypad",	
#endif
	.info		= kp_info,
	.info_count	= ARRAY_SIZE(kp_info)
};

static struct platform_device kp_pdev = {
	.name	= GPIO_EVENT_DEV_NAME,
	.id	= -1,
	.dev	= {
		.platform_data	= &kp_pdata,
	},
};


static struct msm_handset_platform_data hs_platform_data = {
	.hs_name = "7k_handset",
	.pwr_key_delay_ms = 500, /* 0 will disable end key */
};

static struct platform_device hs_pdev = {
	.name   = "msm-handset",
	.id     = -1,
	.dev    = {
		.platform_data = &hs_platform_data,
	},
};

static struct platform_device msm_proccomm_regulator_dev = {
	.name   = PROCCOMM_REGULATOR_DEV_NAME,
	.id     = -1,
	.dev    = {
		.platform_data = &msm7x27a_proccomm_regulator_data
	}
};
#if 0
#ifdef CONFIG_ZTE_PLATFORM
static ssize_t debug_global_read(struct file *file, char __user *buf,
				    size_t len, loff_t *offset)
{
	loff_t pos = *offset;
	ssize_t count;
  ssize_t size;

  size = sizeof(smem_global);

	if (pos >= size)
		return 0;

	count = min(len, (size_t)(size - pos));
	if (copy_to_user(buf, (char *)global + pos, count))
		return -EFAULT;

	*offset += count;
	return count;
}

static struct file_operations debug_global_file_ops = {
	.owner = THIS_MODULE,
	.read = debug_global_read,
};
#endif
#endif

static void __init msm7627a_rumi3_init(void)
{
	msm7x27a_init_ebi2();
	platform_add_devices(rumi_sim_devices,
			ARRAY_SIZE(rumi_sim_devices));
}

#define LED_GPIO_PDM		0xff
#define UART1DM_RX_GPIO		45

static void __init msm7x27a_init_regulators(void)
{
	int rc = platform_device_register(&msm_proccomm_regulator_dev);
	if (rc)
		pr_err("%s: could not register regulator device: %d\n",
				__func__, rc);
}

static void set_camera_reset_low(void)
{
    int rc = 0;

    rc = gpio_request(27, NULL);
    if (0 == rc)
    {
        /* ignore "rc" */
        rc = gpio_direction_output(27, 0);

        mdelay(10);
    }

    gpio_free(27);
    mdelay(10);
}
#if defined (CONFIG_MACH_ATLAS40) || defined (CONFIG_MACH_NICE) || defined(CONFIG_MACH_SEANPLUS)
/*������粻��ȫ������*/
static void touch_vdd(void)
{
	struct vreg *vreg_s3_sp=NULL;
	int ret;
	vreg_s3_sp = vreg_get(NULL, "msme1");
	if (IS_ERR(vreg_s3_sp)) 
	{
		pr_err("%s: vreg_get for S3 failed\n", __func__);
	}
	else
	{
		vreg_disable(vreg_s3_sp);
	}
	gpio_tlmm_config(GPIO_CFG(131, 0, GPIO_CFG_OUTPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_8MA), GPIO_CFG_ENABLE);
	gpio_tlmm_config(GPIO_CFG(132, 0, GPIO_CFG_OUTPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_8MA), GPIO_CFG_ENABLE);
	
	ret = gpio_request(131,"I2C_SCL");
	if (ret) printk(KERN_ERR "%s: gpio_request: %d failed!\n", __func__, 131);			
	gpio_direction_output(131, 0);
	ret = gpio_request(132,"I2C_SDA");
	if (ret) printk(KERN_ERR "%s: gpio_request: %d failed!\n", __func__, 132);	
	gpio_direction_output(132, 0);
	ret = gpio_request(13,"TCHN_3V_EN");
	if (ret) printk(KERN_ERR "%s: gpio_request: %d failed!\n", __func__, 13);	
	gpio_direction_output(13, 0);//TCHN_3V_EN

	ret = gpio_request(12,"TCHN_RST");
	if (ret) printk(KERN_ERR "%s: gpio_request: %d failed!\n", __func__, 12);	
	gpio_direction_output(12, 0);
	
	ret = gpio_request(82,"CAP_TS_INT");
	if (ret) printk(KERN_ERR "%s: gpio_request: %d failed!\n", __func__, 82);	
	gpio_direction_output(82, 0);//CAP_TS_INT
	msleep(500);
	
	gpio_direction_output(12, 1);
	gpio_direction_output(13, 1);
	if (!IS_ERR(vreg_s3_sp)) 
	{
		vreg_enable(vreg_s3_sp);
	}
	gpio_direction_input(82);		
	gpio_free(131);
	gpio_free(132);
	gpio_free(13);
	gpio_free(12);
	gpio_free(82);
}
#elif defined (CONFIG_MACH_ROAMER2)
/*������粻��ȫ������*/
static void touch_vdd(void)
{
	struct vreg *vreg_s3_sp=NULL;
	int ret;
	vreg_s3_sp = vreg_get(NULL, "msme1");
	if (IS_ERR(vreg_s3_sp)) 
	{
		pr_err("%s: vreg_get for S3 failed\n", __func__);
	}
	else
	{
		vreg_disable(vreg_s3_sp);
	}
	gpio_tlmm_config(GPIO_CFG(131, 0, GPIO_CFG_OUTPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_8MA), GPIO_CFG_ENABLE);
	gpio_tlmm_config(GPIO_CFG(132, 0, GPIO_CFG_OUTPUT,
				GPIO_CFG_NO_PULL, GPIO_CFG_8MA), GPIO_CFG_ENABLE);
	
	ret = gpio_request(131,"I2C_SCL");
	if (ret) printk(KERN_ERR "gpio_request: %d failed!\n", 131);			
	gpio_direction_output(131, 0);
	ret = gpio_request(132,"I2C_SDA");
	if (ret) printk(KERN_ERR "gpio_request: %d failed!\n", 132);	
	gpio_direction_output(132, 0);
	ret = gpio_request(107,"TCHN_3V_EN");
	if (ret) printk(KERN_ERR "gpio_request: %d failed!\n", 107);	
	gpio_direction_output(107, 0);//TCHN_3V_EN
	ret = gpio_request(82,"CAP_TS_INT");
	if (ret) printk(KERN_ERR "gpio_request: %d failed!\n", 82);	
	gpio_direction_output(82, 0);//CAP_TS_INT
	msleep(500);
	gpio_direction_output(107, 1);
	if (!IS_ERR(vreg_s3_sp)) 
	{
		vreg_enable(vreg_s3_sp);
	}
	gpio_direction_input(82);		
	gpio_free(131);
	gpio_free(132);
	gpio_free(107);
	gpio_free(82);
		
}
#endif


#ifdef CONFIG_ZTE_PLATFORM

static void zte_get_secboot_value(void)
{
  smem_global *pglobal= ioremap(SMEM_LOG_GLOBAL_BASE, sizeof(smem_global));
  g_zte_secboot_mode = pglobal->secboot_enable;
  
  printk(KERN_DEBUG "g_zte_secboot_mode: %d\n",g_zte_secboot_mode);
}

int zte_is_secboot_mode(void)
{
	return g_zte_secboot_mode;
}

EXPORT_SYMBOL(zte_is_secboot_mode);
static void set_zte_board_id_type(void)
{
       smem_global *global_tmp;
	global_tmp = ioremap(SMEM_LOG_GLOBAL_BASE, sizeof(smem_global));
	g_zte_board_id_type = global_tmp->mboard_id;
	
	printk(KERN_DEBUG "g_zte_board_id_type: %d\n",g_zte_board_id_type);

	sync_sysfs_board_id(g_zte_board_id_type);	
}

int get_zte_board_id_type(void)
{
	return g_zte_board_id_type;
}

EXPORT_SYMBOL(get_zte_board_id_type);

#endif

int wlan_init_power(void);	 

static void __init msm7x2x_init(void)
{
//#ifdef CONFIG_ZTE_PLATFORM
	//struct proc_dir_entry *entry;
//#endif
	msm7x2x_misc_init();
	
#if defined (CONFIG_MACH_ATLAS40) || defined (CONFIG_MACH_NICE) || defined (CONFIG_MACH_SEANPLUS) || defined(CONFIG_MACH_ROAMER2)
	touch_vdd();/*������粻��ȫ������*/
#endif
#ifdef CONFIG_ZTE_PLATFORM
		   set_zte_board_id_type();
		   zte_get_secboot_value();		   
#endif


	/* Initialize regulators first so that other devices can use them */
	msm7x27a_init_regulators();

	/* Common functions for SURF/FFA/RUMI3 */
	msm_device_i2c_init();
	msm7x27a_init_ebi2();
	msm7x27a_cfg_uart2dm_serial();
#ifdef CONFIG_SERIAL_MSM_HS
	msm_uart_dm1_pdata.wakeup_irq = gpio_to_irq(UART1DM_RX_GPIO);
	msm_device_uart_dm1.dev.platform_data = &msm_uart_dm1_pdata;
#endif
	set_camera_reset_low();

#ifdef CONFIG_USB_MSM_OTG_72K
	msm_otg_pdata.swfi_latency =
		msm7x27a_pm_data
		[MSM_PM_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT].latency;
	msm_otg_pdata.phy_init_seq_override = zte_phy_init_seq_override;
	msm_device_otg.dev.platform_data = &msm_otg_pdata;
#endif
	msm_device_gadget_peripheral.dev.platform_data =
		&msm_gadget_pdata;
	msm7x27a_cfg_smsc911x();
	platform_add_devices(msm_footswitch_devices,
			msm_num_footswitch_devices);
	platform_add_devices(surf_ffa_devices,
			ARRAY_SIZE(surf_ffa_devices));

	wlan_init_power(); 
	
#ifdef CONFIG_MMC_MSM
	msm7627a_init_mmc();
#endif
	msm_fb_add_devices();
#ifdef CONFIG_USB_EHCI_MSM_72K
	msm7x2x_init_host();
#endif

	msm_pm_set_platform_data(msm7x27a_pm_data,
				ARRAY_SIZE(msm7x27a_pm_data));
	BUG_ON(msm_pm_boot_init(&msm_pm_boot_pdata));

#if defined(CONFIG_I2C) && defined(CONFIG_GPIO_SX150X)
	register_i2c_devices();
#endif
#if defined(CONFIG_BT) && defined(CONFIG_MARIMBA_CORE)
       msm7627a_bt_power_init();
#endif   //end   config_bt  && config_marimba_core
#ifdef CONFIG_BCM_BT
	bt_power_init();
#endif
#if 0
	if (machine_is_msm7625a_surf() || machine_is_msm7625a_ffa()) {
		atmel_ts_pdata.min_x = 0;
		atmel_ts_pdata.max_x = 480;
		atmel_ts_pdata.min_y = 0;
		atmel_ts_pdata.max_y = 320;
	}

	i2c_register_board_info(MSM_GSBI1_QUP_I2C_BUS_ID,
		atmel_ts_i2c_info,
		ARRAY_SIZE(atmel_ts_i2c_info));
#endif
#ifdef CONFIG_INPUT_TOUCHSCREEN
	if ( !get_ftm_from_tag() )
		msm7x27a_ts_init();
#endif

#if 0	
//#if defined(CONFIG_MSM_CAMERA)
	msm_camera_vreg_init();
	i2c_register_board_info(MSM_GSBI0_QUP_I2C_BUS_ID,
			i2c_camera_devices,
			ARRAY_SIZE(i2c_camera_devices));
#endif
#if defined(CONFIG_MSM_CAMERA)
	msm7627a_camera_init();
#endif

	platform_device_register(&kp_pdev);
	platform_device_register(&hs_pdev);

	/* configure it as a pdm function*/
	if (gpio_tlmm_config(GPIO_CFG(LED_GPIO_PDM, 3,
				GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL,
				GPIO_CFG_8MA), GPIO_CFG_ENABLE))
		pr_err("%s: gpio_tlmm_config for %d failed\n",
			__func__, LED_GPIO_PDM);
	else
	{
#ifndef CONFIG_ZTE_PLATFORM	
		platform_device_register(&led_pdev);
#endif		
	}

#ifdef CONFIG_MSM_RPC_VIBRATOR
	//if (machine_is_msm7x27a_ffa() || machine_is_msm7625a_ffa())//modified by zhang.yu_1 for vibrator init @110726
		msm_init_pmic_vibrator();
#endif
	/*7x25a kgsl initializations*/
	msm7x25a_kgsl_3d0_init();
#if 0
	#ifdef CONFIG_ZTE_PLATFORM	
	global = ioremap(SMEM_LOG_GLOBAL_BASE, sizeof(smem_global));
	if (!global) {
		printk(KERN_ERR "ioremap failed with SCL_SMEM_LOG_RAM_BASE\n");
		return;
	}
	entry = create_proc_entry("smem_global", S_IFREG | S_IRUGO, NULL);
	if (!entry) {
		printk(KERN_ERR "smem_global: failed to create proc entry\n");
		return;
	}
	entry->proc_fops = &debug_global_file_ops;
	entry->size = sizeof(smem_global);
#endif	
#endif

}

static void __init msm7x2x_init_early(void)
{
	//zte_ftm_set_value must be called before socinfo_init in msm7x2x_misc_init function
#ifdef CONFIG_ZTE_FTM_FLAG_SUPPORT
   extern void zte_ftm_set_value(int val);
	zte_ftm_set_value(g_zte_ftm_flag_fixup);
#endif
	msm_msm7x2x_allocate_memory_regions();
}

#if defined (CONFIG_MACH_ATLAS40)
MACHINE_START(ATLAS40, "atlas40")
#elif defined (CONFIG_MACH_NICE)
MACHINE_START(NICE, "nice")
#elif defined (CONFIG_MACH_SEANPLUS)
MACHINE_START(SEANPLUS, "seanplus")
#elif defined (CONFIG_MACH_ROAMER2)
MACHINE_START(ROAMER2, "roamer2")
#endif
	.boot_params	= PHYS_OFFSET + 0x100,
	.map_io		= msm_common_io_init,
	.reserve	= msm7x27a_reserve,
	.init_irq	= msm_init_irq,
	.init_machine	= msm7x2x_init,
	.timer		= &msm_timer,
	.init_early     = msm7x2x_init_early,
#ifdef CONFIG_ZTE_PLATFORM
	.fixup          = zte_fixup,
#endif	

	.handle_irq	= vic_handle_irq,	
MACHINE_END

MACHINE_START(MSM7X27A_RUMI3, "QCT MSM7x27a RUMI3")
	.boot_params	= PHYS_OFFSET + 0x100,
	.map_io		= msm_common_io_init,
	.reserve	= msm7x27a_reserve,
	.init_irq	= msm_init_irq,
	.init_machine	= msm7627a_rumi3_init,
	.timer		= &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= vic_handle_irq,
MACHINE_END
MACHINE_START(MSM7X27A_SURF, "QCT MSM7x27a SURF")
	.boot_params	= PHYS_OFFSET + 0x100,
	.map_io		= msm_common_io_init,
	.reserve	= msm7x27a_reserve,
	.init_irq	= msm_init_irq,
	.init_machine	= msm7x2x_init,
	.timer		= &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= vic_handle_irq,
MACHINE_END
MACHINE_START(MSM7X27A_FFA, "QCT MSM7x27a FFA")
	.boot_params	= PHYS_OFFSET + 0x100,
	.map_io		= msm_common_io_init,
	.reserve	= msm7x27a_reserve,
	.init_irq	= msm_init_irq,
	.init_machine	= msm7x2x_init,
	.timer		= &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= vic_handle_irq,
MACHINE_END
MACHINE_START(MSM7625A_SURF, "QCT MSM7625a SURF")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm_common_io_init,
	.reserve        = msm7x27a_reserve,
	.init_irq       = msm_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= vic_handle_irq,
MACHINE_END
MACHINE_START(MSM7625A_FFA, "QCT MSM7625a FFA")
	.boot_params    = PHYS_OFFSET + 0x100,
	.map_io         = msm_common_io_init,
	.reserve        = msm7x27a_reserve,
	.init_irq       = msm_init_irq,
	.init_machine   = msm7x2x_init,
	.timer          = &msm_timer,
	.init_early     = msm7x2x_init_early,
	.handle_irq	= vic_handle_irq,
MACHINE_END
