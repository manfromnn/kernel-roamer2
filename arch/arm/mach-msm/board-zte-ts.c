#include <linux/i2c.h>
#include <linux/i2c/sx150x.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <asm/mach-types.h>
#include <mach/msm_iomap.h>
#include <mach/board.h>
#include <mach/irqs-7xxx.h>
#include "devices-msm7x2xa.h"
#include "board-msm7627a.h"



#ifdef CONFIG_TOUCHSCREEN_MXT224
//#include <linux/atmel_maxtouch.h>
#include <linux/input/atmel_qt602240.h>
//extern struct atmel_i2c_platform_data atmel_data;
#endif
#ifdef CONFIG_TOUCHSCREEN_SYNAPTICS
#include <linux/input/synaptics_rmi.h> 
#endif
#ifdef CONFIG_TOUCHSCREEN_FOCALTECH
#include <linux/input/focaltech_ft5x0x.h>
#endif
#ifdef CONFIG_TOUCHSCREEN_CY8CTST241
#include <linux/input/cyttsp.h>
#endif
#ifdef CONFIG_TOUCHSCREEN_GOODIX_GT9XX
#include <linux/input/goodix_gt9xx.h>
#endif



//
//  touchscreen gpio definition
//
#if defined (CONFIG_MACH_ATLAS40)
#define GPIO_7X27A_TS_IRQ		82
#define GPIO_7X27A_TS_EN		13
#define GPIO_7X27A_TS_RST		12
#elif defined (CONFIG_MACH_NICE) || defined (CONFIG_MACH_SEANPLUS)
#define GPIO_7X27A_TS_IRQ		82
#define GPIO_7X27A_TS_EN		13
#define GPIO_7X27A_TS_RST		12
#elif defined (CONFIG_MACH_ROAMER2)
#define GPIO_7X27A_TS_IRQ		82
#define GPIO_7X27A_TS_EN		107
#define GPIO_7X27A_TS_RST		12//actually none
#endif


//
//	touchscreen firmware file name
//
#if defined (CONFIG_MACH_ATLAS40)	// p740a
#define FTC_FW_NAME "Ver11_20120409_N880E_9980_ID0x57_ATLAS40_app.bin"
//#define SYN_FW_NAME "PR1101200-s2202_zte_30303039-ATLAS40-truly.img"
#define SYN_FW_NAME "ATLAS40_PR1115996-s2202_Truly_32323038.img"
#define ATM_FW_NAME ""
#elif defined (CONFIG_MACH_NICE) //P752T
//#define FTC_FW_NAME "Ver15_20120206_035_9920_ID0x57_roamer2_app.bin"
#define FTC_FW_NAME "Ver17_20121018_035_9920_ID0x57_app.bin"
#define SYN_FW_NAME "TM2346-P752T-LEAD_PR1197429-s2200_31353033.img"
#define ATM_FW_NAME ""
#elif defined (CONFIG_MACH_SEANPLUS) //P765U10(N850L)
#define FTC_FW_NAME ""
//#define SYN_FW_NAME "N850L_TRULY_PR1228845_31323036.img"
#define SYN_FW_NAME "20121025_N850L_TRULY_PR1228845_31323039.img"
#define ATM_FW_NAME ""
#elif defined (CONFIG_MACH_ROAMER2)//P752D01  V790
//#define FTC_FW_NAME "Ver15_20120206_035_9920_ID0x57_roamer2_app.bin"
#define FTC_FW_NAME "Ver17_20121018_035_9920_ID0x57_app.bin"
#define SYN_FW_NAME "ROAMER2_PR1108473_S2200_ZTE_bigthumb_31333038.img"
#define ATM_FW_NAME ""
#else
#define FTC_FW_NAME ""
#define SYN_FW_NAME ""
#define ATM_FW_NAME ""
#endif



//
//	touchscreen virtual key definition
//

#ifdef CONFIG_TOUCHSCREEN_VIRTUAL_KEYS
//#define CAP_TS_VKEY_CYPRESS "virtualkeys.cyttsp-i2c"
#define CAP_TS_VKEY_SYNAPTICS "virtualkeys.syna-touchscreen"
#define CAP_TS_VKEY_ATMEL "virtualkeys.atmel-touchscreen"
#define CAP_TS_VKEY_FTS "virtualkeys.Fts-touchscreen"
#define CAP_TS_VKEY_CYTTSP "virtualkeys.cysp-touchscreen"
#define CAP_TS_VKEY_MELFAS "virtualkeys.melf-touchscreen"
#define CAP_TS_VKEY_GOODIX "virtualkeys.gdx-touchscreen"

// board atlas40
#if defined (CONFIG_MACH_ATLAS40) 
#define SYNAPTICS_MAX_Y_POSITION	1747
static ssize_t cap_ts_vkeys_show(struct device *dev,	struct device_attribute *attr, char *buf)
{
	bool flag = false;

	//printk("%s, %s\n",__func__,attr->attr.name);

#if defined (CONFIG_VKEY_765E03)
	flag = true;
#endif

	if (flag)	// for P765E03
		return sprintf(
			buf,__stringify(EV_KEY) ":" __stringify(KEY_BACK) ":85:850:100:80"
			":" __stringify(EV_KEY) ":" __stringify(KEY_HOME) ":245:850:100:80"
			":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":395:850:100:80"
			"\n");
	else
		return sprintf(
			buf,__stringify(EV_KEY) ":" __stringify(KEY_MENU) ":45:850:100:80"
			":" __stringify(EV_KEY) ":" __stringify(KEY_HOME) ":175:850:100:80"
			":" __stringify(EV_KEY) ":" __stringify(KEY_BACK) ":306:850:100:80"
			":" __stringify(EV_KEY) ":" __stringify(KEY_SEARCH) ":437:850:100:80"
			"\n");
}

// board nice
#elif defined (CONFIG_MACH_NICE)
#define SYNAPTICS_MAX_Y_POSITION	1478//1602
static ssize_t cap_ts_vkeys_show(struct device *dev,	struct device_attribute *attr, char *buf)
{
	//printk("%s, %s\n",__func__,attr->attr.name);
	return sprintf(
		buf,__stringify(EV_KEY) ":" __stringify(KEY_HOME) ":45:520:50:50"
		":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":120:520:60:50"
		":" __stringify(EV_KEY) ":" __stringify(KEY_BACK) ":200:520:60:50"
		":" __stringify(EV_KEY) ":" __stringify(KEY_SEARCH) ":265:520:60:50"
		"\n");	
}
#elif defined(CONFIG_MACH_SEANPLUS) //N850L (P765U10)  320*480   BACK HOME MENU 
/*ergate-009*/
#define SYNAPTICS_MAX_Y_POSITION	1478//the y cord. read from ic is 1628
static ssize_t cap_ts_vkeys_show(struct device *dev,	struct device_attribute *attr, char *buf)
{
	//printk("%s, %s\n",__func__,attr->attr.name);
	return sprintf(
		buf,__stringify(EV_KEY) ":" __stringify(KEY_BACK) ":40:520:80:50"
		":" __stringify(EV_KEY) ":" __stringify(KEY_HOME) ":160:520:80:50"
		":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":280:520:80:50"
		"\n");	
}
#elif defined (CONFIG_MACH_ROAMER2)//P752D01  V790  320*480       MENU HOME BACK SEARCH
#define SYNAPTICS_MAX_Y_POSITION	1478//1602
static ssize_t cap_ts_vkeys_show(struct device *dev,	struct device_attribute *attr, char *buf)
{
	//printk("%s, %s\n",__func__,attr->attr.name);
	return sprintf(
		buf,__stringify(EV_KEY) ":" __stringify(KEY_MENU) ":45:520:50:50"
		":" __stringify(EV_KEY) ":" __stringify(KEY_HOME) ":120:520:60:50"
		":" __stringify(EV_KEY) ":" __stringify(KEY_BACK) ":200:520:60:50"
		":" __stringify(EV_KEY) ":" __stringify(KEY_SEARCH) ":265:520:50:50"
		"\n");	
}
#endif


static struct device_attribute cap_ts_device_attr[] = {
#if defined(CONFIG_TOUCHSCREEN_FOCALTECH)
	{
		.attr = {
			.name = CAP_TS_VKEY_FTS,
			.mode = S_IRUGO,
		},
		.show	= &cap_ts_vkeys_show,
		.store	= NULL,
	},
#endif
#if defined(CONFIG_TOUCHSCREEN_SYNAPTICS)
{
	.attr = {
		.name = CAP_TS_VKEY_SYNAPTICS,
		.mode = S_IRUGO,
	},
	.show	= &cap_ts_vkeys_show,
	.store	= NULL,
},
#endif
#if defined(CONFIG_TOUCHSCREEN_MXT224)
	{
		.attr = {
			.name = CAP_TS_VKEY_ATMEL,
			.mode = S_IRUGO,
		},
		.show	= &cap_ts_vkeys_show,
		.store	= NULL,
	},
#endif
#if defined(CONFIG_TOUCHSCREEN_CY8CTST241)
	{
		.attr = {
			.name = CAP_TS_VKEY_CYTTSP,
			.mode = S_IRUGO,
		},
		.show	= &cap_ts_vkeys_show,
		.store	= NULL,
	},
#endif
#if defined(CONFIG_TOUCHSCREEN_MELFAS_MIP)
	{
		.attr = {
			.name = CAP_TS_VKEY_MELFAS,
			.mode = S_IRUGO,
		},
		.show	= &cap_ts_vkeys_show,
		.store	= NULL,
	},
#endif
#if defined (CONFIG_TOUCHSCREEN_GOODIX_GT9XX)
{
	.attr = {
		.name = CAP_TS_VKEY_GOODIX,
		.mode = S_IRUGO,
	},
	.show	= &cap_ts_vkeys_show,
	.store	= NULL,
},
#endif
};

struct kobject *android_touch_kobj;
static int cap_ts_vkeys_init(void)
{
	int rc,i;
	struct kobject * cap_ts_properties_kobj=NULL;

	cap_ts_properties_kobj = kobject_create_and_add("board_properties", NULL);
	if (cap_ts_properties_kobj == NULL) {
		printk("%s: subsystem_register failed\n", __func__);
		rc = -ENOMEM;
		return rc;
	}
	android_touch_kobj = cap_ts_properties_kobj;

	for ( i=0; i < ARRAY_SIZE(cap_ts_device_attr); i++ ){
		rc = sysfs_create_file(cap_ts_properties_kobj, &cap_ts_device_attr[i].attr);
		if (rc) {
			printk("%s: sysfs_create_file failed\n", __func__);
			return rc;
		}
	}

	return 0;
}
#else

#if defined(CONFIG_MACH_ATLAS40)
#define SYNAPTICS_MAX_Y_POSITION	1747	//temp for p772a40
#else
#define SYNAPTICS_MAX_Y_POSITION	0
#endif

#endif



static void touchscreen_irq( int hl, bool io_flag )
{
	//io_flag: true- default input, false - output

	if ( io_flag == true )
		gpio_direction_input(GPIO_7X27A_TS_IRQ);
	else
		gpio_direction_output(GPIO_7X27A_TS_IRQ, hl);

	return;
}

static void touchscreen_reset( int hl )
{
	gpio_direction_output(GPIO_7X27A_TS_RST, hl);
	return;
}

static void touchscreen_power(int on_off)
{
	//pr_info("%s %d\n",__func__, on_off );

	if ( on_off == 1 )
		gpio_direction_output(GPIO_7X27A_TS_EN, 1);

	if ( on_off == 0 )
		gpio_direction_output(GPIO_7X27A_TS_EN,0);

	return;
}
static int touchscreen_gpio_init(int flag)
{
	int ret = 0;

	//init
	if ( flag == 1 )
	{
		ret = gpio_request(GPIO_7X27A_TS_EN, "touch voltage");
		if ( ret ){
			pr_err("%s, gpio %d request failed!\n", __func__,GPIO_7X27A_TS_EN);
			return -1;
		}

		ret = gpio_request(GPIO_7X27A_TS_RST, "touch voltage");
		if (ret){
			pr_err("%s: gpio %d request is error!\n", __func__, GPIO_7X27A_TS_RST);
			return -1;
		}

		ret = gpio_request(GPIO_7X27A_TS_IRQ, "touch voltage");
		if (ret){
			pr_err("%s: gpio %d request is error!\n", __func__, GPIO_7X27A_TS_IRQ);
			return -1;
		}
	}

	//deinit
	if ( flag == 0)
	{
		gpio_free(GPIO_7X27A_TS_EN);
		gpio_free(GPIO_7X27A_TS_IRQ);
		gpio_free(GPIO_7X27A_TS_RST);
	}

	return 0;

}


//
// i2c device definition
//

#if defined (CONFIG_TOUCHSCREEN_SYNAPTICS)
static struct synaptics_rmi_data synaptics_ts_data = {
	.gpio_init = touchscreen_gpio_init,
	.power	= touchscreen_power,
	.reset	= touchscreen_reset,
	.irq	= touchscreen_irq,
	.max_y_position = SYNAPTICS_MAX_Y_POSITION,	// 0 - no vkey, do nothing
	.fwfile = SYN_FW_NAME,
};
#endif

#if defined (CONFIG_TOUCHSCREEN_FOCALTECH)
static struct focaltech_ts_platform_data focaltech_ts_data = {
	.gpio_init = touchscreen_gpio_init,
	.power	= touchscreen_power,
	.reset	= touchscreen_reset,
	.irq	= touchscreen_irq,
	.fwfile	= FTC_FW_NAME,
};
#endif

#if defined (CONFIG_TOUCHSCREEN_MXT224)
static struct atmel_platform_data atmel_ts_data = {
	.gpio_init = touchscreen_gpio_init,
	.power	= touchscreen_power,
	.reset	= touchscreen_reset,
	.irq	= touchscreen_irq,
	.fwfile	= ATM_FW_NAME,
};
#endif

#if defined (CONFIG_TOUCHSCREEN_GOODIX_GT9XX)
static struct goodix_platform_data goodix_ts_data = {
	.gpio_init = touchscreen_gpio_init,
	.power	= touchscreen_power,
	.reset	= touchscreen_reset,
	.irq	= touchscreen_irq,
//	.fwfile = NULL,
};
#endif


#ifdef CONFIG_TOUCHSCREEN_CY8CTST241

#define CYPRESS_I2C_ADDR		0x24
#define CY_USE_MT		/* define if using Single-Touch */

/* optional init function; set up IRQ GPIO;
 * call reference in platform data structure
 */
static int cyttsp_i2c_init(int on)
{
	int ret;

	/* add any special code to initialize any required system hw
	 * such as regulators or gpio pins
	 */

	if (on) {
		/* for MSM systems the call to gpio_direction_input can be
		 * replaced with the more explicit call:
		 gpio_tlmm_config(GPIO_CFG(GPIO_TS_IRQ, 0, GPIO_INPUT,
			GPIO_PULL_UP, GPIO_6MA), GPIO_ENABLE);
		 */
		ret = gpio_request(GPIO_7X27A_TS_IRQ, "CYTTSP IRQ GPIO");
		if (ret) {
			printk("%s: Failed to request GPIO %d\n",
			       __func__, GPIO_7X27A_TS_IRQ);
			return ret;
		}
		gpio_direction_input(GPIO_7X27A_TS_IRQ);

	} else {
		gpio_free(GPIO_7X27A_TS_IRQ);
	}
	return 0;
}

static int cyttsp_i2c_wakeup(void)
{
    gpio_direction_output(GPIO_7X27A_TS_IRQ, 1);
	usleep(10);	
	gpio_set_value(GPIO_7X27A_TS_IRQ,0);
	usleep(10);
	gpio_set_value(GPIO_7X27A_TS_IRQ,1);
	usleep(10);
	gpio_set_value(GPIO_7X27A_TS_IRQ,0);
	usleep(10);
	gpio_set_value(GPIO_7X27A_TS_IRQ,1);
	
	usleep(10);
	gpio_direction_input(GPIO_7X27A_TS_IRQ);

	return 0;
}

static struct cyttsp_platform_data cypress_i2c_ttsp_platform_data = {
	.wakeup = cyttsp_i2c_wakeup,
	//.wakeup = NULL,
	.init = cyttsp_i2c_init,
#ifdef CY_USE_MT
	.mt_sync = input_mt_sync,
#endif
	.maxx = 320,
	.maxy = 480,
	.flags = 0,
	.gen = CY_GEN2,
	.use_st = 0,
	.use_mt = 1,
	.use_trk_id = 0,
	.use_hndshk = 1,
	.use_timer = 0,
	.use_sleep = 1,
	.use_gestures = 0,
	.use_load_file = 0,
	.use_force_fw_update = 0,
	.use_virtual_keys = 1,
	/* activate up to 4 groups
	 * and set active distance
	 */
	.gest_set = CY_GEST_GRP_NONE | CY_ACT_DIST,
	/* change act_intrvl to customize the Active power state
	 * scanning/processing refresh interval for Operating mode
	 */
	.act_intrvl = CY_ACT_INTRVL_DFLT,
	/* change tch_tmout to customize the touch timeout for the
	 * Active power state for Operating mode
	 */
	.tch_tmout = CY_TCH_TMOUT_DFLT,
	/* change lp_intrvl to customize the Low Power power state
	 * scanning/processing refresh interval for Operating mode
	 */
	.lp_intrvl = CY_LP_INTRVL_DFLT,
	.name = CY_I2C_NAME,
	.irq_gpio = GPIO_7X27A_TS_IRQ,
};

#endif	// end-CONFIG_TOUCHSCREEN_CY8CTST241

static struct i2c_board_info i2c_touch_devices[] = {
#ifdef CONFIG_TOUCHSCREEN_FOCALTECH	
	{				
		I2C_BOARD_INFO("ft5x0x_ts", 0x3E ),
		.irq = MSM_GPIO_TO_INT(GPIO_7X27A_TS_IRQ),
		.platform_data = &focaltech_ts_data,
	},	
#endif
#ifdef CONFIG_TOUCHSCREEN_SYNAPTICS
	{
		I2C_BOARD_INFO("syna-touchscreen", 0x22 ),
		.irq = MSM_GPIO_TO_INT(GPIO_7X27A_TS_IRQ),
		.platform_data = &synaptics_ts_data,
	},
#endif
#ifdef CONFIG_TOUCHSCREEN_MXT224
	{    
		I2C_BOARD_INFO("atmel_qt602240", 0x4a ),
		.platform_data = &atmel_ts_data,
		.irq = MSM_GPIO_TO_INT(GPIO_7X27A_TS_IRQ),
	},   
#endif
#ifdef CONFIG_TOUCHSCREEN_CY8CTST241
	{
		I2C_BOARD_INFO(CY_I2C_NAME, CYPRESS_I2C_ADDR ),
		.irq = MSM_GPIO_TO_INT(GPIO_7X27A_TS_IRQ),
		//.platform_data = touchscreen_power,
		.platform_data = &cypress_i2c_ttsp_platform_data,
	},
#endif
#ifdef CONFIG_TOUCHSCREEN_MELFAS_MIP
{				
	.type		  = "melf-touchscreen",
	/*.flags		= ,*/
	.addr		  = 0x20, 
	.irq		  = MSM_GPIO_TO_INT(GPIO_7X27A_TS_IRQ),		
},	
#endif
#ifdef CONFIG_TOUCHSCREEN_GOODIX_GT9XX
{				
	.type		  = "gdx-touchscreen",
	/*.flags		= ,*/
	.addr		  = 0x5d,//0x28, 
	.irq		  = MSM_GPIO_TO_INT(GPIO_7X27A_TS_IRQ), 
	.platform_data = &goodix_ts_data,
},
#endif
};


void __init msm7x27a_ts_init(void)
{

#ifdef CONFIG_TOUCHSCREEN_VIRTUAL_KEYS
	cap_ts_vkeys_init();
#endif

	i2c_register_board_info(MSM_GSBI1_QUP_I2C_BUS_ID,
			i2c_touch_devices,
			ARRAY_SIZE(i2c_touch_devices));

	return ;
}

