The helper_uart is a UART download helper for boosting firmware UART
download speed.

Without such a helper, the normal firmware download baudrate is fixed
to a value by bootrom code, which is usually 38400bps or
115200bps. When using the helper, the bootrom first download the
helper itself in the fixed baudrate, say 115200bps. Then the helper
runs and changes the UART baudrate to a higher speed, say 3Mbps. And
finally the firmware is downloaded at this high speed to save the
total download time.

The helper is very small, less than 2KB. So it can be downloaded very
fast even in 38400bps baudrate. The firmware is usually large, bigger
than 100KB. Using higher baudrate for downloading firmware to save
firmware download time can easily compensate the time for downloading
the small helper itself. We can easily decrease the total download
time from 10 seconds to 4 seconds.

uBB R0..uBB U0, Robin R0..Robin W1 :
mfguart\
       |-mfgdl.exe
       |-helper_uart_2000000.code
       |-helper_uart_3000000.code
       |-helper_uart_4000000.code
       |-helper_uart_460800.code
       |-helper_uart_921600.code
Eg: mfgdl.exe com1 115200 helper_uart_3000000.code

uBB W0, Robin A0:
mfguart\
       |-fw_loader.exe
       |-helper_uart_2000000.bin
       |-helper_uart_3000000.bin
       |-helper_uart_4000000.bin
       |-helper_uart_460800.bin
       |-helper_uart_921600.bin
Eg: fw_loader.exe com1 115200 0 helper_uart_3000000.bin


How to download the FW image:

uBB/Robin FW image has different load regions. Therefore, Xmodem protocol
which was used previously to download both the helper and the 
FW image, cannot be used. We use a different protocol to download the FW image.
Please refer to UART_HELPER.txt for more details on the protocol. It is located at 
Perforce\MRVL\main\utils\apps\uart_fw_dl\docs.

For uBB (R0 upto U0) and Robin (R0 upto W1), we use the mfgdl executable to download the helper first.
Once helper starts running on the chip, we use the aforementioned
protocol to download the FW image using the fw_loader exectuable.

For uBB (W0 onwards) and Robin (A0 onwards) and, we can use the new helper protocol to download both
the helper and the FW image.

Once the FW is downloaded on the chip, it automatically stats running
from its entry point. 

There are different mfgdl and fw_loader executables available depending
on the Platform on which it is to be run.

For both uBB and Robin, the user can enable/disable Flow Control via command line.
By default, it is desirable to enable flow control. However, on uBB and Robin Boot ROM code,
flow control is not enabled. This means that while downloading the helper, Flow Control
should be disabled. Once the helper is downloaded, Flow Control can be set to 1, to download
the FW.

For eg: 
<Format: fw_loader_win.exe ComPort BaudRate FlowControl FileName>
Download Helper for uBB(W0 onwards)/Robin (A0 onwards):
   fw_loader_win.exe com1 115200 0 helper_uart_3000000.bin 
Download FW for uBB/Robin:   
   fw_loader_win.exe com1 115200 1 firmware.image

Windows:
Download Helper:
   uBB (R0 upto U0) and Robin (R0 upto W1):
     mfgdl_win.exe com1 115200 helper_uart_3000000.code
   uBB (W0 onwards) and Robin (A0 onwards):
     fw_loader_win.exe com1 115200 0 helper_uart_3000000.bin
Download FW image at specified BaudRate:     
     fw_loader_win.exe com1 3000000 1 firmware.image

Linux:
Download Helper:
    uBB (R0 upto U0) and Robin (R0 upto W1):
     ./mfgdl_linux /dev/ttyUSB# 115200 helper_uart_3000000.code
    uBB (W0 onwards) and Robin (A0 onwards):
     ./fw_loader_linux /dev/ttyUSB# 115200 0 helper_uart_3000000.bin
Download FW image at specified BaudRate     
     ./fw_loader_linux /dev/ttyUSB# 3000000 1 firmware.image

Saar:
Download Helper:
    uBB (R0 upto U0) and Robin (R0 upto W1):
     ./mfgdl_linux_arm /dev/ttyUSB# 115200 helper_uart_3000000.code
    uBB (W0 onwards) and Robin (A0 onwards):
     ./fw_loader_arm /dev/ttyUSB# 115200 0 helper_uart_3000000.bin
Download FW image at specified BaudRate     
     ./fw_loader_arm /dev/ttyUSB# 3000000 1 firmware.image
 
