#
# copy sd8787_uapsta.bin to /etc/firmware/mrvl
#

include $(CLEAR_VARS)

PRODUCT_COPY_FILES += vendor/marvell/generic/sd8787/FwImage/sd8787_uapsta.bin:system/etc/firmware/mrvl/sd8787_uapsta.bin

