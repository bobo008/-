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

#define SCR_ROW 25             //��Ļ���� 
#define SCR_COL 80             //��Ļ����

/**
 *�ͷ�������Ϣ��
 */
typedef struct type_node {
    char type_id;              /**< �ͷ���� ��D��  ˫�˼䣬��S��  ���䣬T �����䣬F ���˼� */
    int people_max;            /**< �����ס����*/
    int room_number;           /**< �ͷ�����*/
    int room_no;               /**< �ͷ�δס����*/ 
    struct item_node *inext;   /**< ָ��ͷ�������Ϣ֧����ָ��*/
    struct type_node *next;    /**< ָ����һ����ָ��*/
} TYPE_NODE;

/**
 *�ͷ�������Ϣ��
 */
typedef struct item_node {
    char number[4];               /**< �ͷ����*/
    char phone[5];                /**< �绰����*/
    char type_id;                 /**< �ͷ����*/
    float area;                   /**< ���*/
    float sale;                   /**< ÿ�����*/
    char balcony;                 /**< �Ƿ�����̨ 1 ��ʾ����̨��0 ��ʾû����̨ */
    char human;                   /**< �Ƿ��п��� ��y�� ���п�����ס ��n�� δס */
    struct client_node *snext;   /**< ָ������ⷿ��Ϣ֧����ָ��*/
    struct item_node *next;    /**< ָ����һ����ָ��*/
} ITEM_NODE;

/**
 *�����ⷿ��Ϣ��
 */
typedef struct client_node {
    char people_id[20];             /**< �������֤����*/
    char people_name[20];           /**< ��������*/
    char number[4];                 /**< ��ס�ͷ����*/
    char time_in[18];               /**< ��סʱ��*/
    char time_out[18];              /**< �˷�ʱ��*/
    float month;                    /**< ��ס����*/
    float money;                    /**< Ѻ��*/
    float should;                   /**< Ӧ�ɽ��*/
    float reality;                  /**< ʵ�ɽ��*/
    struct client_node *next;  /**< ָ����һ����ָ��*/
} CLIENT_NODE;

/**
 *���տͷ����ͳ�ƿͷ���ס�� 
 */
typedef struct type_stat {
    char type_id;             /**< ��������*/
    int room_number;          /**< �ͷ�����*/
    int room_y;              /**< ����ס��*/
    int room_n;              /**< δ��ס��*/
    struct type_stat *next;    /**< ָ����һ����ָ��*/
} TYPE_STAT;

/**
 *�����·�ͳ�Ƹ���ͷ�Ӫҵ�� 
 */
typedef struct month_stat {
    int month;                  /**< �·�*/
    float s;                    /**< ���˼�Ӫҵ��*/
    float d;                    /**< ˫�˼�Ӫҵ��*/
    float t;                    /**< ���˼�Ӫҵ��*/
    struct month_stat *next;     /**< ָ����һ����ָ��*/
} MONTH_STAT;

/**
 *���ͷ����ͳ�ƿͷ���ס���
 */
typedef struct number_stat {
	char number[4];                /**< �ͷ����*/
    float sale;                    /**< ʵ�ɽ��*/
    int month;                     /**< ��ס����*/
    float ruzhu;                   /**< ��ס��*/
    struct number_stat *next;    /**< ָ����һ����ָ��*/
} NUMBER_STAT;

/**
 *����������ͳ�ƿ�����ס������� 
 */
typedef struct client_stat {
	char people_id[20];             /**< �������֤����*/
	char people_name[20];           /**< ��������*/
	int month;                      /**< ��ס����*/
	float should;                   /**< Ӧ�ɽ��*/
	float shiji;                    /**< ʵ�ɽ��*/
	struct client_stat *next;        /**< ָ����һ����ָ��*/
} CLIENT_STAT; 

/**
 *��Ļ������Ϣ�������ṹ
 */
typedef struct layer_node {
    char LayerNo;            /**< �������ڲ���*/
    SMALL_RECT rcArea;       /**< ����������������*/
    CHAR_INFO *pContent;     /**< �������������ַ���Ԫԭ��Ϣ�洢������*/
    char *pScrAtt;           /**< �������������ַ���Ԫԭ����ֵ�洢������*/
    struct layer_node *next; /**< ָ����һ����ָ��*/
} LAYER_NODE;

/**
 *��ǩ���ṹ
 */
typedef struct labe1_bundle {
    char **ppLabel;        /**< ��ǩ�ַ��������׵�ַ*/
    COORD *pLoc;           /**< ��ǩ��λ�����׵�ַ*/
    int num;               /**< ��ǩ����*/
} LABEL_BUNDLE;

/**
 *�����ṹ
 */
typedef struct hot_area {
    SMALL_RECT *pArea;     /**< ������λ�����׵�ַ*/
    char *pSort;           /**< �������(�������ı���ѡ���)�����׵�ַ*/
    char *pTag;            /**< ������������׵�ַ*/
    int num;               /**< ��������*/
} HOT_AREA;

LAYER_NODE *gp_top_layer = NULL;                        /*����������Ϣ����ͷ*/
TYPE_NODE *gp_head = NULL;                              /*����ͷָ��*/

char *gp_sys_name = "���ݳ�����Ϣ����ϵͳ";             /*ϵͳ����*/
char *gp_type_info_filename = "type.dat";               /*�ͷ�������Ϣ�����ļ�*/
char *gp_item_info_filename = "item.dat";               /*�ͷ�������Ϣ�����ļ�*/
char *gp_client_info_filename = "client.dat";           /*�ͷ�������Ϣ�����ļ�*/


char *ga_main_menu[] = {"�ļ�(F)",                      /*ϵͳ���˵���*/
    	                "����ά��(M)",
                        "���ݲ�ѯ(Q)",
                        "����ͳ��(S)",
                        "����(H)"
                       };

char *ga_sub_menu[] = {"[A] ���ݱ���",                  /*ϵͳ�Ӳ˵���*/
                       "[B] ���ݱ���",
                       "[C] ���ݻָ�",
                       "[D] �˳�   Alt+X",
                       "[E] �ͷ�������Ϣ",
                       "[F] �ͷ�������Ϣ",
                       "[G] �ͷ�������Ϣ",
                       "[H] �ͷ�������Ϣ",
                       "[I] �ͷ�������Ϣ",
                       "[J] �ͷ�������Ϣ",
                       "[K] �����ͳ��",
                       "[L] Ӫҵ��ͳ��",
                       "[M] ������ͳ��",
                       "[N] ��������",
                       "",
                       "[O] ����..."
                      };

int ga_sub_menu_count[] = {4, 3, 3, 3, 3};  /*�����˵������Ӳ˵��ĸ���*/
int gi_sel_menu = 1;                        /*��ѡ�е����˵����,��ʼΪ1*/
int gi_sel_sub_menu = 0;                    /*��ѡ�е��Ӳ˵����,��ʼΪ0,��ʾδѡ��*/

CHAR_INFO *gp_buff_menubar_info = NULL;     /*��Ų˵�����Ļ���ַ���Ϣ�Ļ�����*/
CHAR_INFO *gp_buff_stateBar_info = NULL;    /*���״̬����Ļ���ַ���Ϣ�Ļ�����*/

char *gp_scr_att = NULL;          /*�����Ļ���ַ���Ԫ����ֵ�Ļ�����*/
char *gp_type_code = NULL;        /*��ſͷ�������Ϣ���������ݻ�����*/
char *gp_item_code = NULL;        /*��ſͷ�������Ϣ���������ݻ�����*/
char *gp_client_code = NULL;      /*��ſ�����ס��Ϣ���������ݻ�����*/
char gc_sys_state = '\0';         /*��������ϵͳ״̬���ַ�*/

unsigned long gul_type_code_len = 0;         /*�ͷ�������Ϣ�������*/
unsigned long gul_item_code_len = 0;         /*�ͷ�������Ϣ�������*/
unsigned long gul_client_code_len = 0;       /*������ס��Ϣ�������*/

HANDLE gh_std_out;          /*��׼����豸���*/
HANDLE gh_std_in;           /*��׼�����豸���*/

int CreatList(TYPE_NODE **pphead);              /*���������ʼ��*/
void InitInterface(void);                 /*ϵͳ�����ʼ��*/
void ClearScreen(void);                         /*����*/
void ShowMenu(void);                            /*��ʾ�˵���*/
void PopMenu(int num);                          /*��ʾ�����˵�*/
void PopPrompt(int num);                        /*��ʾ��������*/
void PopUp(SMALL_RECT *, WORD, LABEL_BUNDLE *, HOT_AREA *);  /*����������Ļ��Ϣά��*/
void PopOff(void);                              /*�رն��㵯������*/
void DrawBox(SMALL_RECT *parea);                /*���Ʊ߿�*/
void LocSubMenu(int num, SMALL_RECT *parea);    /*���˵������˵���λ*/
void ShowState(void);                           /*��ʾ״̬��*/
void TagMainMenu(int num);                      /*��Ǳ�ѡ�е����˵���*/
void TagSubMenu(int num);                       /*��Ǳ�ѡ�е��Ӳ˵���*/
int DealConInput(HOT_AREA *phot_area, int *pihot_num);  /*����̨���봦��*/
void SetHotPoint(HOT_AREA *phot_area, int hot_num);     /*��������*/
void RunSys(TYPE_NODE **pphd);                  /*ϵͳ����ģ���ѡ�������*/
BOOL ExeFunction(int main_menu_num, int sub_menu_num);  /*����ģ��ĵ���*/
void CloseSys(TYPE_NODE *phd);                  /*�˳�ϵͳ*/
BOOL ShowModule(char **pString, int n);
int ShowModule4(char **pString, int n);
int ShowModule3(char **pString, int n);
void move(int x, int y);              /*ʹ�ù���ƶ���ָ���ĵص� */

BOOL LoadData(void);           /*���ݼ���*/
BOOL SaveData(void);           /*��������*/
BOOL BackupData(void);         /*��������*/
BOOL RestoreData(void);        /*�ָ�����*/
BOOL ExitSys(void);            /*�˳�ϵͳ*/
BOOL HelpTopic(void);          /*��������*/
BOOL AboutDorm(void);          /*����ϵͳ*/

BOOL MaintainTypeInfo(void);         /*ά���ͷ�������Ϣ*/
BOOL MaintainItemInfo(void);         /*ά���ͷ�������Ϣ*/
BOOL MaintainClientInfo(void);       /*ά��������ס��Ϣ*/


BOOL QueryTypeInfo(void);      /*��ѯ�ͷ�������Ϣ*/
BOOL QueryItemInfo(void);      /*��ѯ�ͷ�������Ϣ*/
BOOL QueryClientInfo(void);    /*��ѯ������ס��Ϣ*/

BOOL StatSort(void);       /*�����ͳ��*/
BOOL StatMoney(void);      /*��Ӫҵ��ͳ��*/
BOOL StatClient(void);     /*���ͻ�ͳ��*/

void InsertTypeNode(TYPE_NODE* hd);    /*����ͷ�������Ϣ���*/
void ModifTypeNode(TYPE_NODE *hd);     /*�޸Ŀͷ�������Ϣ���*/
void DelTypeNode(TYPE_NODE *hd);       /*ɾ���ͷ�������Ϣ���*/

void InsertItemNode(TYPE_NODE* head);  /*����ͷ�������Ϣ���*/
void ModifItemNode(TYPE_NODE *hd);     /*�޸Ŀͷ�������Ϣ���*/
void DelItemNode(TYPE_NODE *hd);       /*ɾ���ͷ�������Ϣ���*/

void InsertClientNode(TYPE_NODE *hd);  /*���������ס��Ϣ���*/
void ModifClientNode(TYPE_NODE *hd);   /*�޸Ŀ�����ס��Ϣ���*/
void DelClientNode(TYPE_NODE *hd);     /*ɾ��������ס��Ϣ���*/


void SeekType_id(TYPE_NODE *head);    /*��������Ŀͷ������ҷ�����Ϣ*/
void SeekType(TYPE_NODE *head);       /*������з�����Ϣ*/

void SeekNumber(TYPE_NODE *head);     /*���ͷ���Ų�ѯ�ͷ�������Ϣ */
void SeekId_Price(TYPE_NODE *head);   /*�Է�����͵���Ϊ�������һ�����Ϣ*/

void Seekpeople_id(TYPE_NODE *head);  /*���������֤���������ҿͷ��ⷿ��Ϣ*/
void SeekName_Time(TYPE_NODE *head);  /*����ʱ������ⷿ��Ϣ*/

void StatTyperuzhu(void);                                       /*�����ͳ����������������ס���*/
TYPE_STAT *StatTypeInfo(TYPE_NODE *head, char time[18]);        /*��ʱ����ͳ�Ʋ����ɷ�����Ϣͳ������*/

void StatMonthYear(void);                                       /*�����ͳ��ÿ���µĸ���ͷ�����ҵ�� */
MONTH_STAT *StatYearInfo(TYPE_NODE *head,char year[5]);         /*����ͳ��ÿ���µĿͷ��������������ͳ�ƽ������*/
void StatYearruzhu(TYPE_NODE *head);                            /*�����ͳ�Ƹ��ֿͷ�����ס���*/

void StatClient1(TYPE_NODE *head);                              /*��ʱ��ͳ���ڸ�ʱ���п��˵�ס�����*/
void StatClient2(TYPE_NODE *head);                              /*��ʱ��ͳ���ڸ�ʱ���п��˵�ס�����*/

BOOL SaveSysData(TYPE_NODE *hd);                       /*����ϵͳ����*/
BOOL BackupSysData(TYPE_NODE *hd, char *filename);     /*����ϵͳ����*/
BOOL RestoreSysData(TYPE_NODE **pphd, char *filename);  /*�ָ�ϵͳ����*/

#endif /**< TYPE_H_INCLUDED*/

