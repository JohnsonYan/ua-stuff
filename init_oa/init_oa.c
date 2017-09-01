#include <stdio.h>
#include <string.h>
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
#define DEVICE_EREADER 4
#define DEVICE_DESKTOP 5
#define DEVICE_GAMING 6
#define DEVICE_MEDIA 7

#define UA_OS_CFG "ua-os.cfg"
#define UA_DEVICE_CFG "ua-device.cfg"
#define UA_BROWSER_CFG "ua-browser.cfg"

// 我将变量顺序调整了一下，结构体大小从40B减小到32B
typedef struct _os_device_browser
{
	UINT8 type;							// UI_DEVICE_INFO, UI_BROWSER_INFO, UI_OS_INFO
	UINT8 priority;                  	// 优先级
	UINT8 content_len;               	// 长度都小于256
	UINT8 os_device_browser_len;			// 长度都小于256
	INT8 *content;		             	// 待比较的内容
	UINT8 *os_device_browser;         	// 保存的是真正的浏览器，设备，操作系统字符串
	struct _os_device_browser *next;
}OS_DEVICE_BROWSER;


OS_DEVICE_BROWSER  *init_os_device_browser()
{
	FILE *fp = NULL;
	OS_DEVICE_BROWSER *list, *tmpt, *head;
	char line[256];
	int count = 0;	// 统计链表长度

	// READ OS INFO
	if(access(UA_OS_CFG, R_OK) == 0) {
		if((fp = fopen(UA_OS_CFG, "r")) == NULL) {
			fprintf(stderr, "Couldn't open the file %s\n", UA_OS_CFG);
			return NULL;
		}
		char key[256];
		char value[256];


		while(fgets(line, 256, fp) != NULL) {
			// 初始化一个链表节点,指向头结点head
			if(++count == 1) {
				list = (OS_DEVICE_BROWSER *)malloc(sizeof(OS_DEVICE_BROWSER));
				if(list == NULL) {
					fprintf(stderr, "Memory allocation failed.");
					return NULL;
				}
				head = list;
			}
			else {
				tmpt = (OS_DEVICE_BROWSER *)malloc(sizeof(OS_DEVICE_BROWSER));
				if(tmpt == NULL) {
					fprintf(stderr, "Memory allocation failed.");
					return NULL;
				}
				list->next = tmpt;
				tmpt->next = NULL;
				list = tmpt;
			}

			sscanf(line, "%[^,]%*[,]%[^,]%*[,]%d", key, value, &list->priority);	// 读取出格式化数据

			list->type = UA_OS_INFO;
			list->content_len = strlen(key);
			list->os_device_browser_len = strlen(value);
			list->content = (UINT8 *)malloc((list->content_len) + 1);
			if(list->content == NULL) {
				fprintf(stderr, "Memory allocation failed.");
				return NULL;
			}
			list->os_device_browser = (UINT8 *)malloc((list->os_device_browser_len) + 1);
			if(list->os_device_browser == NULL) {
				fprintf(stderr, "Memory allocation failed.");
				return NULL;
			}
			strncpy(list->content, key, list->content_len);
			strncpy(list->os_device_browser, value, list->os_device_browser_len);
		}

		if(fclose(fp) != 0) {
			fprintf(stderr, "Couldn't close the file %s\n", UA_OS_CFG);
		}
	}

	// READ BROWSER INFO
	if(access(UA_BROWSER_CFG, R_OK) == 0) {
		if((fp = fopen(UA_BROWSER_CFG, "r")) == NULL) {
			fprintf(stderr, "Couldn't open the file %s\n", UA_BROWSER_CFG);
			return NULL;
		}

		char key[256];
		char value[256];

		while(fgets(line, 256, fp) != NULL) {
			// 初始化一个链表节点,指向头结点head
			if(++count == 1) {
				list = (OS_DEVICE_BROWSER *)malloc(sizeof(OS_DEVICE_BROWSER));
				if(list == NULL) {
					fprintf(stderr, "Memory allocation failed.");
					return NULL;
				}
				head = list;
			}
			else {
				tmpt = (OS_DEVICE_BROWSER *)malloc(sizeof(OS_DEVICE_BROWSER));
				if(tmpt == NULL) {
					fprintf(stderr, "Memory allocation failed.");
					return NULL;
				}
				list->next = tmpt;
				tmpt->next = NULL;
				list = tmpt;
			}

			sscanf(line, "%[^,]%*[,]%[^,]%*[,]%d", key, value, &list->priority);	// 读取出格式化数据

			list->type = UA_BROWSER_INFO;
			list->content_len = strlen(key);
			list->os_device_browser_len = strlen(value);
			list->content = (UINT8 *)malloc((list->content_len) + 1);
			if(list->content == NULL) {
				fprintf(stderr, "Memory allocation failed.");
				return NULL;
			}
			list->os_device_browser = (UINT8 *)malloc((list->os_device_browser_len) + 1);
			if(list->os_device_browser == NULL) {
				fprintf(stderr, "Memory allocation failed.");
				return NULL;
			}
			strncpy(list->content, key, list->content_len);
			strncpy(list->os_device_browser, value, list->os_device_browser_len);
		}

		if(fclose(fp) != 0) {
			fprintf(stderr, "Couldn't close the file %s\n", UA_OS_CFG);
		}
	}

	// READ DEVICE INFO
	if(access(UA_DEVICE_CFG, R_OK) == 0) {
		if((fp = fopen(UA_DEVICE_CFG, "r")) == NULL) {
			fprintf(stderr, "Couldn't open the file %s\n", UA_DEVICE_CFG);
			return NULL;
		}
		char key[256];
		char value[256];
		char value1[64], value2[64], value3[64];	// 品牌 型号 终端类型
		const char *terminal_types[] = {
			"mobile", "television", "tablet", "ereader",
			"desktop", "gaming", "media"
		};

		while(fgets(line, 256, fp) != NULL) {
			// 初始化一个链表节点,指向头结点head
			if(++count == 1) {
				list = (OS_DEVICE_BROWSER *)malloc(sizeof(OS_DEVICE_BROWSER));
				if(list == NULL) {
					fprintf(stderr, "Memory allocation failed.");
					return NULL;
				}
				head = list;
			}
			else {
				tmpt = (OS_DEVICE_BROWSER *)malloc(sizeof(OS_DEVICE_BROWSER));
				if(tmpt == NULL) {
					fprintf(stderr, "Memory allocation failed.");
					return NULL;
				}
				list->next = tmpt;
				tmpt->next = NULL;
				list = tmpt;
			}

			sscanf(line, "%[^,]%*[,]%[^,]%*[,]%[^,]%*[,]%[^,]%*[,]", key, value1, value2, value3);	// 读取出格式化数据, key,value1, value2, value3

			// 对value3,即terminal字段进行替换
			int i;
			for(i = 0; i < 7; i++) {
				if(strncmp(value3, terminal_types[i], strlen(terminal_types[i])) == 0) {
					snprintf(value, 256, "%s,%s,%d", value1, value2, i+1);
				}
			}

			list->type = UA_DEVICE_INFO;
			list->os_device_browser_len = strlen(value);
			list->content_len = strlen(key);
			list->content = (UINT8 *)malloc((list->content_len) + 1);
			if(list->content == NULL) {
				fprintf(stderr, "Memory allocation failed.");
				return NULL;
			}
			list->os_device_browser = (UINT8 *)malloc(list->os_device_browser_len + 1);
			if(list->os_device_browser == NULL) {
				fprintf(stderr, "Memory allocation failed.");
				return NULL;
			}
			strncpy(list->content, key, list->content_len);
			strncpy(list->os_device_browser, value, list->os_device_browser_len);
		}

		if(fclose(fp) != 0) {
			fprintf(stderr, "Couldn't close the file %s\n", UA_OS_CFG);
		}
	}

	return head;
}


int  free_os_device_browser(OS_DEVICE_BROWSER *head)
{
	OS_DEVICE_BROWSER *list, *deleted;
	list = head;
	while(list != NULL) {
		free(list->content);
		free(list->os_device_browser);
		deleted = list;
		list = list->next;
		free(deleted);
	}

	return 0;
}

int main()
{
	OS_DEVICE_BROWSER *list, *head;

	head = init_os_device_browser();

	for(list = head; list != NULL; list = list->next) {
		// type:content_len:os_device_browser_len---content,os_device_browser,priority
		printf("%d:%d:%d---%s,%s,%d\n", list->type, list->content_len, list->os_device_browser_len, list->content, list->os_device_browser, list->priority);
	}

	free_os_device_browser(head);

	return 0;
}
