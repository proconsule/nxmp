#include <cstdio>
#include "usbfs.h"


#include "platforms.h"

#ifdef NXMP_USBSUPPORT

static UEvent *g_statusChangeEvent, g_exitEvent;
static UsbHsFsDevice *g_usbDevices;
static u32 g_usbDeviceCount;

static thrd_t g_thread = {0};


int usbThread(void *arg) {
	sleep(1);
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
                        "\t- Mount flags: 0x%08X.\n", \
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
			tmpdev.fstype = LIBUSBHSFS_FS_TYPE_STR(device->fs_type);
			tmpdev.capacity = device->capacity;
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

USBMounter::USBMounter(Playlist *_playlist){
	Result rc;
    
	playlist = _playlist;
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

void USBMounter::DirList(const std::string &path,bool showHidden,const std::vector<std::string> &extensions) {

		currentpath = path;
		currentlist.clear();
		struct dirent *ent;
		DIR *dir;

		if (!path.empty()) {
			if ((dir = opendir(path.c_str())) != nullptr) {
				while ((ent = readdir(dir)) != nullptr) {
					if ((path == "/" || strlen(ent->d_name) == 1) && ent->d_name[0] == '.') {
						continue;
					}
					if ((path == "/" || strlen(ent->d_name) == 2) && ent->d_name[0] == '.' && ent->d_name[1] == '.') {
						continue;
					}
					if (!showHidden && ent->d_name[0] == '.') {
						if (strlen(ent->d_name) != 2 && ent->d_name[1] != '.') {
							continue;
						}
					}

					FS::FileEntry file;
					file.name = ent->d_name;
					file.path = FS::removeLastSlash(path) + "/" + file.name;
					file.checked = playlist->isPresent(file.name,file.path);
					
					struct stat st{};
					if (stat(file.path.c_str(), &st) == 0) {
						file.size = (size_t) st.st_size;
						file.type = S_ISDIR(st.st_mode) ? FS::FileEntryType::Directory : FS::FileEntryType::File;
					}
					if(file.type == FS::FileEntryType::File){
						bool isMediafile = false;
						for (auto &ext : extensions) {
							if (Utility::endsWith(file.name, ext, false)) {
								isMediafile = true;
							}
						}
						if(isMediafile){
							currentlist.push_back(file);
						}
					}else if(file.type == FS::FileEntryType::Directory){
						currentlist.push_back(file);
					}

				}
			
				closedir(dir);
				std::sort(currentlist.begin(), currentlist.end(), FS::Sort);
			}
		}

	}
	
std::string USBMounter::getCurrentPath(){
	return currentpath;
}

std::string USBMounter::getBasePath(){
	return basepath;
}

void USBMounter::setBasePath(std::string _basepath){
	basepath = _basepath;
}

std::vector<FS::FileEntry> USBMounter::getCurrList(){
	return currentlist;
}

void USBMounter::clearChecked(){
	for(int i=0;i<currentlist.size();i++){
		currentlist[i].checked = false;
	}
}

bool *USBMounter::checked(int pos){
	return &currentlist[pos].checked;
}

void USBMounter::backPath(){
		currentpath = currentpath.substr(0, currentpath.find_last_of("\\/"));
		if(currentpath == "")currentpath = "/";

}

bool USBMounter::haveIteminPlaylist(){
	for(int i=0;i<playlist->getPlaylist().size();i++){
		if(Utility::startWith(playlist->getPlaylist()[i].fulluri,"ums",false)){
			return true;
		}
	}
	return false;
}

#endif