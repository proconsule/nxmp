#include <cstdio>
#include "usbfs.h"

static UEvent *g_statusChangeEvent, g_exitEvent;
static UsbHsFsDevice *g_usbDevices;
static u32 g_usbDeviceCount;


int usbThread(void *arg) {
	USBMounter * mymounter = (USBMounter *)arg;
	printf("USB Thread Started\n");
    (void) arg;
    Result rc = 0;
    int idx = 0;
	u32 listed_device_count = 0;

    Waiter status_change_event_waiter = waiterForUEvent(g_statusChangeEvent);
    Waiter exit_event_waiter = waiterForUEvent(&g_exitEvent);

	printf("Started USB Wait Loop\n");
    while (true) {
        rc = waitMulti(&idx, -1, status_change_event_waiter,exit_event_waiter);
		printf("USB Mass Storage status change event triggered!\nMounted USB Mass Storage device count: %u.\n\n", g_usbDeviceCount);
		if (R_FAILED(rc)) continue;
        if (g_usbDevices) {
            free(g_usbDevices);
            g_usbDevices = nullptr;
        }
		
		g_usbDeviceCount = usbHsFsGetMountedDeviceCount();
		g_usbDevices = (UsbHsFsDevice *) calloc(g_usbDeviceCount, sizeof(UsbHsFsDevice));
		mymounter->mounted_devs.clear();
		
		if (idx == 1)
        {
            printf("Exit event triggered!\n");
            break;
        }

		if (!g_usbDevices)
        {
            printf("Failed to allocate memory for mounted USB Mass Storage devices buffer!\n\n");
            continue;
        }

		if (!(listed_device_count = usbHsFsListMountedDevices(g_usbDevices, g_usbDeviceCount)))
        {
            printf("Failed to list mounted USB Mass Storage devices!\n\n");
            continue;
        }
		
		for(u32 i = 0; i < listed_device_count; i++)
        {
            UsbHsFsDevice *device = &(g_usbDevices[i]);
            usb_devices tmpdev;
            printf("Device #%u:\n" \
                        "\t- USB interface ID: %d.\n" \
                        "\t- Logical Unit Number: %u.\n" \
                        "\t- Filesystem index: %u.\n" \
                        "\t- Write protected: %s.\n" \
                        "\t- Vendor ID: 0x%04X.\n" \
                        "\t- Product ID: 0x%04X.\n" \
                        "\t- Manufacturer: \"%s\".\n" \
                        "\t- Product Name: \"%s\".\n" \
                        "\t- Serial Number: \"%s\".\n" \
                        "\t- Logical Unit Capacity: 0x%lX bytes.\n" \
                        "\t- Mount name: \"%s\".\n" \
                        "\t- Filesystem type: %s.\n" \
                        "\t- Mount flags: 0x%08X.\n" \
                        "\t- Filesystem tests:\n", \
                        i + 1, \
                        device->usb_if_id, \
                        device->lun, \
                        device->fs_idx, \
                        device->write_protect ? "yes" : "no", \
                        device->vid, \
                        device->pid, \
                        device->manufacturer, \
                        device->product_name, \
                        device->serial_number, \
                        device->capacity, \
                        device->name, \
                        LIBUSBHSFS_FS_TYPE_STR(device->fs_type), \
                        device->flags);
            
            tmpdev.mount_point = device->name;
			tmpdev.name = device->product_name;
			mymounter->mounted_devs.push_back(tmpdev);
            
        }
	
    }
	
	for(u32 i = 0; i < listed_device_count; i++)
        {
            UsbHsFsDevice *device = &(g_usbDevices[i]);
            usbHsFsUnmountDevice(device, false);
        }
        
	printf("%u device(s) safely unmounted. You may now disconnect them from the console.\n\n", listed_device_count);

    printf("usbThread: end\n");
	return 0;
}

USBMounter::~USBMounter(){
	ueventSignal(&g_exitEvent);
	thrd_join(g_thread, NULL);
	usbHsFsExit();
}

USBMounter::USBMounter(){
	Result rc;
    

    printf("usbInit\n");
	rc = usbHsFsInitialize(0);
	if (R_FAILED(rc))
    {
        printf("usbHsFsInitialize() failed! (0x%08X).\n", rc);
		
    }
	printf("usbHsFsInitialize: %u\n", rc);
	g_statusChangeEvent = usbHsFsGetStatusChangeUserEvent();
	usbHsFsSetFileSystemMountFlags(UsbHsFsMountFlags_ShowHiddenFiles | UsbHsFsMountFlags_ReadOnly);
    ueventCreate(&g_exitEvent, true);
	thrd_create(&g_thread, usbThread, (void *)this);
	
}

void usbInit() {

}

