
#include <hardware/lights.h>
#include <hardware/hardware.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

#define LOG_TAG "lights"
#include <utils/Log.h>

#define LIGHTS_DEBUG 0 

const char * const LCD_BACKLIGHT = "/sys/class/backlight/backlight-0/brightness";
const char * const LCD_MAX_BACKLIGHT = "/sys/class/backlight/backlight-0/max_brightness";
const char * const LCD_CONTROLLER = "/sys/class/graphics/fb0/blank";
const char * const KEYBOARD_BACKLIGHT = "/sys/class/backlight/backlight-2/brightness";
const char * const BUTTON_BACKLIGHT = "";
const char * const BATTERY_BACKLIGHT = "";
const char * const NOTIFICATIONS_BACKLIGHT = "";
const char * const ATTENTION_BACKLIGHT = "";

static int lights_device_open(const struct hw_module_t *module,
				const char *name, struct hw_device_t **device);

static struct hw_module_methods_t lights_module_methods = {
	.open      = lights_device_open
};

struct light_module_t
{
    struct hw_module_t common;
};

const struct light_module_t HAL_MODULE_INFO_SYM = {
	common: {
		tag : HARDWARE_MODULE_TAG,
		version_major : 1,
		version_minor : 0,
		id : LIGHTS_HARDWARE_MODULE_ID,
		name : "lights module",
		author : "",
		methods : &lights_module_methods,
	}
};

struct my_light_device_t
{
    struct light_device_t dev;
    int max_brightness;
};

static int readIntFromFile( const char* file_name, int* pResult )
{
    int fd = -1;
    fd = open( LCD_MAX_BACKLIGHT, O_RDONLY );
    if( fd >= 0 ){
        char buf[20];
        int rlt = read( fd, buf, sizeof(buf) );
        if( rlt > 0 ){
            buf[rlt] = '\0';
            *pResult = atoi(buf);
            return 0;
        }
    }
    return -1;
}

int lights_set_lcd(struct light_device_t* dev, struct light_state_t const* state)
{
	LOGD("lights_set_lcd: device name: %s\n", LIGHT_ID_BACKLIGHT);
	struct light_state_t const *light_state = (struct light_state_t const*) state;
	unsigned char brightness = 0;
	int fd = -1;
	int fd1 = -1;
    struct my_light_device_t* mydev = (struct my_light_device_t*)dev;
	
	if (light_state) {
		unsigned int color = light_state->color;
        brightness = (color & 0xff) * mydev->max_brightness / 0xff;
	} else {
	    LOGE("lights_set_brightness failed: light state is null\n");
	    return -EINVAL;
	}

	fd = open(LCD_BACKLIGHT, O_RDWR);
	if (fd >= 0) {
		char buffer[20];
		int bytes;
		unsigned char old_brightness;
		int get_brightness = 0;

		memset(buffer, 0, 20);
		int rlt = read(fd, buffer, 20);
		if( rlt > 0) {
			get_brightness = 1;
			old_brightness = atoi(buffer);
		}
		bytes = sprintf(buffer, "%d", brightness);
		rlt = write(fd, buffer, bytes);
		if( rlt < 0 ){
			LOGE("lights_set_brightness, fail to set brightness to:%s, errno:%d\n", buffer, errno); 
			close(fd);
			return -1;
		}
		close(fd);


/* meanwhile open keyboard lights*/
	fd = open(KEYBOARD_BACKLIGHT, O_RDWR);
	if (fd >= 0) {
		char buffer[20];
		int bytes;

		memset(buffer, 0, 20);
		int rlt = read(fd, buffer, 20);
		bytes = sprintf(buffer, "%d", brightness);
		rlt = write(fd, buffer, bytes);
		if( rlt < 0 ){
			LOGE("lights_set keyboard brightness, fail to set brightness to:%s, errno:%d\n", buffer, errno); 
			close(fd);
			return -1;
		}
		close(fd);
	} else {
		static int already_warned = 0;
		if (already_warned < 5) {
			LOGE("lights_set keyboard brightness failed to open %s, errno:%d\n", KEYBOARD_BACKLIGHT, errno);
			already_warned++;
		}
		return -1;
	}


/*		if(get_brightness) {
			fd1 = open(LCD_CONTROLLER, O_RDWR);
			if( fd1 >=0) {
				if(old_brightness == 0 && brightness != 0) {
					write(fd1, "0", 1);
				} else if(old_brightness !=0 && brightness == 0) {
					write(fd1, "1", 1);
				}
				close(fd1);
			} else {
				LOGE("lights_set_brightness, fail to open %s, errno:%d\n", LCD_CONTROLLER, errno);
			}
		}
*/
	} else {
		static int already_warned = 0;
		if (already_warned < 5) {
			LOGE("lights_set_brightness failed to open %s, errno:%d\n", LCD_BACKLIGHT, errno);
			already_warned++;
		}
		return -1;
	}
	    
	return 0;
}

int lights_set_keyboard(struct light_device_t* dev, struct light_state_t const* state)
{
#if 0
	LOGD("lights_set_lcd: device name: %s\n", LIGHT_ID_KEYBOARD);
	struct light_state_t const *light_state = (struct light_state_t const*) state;
	unsigned char brightness = 0;
	int fd = -1;
	int fd1 = -1;
    struct my_light_device_t* mydev = (struct my_light_device_t*)dev;
	
	if (light_state) {
		unsigned int color = light_state->color;
        brightness = (color & 0xff) / 0xff;
	} else {
	    LOGE("lights_set keyboard brightness failed: light state is null\n");
	    return -EINVAL;
	}

	fd = open(KEYBOARD_BACKLIGHT, O_RDWR);
	if (fd >= 0) {
		char buffer[20];
		int bytes;

		memset(buffer, 0, 20);
		int rlt = read(fd, buffer, 20);
		bytes = sprintf(buffer, "%d", brightness);
		rlt = write(fd, buffer, bytes);
		if( rlt < 0 ){
			LOGE("lights_set keyboard brightness, fail to set brightness to:%s, errno:%d\n", buffer, errno); 
			close(fd);
			return -1;
		}
		close(fd);
	} else {
		static int already_warned = 0;
		if (already_warned < 5) {
			LOGE("lights_set keyboard brightness failed to open %s, errno:%d\n", LCD_BACKLIGHT, errno);
			already_warned++;
		}
		return -1;
	}
#endif	    
	return 0;

}

int lights_set_button(struct light_device_t* dev, struct light_state_t const* state)
{
	LOGD("lights_set_button: device name: %s\n", LIGHT_ID_BUTTONS);
	return 0;
}

int lights_set_battery(struct light_device_t* dev, struct light_state_t const* state)
{
	return 0;
}

int lights_set_notifications(struct light_device_t* dev, struct light_state_t const* state)
{
	return 0;
}

int lights_set_attention(struct light_device_t* dev, struct light_state_t const* state)
{
	return 0;
}

static int lights_device_close(struct hw_device_t *dev)
{
	struct my_light_device_t *light_dev = (struct my_light_device_t *)dev;

	if (light_dev)
		free(light_dev);
	
	return 0;
}

static int lights_device_open(const struct hw_module_t *module,
				const char *name, struct hw_device_t **device)
{
    int	status = -EINVAL;
    int ret;
#ifdef LIGHTS_DEBUG
	LOGD("lights_device_open: device name: %s\n", name);
#endif

    struct light_device_t *dev = NULL;
    struct my_light_device_t *mydev = NULL;
    
    mydev = (struct my_light_device_t *)calloc(sizeof(struct my_light_device_t), 1);
    dev = &(mydev->dev);
    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t *)(module);
    dev->common.close = lights_device_close;

    mydev->max_brightness = 0xff;//default max brightness is 255
        
    *device = &dev->common;
    
    if (!strcmp(name, LIGHT_ID_BACKLIGHT)) {
    	dev->set_light = lights_set_lcd;
        readIntFromFile( LCD_MAX_BACKLIGHT, &mydev->max_brightness);
    	ret = access(LCD_BACKLIGHT, F_OK);
    	if (ret != 0)
            return status;
    } else if (!strcmp(name, LIGHT_ID_KEYBOARD)) {
    	dev->set_light = lights_set_keyboard;
    	ret = access(KEYBOARD_BACKLIGHT, F_OK);
    	if (ret != 0)
            return status;
    } else if (!strcmp(name, LIGHT_ID_BUTTONS)) {
    	dev->set_light = lights_set_button;
    	ret = access(BUTTON_BACKLIGHT, F_OK);
    	if (ret != 0)
            return status;
    } else if (!strcmp(name, LIGHT_ID_BATTERY)) {
    	dev->set_light = lights_set_battery;
    	ret = access(BATTERY_BACKLIGHT, F_OK);
    	if (ret != 0)
            return status;
    } else if (!strcmp(name, LIGHT_ID_NOTIFICATIONS)) {
    	dev->set_light = lights_set_notifications;
    	ret = access(NOTIFICATIONS_BACKLIGHT, F_OK);
    	if (ret != 0)
            return status;
    } else if (!strcmp(name, LIGHT_ID_ATTENTION)) {
    	dev->set_light = lights_set_attention;
    	ret = access(ATTENTION_BACKLIGHT, F_OK);
    	if (ret != 0)
            return status;
    } else
        return status;
	
	return 0;
}
