#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <wincon.h>
#include <conio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <ctype.h>
#include <time.h>

#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED

#define SCR_ROW 25             //屏幕行数 
#define SCR_COL 80             //屏幕列数

/**
 *客房分类信息表
 */
typedef struct type_node {
    char type_id;              /**< 客房类别 ‘D’  双人间，’S’  单间，T 三个间，F 四人间 */
    int people_max;            /**< 最多入住人数*/
    int room_number;           /**< 客房套数*/
    int room_no;               /**< 客房未住套数*/ 
    struct item_node *inext;   /**< 指向客房基本信息支链的指针*/
    struct type_node *next;    /**< 指向下一结点的指针*/
} TYPE_NODE;

/**
 *客房基本信息表
 */
typedef struct item_node {
    char number[4];               /**< 客房编号*/
    char phone[5];                /**< 电话号码*/
    char type_id;                 /**< 客房类别*/
    float area;                   /**< 面积*/
    float sale;                   /**< 每月租金*/
    char balcony;                 /**< 是否有阳台 1 表示有阳台，0 表示没有阳台 */
    char human;                   /**< 是否有客人 ‘y’ 已有客人入住 ‘n’ 未住 */
    struct client_node *snext;   /**< 指向客人租房信息支链的指针*/
    struct item_node *next;    /**< 指向下一结点的指针*/
} ITEM_NODE;

/**
 *客人租房信息表
 */
typedef struct client_node {
    char people_id[20];             /**< 客人身份证号码*/
    char people_name[20];           /**< 客人姓名*/
    char number[4];                 /**< 入住客房编号*/
    char time_in[18];               /**< 入住时间*/
    char time_out[18];              /**< 退房时间*/
    float month;                    /**< 入住月数*/
    float money;                    /**< 押金*/
    float should;                   /**< 应缴金额*/
    float reality;                  /**< 实缴金额*/
    struct client_node *next;  /**< 指向下一结点的指针*/
} CLIENT_NODE;

/**
 *按照客房类别统计客房入住率 
 */
typedef struct type_stat {
    char type_id;             /**< 分类名称*/
    int room_number;          /**< 客房总数*/
    int room_y;              /**< 以入住数*/
    int room_n;              /**< 未入住数*/
    struct type_stat *next;    /**< 指向下一结点的指针*/
} TYPE_STAT;

/**
 *按照月份统计各类客房营业额 
 */
typedef struct month_stat {
    int month;                  /**< 月份*/
    float s;                    /**< 单人间营业额*/
    float d;                    /**< 双人间营业额*/
    float t;                    /**< 三人间营业额*/
    struct month_stat *next;     /**< 指向下一结点的指针*/
} MONTH_STAT;

/**
 *按客房编号统计客房入住情况
 */
typedef struct number_stat {
	char number[4];                /**< 客房编号*/
    float sale;                    /**< 实缴金额*/
    int month;                     /**< 入住月数*/
    float ruzhu;                   /**< 入住率*/
    struct number_stat *next;    /**< 指向下一结点的指针*/
} NUMBER_STAT;

/**
 *按客人姓名统计客人入住月数情况 
 */
typedef struct client_stat {
	char people_id[20];             /**< 客人身份证号码*/
	char people_name[20];           /**< 客人姓名*/
	int month;                      /**< 入住月数*/
	float should;                   /**< 应缴金额*/
	float shiji;                    /**< 实缴金额*/
	struct client_stat *next;        /**< 指向下一结点的指针*/
} CLIENT_STAT; 

/**
 *屏幕窗口信息链结点结点结构
 */
typedef struct layer_node {
    char LayerNo;            /**< 弹出窗口层数*/
    SMALL_RECT rcArea;       /**< 弹出窗口区域坐标*/
    CHAR_INFO *pContent;     /**< 弹出窗口区域字符单元原信息存储缓冲区*/
    char *pScrAtt;           /**< 弹出窗口区域字符单元原属性值存储缓冲区*/
    struct layer_node *next; /**< 指向下一结点的指针*/
} LAYER_NODE;

/**
 *标签束结构
 */
typedef struct labe1_bundle {
    char **ppLabel;        /**< 标签字符串数组首地址*/
    COORD *pLoc;           /**< 标签定位数组首地址*/
    int num;               /**< 标签个数*/
} LABEL_BUNDLE;

/**
 *热区结构
 */
typedef struct hot_area {
    SMALL_RECT *pArea;     /**< 热区定位数组首地址*/
    char *pSort;           /**< 热区类别(按键、文本框、选项框)数组首地址*/
    char *pTag;            /**< 热区序号数组首地址*/
    int num;               /**< 热区个数*/
} HOT_AREA;

LAYER_NODE *gp_top_layer = NULL;                        /*弹出窗口信息链链头*/
TYPE_NODE *gp_head = NULL;                              /*主链头指针*/

char *gp_sys_name = "房屋出租信息管理系统";             /*系统名称*/
char *gp_type_info_filename = "type.dat";               /*客房分类信息数据文件*/
char *gp_item_info_filename = "item.dat";               /*客房基本信息数据文件*/
char *gp_client_info_filename = "client.dat";           /*客房出租信息数据文件*/


char *ga_main_menu[] = {"文件(F)",                      /*系统主菜单名*/
    	                "数据维护(M)",
                        "数据查询(Q)",
                        "数据统计(S)",
                        "帮助(H)"
                       };

char *ga_sub_menu[] = {"[A] 数据保存",                  /*系统子菜单名*/
                       "[B] 数据备份",
                       "[C] 数据恢复",
                       "[D] 退出   Alt+X",
                       "[E] 客房分类信息",
                       "[F] 客房基本信息",
                       "[G] 客房出租信息",
                       "[H] 客房分类信息",
                       "[I] 客房基本信息",
                       "[J] 客房出租信息",
                       "[K] 按类别统计",
                       "[L] 营业额统计",
                       "[M] 按客人统计",
                       "[N] 帮助主题",
                       "",
                       "[O] 关于..."
                      };

int ga_sub_menu_count[] = {4, 3, 3, 3, 3};  /*各主菜单项下子菜单的个数*/
int gi_sel_menu = 1;                        /*被选中的主菜单项号,初始为1*/
int gi_sel_sub_menu = 0;                    /*被选中的子菜单项号,初始为0,表示未选中*/

CHAR_INFO *gp_buff_menubar_info = NULL;     /*存放菜单条屏幕区字符信息的缓冲区*/
CHAR_INFO *gp_buff_stateBar_info = NULL;    /*存放状态条屏幕区字符信息的缓冲区*/

char *gp_scr_att = NULL;          /*存放屏幕上字符单元属性值的缓冲区*/
char *gp_type_code = NULL;        /*存放客房分类信息代码表的数据缓冲区*/
char *gp_item_code = NULL;        /*存放客房基本信息代码表的数据缓冲区*/
char *gp_client_code = NULL;      /*存放客人入住信息代码表的数据缓冲区*/
char gc_sys_state = '\0';         /*用来保存系统状态的字符*/

unsigned long gul_type_code_len = 0;         /*客房分类信息代码表长度*/
unsigned long gul_item_code_len = 0;         /*客房基本信息代码表长度*/
unsigned long gul_client_code_len = 0;       /*客人入住信息代码表长度*/

HANDLE gh_std_out;          /*标准输出设备句柄*/
HANDLE gh_std_in;           /*标准输入设备句柄*/

int CreatList(TYPE_NODE **pphead);              /*数据链表初始化*/
void InitInterface(void);                 /*系统界面初始化*/
void ClearScreen(void);                         /*清屏*/
void ShowMenu(void);                            /*显示菜单栏*/
void PopMenu(int num);                          /*显示下拉菜单*/
void PopPrompt(int num);                        /*显示弹出窗口*/
void PopUp(SMALL_RECT *, WORD, LABEL_BUNDLE *, HOT_AREA *);  /*弹出窗口屏幕信息维护*/
void PopOff(void);                              /*关闭顶层弹出窗口*/
void DrawBox(SMALL_RECT *parea);                /*绘制边框*/
void LocSubMenu(int num, SMALL_RECT *parea);    /*主菜单下拉菜单定位*/
void ShowState(void);                           /*显示状态栏*/
void TagMainMenu(int num);                      /*标记被选中的主菜单项*/
void TagSubMenu(int num);                       /*标记被选中的子菜单项*/
int DealConInput(HOT_AREA *phot_area, int *pihot_num);  /*控制台输入处理*/
void SetHotPoint(HOT_AREA *phot_area, int hot_num);     /*设置热区*/
void RunSys(TYPE_NODE **pphd);                  /*系统功能模块的选择和运行*/
BOOL ExeFunction(int main_menu_num, int sub_menu_num);  /*功能模块的调用*/
void CloseSys(TYPE_NODE *phd);                  /*退出系统*/
BOOL ShowModule(char **pString, int n);
int ShowModule4(char **pString, int n);
int ShowModule3(char **pString, int n);
void move(int x, int y);              /*使得光标移动到指定的地点 */

BOOL LoadData(void);           /*数据加载*/
BOOL SaveData(void);           /*保存数据*/
BOOL BackupData(void);         /*备份数据*/
BOOL RestoreData(void);        /*恢复数据*/
BOOL ExitSys(void);            /*退出系统*/
BOOL HelpTopic(void);          /*帮助主体*/
BOOL AboutDorm(void);          /*关于系统*/

BOOL MaintainTypeInfo(void);         /*维护客房分类信息*/
BOOL MaintainItemInfo(void);         /*维护客房基本信息*/
BOOL MaintainClientInfo(void);       /*维护客人入住信息*/


BOOL QueryTypeInfo(void);      /*查询客房分类信息*/
BOOL QueryItemInfo(void);      /*查询客房基本信息*/
BOOL QueryClientInfo(void);    /*查询客人入住信息*/

BOOL StatSort(void);       /*按类别统计*/
BOOL StatMoney(void);      /*按营业额统计*/
BOOL StatClient(void);     /*按客户统计*/

void InsertTypeNode(TYPE_NODE* hd);    /*插入客房分类信息结点*/
void ModifTypeNode(TYPE_NODE *hd);     /*修改客房分类信息结点*/
void DelTypeNode(TYPE_NODE *hd);       /*删除客房分类信息结点*/

void InsertItemNode(TYPE_NODE* head);  /*插入客房基本信息结点*/
void ModifItemNode(TYPE_NODE *hd);     /*修改客房基本信息结点*/
void DelItemNode(TYPE_NODE *hd);       /*删除客房基本信息结点*/

void InsertClientNode(TYPE_NODE *hd);  /*插入客人入住信息结点*/
void ModifClientNode(TYPE_NODE *hd);   /*修改客人入住信息结点*/
void DelClientNode(TYPE_NODE *hd);     /*删除客人入住信息结点*/


void SeekType_id(TYPE_NODE *head);    /*根据输入的客房类别查找分类信息*/
void SeekType(TYPE_NODE *head);       /*输出所有分类信息*/

void SeekNumber(TYPE_NODE *head);     /*按客房编号查询客房基本信息 */
void SeekId_Price(TYPE_NODE *head);   /*以分类码和单价为条件查找基本信息*/

void Seekpeople_id(TYPE_NODE *head);  /*按客人身份证号码销查找客房租房信息*/
void SeekName_Time(TYPE_NODE *head);  /*按照时间查找租房信息*/

void StatTyperuzhu(void);                                       /*按类别统计所有类别总体的入住情况*/
TYPE_STAT *StatTypeInfo(TYPE_NODE *head, char time[18]);        /*按时间来统计并生成分类信息统计链表*/

void StatMonthYear(void);                                       /*按年份统计每个月的各类客房销售业绩 */
MONTH_STAT *StatYearInfo(TYPE_NODE *head,char year[5]);         /*按年统计每个月的客房出租情况并创建统计结果链表*/
void StatYearruzhu(TYPE_NODE *head);                            /*按年份统计各种客房的入住情况*/

void StatClient1(TYPE_NODE *head);                              /*按时间统计在该时间中客人的住宿情况*/
void StatClient2(TYPE_NODE *head);                              /*按时间统计在该时间中客人的住宿情况*/

BOOL SaveSysData(TYPE_NODE *hd);                       /*保存系统数据*/
BOOL BackupSysData(TYPE_NODE *hd, char *filename);     /*备份系统数据*/
BOOL RestoreSysData(TYPE_NODE **pphd, char *filename);  /*恢复系统数据*/

#endif /**< TYPE_H_INCLUDED*/

