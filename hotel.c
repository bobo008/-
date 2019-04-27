#include "hotel.h"

unsigned long ul;

int main()
{
    COORD size = {SCR_COL, SCR_ROW};              /*���ڻ�������С*/

    gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE); /* ��ȡ��׼����豸���*/
    gh_std_in = GetStdHandle(STD_INPUT_HANDLE);   /* ��ȡ��׼�����豸���*/

    SetConsoleTitle(gp_sys_name);                 /*���ô��ڱ���*/
    SetConsoleScreenBufferSize(gh_std_out, size); /*���ô��ڻ�������С80*25*/

    LoadData();                   /*���ݼ���*/
    InitInterface();          /*�����ʼ��*/
    RunSys(&gp_head);             /*ϵͳ����ģ���ѡ������*/
    CloseSys(gp_head);            /*�˳�ϵͳ*/

    return 0;
}

/**
 * ��������: LoadData
 * ��������: ����������ݴ������ļ����뵽�ڴ滺������ʮ��������.
 * �������: ��
 * �������: ��
 * �� �� ֵ: BOOL����, ���ܺ����г��˺���ExitSys�ķ���ֵ����ΪFALSE��,
 *           ���������ķ���ֵ����ΪTRUE.
 *
 * ����˵��: Ϊ���ܹ���ͳһ�ķ�ʽ���ø����ܺ���, ����Щ���ܺ�����ԭ����Ϊ
 *           һ��, ���޲����ҷ���ֵΪBOOL. ����ֵΪFALSEʱ, ������������.
 */
BOOL LoadData()
{
    int Re = 0;

    Re = CreatList(&gp_head);
    if (Re < 28)
    {  /*���ݼ�����ʾ��Ϣ*/
        printf("\nϵͳ�������ݲ�����!\n");
    }

    printf("\n�����������...\n");
    getch();

    return TRUE;
}

/**
 * ��������: CreatList
 * ��������: �������ļ���ȡ��������, ����ŵ���������ʮ��������.
 * �������: ��
 * �������: phead ����ͷָ��ĵ�ַ, ����������������ʮ����.
 * �� �� ֵ: int����ֵ, ��ʾ�����������.
 *           0  ����, ������
 *           4  �Ѽ��ؿͷ�������Ϣ���ݣ��޿ͷ�������Ϣ�Ϳ�����ס��Ϣ����
 *           12 �Ѽ��ؿͷ�������Ϣ�Ϳͷ�������Ϣ���ݣ��޿�����ס��Ϣ����
 *           28 ����������ݶ��Ѽ���
 *
 * ����˵��:
 */
int CreatList(TYPE_NODE **phead)
{
    TYPE_NODE *hd = NULL, *pTypeNode, tmp1;
    ITEM_NODE *pItemNode, tmp2;
    CLIENT_NODE *pClientNode, tmp3;
    FILE *pFile;
    int find;
    int re = 0;

    if ((pFile = fopen(gp_type_info_filename, "rb")) == NULL)
    {
        printf("�ͷ�������Ϣ�����ļ���ʧ��!\n");
        return re;
    }
    printf("�ͷ�������Ϣ�����ļ��򿪳ɹ�!\n");

    /*�������ļ��ж��ͷ�������Ϣ���ݣ������Ժ���ȳ��ķ�ʽ������������*/
    while (fread(&tmp1, sizeof(TYPE_NODE), 1, pFile) == 1)
    {
        pTypeNode = (TYPE_NODE *)malloc(sizeof(TYPE_NODE));
        *pTypeNode = tmp1;
        pTypeNode->inext = NULL;
        pTypeNode->next = hd;
        hd = pTypeNode;
    }
    fclose(pFile);

    if (hd == NULL)
    {
        printf("�ͷ�������Ϣ�����ļ�����ʧ��!\n");
        return re;
    }
    printf("�ͷ�������Ϣ�����ļ����سɹ�!\n");
    *phead = hd;
    re += 4;

    if ((pFile = fopen(gp_item_info_filename, "rb")) == NULL)
    {
        printf("�ͷ�������Ϣ�����ļ���ʧ��!\n");
        return re;
    }
    printf("�ͷ�������Ϣ�����ļ��򿪳ɹ�!\n");

    re += 8;

    /*�������ļ��ж�ȡ�ͷ�������Ϣ���ݣ�����������Ӧ���Ŀͷ�������Ϣ֧����*/
    while (fread(&tmp2, sizeof(ITEM_NODE), 1, pFile) == 1)
    {
        /*������㣬��Ŵ������ļ��ж����Ŀͷ�������Ϣ*/
        pItemNode = (ITEM_NODE *)malloc(sizeof(ITEM_NODE));
        *pItemNode = tmp2;
        pItemNode->snext = NULL;

        /*�������ϲ��Ҹÿͷ���������Ӧ���������*/
        pTypeNode = hd;
        while (pTypeNode != NULL && pTypeNode->type_id != pItemNode->type_id)
        {
            pTypeNode = pTypeNode->next;
        }

        if (pTypeNode != NULL) /*����ҵ����򽫽���Ժ���ȳ���ʽ����ͷ�������Ϣ֧��*/
        {
            pItemNode->next = pTypeNode->inext;
            pTypeNode->inext = pItemNode;
        }
        else  /*���δ�ҵ������ͷ������������ڴ�ռ�*/
        {
            free(pItemNode);
        }
    }
    fclose(pFile);
    printf("�ͷ�������Ϣ�����ļ����سɹ�!\n");

    if ((pFile = fopen(gp_client_info_filename, "rb")) == NULL)
    {
        printf("�����ⷿ��Ϣ�����ļ���ʧ��!\n");
        return re;
    }
    printf("�����ⷿ��Ϣ�����ļ��򿪳ɹ�!\n");

    re += 16;

    /*�������ļ��ж�ȡ�����ⷿ��Ϣ���ݣ����������Ϣ֧����Ӧ�����ⷿ��Ϣ֧����*/
    while (fread(&tmp3, sizeof(CLIENT_NODE), 1, pFile) == 1)
    {
        /*������㣬��Ŵ������ļ��ж����Ŀ�����ס��Ϣ*/
        pClientNode = (CLIENT_NODE *)malloc(sizeof(CLIENT_NODE));
        *pClientNode = tmp3;

        /*���ҿͷ�������Ϣ֧���϶�Ӧ������Ϣ���*/
        pTypeNode = hd;
        find = 0;
        while (pTypeNode != NULL && find == 0)
        {
            pItemNode = pTypeNode->inext;
            while (pItemNode != NULL && find == 0)
            {
                if ( strcmp(pItemNode->number , pClientNode->number  ) == 0 )
                {
                    find = 1;
                    break;
                }
                pItemNode = pItemNode->next;
            }
            pTypeNode = pTypeNode->next;
        }

        if (find)  /*����ҵ����򽫽���Ժ���ȳ���ʽ���������ס��Ϣ֧����*/
        {
            pClientNode->next = pItemNode->snext;
            pItemNode->snext = pClientNode;
        }
        else /*���δ�ҵ������ͷ������������ڴ�ռ�*/
        {
            free(pClientNode);
        }
    }

    fclose(pFile);
    printf("�����ⷿ��Ϣ�����ļ����سɹ�!\n");

    return re;
}

/**
 * ��������: InitInterface
 * ��������: ��ʼ������.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void InitInterface()
{
    WORD att = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
               | BACKGROUND_BLUE;  /*��ɫǰ������ɫ����*/

    SetConsoleTextAttribute(gh_std_out, att);  /*���ÿ���̨��Ļ�������ַ�����*/

    ClearScreen();  /* ����*/

    /*��������������Ϣ��ջ������ʼ�������Ļ���ڵ�����һ�㵯������*/
    gp_scr_att = (char *)calloc(SCR_COL * SCR_ROW, sizeof(char));/*��Ļ�ַ�����*/
    gp_top_layer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    gp_top_layer->LayerNo = 0;      /*�������ڵĲ��Ϊ0*/
    gp_top_layer->rcArea.Left = 0;  /*�������ڵ�����Ϊ������Ļ����*/
    gp_top_layer->rcArea.Top = 0;
    gp_top_layer->rcArea.Right = SCR_COL - 1;
    gp_top_layer->rcArea.Bottom = SCR_ROW - 1;
    gp_top_layer->pContent = NULL;
    gp_top_layer->pScrAtt = gp_scr_att;
    gp_top_layer->next = NULL;

    ShowMenu();     /*��ʾ�˵���*/
    ShowState();    /*��ʾ״̬��*/

    return;
}

/**
 * ��������: ClearScreen
 * ��������: �����Ļ��Ϣ.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void ClearScreen()
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD home = {0, 0};
    unsigned long size;

    GetConsoleScreenBufferInfo(gh_std_out, &bInfo );/*ȡ��Ļ��������Ϣ*/
    size =  bInfo.dwSize.X * bInfo.dwSize.Y; /*������Ļ�������ַ���Ԫ��*/

    /*����Ļ���������е�Ԫ���ַ���������Ϊ��ǰ��Ļ�������ַ�����*/
    FillConsoleOutputAttribute(gh_std_out, bInfo.wAttributes, size, home, &ul);

    /*����Ļ���������е�Ԫ���Ϊ�ո��ַ�*/
    FillConsoleOutputCharacter(gh_std_out, ' ', size, home, &ul);

    return;
}

/**
 * ��������: ShowMenu
 * ��������: ����Ļ����ʾ���˵�, ����������, �����˵���һ������ѡ�б��.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void ShowMenu()
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    COORD size;
    COORD pos = {0, 0};
    int i, j;
    int PosA = 2, PosB;
    char ch;

    GetConsoleScreenBufferInfo(gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;
    SetConsoleCursorPosition(gh_std_out, pos);
    for (i=0; i < 5; i++) /*�ڴ��ڵ�һ�е�һ�д�������˵���*/
    {
        printf("  %s  ", ga_main_menu[i]);
    }

    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;
    SetConsoleCursorInfo(gh_std_out, &lpCur);  /*���ع��*/

    /*���붯̬�洢����Ϊ��Ų˵�����Ļ���ַ���Ϣ�Ļ�����*/
    gp_buff_menubar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
    SMALL_RECT rcMenu ={0, 0, size.X-1, 0};

    /*�����ڵ�һ�е����ݶ��뵽��Ų˵�����Ļ���ַ���Ϣ�Ļ�������*/
    ReadConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    /*����һ����Ӣ����ĸ��Ϊ��ɫ�������ַ���Ԫ��Ϊ�׵׺���*/
    for (i=0; i<size.X; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (char)((gp_buff_menubar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*�޸ĺ�Ĳ˵����ַ���Ϣ��д�����ڵĵ�һ��*/
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);
    COORD endPos = {0, 1};
    SetConsoleCursorPosition(gh_std_out, endPos);  /*�����λ�������ڵ�2�е�1��*/

    /*���˵�����Ϊ�������������Ϊ�˵���ţ���������Ϊ0(��ť��)*/
    i = 0;
    do
    {
        PosB = PosA + strlen(ga_main_menu[i]);  /*��λ��i+1�Ų˵������ֹλ��*/
        for (j=PosA; j<PosB; j++)
        {
            gp_scr_att[j] |= (i+1) << 2; /*���ò˵��������ַ���Ԫ������ֵ*/
        }
        PosA = PosB + 4;
        i++;
    } while (i < 5);

    TagMainMenu(gi_sel_menu);  /*��ѡ�����˵���������ǣ�gi_sel_menu��ֵΪ1*/

    return;
}

/**
 * ��������: ShowState
 * ��������: ��ʾ״̬��.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��: ״̬���ַ�����Ϊ�׵׺���, ��ʼ״̬��״̬��Ϣ.
 */
void ShowState()
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int i;

    GetConsoleScreenBufferInfo(gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;
    SMALL_RECT rcMenu ={0, bInfo.dwSize.Y-1, size.X-1, bInfo.dwSize.Y-1};

    if (gp_buff_stateBar_info == NULL)
    {
        gp_buff_stateBar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
        ReadConsoleOutput(gh_std_out, gp_buff_stateBar_info, size, pos, &rcMenu);
    }

    for (i=0; i<size.X; i++)
    {
        (gp_buff_stateBar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                                | BACKGROUND_RED;
/*
        ch = (char)((gp_buff_stateBar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_stateBar_info+i)->Attributes |= FOREGROUND_RED;
        }
*/
    }

    WriteConsoleOutput(gh_std_out, gp_buff_stateBar_info, size, pos, &rcMenu);

    return;
}

/**
 * ��������: TagMainMenu
 * ��������: ��ָ�����˵�������ѡ�б�־.
 * �������: num ѡ�е����˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void TagMainMenu(int num)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int PosA = 2, PosB;
    char ch;
    int i;

    if (num == 0) /*numΪ0ʱ������ȥ�����˵���ѡ�б��*/
    {
        PosA = 0;
        PosB = 0;
    }
    else  /*���򣬶�λѡ�����˵������ֹλ��: PosAΪ��ʼλ��, PosBΪ��ֹλ��*/
    {
        for (i=1; i<num; i++)
        {
            PosA += strlen(ga_main_menu[i-1]) + 4;
        }
        PosB = PosA + strlen(ga_main_menu[num-1]);
    }

    GetConsoleScreenBufferInfo(gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;

    /*ȥ��ѡ�в˵���ǰ��Ĳ˵���ѡ�б��*/
    for (i=0; i<PosA; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (gp_buff_menubar_info+i)->Char.AsciiChar;
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*��ѡ�в˵���������ǣ��ڵװ���*/
    for (i=PosA; i<PosB; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN
                                               | FOREGROUND_RED;
    }

    /*ȥ��ѡ�в˵������Ĳ˵���ѡ�б��*/
    for (i=PosB; i<bInfo.dwSize.X; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (char)((gp_buff_menubar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*�����ñ�ǵĲ˵�����Ϣд�����ڵ�һ��*/
    SMALL_RECT rcMenu ={0, 0, size.X-1, 0};
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    return;
}

/**
 * ��������: CloseSys
 * ��������: �ر�ϵͳ.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void CloseSys(TYPE_NODE *hd)
{
    TYPE_NODE *pTypeNode1 = hd, *pTypeNode2;
    ITEM_NODE *pItemNode1, *pItemNode2;
    CLIENT_NODE *pclientNode1, *pclientNode2;

    while (pTypeNode1 != NULL) /*�ͷ�ʮ�ֽ�������Ķ�̬�洢��*/
    {
        pTypeNode2 = pTypeNode1->next;
        pItemNode1 = pTypeNode1->inext;
        while (pItemNode1 != NULL) /*�ͷ���Ʒ������Ϣ֧���Ķ�̬�洢��*/
        {
            pItemNode2 = pItemNode1->next;
            pclientNode1 = pItemNode1->snext;

            while (pclientNode1 != NULL) /*�ͷ���Ʒ������Ϣ֧���Ķ�̬�洢��*/
            {
                pclientNode2 = pclientNode1->next;
                free(pclientNode1);
                pclientNode1 = pclientNode2;
            }

            free(pItemNode1);
            pItemNode1 = pItemNode2;
        }

        free(pTypeNode1);  /*�ͷ��������Ķ�̬�洢��*/
        pTypeNode1 = pTypeNode2;
    }

    ClearScreen();        /*����*/

    /*�ͷŴ�Ų˵�����״̬������Ʒ���������Ϣ��̬�洢��*/
    free(gp_buff_menubar_info);
    free(gp_buff_stateBar_info);

    /*�رձ�׼���������豸���*/
    CloseHandle(gh_std_out);
    CloseHandle(gh_std_in);

    /*�����ڱ�������Ϊ���н���*/
    SetConsoleTitle("���н���");

    return;
}

/**
 * ��������: RunSys
 * ��������: ����ϵͳ, ��ϵͳ�������������û���ѡ��Ĺ���ģ��.
 * �������: ��
 * �������: phead ����ͷָ��ĵ�ַ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void RunSys(TYPE_NODE **phead)
{
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    BOOL bRet = TRUE;
    int i, loc, num;
    int cNo, cAtt;      /*cNo:�ַ���Ԫ���, cAtt:�ַ���Ԫ����*/
    char vkc, asc;      /*vkc:���������, asc:�ַ���ASCII��ֵ*/

    while (bRet)
    {
        /*�ӿ���̨���뻺�����ж�һ����¼*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);

        if (inRec.EventType == MOUSE_EVENT) /*�����¼������¼�����*/
        {
            pos = inRec.Event.MouseEvent.dwMousePosition;  /*��ȡ�������λ��*/
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3; /*ȡ��λ�õĲ��*/
            cAtt = gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2;/*ȡ���ַ���Ԫ����*/
            if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
            {
                if (cAtt > 0 && cAtt != gi_sel_menu && gp_top_layer->LayerNo > 0)
                {
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                    PopMenu(cAtt);       /*��������������˵����Ӧ���Ӳ˵�*/
                }
            }
            else if (cAtt > 0) /*�������λ��Ϊ�����Ӳ˵��Ĳ˵����ַ���Ԫ*/
            {
                TagSubMenu(cAtt); /*�ڸ��Ӳ˵�������ѡ�б��*/
            }

            if (inRec.Event.MouseEvent.dwButtonState
                == FROM_LEFT_1ST_BUTTON_PRESSED) /*������������ߵ�һ��*/
            {
                if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
                {
                    if (cAtt > 0) /*�����λ�ô�������(���˵����ַ���Ԫ)*/
                    {
                        PopMenu(cAtt);   /*��������������˵����Ӧ���Ӳ˵�*/
                    }
                    /*�����λ�ò��������˵����ַ���Ԫ�������Ӳ˵�����*/
                    else if (gp_top_layer->LayerNo > 0)
                    {
                        PopOff();            /*�رյ������Ӳ˵�*/
                        gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                    }
                }
                else /*��Ų�Ϊ0��������λ�ñ������Ӳ˵�����*/
                {
                    if (cAtt > 0) /*�����λ�ô�������(�Ӳ˵����ַ���Ԫ)*/
                    {
                        PopOff(); /*�رյ������Ӳ˵�*/
                        gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/

                        /*ִ�ж�Ӧ���ܺ���:gi_sel_menu���˵����, cAtt�Ӳ˵����*/
                        bRet = ExeFunction(gi_sel_menu, cAtt);
                    }
                }
            }
            else if (inRec.Event.MouseEvent.dwButtonState
                     == RIGHTMOST_BUTTON_PRESSED) /*�����������Ҽ�*/
            {
                if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
                {
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                }
            }
        }
        else if (inRec.EventType == KEY_EVENT  /*�����¼�ɰ�������*/
                 && inRec.Event.KeyEvent.bKeyDown) /*�Ҽ�������*/
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode; /*��ȡ�������������*/
            asc = inRec.Event.KeyEvent.uChar.AsciiChar; /*��ȡ������ASC��*/

            /*ϵͳ��ݼ��Ĵ���*/
            if (vkc == 112) /*�������F1��*/
            {
                if (gp_top_layer->LayerNo != 0) /*�����ǰ���Ӳ˵�����*/
                {
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                }
                bRet = ExeFunction(5, 1);  /*���а������⹦�ܺ���*/
            }
            else if (inRec.Event.KeyEvent.dwControlKeyState
                     & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
            { /*������������Alt��*/
                switch (vkc)  /*�ж���ϼ�Alt+��ĸ*/
                {
                    case 88:  /*Alt+X �˳�*/
                        if (gp_top_layer->LayerNo != 0)
                        {
                            PopOff();
                            gi_sel_sub_menu = 0;
                        }
                        bRet = ExeFunction(1, 4);
                        break;
                    case 70:  /*Alt+F*/
                        PopMenu(1);
                        break;
                    case 77: /*Alt+M*/
                        PopMenu(2);
                        break;
                    case 81: /*Alt+Q*/
                        PopMenu(3);
                        break;
                    case 83: /*Alt+S*/
                        PopMenu(4);
                        break;
                    case 72: /*Alt+H*/
                        PopMenu(5);
                        break;
                }
            }
            else if (asc == 0) /*�������Ƽ��Ĵ���*/
            {
                if (gp_top_layer->LayerNo == 0) /*���δ�����Ӳ˵�*/
                {
                    switch (vkc) /*�������(���ҡ���)������Ӧ�������Ƽ�*/
                    {
                        case 37:
                            gi_sel_menu--;
                            if (gi_sel_menu == 0)
                            {
                                gi_sel_menu = 5;
                            }
                            TagMainMenu(gi_sel_menu);
                            break;
                        case 39:
                            gi_sel_menu++;
                            if (gi_sel_menu == 6)
                            {
                                gi_sel_menu = 1;
                            }
                            TagMainMenu(gi_sel_menu);
                            break;
                        case 40:
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                    }
                }
                else  /*�ѵ����Ӳ˵�ʱ*/
                {
                    for (loc=0, i=1; i<gi_sel_menu; i++)
                    {
                        loc += ga_sub_menu_count[i-1];
                    }  /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
                    switch (vkc) /*�����(���ҡ��ϡ���)�Ĵ���*/
                    {
                        case 37:
                            gi_sel_menu--;
                            if (gi_sel_menu < 1)
                            {
                                gi_sel_menu = 5;
                            }
                            TagMainMenu(gi_sel_menu);
                            PopOff();
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                        case 38:
                            num = gi_sel_sub_menu - 1;
                            if (num < 1)
                            {
                                num = ga_sub_menu_count[gi_sel_menu-1];
                            }
                            if (strlen(ga_sub_menu[loc+num-1]) == 0)
                            {
                                num--;
                            }
                            TagSubMenu(num);
                            break;
                        case 39:
                            gi_sel_menu++;
                            if (gi_sel_menu > 5)
                            {
                                gi_sel_menu = 1;
                            }
                            TagMainMenu(gi_sel_menu);
                            PopOff();
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                        case 40:
                            num = gi_sel_sub_menu + 1;
                            if (num > ga_sub_menu_count[gi_sel_menu-1])
                            {
                                num = 1;
                            }
                            if (strlen(ga_sub_menu[loc+num-1]) == 0)
                            {
                                num++;
                            }
                            TagSubMenu(num);
                            break;
                    }
                }
            }
            else if ((asc-vkc == 0) || (asc-vkc == 32)){  /*������ͨ��*/
                if (gp_top_layer->LayerNo == 0)  /*���δ�����Ӳ˵�*/
                {
                    switch (vkc)
                    {
                        case 70: /*f��F*/
                            PopMenu(1);
                            break;
                        case 77: /*m��M*/
                            PopMenu(2);
                            break;
                        case 81: /*q��Q*/
                            PopMenu(3);
                            break;
                        case 83: /*s��S*/
                            PopMenu(4);
                            break;
                        case 72: /*h��H*/
                            PopMenu(5);
                            break;
                        case 13: /*�س�*/
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                    }
                }
                else /*�ѵ����Ӳ˵�ʱ�ļ������봦��*/
                {
                    if (vkc == 27) /*�������ESC��*/
                    {
                        PopOff();
                        gi_sel_sub_menu = 0;
                    }
                    else if(vkc == 13) /*������»س���*/
                    {
                        num = gi_sel_sub_menu;
                        PopOff();
                        gi_sel_sub_menu = 0;
                        bRet = ExeFunction(gi_sel_menu, num);
                    }
                    else /*������ͨ���Ĵ���*/
                    {
                        /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
                        for (loc=0, i=1; i<gi_sel_menu; i++)
                        {
                            loc += ga_sub_menu_count[i-1];
                        }

                        /*�����뵱ǰ�Ӳ˵���ÿһ��Ĵ����ַ����бȽ�*/
                        for (i=loc; i<loc+ga_sub_menu_count[gi_sel_menu-1]; i++)
                        {
                            if (strlen(ga_sub_menu[i])>0 && vkc==ga_sub_menu[i][1])
                            { /*���ƥ��ɹ�*/
                                PopOff();
                                gi_sel_sub_menu = 0;
                                bRet = ExeFunction(gi_sel_menu, i-loc+1);
                            }
                        }
                    }
                }
            }
        }
    }
}

void PopPrompt(int num)
{

}

/**
 * ��������: PopMenu
 * ��������: ����ָ�����˵����Ӧ���Ӳ˵�.
 * �������: num ָ�������˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void PopMenu(int num)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh;
    int i, j, loc = 0;

    if (num != gi_sel_menu)       /*���ָ�����˵�������ѡ�в˵�*/
    {
        if (gp_top_layer->LayerNo != 0) /*�����ǰ�����Ӳ˵�����*/
        {
            PopOff();
            gi_sel_sub_menu = 0;
        }
    }
    else if (gp_top_layer->LayerNo != 0) /*���ѵ������Ӳ˵����򷵻�*/
    {
        return;
    }

    gi_sel_menu = num;    /*��ѡ�����˵�����Ϊָ�������˵���*/
    TagMainMenu(gi_sel_menu); /*��ѡ�е����˵����������*/
    LocSubMenu(gi_sel_menu, &rcPop); /*���㵯���Ӳ˵�������λ��, �����rcPop��*/

    /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
    for (i=1; i<gi_sel_menu; i++)
    {
        loc += ga_sub_menu_count[i-1];
    }
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel = ga_sub_menu + loc;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = ga_sub_menu_count[gi_sel_menu-1]; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[labels.num];/*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    for (i=0; i<labels.num; i++) /*ȷ����ǩ�ַ��������λ�ã����������������*/
    {
        aLoc[i].X = rcPop.Left + 2;
        aLoc[i].Y = rcPop.Top + i + 1;
    }
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = labels.num;       /*�����ĸ��������ڱ�ǩ�ĸ��������Ӳ˵�������*/
    SMALL_RECT aArea[areas.num];                    /*������������������λ��*/
    char aSort[areas.num];                      /*��������������������Ӧ���*/
    char aTag[areas.num];                         /*����������ÿ�������ı��*/
    for (i=0; i<areas.num; i++)
    {
        aArea[i].Left = rcPop.Left + 2;  /*������λ*/
        aArea[i].Top = rcPop.Top + i + 1;
        aArea[i].Right = rcPop.Right - 2;
        aArea[i].Bottom = aArea[i].Top;
        aSort[i] = 0;       /*�������Ϊ0(��ť��)*/
        aTag[i] = i + 1;           /*������˳����*/
    }
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/
    pos.X = rcPop.Left + 2;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    {
        /*��ѭ�������ڿմ��Ӳ���λ�û����γɷָ�����ȡ���˲˵������������*/
        pCh = ga_sub_menu[loc+pos.Y-rcPop.Top-1];
        if (strlen(pCh)==0) /*����Ϊ0������Ϊ�մ�*/
        {
            /*���Ȼ�����*/
            FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-3, pos, &ul);
            for (j=rcPop.Left+2; j<rcPop.Right-1; j++)
            {
                /*ȡ���������ַ���Ԫ����������*/
                gp_scr_att[pos.Y*SCR_COL+j] &= 3; /*��λ��Ľ�������˵���λ*/
            }
        }

    }
    /*���Ӳ˵���Ĺ��ܼ���Ϊ�׵׺���*/
    pos.X = rcPop.Left + 3;
    att =  FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    {
        if (strlen(ga_sub_menu[loc+pos.Y-rcPop.Top-1])==0)
        {
            continue;  /*�����մ�*/
        }
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    return;
}

/**
 * ��������: PopUp
 * ��������: ��ָ�������������������Ϣ, ͬʱ��������, ����������λ����Ϣ��ջ.
 * �������: pRc ��������λ�����ݴ�ŵĵ�ַ
 *           att �������������ַ�����
 *           pLabel ���������б�ǩ����Ϣ��ŵĵ�ַ
             pHotArea ����������������Ϣ��ŵĵ�ַ
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void PopUp(SMALL_RECT *pRc, WORD att, LABEL_BUNDLE *pLabel, HOT_AREA *pHotArea)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j, row;

    /*������������λ���ַ���Ԫ��Ϣ��ջ*/
    size.X = pRc->Right - pRc->Left + 1;    /*�������ڵĿ��*/
    size.Y = pRc->Bottom - pRc->Top + 1;    /*�������ڵĸ߶�*/
    /*�����ŵ������������Ϣ�Ķ�̬�洢��*/
    nextLayer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    nextLayer->next = gp_top_layer;
    nextLayer->LayerNo = gp_top_layer->LayerNo + 1;
    nextLayer->rcArea = *pRc;
    nextLayer->pContent = (CHAR_INFO *)malloc(size.X*size.Y*sizeof(CHAR_INFO));
    nextLayer->pScrAtt = (char *)malloc(size.X*size.Y*sizeof(char));
    pCh = nextLayer->pScrAtt;
    /*���������ڸ���������ַ���Ϣ���棬�����ڹرյ�������ʱ�ָ�ԭ��*/
    ReadConsoleOutput(gh_std_out, nextLayer->pContent, size, pos, pRc);
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {   /*�˶���ѭ�����������ַ���Ԫ��ԭ������ֵ���붯̬�洢���������Ժ�ָ�*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            *pCh = gp_scr_att[i*SCR_COL+j];
            pCh++;
        }
    }

    gp_top_layer = nextLayer;  /*��ɵ������������Ϣ��ջ����*/
    /*���õ������������ַ���������*/
    pos.X = pRc->Left;
    pos.Y = pRc->Top;
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        FillConsoleOutputAttribute(gh_std_out, att, size.X, pos, &ul);
        pos.Y++;
    }

    /*����ǩ���еı�ǩ�ַ������趨��λ�����*/
    for (i=0; i<pLabel->num; i++)
    {
        pCh = pLabel->ppLabel[i];
        if (strlen(pCh) != 0)
        {
            WriteConsoleOutputCharacter(gh_std_out, pCh, strlen(pCh),
                                        pLabel->pLoc[i], &ul);
        }
    }

    /*���õ������������ַ���Ԫ��������*/
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {   /*�˶���ѭ�������ַ���Ԫ�Ĳ��*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = gp_top_layer->LayerNo;
        }
    }

    for (i=0; i<pHotArea->num; i++)
    {   /*�˶���ѭ�����������������ַ���Ԫ���������ͺ��������*/
        row = pHotArea->pArea[i].Top;
        for (j=pHotArea->pArea[i].Left; j<=pHotArea->pArea[i].Right; j++)
        {
            gp_scr_att[row*SCR_COL+j] |= (pHotArea->pSort[i] << 6)
                                    | (pHotArea->pTag[i] << 2);
        }
    }

    return;
}

/**
 * ��������: PopOff
 * ��������: �رն��㵯������, �ָ���������ԭ��ۺ��ַ���Ԫԭ����.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void PopOff(void)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j;

    if ((gp_top_layer->next==NULL) || (gp_top_layer->pContent==NULL))
    {   /*ջ�״�ŵ���������Ļ��Ϣ�����ùر�*/
        return;
    }

    nextLayer = gp_top_layer->next;
    /*�ָ�������������ԭ���*/
    size.X = gp_top_layer->rcArea.Right - gp_top_layer->rcArea.Left + 1;
    size.Y = gp_top_layer->rcArea.Bottom - gp_top_layer->rcArea.Top + 1;
    WriteConsoleOutput(gh_std_out, gp_top_layer->pContent, size, pos, &(gp_top_layer->rcArea));
    /*�ָ��ַ���Ԫԭ����*/
    pCh = gp_top_layer->pScrAtt;
    for (i=gp_top_layer->rcArea.Top; i<=gp_top_layer->rcArea.Bottom; i++)
    {
        for (j=gp_top_layer->rcArea.Left; j<=gp_top_layer->rcArea.Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = *pCh;
            pCh++;
        }
    }

    free(gp_top_layer->pContent);    /*�ͷŶ�̬�洢��*/
    free(gp_top_layer->pScrAtt);
    free(gp_top_layer);
    gp_top_layer = nextLayer;
    gi_sel_sub_menu = 0;

    return;
}

/**
 * ��������: DrawBox
 * ��������: ��ָ�����򻭱߿�.
 * �������: pRc �������λ����Ϣ�ĵ�ַ
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void DrawBox(SMALL_RECT *pRc)
{
    char chBox[] = {'+', '-', '|'};  /*�����õ��ַ�*/
    COORD pos = {pRc->Left, pRc->Top};  /*��λ����������Ͻ�*/

    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����Ͻ�*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {   /*��ѭ�����ϱ߿����*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }

    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����Ͻ�*/
    for (pos.Y = pRc->Top+1; pos.Y < pRc->Bottom; pos.Y++)
    {   /*��ѭ�����߿�����ߺ��ұ���*/
        pos.X = pRc->Left;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
        pos.X = pRc->Right;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
    }

    pos.X = pRc->Left;
    pos.Y = pRc->Bottom;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����½�*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {   /*���±߿����*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }

    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����½�*/

    return;
}

/**
 * ��������: TagSubMenu
 * ��������: ��ָ���Ӳ˵�������ѡ�б��.
 * �������: num ѡ�е��Ӳ˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void TagSubMenu(int num)
{
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int width;

    LocSubMenu(gi_sel_menu, &rcPop);  /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    if ((num<1) || (num == gi_sel_sub_menu) || (num>rcPop.Bottom-rcPop.Top-1))
    {   /*����Ӳ˵����Խ�磬������Ӳ˵��ѱ�ѡ�У��򷵻�*/
        return;
    }

    pos.X = rcPop.Left + 2;
    width = rcPop.Right - rcPop.Left - 3;
    if (gi_sel_sub_menu != 0) /*����ȡ��ԭѡ���Ӳ˵����ϵı��*/
    {
        pos.Y = rcPop.Top + gi_sel_sub_menu;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
        FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
        pos.X += 1;
        att |=  FOREGROUND_RED;/*�׵׺���*/
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    /*���ƶ��Ӳ˵�������ѡ�б��*/
    pos.X = rcPop.Left + 2;
    pos.Y = rcPop.Top + num;
    att = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*�ڵװ���*/
    FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
    gi_sel_sub_menu = num;  /*�޸�ѡ���Ӳ˵����*/
    return;
}

/**
 * ��������: LocSubMenu
 * ��������: ���㵯���Ӳ˵��������ϽǺ����½ǵ�λ��.
 * �������: num ѡ�е����˵����
 * �������: rc �������λ����Ϣ�ĵ�ַ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void LocSubMenu(int num, SMALL_RECT *rc)
{
    int i, len, loc = 0;

    rc->Top = 1; /*������ϱ߶��ڵ�2�У��к�Ϊ1*/
    rc->Left = 1;
    for (i=1; i<num; i++)
    {   /*����������߽�λ��, ͬʱ�����һ���Ӳ˵������Ӳ˵��ַ��������е�λ��*/
        rc->Left += strlen(ga_main_menu[i-1]) + 4;
        loc += ga_sub_menu_count[i-1];
    }
    rc->Right = strlen(ga_sub_menu[loc]);/*��ʱ��ŵ�һ���Ӳ˵����ַ�������*/
    for (i=1; i<ga_sub_menu_count[num-1]; i++)
    {   /*������Ӳ˵��ַ��������䳤�ȴ����rc->Right*/
        len = strlen(ga_sub_menu[loc+i]);
        if (rc->Right < len)
        {
            rc->Right = len;
        }
    }
    rc->Right += rc->Left + 3;  /*����������ұ߽�*/
    rc->Bottom = rc->Top + ga_sub_menu_count[num-1] + 1;/*���������±ߵ��к�*/
    if (rc->Right >= SCR_COL)  /*�ұ߽�Խ��Ĵ���*/
    {
        len = rc->Right - SCR_COL + 1;
        rc->Left -= len;
        rc->Right = SCR_COL - 1;
    }
    return;
}

/**
 * ��������: DealInput
 * ��������: �ڵ�������������������, �ȴ�����Ӧ�û�����.
 * �������: pHotArea
 *           piHot ����������ŵĴ�ŵ�ַ, ��ָ�򽹵�������ŵ�ָ��
 * �������: piHot ����굥�������س���ո�ʱ���ص�ǰ�������
 * �� �� ֵ:
 *
 * ����˵��:
*/
int DealInput(HOT_AREA *pHotArea, int *piHot)
{
    INPUT_RECORD inRec;
	CONSOLE_CURSOR_INFO lpCur;
    DWORD res;
    COORD pos = {0, 0};
    int iRet = 0;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc;       /*vkc:���������*/

    SetHotPoint(pHotArea, *piHot);
    while (TRUE)
    {    /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if ((inRec.EventType == MOUSE_EVENT) &&
            (inRec.Event.MouseEvent.dwButtonState
             == FROM_LEFT_1ST_BUTTON_PRESSED))
        {
            pos = inRec.Event.MouseEvent.dwMousePosition;    /* ��ȡ���λ�� */
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3;
            cTag = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2) & 15;
            cSort = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 6) & 3;    /* ��ȡ�ô�ҳ������ */

            if ((cNo == gp_top_layer->LayerNo) && cTag > 0)
            {/* ������������� */
                *piHot = cTag;
                SetHotPoint(pHotArea, *piHot);
                if (cSort == 0)
                {
                    iRet = cTag;
                    return iRet;
                }
            }
			else if(cTag == 0)
            {/* ������ڷ��ȵ����� */
                GetConsoleCursorInfo(gh_std_out, &lpCur);
                lpCur.bVisible = FALSE;
                SetConsoleCursorInfo(gh_std_out, &lpCur);    /* ���ع�� */
            }
        }
		/*����Ǽ�������*/
        else if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown)
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;

            switch(vkc)
            {

            case 37 :/* �����ϡ���� */
            case 38 :
                -- *piHot;
                if(*piHot < 1)
                {
                    *piHot = pHotArea->num;
                }
                SetHotPoint(pHotArea, *piHot);
                GetConsoleCursorInfo(gh_std_out, &lpCur);
                break;
            case 13 :/*�س�����ո��ʾ���µ�ǰ��ť*/
            case 32 :
                if(pHotArea->pSort[*piHot-1] == 0)
                {   //��Ϊ��ť
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;
                    SetConsoleCursorInfo(gh_std_out, &lpCur);    /* ���ع�� */
                    iRet = *piHot;
                    return iRet;
                }
            case 39 :/* �����ҡ��¼� */
            case 40 :
                ++ *piHot;
                if(*piHot > pHotArea->num)
                {
                    *piHot = 1;
                }
                SetHotPoint(pHotArea, *piHot);
                GetConsoleCursorInfo(gh_std_out, &lpCur);
                break;
            case 27 :/* ��ESC�� */
                GetConsoleCursorInfo(gh_std_out, &lpCur);
                lpCur.bVisible = FALSE;
                SetConsoleCursorInfo(gh_std_out, &lpCur);    /* ���ع�� */
                iRet = 27;
                return iRet;
                }
            }
        }
    return iRet;
}

void SetHotPoint(HOT_AREA *pHotArea, int iHot)
{
    CONSOLE_CURSOR_INFO lpCur;
    COORD pos = {0, 0};
    WORD att1, att2;
    int i, width;

    att1 = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*�ڵװ���*/
    att2 = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    for (i=0; i<pHotArea->num; i++)
    {  /*����ť��������Ϊ�׵׺���*/
        pos.X = pHotArea->pArea[i].Left;
        pos.Y = pHotArea->pArea[i].Top;
        width = pHotArea->pArea[i].Right - pHotArea->pArea[i].Left + 1;
        if (pHotArea->pSort[i] == 0)
        {  /*�����ǰ�ť��*/
            FillConsoleOutputAttribute(gh_std_out, att2, width, pos, &ul);
        }
    }

    pos.X = pHotArea->pArea[iHot-1].Left;
    pos.Y = pHotArea->pArea[iHot-1].Top;
    width = pHotArea->pArea[iHot-1].Right - pHotArea->pArea[iHot-1].Left + 1;
    if (pHotArea->pSort[iHot-1] == 0)
    {  /*�����������ǰ�ť��*/
        GetConsoleCursorInfo(gh_std_out, &lpCur);
        lpCur.bVisible = FALSE;
        SetConsoleCursorInfo(gh_std_out, &lpCur);    /*���ع��*/
        FillConsoleOutputAttribute(gh_std_out, att1, width, pos, &ul);
    }
    else if (pHotArea->pSort[iHot-1] == 1)
    {  /*�������������ı�����*/
        SetConsoleCursorPosition(gh_std_out, pos);
        GetConsoleCursorInfo(gh_std_out, &lpCur);
        lpCur.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out, &lpCur);   /* ��������������ʼλ�� */
    }
}

/**
 * ��������: ExeFunction
 * ��������: ִ�������˵��ź��Ӳ˵���ȷ���Ĺ��ܺ���.
 * �������: m ���˵����
 *           s �Ӳ˵����
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE �� FALSE
 *
 * ����˵��: ����ִ�к���ExitSysʱ, �ſ��ܷ���FALSE, ������������Ƿ���TRUE
 */
BOOL ExeFunction(int m, int s)
{
    BOOL bRet = TRUE;
    /*����ָ�����飬����������й��ܺ�������ڵ�ַ*/
    BOOL (*pFunction[ga_sub_menu_count[0]+ga_sub_menu_count[1]+ga_sub_menu_count[2]+ga_sub_menu_count[3]+ga_sub_menu_count[4]])(void);
    int i, loc;

    /*�����ܺ�����ڵ�ַ�����빦�ܺ����������˵��ź��Ӳ˵��Ŷ�Ӧ�±������Ԫ��*/
    pFunction[0] = SaveData;
    pFunction[1] = BackupData;
    pFunction[2] = RestoreData;
    pFunction[3] = ExitSys;
    pFunction[4] = MaintainTypeInfo;
    pFunction[5] = MaintainItemInfo;
    pFunction[6] = MaintainClientInfo;
    pFunction[7] = QueryTypeInfo;
    pFunction[8] = QueryItemInfo;
    pFunction[9] = QueryClientInfo;
    pFunction[10] = StatSort;
    pFunction[11] = StatMoney;
    pFunction[12] = StatClient;
    pFunction[13] = HelpTopic;
    pFunction[14] = NULL;
    pFunction[15] = AboutDorm;
    pFunction[16] = NULL;
    

    for (i=1,loc=0; i<m; i++)  /*�������˵��ź��Ӳ˵��ż����Ӧ�±�*/
    {
        loc += ga_sub_menu_count[i-1];
    }
    loc += s - 1;

    if (pFunction[loc] != NULL)
    {
        bRet = (*pFunction[loc])();  /*�ú���ָ�������ָ��Ĺ��ܺ���*/
    }

    return bRet;
}

BOOL SaveData(void)
{
    BOOL bRet = TRUE;
    char *plabel_name[] = {"���˵���ļ�",
                           "�Ӳ˵�����ݱ���",
                           "ȷ��"
                          };

    ShowModule(plabel_name, 3);
    SaveSysData(gp_head);

    char *plabel_name1[] = {"���ݱ���ɹ���",
                           "ȷ��"
                          };

    ShowModule(plabel_name1, 2);

    return bRet;
}

BOOL BackupData(void)
{
    BOOL bRet = TRUE;
    char *plabel_name[] = {"���˵���ļ�",
                           "�Ӳ˵�����ݱ���",
                           "ȷ��"
                          };

    ShowModule(plabel_name, 3);
    BackupSysData(gp_head, "Backup.dat");
    char *plabel_name1[] = {"���ݱ��ݳɹ���",
                            "ȷ��"
                          };

    ShowModule(plabel_name1, 2);

    ClearScreen();
    ShowMenu();

    return bRet;
}
BOOL RestoreData(void)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"��ǰ���ݽ������ǣ�����ִ�У�", 
	               "  ȷ��            ȡ��"};
    char *pCh1[] = {"���ݻָ��ɹ���",
                     "ȷ��"
                    };    
					 
    int iHot = 1;

    pos.X = strlen(pCh[0]) + 6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    labels.num = 2;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+3, rcPop.Top+2},
                    {rcPop.Left+5, rcPop.Top+5}};
    labels.pLoc = aLoc;

    areas.num = 2;
    SMALL_RECT aArea[] = {{rcPop.Left + 7, rcPop.Top + 5,
                           rcPop.Left + 10, rcPop.Top + 5},
                          {rcPop.Left + 23, rcPop.Top + 5,
                           rcPop.Left + 26, rcPop.Top + 5}};
    char aSort[] = {0, 0};
    char aTag[] = {1, 2};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    if (DealInput(&areas, &iHot) == 1 && iHot == 1)
    {
	    PopOff();
        RestoreSysData(&gp_head, "Backup.dat");

        ShowModule(pCh1, 2);
	}
    else
		PopOff();

    return TRUE;
}

BOOL ExitSys(void)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"ȷ���˳�ϵͳ��", "ȷ��    ȡ��"};
    int iHot = 1;

    pos.X = strlen(pCh[0]) + 6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    labels.num = 2;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+3, rcPop.Top+2},
                    {rcPop.Left+5, rcPop.Top+5}};
    labels.pLoc = aLoc;

    areas.num = 2;
    SMALL_RECT aArea[] = {{rcPop.Left + 5, rcPop.Top + 5,
                           rcPop.Left + 8, rcPop.Top + 5},
                          {rcPop.Left + 13, rcPop.Top + 5,
                           rcPop.Left + 16, rcPop.Top + 5}};
    char aSort[] = {0, 0};
    char aTag[] = {1, 2};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    if (DealInput(&areas, &iHot) == 1 && iHot == 1)
    {
        bRet = FALSE;
        PopOff();
        
    }
    else
    {
        PopOff();
        bRet = TRUE;
    }

    return bRet;
}

BOOL MaintainTypeInfo(void)
{
    char *plabel_name[] = {"����ͷ�������Ϣ",
                       "�޸Ŀͷ�������Ϣ",
                       "ɾ���ͷ�������Ϣ",
                       "����"
                   };
                   
    int x = 0;
    
    while(x != 4)
    {
        x = ShowModule4(plabel_name, 4);
	    switch (x)
	    {
        case 1:
			InsertTypeNode(gp_head);/*����ͷ�������Ϣ*/
			break;
        case 2:
		    ModifTypeNode(gp_head);/*�޸Ŀͷ�������Ϣ*/
			break;
        case 3:
			DelTypeNode(gp_head);/*ɾ���ͷ�������Ϣ*/
			break;
        case 4:
            SaveSysData(gp_head); /*����SaveSysData������������������*/ 
			break;
        }
    }

    return TRUE;
}

BOOL MaintainItemInfo(void)
{
    char *plabel_name[] = {"����ͷ�������Ϣ",
                       "�޸Ŀͷ�������Ϣ",
                       "ɾ���ͷ�������Ϣ",
                       "����"
                   };
                   
    int x = 0;

    while(x != 4)
    {
        x = ShowModule4(plabel_name,4);
        switch (x)
        {
        case 1:
			InsertItemNode(gp_head);/*����ͷ�������Ϣ*/
			break;
		case 2:
		    ModifItemNode(gp_head);/*�޸Ŀͷ�������Ϣ*/
			break;
        case 3:
			DelItemNode(gp_head);/*ɾ���ͷ�������Ϣ*/
			break;
        case 4:
            SaveSysData(gp_head); /*����SaveSysData������������������*/ 
			break;
        }
	}

    return TRUE;
}

BOOL MaintainClientInfo(void)
{
    char *plabel_name[] = {"���������ס��Ϣ",
                       "�޸Ŀ�����ס��Ϣ",
                       "ɾ��������ס��Ϣ",
                       "����"
                   };

    int x = 0;
    
    while(x != 4)
    {
        x = ShowModule4(plabel_name,4);
        switch (x)
        {
        case 1:
			InsertClientNode(gp_head);/*���������ס��Ϣ*/
			break;
		case 2:
		    ModifClientNode(gp_head);/*�޸Ŀ�����ס��Ϣ*/
			break;
        case 3:
			DelClientNode(gp_head);/*ɾ��������ס��Ϣ*/
			break;
        case 4:
            SaveSysData(gp_head); /*����SaveSysData������������������*/ 
			break;
        }
	}

    return TRUE;
}

BOOL QueryTypeInfo(void)
{
    char *plabel_name[] = {"����������ѯ",
                       "   ���ȫ��",
                       "����"
                      };
                      
    int x = 0;
    
    while(x!=3)
    {
        x = ShowModule3(plabel_name,3);
	    switch (x)
	    {
	    	case 1:
	    	    SeekType_id(gp_head);/*����������ѯ*/
	    	    break;
            case 2:
                SeekType(gp_head);/*���ȫ��*/
                break;
            case 3:
                break;/*����*/
	    }
	}

    return TRUE;
}

BOOL QueryItemInfo(void)
{
    char *plabel_name[] = {"  ���ͷ���Ų�ѯ",
                       "���ͷ����������ѯ",
                       "����"
                   };

    int x = 0;
    
    while(x != 3)
    {
        x = ShowModule3(plabel_name,3);
        switch (x)
	    {
		    case 1:
		        SeekNumber(gp_head);/*���ͷ���Ų�ѯ*/
			    break;
            case 2:
                SeekId_Price(gp_head);/*���ͷ����������ѯ*/
                break;
            case 3:
                break;/*����*/

        }
    }

    return TRUE;
}

BOOL QueryClientInfo(void)
{
    char *plabel_name[] = {"���������֤�������",
	                   "�Կ���������ʱ�����",
                       "����"
                       };

    int x = 0;
    
    while(x != 3)
    {
        x = ShowModule3(plabel_name, 3);
        switch (x)
	    {
		    case 1:
		        Seekpeople_id(gp_head);/*���������֤�������*/
			    break;
            case 2:
                SeekName_Time(gp_head);/*�Կ���������ʱ�����*/
                break;
            case 3:
                break;/*����*/

        }
    }

    return TRUE;
}

BOOL StatSort(void)
{
    StatTyperuzhu();/*��ʱ��ͳ����ס��*/
    return TRUE;
}

BOOL StatMoney(void)
{
    int x = 0;
    char *plabel_name[] = {"����ͳ��Ӫҵ��",
                       "����ͳ��Ӫҵ��",
                       "����"
                       };
                       
    while(x != 3)
    {
        x = ShowModule3(plabel_name, 3);
        switch (x)
	    {
	    	case 1:
	    	    StatMonthYear();/*����ͳ��Ӫҵ��*/
		    	break;
		    case 2:
                StatYearruzhu(gp_head);/*����ͳ��Ӫҵ��*/
		    	break;
		    case 3:
			    break;
        }
    }

    return TRUE;
}

BOOL StatClient(void)
{
    int x = 0;
    char *plabel_name[] = {"ͳ�ƿ���ס�����",
                       "����ס���������",
                       "����"
                       };
                       
    while(x != 3)
    {
        x = ShowModule3(plabel_name, 3);/*ѡ��*/
        switch (x)
	    {
	    	case 1:
	    	    StatClient1(gp_head);/*ͳ�ƿ���ס�����*/
		    	break;
		    case 2:
		        StatClient2(gp_head);/*����ס���������*/
		    	break;
		    case 3:
			    break;/*����*/
        }
    }

    return TRUE;
}

BOOL HelpTopic(void)
{
    BOOL bRet = TRUE;
    char *plabel_name[] = {"ʹ�ñ��������ϸ�����ʾ����",
                           "���������ɲ���Ԥ֪�Ĵ���",
                           "ȷ��"
                          };
                          
    ShowModule(plabel_name, 3);

    return bRet;
}

BOOL AboutDorm(void)
{
    char *plabel_name[] = {"���ݳ���ϵͳ����2.0�汾",
                           "  ��Ȩ�����ˣ����鲨",
                           "ȷ��"
                          };

    ShowModule(plabel_name, 3);

    return TRUE;
}

/**
 * ��������: move
 * ��������: ʹ�ù���ƶ���ָ���ĵص� 
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��: 
 */

void move(int x, int y)
{   //ʹ�ù���ƶ���ָ���ĵص� 
	COORD pos;
	
	pos.X = x - 1;
	pos.Y = y - 1;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);

}

/**
 * ��������: InsertTypeNode
 * ��������: ��ʮ�������в���һ��������Ϣ���.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void InsertTypeNode(TYPE_NODE* hd)
{
    TYPE_NODE *p, *pt = hd;
    
    char *plabel_name1[] = {"�˿ͷ�����ѱ�ʹ�á�",
                              "���������Ϣʧ�ܡ�",
                              "ȷ��"
                             };
    char *plabel_name2[] = {"���������Ϣ�ɹ���",
                              "ȷ��"
                             };
    move(25, 4);
    printf("����������Ҫ��ӵĿͷ��������Ϣ��");
    p = (TYPE_NODE*)malloc(sizeof(TYPE_NODE));/*�½�������Ϣ���*/
    move(29, 6);
    printf("�ͷ����\n\n\t\t\t    ��ס������\n\n\t\t\t    �ͷ�������\n\n\t\t\t    δס������");
    move(39, 6);
    scanf("%c", &p->type_id);
    move(39, 8);
    scanf("%d", &p->people_max);
    move(39, 10);
    scanf("%d",&p->room_number);
    move(39, 12);
    scanf("%d",&p->room_no);
    getchar();

    while(pt != NULL)  /*����������Ϣ������鷿������Ƿ��ѱ�ʹ��*/
    {
        if(p->type_id == pt->type_id)
        {
            
            ClearScreen();
            ShowMenu();
            ShowModule(plabel_name1, 3);
            ClearScreen();
            ShowMenu();
            free(p);

            return;
        }
        pt = pt->next;
    }

    p->inext = NULL;
    p->next = hd;
    gp_head = p;/*���������û���ظ������½��Ľ����Ϊ�µ�������ͷ���*/
    
    ClearScreen();
    ShowMenu();
    ShowModule(plabel_name2, 2);
    ClearScreen();
    ShowMenu();

    return;
}

/**
 * ��������: ModifTypeNode
 * ��������: ��ʮ���������޸�ָ���ķ�����Ϣ���.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��: ���ݷ��ݷ�����ҷ�����Ϣ�������޸�
 */
void ModifTypeNode(TYPE_NODE *hd)
{
    TYPE_NODE *pt = hd, *p1;
    ITEM_NODE *pi;
    char type_id;              /**< �ͷ����*/
    int people_max;            /**< �����ס����*/
    int room_number;           /**< �ͷ�����*/
    int room_no;               /**< �ͷ�δס����*/ 
    int x = 0;
    
    char *plabel_name1[] = {"�޸ķ�����Ϣʧ�ܡ�",
                    "ȷ��"
                    };
    char *plabel_name2[] = {"�޸ķ�����Ϣ�ɹ���",
                    "ȷ��"
                    };

    move(23, 2);
    printf("��������Ҫ�޸ĵķ����������ƣ�");
    scanf("%c", &type_id);
    getchar();

    while(pt!=NULL)/*���ҷ�����Ϣ���*/
    {
        if(type_id == pt->type_id) break;
        pt=pt->next;
    }

    if(pt == NULL)/*���޷��������Ľ�㣬��ʾ�޸�ʧ��*/
    {
        move(23, 4);
        printf("�˷��಻����");
        getch();
        ShowModule(plabel_name1, 2);
        ClearScreen();
        ShowMenu();
        
        return;
    }
    
    printf("\n\n\t\t\t  ��Ҫ�޸ĵĿͷ�������Ϣ����");
    printf("\n\n\t\t\t  �ͷ����%c",pt->type_id);
	printf("\n\t\t\t  �����ס������%d",pt->people_max);
	printf("\n\t\t\t  �ͷ�������%d",pt->room_number);
	printf("\n\t\t\t  �ͷ�δ��ס������%d",pt->room_no); 
    printf("\n\t\t\t  �����������...");
    getch();
    printf("\n\n\t\t\t  �������µĿͷ����:");
    scanf("%c", &type_id);
    getchar();
    p1 = hd;
    while(p1 != NULL)
    {
        if(p1->type_id == type_id)break;
        p1 =p1->next;
    }    /*����µĿͷ�����Ƿ��ѱ�ʹ��*/

    if (p1 != NULL)
    {
    printf("\n\t\t\t  �ÿͷ�����ѱ�ʹ��");
    getch();
    ClearScreen();
    ShowMenu();
    
    ShowModule(plabel_name1, 2);
    return; 
    }
    else
    { 
		/*�޸ķ�����Ϣ��Ӧ�Ļ�����Ϣ�еĿͷ����*/
        pt->type_id = type_id;
        for(pi=pt->inext; pi != NULL; pi->type_id = type_id, pi = pi->next);
                    
    }

    printf("\n\t\t\t  �������µ������ס������");
    scanf("%d",&people_max);
    getchar();
    pt->people_max=people_max;
    printf("\n\t\t\t  �������µĿͷ�������");
    scanf("%d",&room_number);
    getchar();
    pt->room_number=room_number; 
    printf("\n\t\t\t  �������µĿͷ�δס������");
    scanf("%d",&room_no);
    getchar();
    pt->room_no=room_no;
    ClearScreen();
    ShowMenu(); 
    ShowModule(plabel_name2, 2);
    
    return;    
}

/**
 * ��������: DelTypeNode
 * ��������: ��ʮ��������ɾ��ָ���ķ�����Ϣ���.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��: ���ݷ��ݷ���ɾ����Ϣ
 */
void DelTypeNode(TYPE_NODE *hd)
{
    TYPE_NODE *pt = hd, *p = NULL;
    ITEM_NODE *pi1, *pi2;
    CLIENT_NODE *pc1, *pc2;
    char type_idno;
    
    char *plabel_name1[] = {"�˷��಻���ڡ�",
                          "ɾ��������Ϣʧ�ܡ�",
                          "ȷ��"
                         };
    char *plabel_name2[] = {"ɾ��������Ϣ�ɹ���",
                          "ȷ��"
                         };

    printf("\n\n\t\t��������Ҫɾ���ķ��ݷ�������ƣ�");
    scanf("%c",&type_idno);
    getchar();
    
    while(pt!=NULL && (type_idno != (pt->type_id)))
    {
    	p = pt;
		pt=pt->next;
	}                /*���ݷ������Ʋ��ҷ�����Ϣ���pt,��ʹp��Ϊpt��ǰ��*/
     
    if(pt == NULL)/*���޷��������Ľ�㣬��ʾɾ��ʧ��*/
    {
        ShowModule(plabel_name1, 3);
        ClearScreen();
        ShowMenu();

        return;
    }
    else if(pt == gp_head)      /*�ж��Ƿ�Ϊͷ���*/
    {
        gp_head = pt->next;/*�޸�ͷ���*/
    }
    else
    {
        p->next = pt->next;/*ʹpt��ǰ��ָ��pt�ĺ��*/
    }

    pi1 = pt->inext;
    while(pi1 != NULL)
    {/*�ͷ�pt��Ӧ�Ļ�����������Ϣ�Ĵ洢�ռ�*/
        pi2 = pi1->next;
        pc1 = pi1->snext;
        while(pc1 != NULL)
        {
            pc2 = pc1->next;
            free(pc1);
            pc1 = pc2;
        }

        free(pi1);
        pi1 = pi2;
    }

    free(pt);/*�ͷ�pt�Ĵ洢�ռ�*/

    ShowModule(plabel_name2,2);
    ClearScreen();
    ShowMenu();

    return;
}




/**
 * ��������: InsertItemNode
 * ��������: ��ʮ�������в���һ��������Ϣ���.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��  
 */
void InsertItemNode(TYPE_NODE* hd)
{
    TYPE_NODE *pt = hd, *ptype = hd;
    ITEM_NODE *pi, *p;
    char type_name;
    
    char *plabel_name1[] = {"����𲻴��ڡ�",
                           "���������Ϣʧ�ܡ�",
                           "ȷ��"
                          };
    char *plabel_name2[] = {"�˿ͷ�����ѱ�ʹ��",
                           "���������Ϣʧ��",
                           "ȷ��"
                          };
    move(30, 4);
    printf("����������Ҫ��ӵĿͷ�������Ϣ��");
    p = (ITEM_NODE*)malloc(sizeof(ITEM_NODE));
    move(30, 6);
    printf("�ͷ����: ");
    scanf("%c",&type_name);
    getchar();
    for(pt = hd; pt != NULL && type_name != (pt->type_id); pt = pt->next);  /*���һ�����Ϣ��Ӧ�ķ�����pt*/
    if(pt == NULL)
    {
        ShowModule(plabel_name1, 3);
        ClearScreen();
        ShowMenu();

        return;
    }
    p->type_id = pt->type_id;
    move(30, 8);
    pi=pt->inext;
    printf("�ͷ���ţ���λ���֣�: ");
    scanf("%s", p->number);
    getchar();
    while(ptype != NULL)
    {
        pi = ptype->inext;
        while(pi != NULL)
        {  /*�����������ͷ���ŷ��ظ�*/
            if(strcmp(p->number, pi->number) == 0)
            {
                ClearScreen();
                ShowMenu();
                free(p);
                ShowModule(plabel_name2, 3);
                return;
            }
            pi = pi->next;

    }
     ptype = ptype->next;
    }
    move(30, 9);
    printf("�绰���루��λ���֣���");
    scanf("%s", p->phone);
    getchar(); 
    move(30, 10);
    printf("�����");
	scanf("%f",&p->area);
	getchar();
	move(30, 11);
	printf("ÿ�����");
	scanf("%f",&p->sale);
	getchar();
	move(30, 12);
	printf("�Ƿ�����̨'1'��ʾ����̨'0'��ʾû����̨��");
	scanf("%c",&p->balcony);
	getchar();
	move(30, 13);
	printf("�Ƿ��п���'y'���п�����ס'n'δס:");
	scanf("%c",&p->human);
    getchar();
    p->snext = NULL;
    p->next = pt->inext;
    pt->inext = p;
    pt = hd;
    move(30, 14);
    printf("�����������...\n");
    getch();

    char *plabel_name3[] = {"���������Ϣ�ɹ�",
                       "ȷ��"
                      };

    ClearScreen();
    ShowMenu();
    ShowModule(plabel_name3, 2);

    return;
}



/**
 * ��������: ModifItemNode
 * ��������: ��ʮ���������޸�ָ���Ļ�����Ϣ���.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��: ���ݿͷ���Ų��һ�����Ϣ�������޸�
 */
void ModifItemNode(TYPE_NODE *hd)
{
    TYPE_NODE *pt = hd, *pt2;
    ITEM_NODE *pi = NULL, *pitem = NULL, *pi2;
    CLIENT_NODE *pc;
    int flag = 0, find = 0;
    char name[4], type_id;
    
    char *plabel_name1[] = {"�޸Ļ�����Ϣʧ�ܡ�",
                           "ȷ��"
                           };
    char *plabel_name2[] = {"�޸Ļ�����Ϣ�ɹ���",
                            "ȷ��"
                            };

    move(17, 2);
    printf("����������Ҫ�޸ĵĻ�����Ϣ�Ŀͷ���ţ�");
    scanf("%s", name);
    getchar();
    while(pt != NULL)
    {
        pi = pt->inext;
        while(pi != NULL)
        {  /*���ݿͷ���Ų��һ�����Ϣ���pi,�Լ�pi��ǰ��pi2*/
            if(strcmp(name, pi->number) == 0)
            {
                flag = 1;
                break;
            }
            pitem = pi;
            pi = pi->next;
        }

        if(flag == 1)
        {
            break;
        }

        pt = pt->next;
    }

    if (pi == NULL)
    {
        move(30, 4);
        printf("�˿ͷ���Ų����ڣ�");
        ShowModule(plabel_name1, 2);
        ClearScreen();
        ShowMenu();

        return;
    }
    
	printf("\n\t\t���޸ĵ绰���루��λ���֣���");
	scanf("%s",&pi->phone) ;
	getchar();
    printf("\n\t\t���޸Ŀͷ����('D''S''T''F')��");
	scanf("%c",&type_id) ;
	getchar();
	for(pt2 = hd; pt2 != NULL && type_id != pt2->type_id; pt2 = pt2->next);/*��������Ӧ�ķ�����Ϣ���*/
    pi->type_id = type_id;

    if(pt->inext == pi) 
    {
        pt->inext = pi->next;
    }
    else
    {
        pitem->next = pi->next;
    }

    pi->next = pt2->inext;
    pt2->inext = pi;
    pt = pt2;
    printf("\n\t\t���޸Ŀͷ������");/*�޸Ŀͷ����*/
	scanf("%f",&pi->area) ;
	getchar();
    printf("\n\t\t���޸Ŀͷ����");/*�޸Ŀͷ����*/
	scanf("%f",&pi->sale) ;
	getchar();
	pc = pi->snext;
	while(pc != NULL)
	{
		pc->should = (pi->sale)*pc->month;
		pc = pc->next;
    } 
    printf("\n\t\t��ȷ���Ƿ�����̨��");/*ȷ���Ƿ�����̨*/
	scanf("%c",&pi->balcony) ;
	getchar();
    printf("\n\t\t��ȷ���Ƿ�������ס��");/*ȷ���Ƿ�������ס*/
	scanf("%c",&pi->human) ;
	getchar();
    ClearScreen();
    ShowMenu();
    ShowModule(plabel_name2, 2);
    
    return;            
}

/**
 * ��������: DelItemNode
 * ��������: ��ʮ��������ɾ��ָ���Ļ�����Ϣ���.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��: ���ݿͷ����ɾ����Ϣ
 */
void DelItemNode(TYPE_NODE *hd)
{
    TYPE_NODE *pt = hd;
    ITEM_NODE *pi, *pi2 = NULL;
    CLIENT_NODE *pc;
    char name[4];
    
    char *plabel_name1[] = {"ɾ��������Ϣ�ɹ�",
                         "ȷ��"
                         };
    char *plabel_name2[] = {"�˿ͷ���Ų�����",
                       "ɾ��������Ϣʧ��",
                       "ȷ��"
                      };
                      
    move(20, 4);
    printf("����������Ҫɾ���Ļ�����Ϣ�Ŀͷ���ţ�");
    scanf("%s", name);
    getchar();

    while(pt != NULL)
    {   /*������Ʒ���Ʋ�����Ʒ������Ϣ���pi,�Լ�pi��ǰ��pi2*/
        pi = pt->inext;
        while(pi != NULL)
        {
            if(strcmp(name, pi->number) == 0)
            {
                if(pt->inext == pi)      /*�ж��Ƿ�Ϊͷ���*/
                {
                    pt->inext = pi->next;
                }
                else
                {
                    pi2->next = pi->next;
                }

                pc = pi->snext;    /*�ͷŴ���Ʒ��Ӧ��������Ϣ���Ĵ洢�ռ�*/
                while(pc != NULL)
                {
                    free(pc);
                    pc = pc->next;
                }

                free(pi);

                ShowModule(plabel_name1,2);
                ClearScreen();
                ShowMenu();

                return;
            }
            pi2 = pi;
            pi = pi->next;
        }
        pt = pt->next;
    }
   /*��û�з��������Ļ�����Ϣ��㣬��ʾɾ��ʧ��*/

    ShowModule(plabel_name2, 3);
    ClearScreen();
    ShowMenu();
    
    return;
}


/**
 * ��������: InsertClientNode
 * ��������: ��ʮ�������в���һ��������ס��Ϣ���.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void InsertClientNode(TYPE_NODE* hd)
{
    TYPE_NODE *pt = hd;
    ITEM_NODE *pi;
    CLIENT_NODE  *p;
    char number[4];
    int find=0;
    
    char *plabel_name2[] = {"���������ס��Ϣ�ɹ���",
                           "ȷ��"
                           };
    char *plabel_name1[] = {"�ÿͷ������ڡ�",
                           "���������ס��Ϣʧ�ܡ�",
                           "ȷ��"
                           };
                           
    move(28, 4);
    printf("��������Ҫ��ӵĿ�����Ϣ��");/*���ݿ�����Ϣ���Ҷ�Ӧ�Ļ�����Ϣ���*/
    p = (CLIENT_NODE*)malloc(sizeof(CLIENT_NODE));
    move(28, 6);
    printf("������ס�ͷ����: ");/*������ס�ͷ���Ų��Ҷ�Ӧ�Ļ�����Ϣ���*/
    scanf("%s", number);
    getchar();
    find = 0;
    for(pi = pt->inext; pt != NULL && find == 0; pt = pt->next)
        for(pi=pt->inext; pi != NULL; pi = pi->next)
        {
            if(strcmp(pi->number, number) == 0)
            {
                find = 1;
                break;
            }
        }/*���ݿͷ���Ų��Ҷ�Ӧ�Ļ�����Ϣ���*/

    if(find == 0)
    {
        ShowModule(plabel_name1, 3);
        ClearScreen();
        ShowMenu();

        return;
    }
    strcpy(p->number, pi->number);

    printf("\n\t\t\t   �������֤�ţ�18λ����");/*�������֤�ţ�18λ��*/
	scanf("%s",p->people_id);
	getchar();
	printf("\n\t\t\t   ����������");/*��������*/
	scanf("%s",p->people_name);
	getchar();
	printf("\n\t\t\t   ��סʱ�䣨��2015/03/05-13:00����");/*��סʱ�䣨��2015/03/05-13:00��*/
	scanf("%s",p->time_in);
	getchar(); 
	printf("\n\t\t\t   �˷�ʱ�䣨��2015/03/06-11:00����");/*�˷�ʱ�䣨��2015/03/06-11:00��*/
	scanf("%s",p->time_out);
	getchar(); 
	printf("\n\t\t\t   ��ס������");/*��ס����*/
	scanf("%f",&p->month);
	getchar();
	printf("\n\t\t\t   Ѻ��");/*Ѻ��*/
	scanf("%f",&p->money);
	getchar();
	printf("\n\t\t\t   Ӧ�ɽ�");/* Ӧ�ɽ��*/
	scanf("%f",&p->should);
	getchar();
	printf("\n\t\t\t   ʵ�ɽ�");/* ʵ�ɽ��*/
	scanf("%f",&p->reality);
	getchar();
    p->next = pi->snext;
    pi->snext = p;
    ClearScreen();
    ShowMenu();
    ShowModule(plabel_name2, 2);
    
    return;
}





/**
 * ��������: ModifClientNode
 * ��������: ��ʮ���������޸�ָ���Ļ�����Ϣ���.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��: ���ݿͷ�����Լ������������Ҳ��޸���Ϣ
 */
void ModifClientNode(TYPE_NODE *hd)
{
    TYPE_NODE *pt = hd;
    ITEM_NODE *pi, *pi2;
    CLIENT_NODE *pc = NULL, *pclient = NULL;
    int sale_id, item_id;
    int flag = 0, find = 0;
    char number[4];
    char name[20];
    
    char *plabel_name1[] = {"�޸Ŀ�����ס��Ϣ�ɹ���",
                       "ȷ��"
                      };
    char *plabel_name2[] = {"�޸Ŀ�����ס��Ϣʧ�ܡ�",
                       "ȷ��"
                      };
                      
    move(24, 3);
    printf("��������Ҫ�޸ĵĿ�����ס��Ϣ�Ŀͷ���ţ�");/*���ݿ�����ס��Ϣ�Ŀͷ���Ų��ҿ�����ס��Ϣ�ڵ�*/
    scanf("%s", number);
    getchar();
    
    move(24, 5);
    printf("����������Ҫ�޸ĵĿ�����ס��Ϣ��������");/*���ݿ�����ס��Ϣ���������ҿ�����ס��Ϣ�ڵ�*/
	scanf("%s",name);
	getchar();
	
    while(pt != NULL && flag != 1)
    {   /*���ݿͷ���Ų��ҿ�����ס��Ϣ�ڵ�*/
        pi = pt->inext;
        while(pi != NULL && flag != 1)
        {
            pc = pi->snext;
            while(pc != NULL)
            {
                if(strcmp(number,pc->number) == 0  && strcmp(name , pc->people_name) == 0)
                {
                    flag = 1;
                    break;
                }
                pclient = pc;
                pc = pc->next;
            }

            if(flag != 1)
            {
                pi = pi->next;
            }
        }
        pt = pt->next;
    }

    if(flag == 0)
    {
        move(24,7);
        printf("�˿��˲�����");
        move(24,9);
        printf("�����������...");
        getch();
        ShowModule(plabel_name2, 2);
        ClearScreen();
        ShowMenu();
        return;
    }

        move(24,7);
        printf("���޸Ŀ������֤�ţ�18λ����"); /*���޸Ŀ������֤�ţ�18λ��*/
        scanf("%s",pc->people_id);
        getchar(); 
        move(24,8);
        printf("���޸Ŀ���������");  /*���޸Ŀ�������*/
        scanf("%s",pc->people_name);
        getchar(); 
		move(24,9);
		printf("���޸Ŀ�����סʱ�䣺");  /*���޸Ŀ�����סʱ��*/
		scanf("%s",pc->time_in); 
		getchar(); 
		move(24,10);
		printf("���޸Ŀ����˷�ʱ�䣺");  /*���޸Ŀ����˷�ʱ��*/
		scanf("%s",pc->time_out); 
		getchar(); 
		move(24,11);
		printf("���޸Ŀ�����ס������");  /*���޸Ŀ�����ס����*/
		scanf("%f",&pc->month); 
		getchar();  
		move(24,12);
		printf("���޸�Ѻ��");  /*���ݿͷ���Ų��ҿ�����ס��Ϣ�ڵ�*/
		scanf("%f",&pc->money); 
		getchar(); 
		move(24,13);
		printf("���޸Ŀ���Ӧ�ɽ�");  /*�޸Ŀ���Ӧ�ɽ��*/
		scanf("%f",&pc->should); 
		getchar(); 
		move(24,14);
		printf("���޸Ŀ���ʵ�ɽ�");  /*���޸Ŀ���ʵ�ɽ��*/
		scanf("%f",&pc->reality); 
		getchar(); 
		
		ClearScreen();
        ShowMenu();
		ShowModule(plabel_name1, 2);
		
        return;          
}

/**
 * ��������: DelClientNode
 * ��������: ��ʮ��������ɾ��ָ���Ļ�����Ϣ���.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��: ���ݿͷ�����Լ������������Ҳ�ɾ����Ϣ
 */
void DelClientNode(TYPE_NODE *hd)
{
    TYPE_NODE *pt = hd;
    ITEM_NODE *pi;
    CLIENT_NODE *pc, *pc2;
    char id[4];
    char name[20];
    int flag = 0;
    
    char *plabel_name1[] = {"ɾ��������ס��Ϣ�ɹ�",
                       "ȷ��"
                      };
    char *plabel_name2[] = {"�˿ͷ�����˲�����",
                       "ɾ��������ס��Ϣʧ��",
                       "ȷ��"
                      };

    move(20, 4);
    printf("����������Ҫɾ���Ŀ�����ס��Ϣ�Ŀͷ���ţ�");/*������ס��Ϣ�Ŀͷ���ţ����ҿ�����ס��Ϣ���*/
    scanf("%s", id);
    getchar();
    move(20, 6);
    printf("����������Ҫɾ���Ŀ�����ס��Ϣ��������");/*������ס��Ϣ�����������ҿ�����ס��Ϣ���*/
	scanf("%s",name) ;
	getchar();

    while(pt != NULL && flag != 1)
    { /*�����������ҿ�����ס��Ϣ���*/
        pi = pt->inext;
        while(pi != NULL && flag != 1)
        {
            pc = pi->snext;
            while(pc != NULL && flag != 1)
            {
                if(strcmp(id,pc->number) == 0 && strcmp(name , pc->people_name) == 0 )
                {
                    flag = 1;
                    
                    if(pi->snext == pc)
                    {
                        pi->snext = pc->next;  /*�ж��Ƿ�Ϊͷ���*/
                    }
                    else
                    {
                        pc2->next = pc->next;
                    }
                    free(pc);
                    ShowModule(plabel_name1, 2);
                    ClearScreen();
                    ShowMenu();
                    return;
                }
                pc2 = pc;
                pc = pc->next;
            }pi = pi->next;
        }pt = pt->next;
    }

    ShowModule(plabel_name2, 3);
    ClearScreen();
    ShowMenu();
    
    return;
}



/**
 * ��������: SeekType_id
 * ��������: ��������Ŀͷ������ҷ�����Ϣ
 * �������: head ����ͷָ��
 * �������: ���������Ŀͷ�������Ϣ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void SeekType_id(TYPE_NODE *head)
{
    char type_Id;
    TYPE_NODE *find;
    
    move(25, 5);
    printf("������ͷ����:");/*������ͷ����*/
    scanf("%c", &type_Id);
    getchar();
    
    for(find = gp_head;find != NULL;find = find->next)
        if(find->type_id == type_Id)	
        {   /*����������Ϣ�������ݷ��������ҷ�����Ϣ���*/
            break;
		}   
    
    if(find != NULL)
    {
    	move(25,6);
    	printf("�����ס����:%d",find->people_max);/*�����ס����*/
		move(25,7);
		printf("�ͷ�����:%d",find->room_number);/*�ͷ�����*/
		move(25,8);
		printf("�ͷ�δס����:%d",find->room_no);/*�ͷ�δס����*/
    }
    else 
	{
		move(25,6);
		printf("�˷�����벻����");/*�˷�����벻����*/
	}
	
    move(25,9);
    printf("�����������...");
    getch();
    ClearScreen();
    ShowMenu();
    
    return;
}

/**
 * ��������: SeekType
 * ��������: ����������Ϣ����������з�����Ϣ
 * �������: head ����ͷָ��
 * �������: ���еĿͷ�������Ϣ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void SeekType(TYPE_NODE *head)
{
    TYPE_NODE *pt = head;
    int node_num = 0;
    
    printf("\n\t\t\t\t�ͷ����\t�����ס\t�ͷ�����\tδס����\n");
    while(pt != NULL)
    {
        if(node_num == 15)/*ÿҳ���15��*/
        {
            printf("\n\t\t\t\t��������л�����һҳ...");
            getch();
            ClearScreen();
            ShowMenu();
            printf("\n\t\t\t\t�ͷ����\t�����ס\t�ͷ�����\tδס����\n");
            node_num = 0;
        }
        printf("\t\t\t\t%-8c\t%-8d\t%-8d\t%-8d\n", pt->type_id, pt->people_max,pt->room_number,pt->room_no);
        pt = pt->next;
        node_num ++;
    }

    printf("\n\t\t\t\t�����������...");
    getch();
    ClearScreen();
    ShowMenu();
    
    return;
}
/**
 * ��������: SeekNumber
 * ��������: ���ͷ���Ų�ѯ�ͷ�������Ϣ 
 * �������: head ����ͷָ��
 * �������: �ñ���µĿͷ�������Ϣ 
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void SeekNumber(TYPE_NODE *head)
{
    TYPE_NODE *pt;
    ITEM_NODE *pi;
    char number[4];
    int flag = 0;
    
    pt = head;
    move(30, 2);
    printf("��������Ҫ��ѯ�Ŀͷ���ţ�"); /*������Ҫ��ѯ�Ŀͷ����*/
    scanf("%s", number);
    getchar();
    while(pt != NULL)
    {
        pi = pt->inext;

        while(pi != NULL)
        {
            if(strstr(pi->number, number))   /*���һ�����Ϣ���*/
            {
            	move(30, 4);
            	printf("�ÿͷ��绰���룺%s",pi->phone); /*�ͷ��绰����*/
            	move(30, 5);
            	printf("�ÿͷ����ͣ�%c",pi->type_id); /*�ͷ�����*/
            	move(30, 6);
            	printf("�ÿͷ������%f",pi->area); /*�ͷ����*/
            	move(30, 7);
            	printf("�ÿͷ�ÿ�����%f",pi->sale); /*�ͷ�ÿ�����*/
            	move(30, 8);
            	printf("�ÿͷ��Ƿ�����̨��%c",pi->balcony); /*�ͷ��Ƿ�����̨*/
            	move(30, 9);
            	printf("�ÿͷ��Ƿ��п��ˣ�%c",pi->human); /*�ͷ��Ƿ��п���*/
                flag = 1;  
            }pi = pi->next;
        }pt = pt->next;
    }   /*��������������������Ļ�����Ϣ*/


    if(flag == 0)
    {
    	move(30,4);
        printf("�鲻���ÿͷ����\n");
    }
    move(30,10);

    printf("�����������...");
    getch();
    ClearScreen();
	ShowMenu();
	
	return;
}

 /**
 * ��������: SeekId_Price
 * ��������: ������ÿ�������ҿͷ�������Ϣ
 * �������: hd ����ͷָ��
 * �������: �������������Ŀͷ�������Ϣ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void SeekId_Price(TYPE_NODE *head)
{
    TYPE_NODE *pt = head;
    ITEM_NODE *pi;
    char id;
    float sale_a;
    int flag = 0;

    move(30, 2);
    printf("������ͷ����"); /*����ͷ����*/
    scanf("%c", &id);
    getchar();
    move(30, 4);
    printf("��������Ҫ��ѯ��ÿ�����"); /*������Ҫ��ѯ��ÿ�����*/
    scanf("%f",&sale_a);
    getchar();
    printf("\n\t�ͷ����\t�绰����\t�ͷ����\t�ͷ����\tÿ�����\t�Ƿ�����̨\t�Ƿ�������ס\n");
    for(; pt!=NULL && pt->type_id!=id; pt = pt->next);

    if(pt == NULL)
    {
        printf("\n\t\t\t     �˿ͷ���𲻴���\n");
        printf("\n\t\t\t     �����������...");
        getch();
        ClearScreen();
        ShowMenu();
        return;
    }
    else
    {
        pi = pt->inext;
        while(pi != NULL)
        {
            if(pi->sale ==  sale_a )
            {  /*���Ҵ��ڼ۸������ڵĻ�����Ϣ��㣨�����˵㣩*/
                printf("\t%-8s\t%-8s\t%-8c\t%-8.0f\t%-8.0f\t%-10c\t%-12c\t\n",pi->number,pi->phone,pi->type_id,pi->area,pi->sale,pi->balcony,pi->human);
                flag = 1;  
            }pi = pi->next;
        }
    }

    if(flag == 0)
    {
        printf("\n\t\t\t     �޷��������Ŀͷ�\n");
    }

    printf("\n\t\t\t     �����������...");
    getch();
    ClearScreen();
	ShowMenu();
	
	return;
}


 /**
 * ��������: Seekpeople_id
 * ��������: ���������֤���������ҿͷ��ⷿ��Ϣ
 * �������: head ����ͷָ��
 * �������: �������������Ŀ����ⷿ��Ϣ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void Seekpeople_id(TYPE_NODE *head)
{
    TYPE_NODE *pt;
    ITEM_NODE *pi;
    CLIENT_NODE *pc;
    char people_ID[20];
    int flag = 0, node_num = 0;
    
    pt = head;
    move(0, 2);
    printf("\t\t\t\t��������˵����֤���루18λ���� ");/*������˵����֤����*/
    scanf("%s", people_ID);
    getchar();
    while(pt != NULL)
    {   /*���������������������������Ϣ*/
        pi = pt->inext;
        while(pi != NULL)
        {
            pc = pi->snext;
            while(pc != NULL)
            {
                if(strcmp(people_ID, pc->people_id) == 0)
                {
                    if(node_num ==1)/*ÿҳ���1��*/
                    {
                        printf("\n\t\t\t\t��������л�����һҳ...");
                        getch();
                        ClearScreen();
                        ShowMenu();
                        node_num = 0;
                    }
                    flag = 1;
                    printf("\n\n\n\n\n") ;
                    printf("\t\t\t\t�������֤���룺%s\n",pc->people_id);/*�������֤����*/
                    printf("\t\t\t\t����������%s\n",pc->people_name);/*��������*/
                    printf("\t\t\t\t��ס�ͷ���ţ�%s\n",pc->number);/*��ס�ͷ����*/
                    printf("\t\t\t\t������סʱ�䣺%s\n",pc->time_in);/*������סʱ��*/
                    printf("\t\t\t\t�����˷�ʱ�䣺%s\n",pc->time_out);/*�����˷�ʱ��*/
                    printf("\t\t\t\t������ס������%f\n",pc->month);/*������ס����*/
                    printf("\t\t\t\tѺ��%f\n",pc->money);/*Ѻ��*/
                    printf("\t\t\t\t����Ӧ�ɽ�%f\n",pc->should);/*����Ӧ�ɽ��*/
                    printf("\t\t\t\t����ʵ�ɽ�%f\n",pc->reality);/*����ʵ�ɽ��*/
                    node_num++;
                }pc = pc->next;
            }pi = pi->next;
        }pt = pt->next;
    }

    if(flag == 0)
    {
        printf("\n\t\t\t�޷��������Ŀ����ⷿ��Ϣ\n");
    }

    printf("\n\t\t\t\t�����������...");
    getch();
    ClearScreen();
    ShowMenu();
    
    return;
}

 /**
 * ��������: SeekName_Time
 * ��������: �����˵��ջ�������סʱ�䷶Χ���ҿ����ⷿ��Ϣ
 * �������: head ����ͷָ��
 * �������: �������������Ŀ�����ס��Ϣ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void SeekName_Time(TYPE_NODE *head)
{
    TYPE_NODE *pt;
    ITEM_NODE *pi;
    CLIENT_NODE *pc;
    char name[20];
    char time[18];
    char time_a[18]="0";
    int f = 0, node_num = 0;
    
    pt = head;
    move(30, 2);
    printf("�������������: ");/*�����˵��ջ�������סʱ�䷶Χ���ҿ����ⷿ��Ϣ*/ 
    scanf("%s", name);
    getchar(); 
    move(30, 4);
    printf("��������Ҫ��ѯ��ʱ�䣨2015/03/05-13:00��: ");/*�����˵��ջ�������סʱ�䷶Χ���ҿ����ⷿ��Ϣ*/ 
    scanf("%s", time);
    getchar();

    while(pt != NULL)
    {   /*���������������������������Ϣ*/
        pi = pt->inext;
        while(pi != NULL)
        {
            pc = pi->snext;
            while(pc != NULL)
            {
            	if(strcmp(pc->time_out , time_a) == 0 )
            	{
            		strcpy(pc->time_out,time);
				}
            
            	
            	
                if(strcmp(name, pc->people_name) == 0 && strcmp(time, pc->time_in) >= 0 && strcmp(time, pc->time_out) <= 0)
                {
                    if(node_num == 1)/*ÿҳ���1��*/
                    {
                        printf("\n\t\t\t\t\t��������л�����һҳ...");
                        getch();
                        ClearScreen();
                        ShowMenu();
                        node_num = 0;
                    }
                    f = 1;
                    printf("\t\t\t\t\t�������֤���룺%s\n",pc->people_id);/*�������֤����*/
                    printf("\t\t\t\t\t����������%s\n",pc->people_name);/*��������*/
                    printf("\t\t\t\t\t��ס�ͷ���ţ�%s\n",pc->number);/*��ס�ͷ����*/
                    printf("\t\t\t\t\t������סʱ�䣺%s\n",pc->time_in);/*������סʱ��*/
                    printf("\t\t\t\t\t�����˷�ʱ�䣺%s\n",pc->time_out);/*�����˷�ʱ��*/
                    printf("\t\t\t\t\t������ס������%f\n",pc->month);/*������ס����*/
                    printf("\t\t\t\t\tѺ��%f\n",pc->money);/*Ѻ��*/
                    printf("\t\t\t\t\t����Ӧ�ɽ�%f\n",pc->should);/*����Ӧ�ɽ��*/
                    printf("\t\t\t\t\t����ʵ�ɽ�%f\n",pc->reality);/*����ʵ�ɽ��*/
                    node_num++;
                }
                pc = pc->next;
            }
            pi = pi->next;
        }
        pt = pt->next;
    }

    if(f == 0)
    {
    	move(30, 6) ;
        printf("�޷��������Ŀ�����ס��Ϣ\n");
    }
    printf("\n\t\t\t\t\t�����������...");
    getch();
    ClearScreen();
    ShowMenu();
    
    return;
}


 /**
 * ��������: StatTyperuzhu
 * ��������: �����ͳ����������������ס���
 * �������: ��
 * �������: �������������Ŀ�����ס��Ϣ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void StatTyperuzhu(void)
{	
	TYPE_NODE *pt;
    ITEM_NODE *pi;
    CLIENT_NODE *ps;
    TYPE_STAT *head;
    char time[18];

    printf("\n\t\t��������Ҫͳ�Ƶ�ʱ�䣺");
    scanf("%s",time);
    getchar();
    head = StatTypeInfo(gp_head, time);  /*���ú���StatTypeInfo,���ɷ�����Ϣͳ������*/
    printf("\n\t\t�ͷ����\t�ͷ�����\t����ס��\tδ��ס��\n");
    while(head !=NULL)
    {

        printf("\t\t%-8c\t%-8d\t%-8d\t%8d\n", head->type_id, head->room_number, head->room_y,head->room_n);
        head = head->next;

    }

    printf("\n\t\t�����������...");
    getch();
    ClearScreen();
    ShowMenu();
    
    return;
}
 /**
 * ��������: StatTypeInfo
 * ��������: ���ɷ�����Ϣͳ������
 * �������: head ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��ͷָ��phead
 *
 * ����˵��:
 */
TYPE_STAT * StatTypeInfo(TYPE_NODE *head,char time[18])
{
    TYPE_STAT *p1, *phead;
    ITEM_NODE *pi;
    CLIENT_NODE *ps;
    char time_a[18]="0";

    phead = (TYPE_STAT *)malloc(sizeof(TYPE_STAT));/*����ͷ���*/
    p1 = phead;
    /*������ͷָ����Ƚ������ͳ������*/
    while(head != NULL)
    {
        p1->next = (TYPE_STAT *)malloc(sizeof(TYPE_STAT));/*�����µĽ��*/
        p1 = p1->next;
        p1->type_id = head->type_id;
        p1->room_number = head->room_number;
        p1->room_n = p1->room_number;
        p1->room_y = 0;
        pi = head->inext;
        while(pi != NULL) /*����֧����ͳ����ס��δ��ס�ͷ���*/
        {
            ps = pi->snext;
            while(ps != NULL)
            {
                if(strcmp(ps->time_out , time_a) == 0 )
            	{
            		strcpy(ps->time_out,time);
				}
                if(strcmp(time, ps->time_in) > 0 && strcmp(time, ps->time_out) < 0)
                {
                    p1->room_y=1+p1->room_y;
                    p1->room_n=(p1->room_n)-1;
                }ps = ps->next; /*ָ����һ�����*/
            }pi = pi->next; /*ָ����һ�����*/
        }head = head->next;  /*ͳ����һ���������Ϣ*/
    }
    p1->next = NULL;
    phead = phead->next;

    return phead;
}

 /**
 * ��������: StatMonthYear
 * ��������: �����ͳ��ÿ���µĸ���ͷ�����ҵ�� 
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void StatMonthYear(void)
{
    MONTH_STAT * head;
    char year[5];
    
    printf("\n\t\t\t��������Ҫ��ѯ����ݣ�");
    scanf("%s", year); /*��ȡ��ݣ�ȷ��ͳ�Ʒ�Χ*/
    getchar();
    head = StatYearInfo(gp_head, year);   /*����StatYearInfo������ͳ�ƽ������*/
    printf("\n\t\t\t�·�\t���˼�\t˫�˼�\t���˼�");
    while(head !=NULL)
    {  /*�����������ͳ�ƽ��*/

        printf("\n\t\t\t%-4d\t%-6.0f\t%-6.0f\t%-6.0f", head->month, head->s, head->d, head->t);
        head = head->next;
    }

    printf("\n\t\t\t�����������...");
    getch();
    ClearScreen();
    ShowMenu();
    
    return;
}



 /**
 * ��������: StatYearInfo
 * ��������: ����ͳ��ÿ���µĿͷ��������������ͳ�ƽ������
 * �������: head ����ͷָ��
 * �������: ��
 * �� �� ֵ: ͳ�Ƶõ��Ŀͷ�����������ͷָ��phead
 *
 * ����˵��:
 */
MONTH_STAT *StatYearInfo(TYPE_NODE *head, char year[5])
{
    TYPE_NODE *pt = head;
    ITEM_NODE *pi;
    CLIENT_NODE *pc;
    MONTH_STAT *phead = NULL, *p1;
    int j=1;
    
    phead = (MONTH_STAT *)malloc(sizeof(MONTH_STAT));
    p1 = phead;
    /*������ͷָ����Ƚ��ȳ���ͳ�ƽ������*/
    for(j=1;j<13;j++)
    {
    	pt=head;
    	p1->next = (MONTH_STAT *)malloc(sizeof(MONTH_STAT));/*�½�ͳ�ƽ�����*/
        p1 = p1->next;
        p1->month = j;
        p1->d=0;
        p1->s=0;
        p1->t=0;
    	while(pt != NULL)
		{
        pi = pt->inext;
        while(pi != NULL)
        {
        	pc = pi->snext;	
        	while(pc != NULL)
        	{
        		if(pc->time_in[3] == year[3])
        		{
        			if(j<10){
        				if((pc->time_in[6]-48) == j && pi->type_id=='D')
        				{
        					p1->d += pc->money;
						}
						if((pc->time_in[6]-48) == j && pi->type_id=='S')
        				{
        					p1->s += pc->money;
						}
						if((pc->time_in[6]-48) == j && pi->type_id=='T')
        				{
        					p1->t += pc->money;
						}	
					}
        			else{
        				if((pc->time_in[6]-38) == j && pi->type_id == 'D' && (pc->time_in[5]-48) == 1)
        				{
        					p1->d += pc->money;
						}
						if((pc->time_in[6]-38) == j && pi->type_id == 'S' && (pc->time_in[5]-48) == 1)
        				{
        					p1->s += pc->money;
						}
						if((pc->time_in[6]-38) == j && pi->type_id == 'T' && (pc->time_in[5]-48) == 1)
        				{
        					p1->t += pc->money;
						}    				
					}					
				}pc = pc->next;
			}pi = pi->next;
        } pt = pt->next;
    }    	
	}
    p1->next = NULL;
    phead = phead->next; /*��������ͷָ���βָ��*/
    return phead; /*���ؽ�������ͷָ��*/
}

 /**
 * ��������: StatYearruzhu
 * ��������: �����ͳ�Ƹ��ֿͷ�����ס���
 * �������: ��
 * �������: �� 
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void StatYearruzhu(TYPE_NODE *head)
{
	TYPE_NODE *pt = head;
    ITEM_NODE *pi;
    CLIENT_NODE *ps;
    NUMBER_STAT *phead = NULL,*pc;
	char year[5]; 
	
	phead = (NUMBER_STAT *)malloc(sizeof(NUMBER_STAT));
	pc =phead;
	printf("\n\t��������Ҫ��ѯӪҵ�����ݣ�");
	scanf("%s",year);
	getchar();
	while(pt != NULL)
    {
        pi = pt->inext;
        while(pi != NULL)
        {
        	pc->next = (NUMBER_STAT *)malloc(sizeof(NUMBER_STAT));/*�½�ͳ�ƽ�����*/
            pc = pc->next;
            pc->sale=0;
            pc->ruzhu=0;
            pc->month=0;
            strcpy(pc->number,pi->number);
        	ps = pi->snext;
        	while(ps != NULL)
			{
        		pc->sale += ps->reality;
        	    pc->month += ps->month;
        	    ps = ps->next;
			}			
        	pc->ruzhu= pc->month/12.0;
        	pi = pi->next;	
        }
        pt=pt->next;
    }
    pc->next=NULL;
    phead = phead->next; /*��������ͷָ���βָ��*/
    
    printf("\t�ͷ����\tӪҵ��\t��ס����\t��ס��");
    while(phead!=NULL){
    	printf("\n\t%-8s\t%-6.0f\t%-8d\t%-6.2f",phead->number,phead->sale,phead->month,phead->ruzhu);
    	phead =phead->next;
	} 
	printf("\n\t�����������...");
    getch();
    ClearScreen();
    ShowMenu();
    
    return;
}

 /**
 * ��������: StatClient1
 * ��������: ��ʱ��ͳ���ڸ�ʱ���п��˵�ס����� 
 * �������: ��
 * �������: ���п��˵�ס�޽ɷ���� 
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void StatClient1(TYPE_NODE *head)
{
	TYPE_NODE *pt = head;
    ITEM_NODE *pi;
    CLIENT_NODE *pc;
    CLIENT_STAT *phead,*pk,*phead1,*phead2,*phead3;
	char time[18];
	
	phead = (CLIENT_STAT *)malloc(sizeof(CLIENT_STAT));
	pk =phead;
    printf("������ͳ��ʱ��:");
    scanf("%s",time);
    getchar();
    while(pt != NULL)
    {
        pi = pt->inext;
        while(pi != NULL)
        {    	
        	pc = pi->snext;
        	while(pc != NULL)
			{
				pk->next = (CLIENT_STAT *)malloc(sizeof(CLIENT_STAT));/*�½�ͳ�ƽ�����*/
                pk = pk->next;
                strcpy(pk->people_id,pc->people_id);
                strcpy(pk->people_name,pc->people_name);
                if(pc->month == 0)
                {
                	if(((pc->time_in[6])-(time[6]))>0)
                	{
                		pk->month=0;pk->should=0;pk->shiji=0;
					}
					else{
						pk->month=(time[6]-pc->time_in[6]);
						pk->should=pc->money*pk->month;
						pk->shiji=pc->reality;
					}
				}
				else
				{
				    if(((pc->time_in[6])-(time[6]))>=0)
                	{
                		pk->month=0;pk->should=0;pk->shiji=0;
					}
					else{
						pk->month=(time[6]-pc->time_in[6]);
						pk->should=pc->money*pk->month;
						pk->shiji=pc->reality;
					}
				}
        	    pc = pc->next;
			}
        	pi = pi->next;
        }
        pt=pt->next;
    }
    pk->next=NULL;
    phead = phead->next; /*��������ͷָ���βָ��*/
	phead1=phead;
    phead2=phead;
    phead3=phead;
    while(phead1!=NULL){
    	phead2=phead1->next;
    	while(phead2!=NULL){
    	if(strcmp(phead1->people_name,phead2->people_name) == 0)
    	{
    		
    		phead1->month=phead1->month+phead2->month;
    		phead1->shiji=phead1->shiji+phead2->shiji;
    		phead1->should=phead1->should+phead2->should;
    		phead3->next=phead2->next;
		}phead3=phead2;
		phead2=phead2->next;		
	}phead1=phead1->next;		
	}
	printf("���֤��        \t����            \tס������\tӦ�ɷ���\tʵ�ɷ���");
	while(phead!=NULL){
    	printf("\n%-20s\t%-20s\t%-8d\t%-8.0f\t%-8.0f",phead->people_id,phead->people_name,phead->month,phead->should,phead->should);
    	phead =phead->next;
	} 
	printf("\n�����������...");
    getch();
    ClearScreen();
    ShowMenu();
    
    return;

}

 /**
 * ��������: StatClient2
 * ��������: ��ʱ��ͳ�Ƹ����ͻ��Ŀͷ�ס����� 
 * �������: ��
 * �������: ʮ��ס���������Ŀ��˵�ס����Ϣ
  
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void StatClient2(TYPE_NODE *head)
{
    TYPE_NODE *pt = head;
    ITEM_NODE *pi;
    CLIENT_NODE *pc;
    CLIENT_STAT *phead,*pk,*phead1,*phead2,*phead3;
	char time[18];
	int len=0;
	int i, j,k=0;
	char people_id[20];
	char people_name[20];
	int month;
	float should;
	float shiji;
    CLIENT_STAT *p1 = NULL, *p2 = NULL;
    
	phead = (CLIENT_STAT *)malloc(sizeof(CLIENT_STAT));
	pk =phead;
    printf("������ͳ��ʱ��:");/*����ͳ��ʱ��*/
    scanf("%s",time);
    getchar();
    while(pt != NULL)
    {
        pi = pt->inext;
        while(pi != NULL)
        {  	
        	pc = pi->snext;
        	while(pc != NULL)
			{
				pk->next = (CLIENT_STAT *)malloc(sizeof(CLIENT_STAT));/*�½�ͳ�ƽ�����*/
                pk = pk->next;
                strcpy(pk->people_id,pc->people_id);
                strcpy(pk->people_name,pc->people_name);
                if(pc->month == 0)
                {
                	if(((pc->time_in[6])-(time[6]))>=0)
                	{
                		pk->month=0;pk->should=0;pk->shiji=0;
					}
					else{
						pk->month=(time[6]-pc->time_in[6]);
						pk->should=pc->money*pk->month;
						pk->shiji=pc->reality;
					}
				}
				else
				{
					if(((pc->time_in[6])-(time[6]))>=0)
                	{
                		pk->month=0;pk->should=0;pk->shiji=0;
					}
					else{
						pk->month=(time[6]-pc->time_in[6]);
						pk->should=pc->money*pk->month;
						pk->shiji=pc->reality;
					}
				}
        	    pc = pc->next;
			}
        	pi = pi->next; 
        }
        pt=pt->next;
    }
    pk->next=NULL;
    phead = phead->next; /*��������ͷָ���βָ��*/	
	phead1=phead;
    phead2=phead;
    phead3=phead;
    while(phead1!=NULL)
	{
    	phead2=phead1->next;
    	while(phead2!=NULL)
		{
    	if(strcmp(phead1->people_name,phead2->people_name) == 0)
    	{
    		phead1->month=phead1->month+phead2->month;
    		phead1->shiji=phead1->shiji+phead2->shiji;
    		phead1->should=phead1->should+phead2->should;
    		phead3->next=phead2->next;	
	   	}phead3=phead2;
		phead2=phead2->next;
		
     	}phead1=phead1->next;		
    }
	phead1=phead;
	while(phead1!=NULL) 
	{
		len++;
		phead1=phead1->next; 	
	}	
    /*���ݹ�������ͨ������������ķ�ʽ����ð������*/
    for(i = 0, p1 = phead; i < len-1; i++, p1 = p1->next)
        for(j = i + 1, p2 = p1->next; j < len; j++, p2 = p2->next)
            if(p1->month < p2->month)
            {
                strcpy(people_id, p1->people_id);
                strcpy(people_name, p1->people_name);
                month = p1->month;
                should = p1->should;
                shiji = p1->shiji;
                strcpy(p1->people_id, p2->people_id);
                strcpy(p1->people_name, p2->people_name);
                p1->month = p2->month;
                p1->shiji =p2->shiji;
                p1->should = p2->should;
                strcpy(p2->people_id, people_id);
                strcpy(p2->people_name, people_name);
                p2->month =month;
                p2->shiji =shiji;
                p2->should = should;
            }
	
	printf("���֤��        \t����            \tס������\tӦ�ɷ���\tʵ�ɷ���");
	while(phead!=NULL && k!=10){
    	printf("\n%-20s\t%-20s\t%-8d\t%-8.0f\t%-8.0f",phead->people_id,phead->people_name,phead->month,phead->should,phead->should);
    	phead =phead->next;
    	k++;
	} 
	printf("\n�����������...");
    getch();
    ClearScreen();
    ShowMenu();
    
    return;
}

/**
 * ��������: SaveSysData
 * ��������: ����ϵͳ�����������������.
 * �������: hd ����ͷ���ָ��
 * �������:
 * �� �� ֵ: BOOL����, ����ΪTRUE
 *
 * ����˵��:
 */
BOOL SaveSysData(TYPE_NODE *hd)
{
    TYPE_NODE *ptype_node;
    ITEM_NODE *pitem_node;
    CLIENT_NODE *pclient_node;
    FILE *pfout;
    
    pfout = fopen(gp_type_info_filename, "wb");
    for(ptype_node = hd; ptype_node!= NULL; ptype_node = ptype_node->next)
    {
        /*����ͷ�������Ϣ*/
        fwrite(ptype_node, sizeof(TYPE_NODE), 1, pfout);
    }
    fclose(pfout);

    pfout = fopen(gp_item_info_filename, "wb");
    for(ptype_node = hd; ptype_node != NULL; ptype_node = ptype_node->next)
    {
        /*����ͷ�������Ϣ*/
        pitem_node = ptype_node->inext;
        while(pitem_node != NULL)
        {
            fwrite(pitem_node, sizeof(ITEM_NODE), 1, pfout);
            pitem_node = pitem_node->next;
        }
    }

    fclose(pfout);

    pfout = fopen(gp_client_info_filename, "wb");
    for(ptype_node = hd; ptype_node != NULL; ptype_node = ptype_node->next)
    {
        /*���������ס��Ϣ*/
        pitem_node = ptype_node->inext;
        while(pitem_node != NULL)
        {
            pclient_node = pitem_node->snext;
            while(pclient_node != NULL)
            {
                fwrite(pclient_node, sizeof(CLIENT_NODE), 1, pfout);
                pclient_node = pclient_node->next;
            }

            pitem_node = pitem_node->next;
        }
    }

    fclose(pfout);

    return TRUE;
}

/**
 * ��������: BackupSysData
 * ��������: ��ϵͳ����������������ݱ��ݵ�һ�������ļ�.
 * �������: hd ����ͷ���ָ��
 *           filename �����ļ���
 * �������:
 * �� �� ֵ: BOOL����, ����ΪTRUE
 *
 * ����˵��:
 */
BOOL BackupSysData(TYPE_NODE *hd, char *filename)
{
    TYPE_NODE *ptype_node;
    ITEM_NODE *pitem_node;
    CLIENT_NODE *pclient_node;
	unsigned long type_node_num = 0;
	unsigned long item_node_num = 0;
	unsigned long client_node_num = 0;
	int handle;

	/* ����ʮ�������ֱ�ͳ�����ֻ�����Ϣ�ļ�¼���� */
    for(ptype_node = hd; ptype_node != NULL; ptype_node = ptype_node->next)
    {
        type_node_num++;
        pitem_node = ptype_node->inext;
        while(pitem_node != NULL)
        {
            item_node_num++;
            pclient_node = pitem_node->snext;
            while(pclient_node != NULL)
            {
                client_node_num++;
                pclient_node = pclient_node->next;
            }
            pitem_node = pitem_node->next;
        }
    }

	if((handle = open(filename,O_WRONLY | O_BINARY)) == -1)
	{
		handle = open(filename,O_CREAT | O_BINARY,S_IWRITE);
		handle = open(filename,O_WRONLY | O_BINARY);
	}

	/* ��������������ݵļ�¼���� */
    write(handle, (char *)&type_node_num, sizeof(type_node_num));
    write(handle, (char *)&item_node_num, sizeof(item_node_num));
    write(handle, (char *)&client_node_num, sizeof(client_node_num));

    for(ptype_node = hd; ptype_node != NULL; ptype_node = ptype_node->next)
    {
        /*����ͷ�����Ϣ*/
        write(handle, (char *)ptype_node, sizeof(TYPE_NODE));
    }

    for(ptype_node = hd; ptype_node != NULL; ptype_node = ptype_node->next)
    {
        /*����ͷ�������Ϣ*/
        pitem_node = ptype_node->inext;
        while(pitem_node != NULL)
        {
            write(handle, (char *)pitem_node, sizeof(ITEM_NODE));
            pitem_node = pitem_node->next;
        }
    }

    for(ptype_node = hd; ptype_node != NULL; ptype_node = ptype_node->next)
    {
        /*���������ס��Ϣ*/
        pitem_node = ptype_node->inext;
        while(pitem_node != NULL)
        {
            pclient_node = pitem_node->snext;
            while(pclient_node != NULL)
            {
                write(handle, (char*)pclient_node, sizeof(CLIENT_NODE));
                pclient_node = pclient_node->next;
            }
            pitem_node = pitem_node->next;
        }
    }

    close(handle);
    return TRUE;
}

/**
 * ��������: RestoreSysData
 * ��������: ��ָ�������ļ��лָ������������.
 * �������: phead ����ͷ���ָ��ĵ�ַ
 *           filename ��ű������ݵ������ļ���
 * �������:
 * �� �� ֵ: BOOL����, ����ΪTRUE
 *
 * ����˵��:
 */
BOOL RestoreSysData(TYPE_NODE **phead, char *filename)
{
    TYPE_NODE *hd = NULL;
    TYPE_NODE *ptype_node;
    ITEM_NODE *pitem_node;
    CLIENT_NODE *pclient_node;
    unsigned long type_node_num = 0;
    unsigned long item_node_num = 0;
    unsigned long client_node_num = 0;
    unsigned long ulloop;
    int handle;
    int find;

	if((handle = open(filename,O_CREAT | O_BINARY)) == -1)
	{
		handle = open(filename,O_CREAT | O_BINARY,S_IREAD);
	}

   /*��ȡ���ֻ���������Ϣ�ļ�¼��*/
    read(handle, (char *)&type_node_num, sizeof(type_node_num));
    read(handle, (char *)&item_node_num, sizeof(item_node_num));
    read(handle, (char *)&client_node_num, sizeof(client_node_num));

    /*��ȡ�ͷ�������Ϣ������ʮ������*/
    for(ulloop = 1; ulloop <= type_node_num; ulloop++)
    {
        ptype_node = (TYPE_NODE *)malloc(sizeof(TYPE_NODE));
        read(handle, (char *)ptype_node, sizeof(TYPE_NODE));
        ptype_node->inext = NULL;
        ptype_node->next = hd;
        hd = ptype_node;
    }
    *phead = hd;

    /*��ȡ�ͷ�������Ϣ�������ͷ���Ϣ֧��*/
    for(ulloop = 1; ulloop <= item_node_num; ulloop++)
    {
        pitem_node = (ITEM_NODE *)malloc(sizeof(ITEM_NODE));
        read(handle, (char *)pitem_node, sizeof(ITEM_NODE));
        pitem_node->snext = NULL;
        ptype_node = hd;
        while(ptype_node != NULL && (ptype_node->type_id != pitem_node->type_id))
        {
            ptype_node = ptype_node->next;
        }

        if(ptype_node != NULL)
        {
            pitem_node->next = ptype_node->inext;
            ptype_node->inext = pitem_node;
        }
        else
        {
            free(pitem_node);
        }
    }

    /*��ȡ������ס��Ϣ������������ס��Ϣ����*/
    for(ulloop = 1; ulloop <= client_node_num; ulloop++)
    {
        pclient_node = (CLIENT_NODE *)malloc(sizeof(CLIENT_NODE));
        read(handle, (char *)pclient_node, sizeof(CLIENT_NODE));
        ptype_node = hd;
        find = 0;

        while(ptype_node != NULL && find == 0)
        {
            pitem_node = ptype_node -> inext;
            while(pitem_node != NULL && find == 0)
            {
                if(strcmp(pitem_node -> number , pclient_node -> number) == 0)
                {
                    find = 1;
                    break;
                }
                pitem_node = pitem_node -> next;
            }
            ptype_node = ptype_node -> next;
        }

        if(find)
        {
            pclient_node -> next = pitem_node -> snext;
            pitem_node -> snext = pclient_node;
        }
        else
        {
            free(pclient_node);
        }
    }

    close(handle);
    SaveSysData(hd);    /*���ڴ������ݱ��浽�����ļ�*/

    return TRUE;
}

 /**
 * ��������: ShowModule
 * ��������: չʾС���ں��� 
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */



BOOL ShowModule(char **pString, int n)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    int i, maxlen, str_len;

    for (i=0,maxlen=0; i<n; i++) {
        str_len = strlen(pString[i]);
        if (maxlen < str_len) {
            maxlen = str_len;
        }
    }

    pos.X = maxlen + 6;
    pos.Y = n + 5;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    labels.num = n;
    labels.ppLabel = pString;
    COORD aLoc[n];

    for (i=0; i<n; i++) {
        aLoc[i].X = rcPop.Left + 3;
        aLoc[i].Y = rcPop.Top + 2 + i;
    }

    str_len = strlen(pString[n-1]);
    aLoc[n-1].X = rcPop.Left + 3 + (maxlen-str_len)/2;
    aLoc[n-1].Y = aLoc[n-1].Y + 2;

    labels.pLoc = aLoc;

    areas.num = 1;
    SMALL_RECT aArea[] = {{
        aLoc[n-1].X, aLoc[n-1].Y,
        aLoc[n-1].X + 3, aLoc[n-1].Y
        }
    };

    char aSort[] = {0};
    char aTag[] = {1};

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
	DrawBox(&rcPop);

    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 + n;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    DealInput(&areas, &iHot);
    PopOff();

    return bRet;
}


/**
 * ��������: ShowModule3
 * ��������: ������2���ַ�����1��"����"�ַ����ı�ǩ�����ó��������ȴ��û�����
 * �������: pString ָ�룬n��ǩ����
 * �������: ��
 * �� �� ֵ: ����굥�������س���ո�ʱ���ص�ǰ�������
 *
 * ����˵��:
 */
int ShowModule3(char **pString, int n)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    int i, maxlen, str_len,x;

    for (i=0, maxlen=0; i<n; i++)
    {
        str_len = strlen(pString[i]);
        if (maxlen < str_len)
        {
            maxlen = str_len;
        }
    }

    pos.X = maxlen + 6;
    pos.Y = n + 5;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    labels.num = n;
    labels.ppLabel = pString;
    COORD aLoc[n];

    for (i=0; i<n; i++)
    {
        aLoc[i].X = rcPop.Left + 3;
        aLoc[i].Y = rcPop.Top + 2 + i;
    }

    str_len = strlen(pString[n-1]);
    aLoc[n-1].X = rcPop.Left + 3 + (maxlen-str_len)/2;
    aLoc[n-1].Y = aLoc[n-1].Y + 2;

    labels.pLoc = aLoc;

    areas.num = 3;
    SMALL_RECT aArea[] = {{aLoc[0].X, aLoc[0].Y,
                        aLoc[0].X + maxlen - 1, aLoc[0].Y},
                        {aLoc[1].X, aLoc[1].Y,
                        aLoc[1].X + maxlen - 1, aLoc[1].Y},
                        {aLoc[2].X, aLoc[2].Y,
                        aLoc[2].X + 3, aLoc[2].Y}
                        };

    char aSort[] = {0, 0, 0};
    char aTag[] = {1, 2, 3};

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 + n;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    x = DealInput(&areas, &iHot);
    PopOff();
    return x;
}



/**
 * ��������: ShowModule4
 * ��������: ������3���ַ�����1��"����"�ַ����ı�ǩ�����ó��������ȴ��û�����
 * �������: pString ָ�룬n��ǩ����
 * �������: ��
 * �� �� ֵ: ����굥�������س���ո�ʱ���ص�ǰ�������
 *
 * ����˵��:
 */
int ShowModule4(char **pString, int n)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    int i, maxlen, str_len,x;

    for (i=0, maxlen=0; i<n; i++)
    {
        str_len = strlen(pString[i]);
        if (maxlen < str_len)
        {
            maxlen = str_len;
        }
    }

    pos.X = maxlen + 6;
    pos.Y = n + 5;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    labels.num = n;
    labels.ppLabel = pString;
    COORD aLoc[n];

    for (i=0; i<n; i++)
    {
        aLoc[i].X = rcPop.Left + 3;
        aLoc[i].Y = rcPop.Top + 2 + i;
    }

    str_len = strlen(pString[n-1]);
    aLoc[n-1].X = rcPop.Left + 3 + (maxlen-str_len)/2;
    aLoc[n-1].Y = aLoc[n-1].Y + 2;

    labels.pLoc = aLoc;

    areas.num = 4;
    SMALL_RECT aArea[] = {{aLoc[0].X, aLoc[0].Y,
                        aLoc[0].X + maxlen - 1, aLoc[0].Y},
                        {aLoc[1].X, aLoc[1].Y,
                        aLoc[1].X + maxlen - 1, aLoc[1].Y},
                        {aLoc[2].X, aLoc[2].Y,
                        aLoc[2].X + maxlen - 1, aLoc[2].Y},
                        {aLoc[3].X, aLoc[3].Y,
                        aLoc[3].X + 3, aLoc[3].Y}
                        };

    char aSort[] = {0, 0, 0, 0};
    char aTag[] = {1, 2, 3, 4};

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 + n;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    x = DealInput(&areas, &iHot);
    PopOff();
    return x;
}


