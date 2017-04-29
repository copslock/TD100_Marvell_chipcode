#ifndef __ASM_ARCH_CAMERA_H__
#define __ASM_ARCH_CAMERA_H__

#define SENSOR_LOW  0
#define SENSOR_HIGH 1

#define MAX_CAM_CLK 3
struct mv_cam_pdata {
	struct clk	*clk[MAX_CAM_CLK];
	char		*name;
	int		clk_enabled;
	int		dphy[3];
	int		bus_type;
	int		dma_burst;
	int		qos_req_min;
	int		mclk_min;
	int		mclk_src;
	void		(*controller_power)(int on);
	int		(*clk_init)(struct device *dev, int init);
	void		(*set_clock)(struct device *dev, int on);
	int		(*get_mclk_src)(int src);
};
struct clk;
struct sensor_platform_data {
	int id;
	int (*power_on)(int,int);
	int (*platform_set)(int, struct clk*);
};

#endif

