#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//-----------通用类型定义-------------------
#define  UINT8     unsigned char
#define  INT8      char
#define  UINT16    unsigned short int
#define  INT16     short int
#define  UINT32    unsigned int
#define  INT32     int
#define  UINT64    unsigned long long int
#define  INT64     long long int
//---------------------------------------------------

#define UA_DEVICE_INFO 1
#define UA_BROWSER_INFO 2
#define UA_OS_INFO 3
#define DEVICE_MOBILE 1
#define DEVICE_TELEVISION 2
#define DEVICE_TABLET 3
#define DEVICE_EAEADER 4
#define DEVICE_DESKTOP 5
#define DEVICE_GAMEING 6
#define DEVICE_MEDIA 7
#define UA_OS_CFG "ua-os.cfg"				// 之前是UI_OS_CFG，我改成了UA_OS_CFG,下同
#define UA_DEVICE_CFG "ua-device.cfg"
#define UA_BROWSER_CFG "ua-browser.cfg"

// 我将变量顺序调整了一下，结构体大小从40B减小到32B
typedef struct _os_device_browser
{
	UINT8 type;							// UI_DEVICE_INFO, UI_BROWSER_INFO, UI_OS_INFO
	UINT8 priority;                  	// 优先级
	UINT8 content_len;               	// 长度都小于256
	UINT8 os_brand_browser_len;			// 长度都小于256
	INT8 *content;		             	// 待比较的内容
	UINT8 *os_brand_browser;         	// 保存的是真正的浏览器，设备，操作系统字符串
	struct _os_brand_browser * next;
}OS_DEVICE_BROWSER;

OS_DEVICE_BROWSER  *init_os_device_browser()
{
	FILE *fp = NULL;
	
	char line[256] = {0};
	char buf[256] = {0};
	if(access(UA_OS_CFG, R_OK) == 0) {
		if((fp = fopen(UA_OS_CFG, "r")) == NULL) {
			fprintf(stderr, "Couldn't open the file %s\n", UA_OS_CFG);
			return NULL;
		}
		char key[256];
		char value[256];

		while(fgets(line, 256, fp) != NULL) {
			// 初始化一个链表节点
			OS_DEVICE_BROWSER *list = NULL;
			list = (OS_DEVICE_BROWSER *)malloc(sizeof(OS_DEVICE_BROWSER));

			list->type = UA_OS_INFO;

			sscanf(line, "%[^,]%*[,]%[^,]%d", key, value, &(list->priority));	// 读取出格式化数据

			list->content_len = strlen(key);
			list->os_brand_browser_len = strlen(value);
			list->content = (UINT8 *)malloc((list->content_len) + 1);
			list->os_brand_browser = (UINT8 *)malloc((list->os_brand_browser_len) + 1);
			printf("%s,%s,%d\n", key, value, list->priority);
		}
		if(fclose(fp) != 0) {
			fprintf(stderr, "Couldn't close the file %s\n", UA_OS_CFG);
		}
	}
	if(access(UA_DEVICE_CFG, R_OK) == 0) {
		if((fp = fopen(UA_DEVICE_CFG, "r")) == NULL) {
			fprintf(stderr, "Couldn't open the file %s\n", UA_DEVICE_CFG);
			return NULL;
		}
	}
	if(access(UA_BROWSER_CFG, R_OK) == 0) {
		if((fp = fopen(UA_BROWSER_CFG, "r")) == NULL) {
			fprintf(stderr, "Couldn't open the file %s\n", UA_BROWSER_CFG);
			return NULL;
		}
	}

	
	return list;
}

int main()
{
	OS_DEVICE_BROWSER *l = NULL;
	l = init_os_device_browser();

	return 0;
}
