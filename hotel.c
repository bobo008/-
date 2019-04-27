#include "hotel.h"

unsigned long ul;

int main()
{
    COORD size = {SCR_COL, SCR_ROW};              /*窗口缓冲区大小*/

    gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE); /* 获取标准输出设备句柄*/
    gh_std_in = GetStdHandle(STD_INPUT_HANDLE);   /* 获取标准输入设备句柄*/

    SetConsoleTitle(gp_sys_name);                 /*设置窗口标题*/
    SetConsoleScreenBufferSize(gh_std_out, size); /*设置窗口缓冲区大小80*25*/

    LoadData();                   /*数据加载*/
    InitInterface();          /*界面初始化*/
    RunSys(&gp_head);             /*系统功能模块的选择及运行*/
    CloseSys(gp_head);            /*退出系统*/

    return 0;
}

/**
 * 函数名称: LoadData
 * 函数功能: 三类基础数据从数据文件载入到内存缓冲区和十字链表中.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: BOOL类型, 功能函数中除了函数ExitSys的返回值可以为FALSE外,
 *           其他函数的返回值必须为TRUE.
 *
 * 调用说明: 为了能够以统一的方式调用各功能函数, 将这些功能函数的原型设为
 *           一致, 即无参数且返回值为BOOL. 返回值为FALSE时, 结束程序运行.
 */
BOOL LoadData()
{
    int Re = 0;

    Re = CreatList(&gp_head);
    if (Re < 28)
    {  /*数据加载提示信息*/
        printf("\n系统基础数据不完整!\n");
    }

    printf("\n按任意键继续...\n");
    getch();

    return TRUE;
}

/**
 * 函数名称: CreatList
 * 函数功能: 从数据文件读取基础数据, 并存放到所创建的十字链表中.
 * 输入参数: 无
 * 输出参数: phead 主链头指针的地址, 用来返回所创建的十字链.
 * 返 回 值: int型数值, 表示链表创建的情况.
 *           0  空链, 无数据
 *           4  已加载客房分类信息数据，无客房基本信息和客人入住信息数据
 *           12 已加载客房分类信息和客房基本信息数据，无客人入住信息数据
 *           28 三类基础数据都已加载
 *
 * 调用说明:
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
        printf("客房分类信息数据文件打开失败!\n");
        return re;
    }
    printf("客房分类信息数据文件打开成功!\n");

    /*从数据文件中读客房分类信息数据，存入以后进先出的方式建立的主链中*/
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
        printf("客房分类信息数据文件加载失败!\n");
        return re;
    }
    printf("客房分类信息数据文件加载成功!\n");
    *phead = hd;
    re += 4;

    if ((pFile = fopen(gp_item_info_filename, "rb")) == NULL)
    {
        printf("客房基本信息数据文件打开失败!\n");
        return re;
    }
    printf("客房基本信息数据文件打开成功!\n");

    re += 8;

    /*从数据文件中读取客房基本信息数据，存入主链对应结点的客房基本信息支链中*/
    while (fread(&tmp2, sizeof(ITEM_NODE), 1, pFile) == 1)
    {
        /*创建结点，存放从数据文件中读出的客房基本信息*/
        pItemNode = (ITEM_NODE *)malloc(sizeof(ITEM_NODE));
        *pItemNode = tmp2;
        pItemNode->snext = NULL;

        /*在主链上查找该客房所属类别对应的主链结点*/
        pTypeNode = hd;
        while (pTypeNode != NULL && pTypeNode->type_id != pItemNode->type_id)
        {
            pTypeNode = pTypeNode->next;
        }

        if (pTypeNode != NULL) /*如果找到，则将结点以后进先出方式插入客房基本信息支链*/
        {
            pItemNode->next = pTypeNode->inext;
            pTypeNode->inext = pItemNode;
        }
        else  /*如果未找到，则释放所创建结点的内存空间*/
        {
            free(pItemNode);
        }
    }
    fclose(pFile);
    printf("客房基本信息数据文件加载成功!\n");

    if ((pFile = fopen(gp_client_info_filename, "rb")) == NULL)
    {
        printf("客人租房信息数据文件打开失败!\n");
        return re;
    }
    printf("客人租房信息数据文件打开成功!\n");

    re += 16;

    /*从数据文件中读取客人租房信息数据，存入基本信息支链对应结点的租房信息支链中*/
    while (fread(&tmp3, sizeof(CLIENT_NODE), 1, pFile) == 1)
    {
        /*创建结点，存放从数据文件中读出的客人入住信息*/
        pClientNode = (CLIENT_NODE *)malloc(sizeof(CLIENT_NODE));
        *pClientNode = tmp3;

        /*查找客房基本信息支链上对应基本信息结点*/
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

        if (find)  /*如果找到，则将结点以后进先出方式插入客人入住信息支链中*/
        {
            pClientNode->next = pItemNode->snext;
            pItemNode->snext = pClientNode;
        }
        else /*如果未找到，则释放所创建结点的内存空间*/
        {
            free(pClientNode);
        }
    }

    fclose(pFile);
    printf("客人租房信息数据文件加载成功!\n");

    return re;
}

/**
 * 函数名称: InitInterface
 * 函数功能: 初始化界面.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void InitInterface()
{
    WORD att = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
               | BACKGROUND_BLUE;  /*黄色前景和蓝色背景*/

    SetConsoleTextAttribute(gh_std_out, att);  /*设置控制台屏幕缓冲区字符属性*/

    ClearScreen();  /* 清屏*/

    /*创建弹出窗口信息堆栈，将初始化后的屏幕窗口当作第一层弹出窗口*/
    gp_scr_att = (char *)calloc(SCR_COL * SCR_ROW, sizeof(char));/*屏幕字符属性*/
    gp_top_layer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    gp_top_layer->LayerNo = 0;      /*弹出窗口的层号为0*/
    gp_top_layer->rcArea.Left = 0;  /*弹出窗口的区域为整个屏幕窗口*/
    gp_top_layer->rcArea.Top = 0;
    gp_top_layer->rcArea.Right = SCR_COL - 1;
    gp_top_layer->rcArea.Bottom = SCR_ROW - 1;
    gp_top_layer->pContent = NULL;
    gp_top_layer->pScrAtt = gp_scr_att;
    gp_top_layer->next = NULL;

    ShowMenu();     /*显示菜单栏*/
    ShowState();    /*显示状态栏*/

    return;
}

/**
 * 函数名称: ClearScreen
 * 函数功能: 清除屏幕信息.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void ClearScreen()
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD home = {0, 0};
    unsigned long size;

    GetConsoleScreenBufferInfo(gh_std_out, &bInfo );/*取屏幕缓冲区信息*/
    size =  bInfo.dwSize.X * bInfo.dwSize.Y; /*计算屏幕缓冲区字符单元数*/

    /*将屏幕缓冲区所有单元的字符属性设置为当前屏幕缓冲区字符属性*/
    FillConsoleOutputAttribute(gh_std_out, bInfo.wAttributes, size, home, &ul);

    /*将屏幕缓冲区所有单元填充为空格字符*/
    FillConsoleOutputCharacter(gh_std_out, ' ', size, home, &ul);

    return;
}

/**
 * 函数名称: ShowMenu
 * 函数功能: 在屏幕上显示主菜单, 并设置热区, 在主菜单第一项上置选中标记.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
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
    for (i=0; i < 5; i++) /*在窗口第一行第一列处输出主菜单项*/
    {
        printf("  %s  ", ga_main_menu[i]);
    }

    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;
    SetConsoleCursorInfo(gh_std_out, &lpCur);  /*隐藏光标*/

    /*申请动态存储区作为存放菜单条屏幕区字符信息的缓冲区*/
    gp_buff_menubar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
    SMALL_RECT rcMenu ={0, 0, size.X-1, 0};

    /*将窗口第一行的内容读入到存放菜单条屏幕区字符信息的缓冲区中*/
    ReadConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    /*将这一行中英文字母置为红色，其他字符单元置为白底黑字*/
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

    /*修改后的菜单条字符信息回写到窗口的第一行*/
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);
    COORD endPos = {0, 1};
    SetConsoleCursorPosition(gh_std_out, endPos);  /*将光标位置设置在第2行第1列*/

    /*将菜单项置为热区，热区编号为菜单项号，热区类型为0(按钮型)*/
    i = 0;
    do
    {
        PosB = PosA + strlen(ga_main_menu[i]);  /*定位第i+1号菜单项的起止位置*/
        for (j=PosA; j<PosB; j++)
        {
            gp_scr_att[j] |= (i+1) << 2; /*设置菜单项所在字符单元的属性值*/
        }
        PosA = PosB + 4;
        i++;
    } while (i < 5);

    TagMainMenu(gi_sel_menu);  /*在选中主菜单项上做标记，gi_sel_menu初值为1*/

    return;
}

/**
 * 函数名称: ShowState
 * 函数功能: 显示状态条.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明: 状态条字符属性为白底黑字, 初始状态无状态信息.
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
 * 函数名称: TagMainMenu
 * 函数功能: 在指定主菜单项上置选中标志.
 * 输入参数: num 选中的主菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void TagMainMenu(int num)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int PosA = 2, PosB;
    char ch;
    int i;

    if (num == 0) /*num为0时，将会去除主菜单项选中标记*/
    {
        PosA = 0;
        PosB = 0;
    }
    else  /*否则，定位选中主菜单项的起止位置: PosA为起始位置, PosB为截止位置*/
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

    /*去除选中菜单项前面的菜单项选中标记*/
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

    /*在选中菜单项上做标记，黑底白字*/
    for (i=PosA; i<PosB; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN
                                               | FOREGROUND_RED;
    }

    /*去除选中菜单项后面的菜单项选中标记*/
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

    /*将做好标记的菜单条信息写到窗口第一行*/
    SMALL_RECT rcMenu ={0, 0, size.X-1, 0};
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    return;
}

/**
 * 函数名称: CloseSys
 * 函数功能: 关闭系统.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void CloseSys(TYPE_NODE *hd)
{
    TYPE_NODE *pTypeNode1 = hd, *pTypeNode2;
    ITEM_NODE *pItemNode1, *pItemNode2;
    CLIENT_NODE *pclientNode1, *pclientNode2;

    while (pTypeNode1 != NULL) /*释放十字交叉链表的动态存储区*/
    {
        pTypeNode2 = pTypeNode1->next;
        pItemNode1 = pTypeNode1->inext;
        while (pItemNode1 != NULL) /*释放商品基本信息支链的动态存储区*/
        {
            pItemNode2 = pItemNode1->next;
            pclientNode1 = pItemNode1->snext;

            while (pclientNode1 != NULL) /*释放商品销售信息支链的动态存储区*/
            {
                pclientNode2 = pclientNode1->next;
                free(pclientNode1);
                pclientNode1 = pclientNode2;
            }

            free(pItemNode1);
            pItemNode1 = pItemNode2;
        }

        free(pTypeNode1);  /*释放主链结点的动态存储区*/
        pTypeNode1 = pTypeNode2;
    }

    ClearScreen();        /*清屏*/

    /*释放存放菜单条、状态条、商品类别代码等信息动态存储区*/
    free(gp_buff_menubar_info);
    free(gp_buff_stateBar_info);

    /*关闭标准输入和输出设备句柄*/
    CloseHandle(gh_std_out);
    CloseHandle(gh_std_in);

    /*将窗口标题栏置为运行结束*/
    SetConsoleTitle("运行结束");

    return;
}

/**
 * 函数名称: RunSys
 * 函数功能: 运行系统, 在系统主界面下运行用户所选择的功能模块.
 * 输入参数: 无
 * 输出参数: phead 主链头指针的地址
 * 返 回 值: 无
 *
 * 调用说明:
 */
void RunSys(TYPE_NODE **phead)
{
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    BOOL bRet = TRUE;
    int i, loc, num;
    int cNo, cAtt;      /*cNo:字符单元层号, cAtt:字符单元属性*/
    char vkc, asc;      /*vkc:虚拟键代码, asc:字符的ASCII码值*/

    while (bRet)
    {
        /*从控制台输入缓冲区中读一条记录*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);

        if (inRec.EventType == MOUSE_EVENT) /*如果记录由鼠标事件产生*/
        {
            pos = inRec.Event.MouseEvent.dwMousePosition;  /*获取鼠标坐标位置*/
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3; /*取该位置的层号*/
            cAtt = gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2;/*取该字符单元属性*/
            if (cNo == 0) /*层号为0，表明该位置未被弹出子菜单覆盖*/
            {
                if (cAtt > 0 && cAtt != gi_sel_menu && gp_top_layer->LayerNo > 0)
                {
                    PopOff();            /*关闭弹出的子菜单*/
                    gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                    PopMenu(cAtt);       /*弹出鼠标所在主菜单项对应的子菜单*/
                }
            }
            else if (cAtt > 0) /*鼠标所在位置为弹出子菜单的菜单项字符单元*/
            {
                TagSubMenu(cAtt); /*在该子菜单项上做选中标记*/
            }

            if (inRec.Event.MouseEvent.dwButtonState
                == FROM_LEFT_1ST_BUTTON_PRESSED) /*如果按下鼠标左边第一键*/
            {
                if (cNo == 0) /*层号为0，表明该位置未被弹出子菜单覆盖*/
                {
                    if (cAtt > 0) /*如果该位置处于热区(主菜单项字符单元)*/
                    {
                        PopMenu(cAtt);   /*弹出鼠标所在主菜单项对应的子菜单*/
                    }
                    /*如果该位置不属于主菜单项字符单元，且有子菜单弹出*/
                    else if (gp_top_layer->LayerNo > 0)
                    {
                        PopOff();            /*关闭弹出的子菜单*/
                        gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                    }
                }
                else /*层号不为0，表明该位置被弹出子菜单覆盖*/
                {
                    if (cAtt > 0) /*如果该位置处于热区(子菜单项字符单元)*/
                    {
                        PopOff(); /*关闭弹出的子菜单*/
                        gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/

                        /*执行对应功能函数:gi_sel_menu主菜单项号, cAtt子菜单项号*/
                        bRet = ExeFunction(gi_sel_menu, cAtt);
                    }
                }
            }
            else if (inRec.Event.MouseEvent.dwButtonState
                     == RIGHTMOST_BUTTON_PRESSED) /*如果按下鼠标右键*/
            {
                if (cNo == 0) /*层号为0，表明该位置未被弹出子菜单覆盖*/
                {
                    PopOff();            /*关闭弹出的子菜单*/
                    gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                }
            }
        }
        else if (inRec.EventType == KEY_EVENT  /*如果记录由按键产生*/
                 && inRec.Event.KeyEvent.bKeyDown) /*且键被按下*/
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode; /*获取按键的虚拟键码*/
            asc = inRec.Event.KeyEvent.uChar.AsciiChar; /*获取按键的ASC码*/

            /*系统快捷键的处理*/
            if (vkc == 112) /*如果按下F1键*/
            {
                if (gp_top_layer->LayerNo != 0) /*如果当前有子菜单弹出*/
                {
                    PopOff();            /*关闭弹出的子菜单*/
                    gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                }
                bRet = ExeFunction(5, 1);  /*运行帮助主题功能函数*/
            }
            else if (inRec.Event.KeyEvent.dwControlKeyState
                     & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
            { /*如果按下左或右Alt键*/
                switch (vkc)  /*判断组合键Alt+字母*/
                {
                    case 88:  /*Alt+X 退出*/
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
            else if (asc == 0) /*其他控制键的处理*/
            {
                if (gp_top_layer->LayerNo == 0) /*如果未弹出子菜单*/
                {
                    switch (vkc) /*处理方向键(左、右、下)，不响应其他控制键*/
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
                else  /*已弹出子菜单时*/
                {
                    for (loc=0, i=1; i<gi_sel_menu; i++)
                    {
                        loc += ga_sub_menu_count[i-1];
                    }  /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
                    switch (vkc) /*方向键(左、右、上、下)的处理*/
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
            else if ((asc-vkc == 0) || (asc-vkc == 32)){  /*按下普通键*/
                if (gp_top_layer->LayerNo == 0)  /*如果未弹出子菜单*/
                {
                    switch (vkc)
                    {
                        case 70: /*f或F*/
                            PopMenu(1);
                            break;
                        case 77: /*m或M*/
                            PopMenu(2);
                            break;
                        case 81: /*q或Q*/
                            PopMenu(3);
                            break;
                        case 83: /*s或S*/
                            PopMenu(4);
                            break;
                        case 72: /*h或H*/
                            PopMenu(5);
                            break;
                        case 13: /*回车*/
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                    }
                }
                else /*已弹出子菜单时的键盘输入处理*/
                {
                    if (vkc == 27) /*如果按下ESC键*/
                    {
                        PopOff();
                        gi_sel_sub_menu = 0;
                    }
                    else if(vkc == 13) /*如果按下回车键*/
                    {
                        num = gi_sel_sub_menu;
                        PopOff();
                        gi_sel_sub_menu = 0;
                        bRet = ExeFunction(gi_sel_menu, num);
                    }
                    else /*其他普通键的处理*/
                    {
                        /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
                        for (loc=0, i=1; i<gi_sel_menu; i++)
                        {
                            loc += ga_sub_menu_count[i-1];
                        }

                        /*依次与当前子菜单中每一项的代表字符进行比较*/
                        for (i=loc; i<loc+ga_sub_menu_count[gi_sel_menu-1]; i++)
                        {
                            if (strlen(ga_sub_menu[i])>0 && vkc==ga_sub_menu[i][1])
                            { /*如果匹配成功*/
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
 * 函数名称: PopMenu
 * 函数功能: 弹出指定主菜单项对应的子菜单.
 * 输入参数: num 指定的主菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
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

    if (num != gi_sel_menu)       /*如果指定主菜单不是已选中菜单*/
    {
        if (gp_top_layer->LayerNo != 0) /*如果此前已有子菜单弹出*/
        {
            PopOff();
            gi_sel_sub_menu = 0;
        }
    }
    else if (gp_top_layer->LayerNo != 0) /*若已弹出该子菜单，则返回*/
    {
        return;
    }

    gi_sel_menu = num;    /*将选中主菜单项置为指定的主菜单项*/
    TagMainMenu(gi_sel_menu); /*在选中的主菜单项上做标记*/
    LocSubMenu(gi_sel_menu, &rcPop); /*计算弹出子菜单的区域位置, 存放在rcPop中*/

    /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
    for (i=1; i<gi_sel_menu; i++)
    {
        loc += ga_sub_menu_count[i-1];
    }
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel = ga_sub_menu + loc;   /*标签束第一个标签字符串的地址*/
    labels.num = ga_sub_menu_count[gi_sel_menu-1]; /*标签束中标签字符串的个数*/
    COORD aLoc[labels.num];/*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    for (i=0; i<labels.num; i++) /*确定标签字符串的输出位置，存放在坐标数组中*/
    {
        aLoc[i].X = rcPop.Left + 2;
        aLoc[i].Y = rcPop.Top + i + 1;
    }
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = labels.num;       /*热区的个数，等于标签的个数，即子菜单的项数*/
    SMALL_RECT aArea[areas.num];                    /*定义数组存放所有热区位置*/
    char aSort[areas.num];                      /*定义数组存放所有热区对应类别*/
    char aTag[areas.num];                         /*定义数组存放每个热区的编号*/
    for (i=0; i<areas.num; i++)
    {
        aArea[i].Left = rcPop.Left + 2;  /*热区定位*/
        aArea[i].Top = rcPop.Top + i + 1;
        aArea[i].Right = rcPop.Right - 2;
        aArea[i].Bottom = aArea[i].Top;
        aSort[i] = 0;       /*热区类别都为0(按钮型)*/
        aTag[i] = i + 1;           /*热区按顺序编号*/
    }
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/
    pos.X = rcPop.Left + 2;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    {
        /*此循环用来在空串子菜项位置画线形成分隔，并取消此菜单项的热区属性*/
        pCh = ga_sub_menu[loc+pos.Y-rcPop.Top-1];
        if (strlen(pCh)==0) /*串长为0，表明为空串*/
        {
            /*首先画横线*/
            FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-3, pos, &ul);
            for (j=rcPop.Left+2; j<rcPop.Right-1; j++)
            {
                /*取消该区域字符单元的热区属性*/
                gp_scr_att[pos.Y*SCR_COL+j] &= 3; /*按位与的结果保留了低两位*/
            }
        }

    }
    /*将子菜单项的功能键设为白底红字*/
    pos.X = rcPop.Left + 3;
    att =  FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    {
        if (strlen(ga_sub_menu[loc+pos.Y-rcPop.Top-1])==0)
        {
            continue;  /*跳过空串*/
        }
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    return;
}

/**
 * 函数名称: PopUp
 * 函数功能: 在指定区域输出弹出窗口信息, 同时设置热区, 将弹出窗口位置信息入栈.
 * 输入参数: pRc 弹出窗口位置数据存放的地址
 *           att 弹出窗口区域字符属性
 *           pLabel 弹出窗口中标签束信息存放的地址
             pHotArea 弹出窗口中热区信息存放的地址
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void PopUp(SMALL_RECT *pRc, WORD att, LABEL_BUNDLE *pLabel, HOT_AREA *pHotArea)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j, row;

    /*弹出窗口所在位置字符单元信息入栈*/
    size.X = pRc->Right - pRc->Left + 1;    /*弹出窗口的宽度*/
    size.Y = pRc->Bottom - pRc->Top + 1;    /*弹出窗口的高度*/
    /*申请存放弹出窗口相关信息的动态存储区*/
    nextLayer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    nextLayer->next = gp_top_layer;
    nextLayer->LayerNo = gp_top_layer->LayerNo + 1;
    nextLayer->rcArea = *pRc;
    nextLayer->pContent = (CHAR_INFO *)malloc(size.X*size.Y*sizeof(CHAR_INFO));
    nextLayer->pScrAtt = (char *)malloc(size.X*size.Y*sizeof(char));
    pCh = nextLayer->pScrAtt;
    /*将弹出窗口覆盖区域的字符信息保存，用于在关闭弹出窗口时恢复原样*/
    ReadConsoleOutput(gh_std_out, nextLayer->pContent, size, pos, pRc);
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {   /*此二重循环将所覆盖字符单元的原先属性值存入动态存储区，便于以后恢复*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            *pCh = gp_scr_att[i*SCR_COL+j];
            pCh++;
        }
    }

    gp_top_layer = nextLayer;  /*完成弹出窗口相关信息入栈操作*/
    /*设置弹出窗口区域字符的新属性*/
    pos.X = pRc->Left;
    pos.Y = pRc->Top;
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        FillConsoleOutputAttribute(gh_std_out, att, size.X, pos, &ul);
        pos.Y++;
    }

    /*将标签束中的标签字符串在设定的位置输出*/
    for (i=0; i<pLabel->num; i++)
    {
        pCh = pLabel->ppLabel[i];
        if (strlen(pCh) != 0)
        {
            WriteConsoleOutputCharacter(gh_std_out, pCh, strlen(pCh),
                                        pLabel->pLoc[i], &ul);
        }
    }

    /*设置弹出窗口区域字符单元的新属性*/
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {   /*此二重循环设置字符单元的层号*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = gp_top_layer->LayerNo;
        }
    }

    for (i=0; i<pHotArea->num; i++)
    {   /*此二重循环设置所有热区中字符单元的热区类型和热区编号*/
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
 * 函数名称: PopOff
 * 函数功能: 关闭顶层弹出窗口, 恢复覆盖区域原外观和字符单元原属性.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void PopOff(void)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j;

    if ((gp_top_layer->next==NULL) || (gp_top_layer->pContent==NULL))
    {   /*栈底存放的主界面屏幕信息，不用关闭*/
        return;
    }

    nextLayer = gp_top_layer->next;
    /*恢复弹出窗口区域原外观*/
    size.X = gp_top_layer->rcArea.Right - gp_top_layer->rcArea.Left + 1;
    size.Y = gp_top_layer->rcArea.Bottom - gp_top_layer->rcArea.Top + 1;
    WriteConsoleOutput(gh_std_out, gp_top_layer->pContent, size, pos, &(gp_top_layer->rcArea));
    /*恢复字符单元原属性*/
    pCh = gp_top_layer->pScrAtt;
    for (i=gp_top_layer->rcArea.Top; i<=gp_top_layer->rcArea.Bottom; i++)
    {
        for (j=gp_top_layer->rcArea.Left; j<=gp_top_layer->rcArea.Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = *pCh;
            pCh++;
        }
    }

    free(gp_top_layer->pContent);    /*释放动态存储区*/
    free(gp_top_layer->pScrAtt);
    free(gp_top_layer);
    gp_top_layer = nextLayer;
    gi_sel_sub_menu = 0;

    return;
}

/**
 * 函数名称: DrawBox
 * 函数功能: 在指定区域画边框.
 * 输入参数: pRc 存放区域位置信息的地址
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void DrawBox(SMALL_RECT *pRc)
{
    char chBox[] = {'+', '-', '|'};  /*画框用的字符*/
    COORD pos = {pRc->Left, pRc->Top};  /*定位在区域的左上角*/

    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框左上角*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {   /*此循环画上边框横线*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }

    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框右上角*/
    for (pos.Y = pRc->Top+1; pos.Y < pRc->Bottom; pos.Y++)
    {   /*此循环画边框左边线和右边线*/
        pos.X = pRc->Left;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
        pos.X = pRc->Right;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
    }

    pos.X = pRc->Left;
    pos.Y = pRc->Bottom;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框左下角*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {   /*画下边框横线*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }

    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框右下角*/

    return;
}

/**
 * 函数名称: TagSubMenu
 * 函数功能: 在指定子菜单项上做选中标记.
 * 输入参数: num 选中的子菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void TagSubMenu(int num)
{
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int width;

    LocSubMenu(gi_sel_menu, &rcPop);  /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    if ((num<1) || (num == gi_sel_sub_menu) || (num>rcPop.Bottom-rcPop.Top-1))
    {   /*如果子菜单项号越界，或该项子菜单已被选中，则返回*/
        return;
    }

    pos.X = rcPop.Left + 2;
    width = rcPop.Right - rcPop.Left - 3;
    if (gi_sel_sub_menu != 0) /*首先取消原选中子菜单项上的标记*/
    {
        pos.Y = rcPop.Top + gi_sel_sub_menu;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
        FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
        pos.X += 1;
        att |=  FOREGROUND_RED;/*白底红字*/
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    /*在制定子菜单项上做选中标记*/
    pos.X = rcPop.Left + 2;
    pos.Y = rcPop.Top + num;
    att = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*黑底白字*/
    FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
    gi_sel_sub_menu = num;  /*修改选中子菜单项号*/
    return;
}

/**
 * 函数名称: LocSubMenu
 * 函数功能: 计算弹出子菜单区域左上角和右下角的位置.
 * 输入参数: num 选中的主菜单项号
 * 输出参数: rc 存放区域位置信息的地址
 * 返 回 值: 无
 *
 * 调用说明:
 */
void LocSubMenu(int num, SMALL_RECT *rc)
{
    int i, len, loc = 0;

    rc->Top = 1; /*区域的上边定在第2行，行号为1*/
    rc->Left = 1;
    for (i=1; i<num; i++)
    {   /*计算区域左边界位置, 同时计算第一个子菜单项在子菜单字符串数组中的位置*/
        rc->Left += strlen(ga_main_menu[i-1]) + 4;
        loc += ga_sub_menu_count[i-1];
    }
    rc->Right = strlen(ga_sub_menu[loc]);/*暂时存放第一个子菜单项字符串长度*/
    for (i=1; i<ga_sub_menu_count[num-1]; i++)
    {   /*查找最长子菜单字符串，将其长度存放在rc->Right*/
        len = strlen(ga_sub_menu[loc+i]);
        if (rc->Right < len)
        {
            rc->Right = len;
        }
    }
    rc->Right += rc->Left + 3;  /*计算区域的右边界*/
    rc->Bottom = rc->Top + ga_sub_menu_count[num-1] + 1;/*计算区域下边的行号*/
    if (rc->Right >= SCR_COL)  /*右边界越界的处理*/
    {
        len = rc->Right - SCR_COL + 1;
        rc->Left -= len;
        rc->Right = SCR_COL - 1;
    }
    return;
}

/**
 * 函数名称: DealInput
 * 函数功能: 在弹出窗口区域设置热区, 等待并相应用户输入.
 * 输入参数: pHotArea
 *           piHot 焦点热区编号的存放地址, 即指向焦点热区编号的指针
 * 输出参数: piHot 用鼠标单击、按回车或空格时返回当前热区编号
 * 返 回 值:
 *
 * 调用说明:
*/
int DealInput(HOT_AREA *pHotArea, int *piHot)
{
    INPUT_RECORD inRec;
	CONSOLE_CURSOR_INFO lpCur;
    DWORD res;
    COORD pos = {0, 0};
    int iRet = 0;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc;       /*vkc:虚拟键代码*/

    SetHotPoint(pHotArea, *piHot);
    while (TRUE)
    {    /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if ((inRec.EventType == MOUSE_EVENT) &&
            (inRec.Event.MouseEvent.dwButtonState
             == FROM_LEFT_1ST_BUTTON_PRESSED))
        {
            pos = inRec.Event.MouseEvent.dwMousePosition;    /* 获取光标位置 */
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3;
            cTag = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2) & 15;
            cSort = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 6) & 3;    /* 获取该处页面属性 */

            if ((cNo == gp_top_layer->LayerNo) && cTag > 0)
            {/* 如果点在热区上 */
                *piHot = cTag;
                SetHotPoint(pHotArea, *piHot);
                if (cSort == 0)
                {
                    iRet = cTag;
                    return iRet;
                }
            }
			else if(cTag == 0)
            {/* 如果点在非热点区域 */
                GetConsoleCursorInfo(gh_std_out, &lpCur);
                lpCur.bVisible = FALSE;
                SetConsoleCursorInfo(gh_std_out, &lpCur);    /* 隐藏光标 */
            }
        }
		/*如果是键盘输入*/
        else if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown)
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;

            switch(vkc)
            {

            case 37 :/* 处理上、左键 */
            case 38 :
                -- *piHot;
                if(*piHot < 1)
                {
                    *piHot = pHotArea->num;
                }
                SetHotPoint(pHotArea, *piHot);
                GetConsoleCursorInfo(gh_std_out, &lpCur);
                break;
            case 13 :/*回车键或空格表示按下当前按钮*/
            case 32 :
                if(pHotArea->pSort[*piHot-1] == 0)
                {   //若为按钮
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;
                    SetConsoleCursorInfo(gh_std_out, &lpCur);    /* 隐藏光标 */
                    iRet = *piHot;
                    return iRet;
                }
            case 39 :/* 处理右、下键 */
            case 40 :
                ++ *piHot;
                if(*piHot > pHotArea->num)
                {
                    *piHot = 1;
                }
                SetHotPoint(pHotArea, *piHot);
                GetConsoleCursorInfo(gh_std_out, &lpCur);
                break;
            case 27 :/* 按ESC键 */
                GetConsoleCursorInfo(gh_std_out, &lpCur);
                lpCur.bVisible = FALSE;
                SetConsoleCursorInfo(gh_std_out, &lpCur);    /* 隐藏光标 */
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

    att1 = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*黑底白字*/
    att2 = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    for (i=0; i<pHotArea->num; i++)
    {  /*将按钮类热区置为白底黑字*/
        pos.X = pHotArea->pArea[i].Left;
        pos.Y = pHotArea->pArea[i].Top;
        width = pHotArea->pArea[i].Right - pHotArea->pArea[i].Left + 1;
        if (pHotArea->pSort[i] == 0)
        {  /*热区是按钮类*/
            FillConsoleOutputAttribute(gh_std_out, att2, width, pos, &ul);
        }
    }

    pos.X = pHotArea->pArea[iHot-1].Left;
    pos.Y = pHotArea->pArea[iHot-1].Top;
    width = pHotArea->pArea[iHot-1].Right - pHotArea->pArea[iHot-1].Left + 1;
    if (pHotArea->pSort[iHot-1] == 0)
    {  /*被激活热区是按钮类*/
        GetConsoleCursorInfo(gh_std_out, &lpCur);
        lpCur.bVisible = FALSE;
        SetConsoleCursorInfo(gh_std_out, &lpCur);    /*隐藏光标*/
        FillConsoleOutputAttribute(gh_std_out, att1, width, pos, &ul);
    }
    else if (pHotArea->pSort[iHot-1] == 1)
    {  /*被激活热区是文本框类*/
        SetConsoleCursorPosition(gh_std_out, pos);
        GetConsoleCursorInfo(gh_std_out, &lpCur);
        lpCur.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out, &lpCur);   /* 光标移至输入框起始位置 */
    }
}

/**
 * 函数名称: ExeFunction
 * 函数功能: 执行由主菜单号和子菜单号确定的功能函数.
 * 输入参数: m 主菜单项号
 *           s 子菜单项号
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE 或 FALSE
 *
 * 调用说明: 仅在执行函数ExitSys时, 才可能返回FALSE, 其他情况下总是返回TRUE
 */
BOOL ExeFunction(int m, int s)
{
    BOOL bRet = TRUE;
    /*函数指针数组，用来存放所有功能函数的入口地址*/
    BOOL (*pFunction[ga_sub_menu_count[0]+ga_sub_menu_count[1]+ga_sub_menu_count[2]+ga_sub_menu_count[3]+ga_sub_menu_count[4]])(void);
    int i, loc;

    /*将功能函数入口地址存入与功能函数所在主菜单号和子菜单号对应下标的数组元素*/
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
    

    for (i=1,loc=0; i<m; i++)  /*根据主菜单号和子菜单号计算对应下标*/
    {
        loc += ga_sub_menu_count[i-1];
    }
    loc += s - 1;

    if (pFunction[loc] != NULL)
    {
        bRet = (*pFunction[loc])();  /*用函数指针调用所指向的功能函数*/
    }

    return bRet;
}

BOOL SaveData(void)
{
    BOOL bRet = TRUE;
    char *plabel_name[] = {"主菜单项：文件",
                           "子菜单项：数据保存",
                           "确认"
                          };

    ShowModule(plabel_name, 3);
    SaveSysData(gp_head);

    char *plabel_name1[] = {"数据保存成功！",
                           "确认"
                          };

    ShowModule(plabel_name1, 2);

    return bRet;
}

BOOL BackupData(void)
{
    BOOL bRet = TRUE;
    char *plabel_name[] = {"主菜单项：文件",
                           "子菜单项：数据备份",
                           "确认"
                          };

    ShowModule(plabel_name, 3);
    BackupSysData(gp_head, "Backup.dat");
    char *plabel_name1[] = {"数据备份成功！",
                            "确认"
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
    char *pCh[] = {"当前数据将被覆盖，继续执行？", 
	               "  确定            取消"};
    char *pCh1[] = {"数据恢复成功！",
                     "确认"
                    };    
					 
    int iHot = 1;

    pos.X = strlen(pCh[0]) + 6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
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
    char *pCh[] = {"确认退出系统吗？", "确定    取消"};
    int iHot = 1;

    pos.X = strlen(pCh[0]) + 6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
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
    char *plabel_name[] = {"插入客房分类信息",
                       "修改客房分类信息",
                       "删除客房分类信息",
                       "返回"
                   };
                   
    int x = 0;
    
    while(x != 4)
    {
        x = ShowModule4(plabel_name, 4);
	    switch (x)
	    {
        case 1:
			InsertTypeNode(gp_head);/*插入客房分类信息*/
			break;
        case 2:
		    ModifTypeNode(gp_head);/*修改客房分类信息*/
			break;
        case 3:
			DelTypeNode(gp_head);/*删除客房分类信息*/
			break;
        case 4:
            SaveSysData(gp_head); /*调用SaveSysData函数，保存三类数据*/ 
			break;
        }
    }

    return TRUE;
}

BOOL MaintainItemInfo(void)
{
    char *plabel_name[] = {"插入客房基本信息",
                       "修改客房基本信息",
                       "删除客房基本信息",
                       "返回"
                   };
                   
    int x = 0;

    while(x != 4)
    {
        x = ShowModule4(plabel_name,4);
        switch (x)
        {
        case 1:
			InsertItemNode(gp_head);/*插入客房基本信息*/
			break;
		case 2:
		    ModifItemNode(gp_head);/*修改客房基本信息*/
			break;
        case 3:
			DelItemNode(gp_head);/*删除客房基本信息*/
			break;
        case 4:
            SaveSysData(gp_head); /*调用SaveSysData函数，保存三类数据*/ 
			break;
        }
	}

    return TRUE;
}

BOOL MaintainClientInfo(void)
{
    char *plabel_name[] = {"插入客人入住信息",
                       "修改客人入住信息",
                       "删除客人入住信息",
                       "返回"
                   };

    int x = 0;
    
    while(x != 4)
    {
        x = ShowModule4(plabel_name,4);
        switch (x)
        {
        case 1:
			InsertClientNode(gp_head);/*插入客人入住信息*/
			break;
		case 2:
		    ModifClientNode(gp_head);/*修改客人入住信息*/
			break;
        case 3:
			DelClientNode(gp_head);/*删除客人入住信息*/
			break;
        case 4:
            SaveSysData(gp_head); /*调用SaveSysData函数，保存三类数据*/ 
			break;
        }
	}

    return TRUE;
}

BOOL QueryTypeInfo(void)
{
    char *plabel_name[] = {"按分类编码查询",
                       "   输出全部",
                       "返回"
                      };
                      
    int x = 0;
    
    while(x!=3)
    {
        x = ShowModule3(plabel_name,3);
	    switch (x)
	    {
	    	case 1:
	    	    SeekType_id(gp_head);/*按分类编码查询*/
	    	    break;
            case 2:
                SeekType(gp_head);/*输出全部*/
                break;
            case 3:
                break;/*返回*/
	    }
	}

    return TRUE;
}

BOOL QueryItemInfo(void)
{
    char *plabel_name[] = {"  按客房编号查询",
                       "按客房类别与租金查询",
                       "返回"
                   };

    int x = 0;
    
    while(x != 3)
    {
        x = ShowModule3(plabel_name,3);
        switch (x)
	    {
		    case 1:
		        SeekNumber(gp_head);/*按客房编号查询*/
			    break;
            case 2:
                SeekId_Price(gp_head);/*按客房类别与租金查询*/
                break;
            case 3:
                break;/*返回*/

        }
    }

    return TRUE;
}

BOOL QueryClientInfo(void)
{
    char *plabel_name[] = {"按客人身份证号码查找",
	                   "以客人姓名及时间查找",
                       "返回"
                       };

    int x = 0;
    
    while(x != 3)
    {
        x = ShowModule3(plabel_name, 3);
        switch (x)
	    {
		    case 1:
		        Seekpeople_id(gp_head);/*按客人身份证号码查找*/
			    break;
            case 2:
                SeekName_Time(gp_head);/*以客人姓名及时间查找*/
                break;
            case 3:
                break;/*返回*/

        }
    }

    return TRUE;
}

BOOL StatSort(void)
{
    StatTyperuzhu();/*按时间统计入住率*/
    return TRUE;
}

BOOL StatMoney(void)
{
    int x = 0;
    char *plabel_name[] = {"按月统计营业额",
                       "按年统计营业额",
                       "返回"
                       };
                       
    while(x != 3)
    {
        x = ShowModule3(plabel_name, 3);
        switch (x)
	    {
	    	case 1:
	    	    StatMonthYear();/*按月统计营业额*/
		    	break;
		    case 2:
                StatYearruzhu(gp_head);/*按年统计营业额*/
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
    char *plabel_name[] = {"统计客人住宿情况",
                       "客人住宿情况排序",
                       "返回"
                       };
                       
    while(x != 3)
    {
        x = ShowModule3(plabel_name, 3);/*选择*/
        switch (x)
	    {
	    	case 1:
	    	    StatClient1(gp_head);/*统计客人住宿情况*/
		    	break;
		    case 2:
		        StatClient2(gp_head);/*客人住宿情况排序*/
		    	break;
		    case 3:
			    break;/*返回*/
        }
    }

    return TRUE;
}

BOOL HelpTopic(void)
{
    BOOL bRet = TRUE;
    char *plabel_name[] = {"使用本程序请严格按照提示输入",
                           "输入错误将造成不可预知的错误",
                           "确认"
                          };
                          
    ShowModule(plabel_name, 3);

    return bRet;
}

BOOL AboutDorm(void)
{
    char *plabel_name[] = {"房屋出租系统程序2.0版本",
                           "  版权所有人：汤洪波",
                           "确认"
                          };

    ShowModule(plabel_name, 3);

    return TRUE;
}

/**
 * 函数名称: move
 * 函数功能: 使得光标移动到指定的地点 
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明: 
 */

void move(int x, int y)
{   //使得光标移动到指定的地点 
	COORD pos;
	
	pos.X = x - 1;
	pos.Y = y - 1;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);

}

/**
 * 函数名称: InsertTypeNode
 * 函数功能: 在十字链表中插入一个分类信息结点.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void InsertTypeNode(TYPE_NODE* hd)
{
    TYPE_NODE *p, *pt = hd;
    
    char *plabel_name1[] = {"此客房类别已被使用。",
                              "插入分类信息失败。",
                              "确认"
                             };
    char *plabel_name2[] = {"插入分类信息成功。",
                              "确认"
                             };
    move(25, 4);
    printf("请输入您所要添加的客房分类的信息：");
    p = (TYPE_NODE*)malloc(sizeof(TYPE_NODE));/*新建分类信息结点*/
    move(29, 6);
    printf("客房类别：\n\n\t\t\t    入住人数：\n\n\t\t\t    客房套数：\n\n\t\t\t    未住套数：");
    move(39, 6);
    scanf("%c", &p->type_id);
    move(39, 8);
    scanf("%d", &p->people_max);
    move(39, 10);
    scanf("%d",&p->room_number);
    move(39, 12);
    scanf("%d",&p->room_no);
    getchar();

    while(pt != NULL)  /*遍历分类信息链表，检查房屋类别是否已被使用*/
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
    gp_head = p;/*若房屋类别没有重复，将新建的结点作为新的主链的头结点*/
    
    ClearScreen();
    ShowMenu();
    ShowModule(plabel_name2, 2);
    ClearScreen();
    ShowMenu();

    return;
}

/**
 * 函数名称: ModifTypeNode
 * 函数功能: 从十字链表中修改指定的分类信息结点.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明: 根据房屋分类查找分类信息并作出修改
 */
void ModifTypeNode(TYPE_NODE *hd)
{
    TYPE_NODE *pt = hd, *p1;
    ITEM_NODE *pi;
    char type_id;              /**< 客房类别*/
    int people_max;            /**< 最多入住人数*/
    int room_number;           /**< 客房套数*/
    int room_no;               /**< 客房未住套数*/ 
    int x = 0;
    
    char *plabel_name1[] = {"修改分类信息失败。",
                    "确认"
                    };
    char *plabel_name2[] = {"修改分类信息成功。",
                    "确认"
                    };

    move(23, 2);
    printf("请输入您要修改的房屋类别的名称：");
    scanf("%c", &type_id);
    getchar();

    while(pt!=NULL)/*查找分类信息结点*/
    {
        if(type_id == pt->type_id) break;
        pt=pt->next;
    }

    if(pt == NULL)/*若无符合条件的结点，提示修改失败*/
    {
        move(23, 4);
        printf("此分类不存在");
        getch();
        ShowModule(plabel_name1, 2);
        ClearScreen();
        ShowMenu();
        
        return;
    }
    
    printf("\n\n\t\t\t  您要修改的客房分类信息如下");
    printf("\n\n\t\t\t  客房类别：%c",pt->type_id);
	printf("\n\t\t\t  最多入住人数：%d",pt->people_max);
	printf("\n\t\t\t  客房套数：%d",pt->room_number);
	printf("\n\t\t\t  客房未入住人数：%d",pt->room_no); 
    printf("\n\t\t\t  按任意键继续...");
    getch();
    printf("\n\n\t\t\t  请输入新的客房类别:");
    scanf("%c", &type_id);
    getchar();
    p1 = hd;
    while(p1 != NULL)
    {
        if(p1->type_id == type_id)break;
        p1 =p1->next;
    }    /*检查新的客房类别是否已被使用*/

    if (p1 != NULL)
    {
    printf("\n\t\t\t  该客房类别已被使用");
    getch();
    ClearScreen();
    ShowMenu();
    
    ShowModule(plabel_name1, 2);
    return; 
    }
    else
    { 
		/*修改分类信息对应的基本信息中的客房类别*/
        pt->type_id = type_id;
        for(pi=pt->inext; pi != NULL; pi->type_id = type_id, pi = pi->next);
                    
    }

    printf("\n\t\t\t  请输入新的最多入住人数：");
    scanf("%d",&people_max);
    getchar();
    pt->people_max=people_max;
    printf("\n\t\t\t  请输入新的客房套数：");
    scanf("%d",&room_number);
    getchar();
    pt->room_number=room_number; 
    printf("\n\t\t\t  请输入新的客房未住套数：");
    scanf("%d",&room_no);
    getchar();
    pt->room_no=room_no;
    ClearScreen();
    ShowMenu(); 
    ShowModule(plabel_name2, 2);
    
    return;    
}

/**
 * 函数名称: DelTypeNode
 * 函数功能: 从十字链表中删除指定的分类信息结点.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明: 根据房屋分类删除信息
 */
void DelTypeNode(TYPE_NODE *hd)
{
    TYPE_NODE *pt = hd, *p = NULL;
    ITEM_NODE *pi1, *pi2;
    CLIENT_NODE *pc1, *pc2;
    char type_idno;
    
    char *plabel_name1[] = {"此分类不存在。",
                          "删除分类信息失败。",
                          "确认"
                         };
    char *plabel_name2[] = {"删除分类信息成功。",
                          "确认"
                         };

    printf("\n\n\t\t请输入您要删除的房屋分类的名称：");
    scanf("%c",&type_idno);
    getchar();
    
    while(pt!=NULL && (type_idno != (pt->type_id)))
    {
    	p = pt;
		pt=pt->next;
	}                /*根据分类名称查找分类信息结点pt,并使p作为pt的前驱*/
     
    if(pt == NULL)/*若无符合条件的结点，提示删除失败*/
    {
        ShowModule(plabel_name1, 3);
        ClearScreen();
        ShowMenu();

        return;
    }
    else if(pt == gp_head)      /*判断是否为头结点*/
    {
        gp_head = pt->next;/*修改头结点*/
    }
    else
    {
        p->next = pt->next;/*使pt的前驱指向pt的后继*/
    }

    pi1 = pt->inext;
    while(pi1 != NULL)
    {/*释放pt对应的基础和销售信息的存储空间*/
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

    free(pt);/*释放pt的存储空间*/

    ShowModule(plabel_name2,2);
    ClearScreen();
    ShowMenu();

    return;
}




/**
 * 函数名称: InsertItemNode
 * 函数功能: 在十字链表中插入一个基本信息结点.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明  
 */
void InsertItemNode(TYPE_NODE* hd)
{
    TYPE_NODE *pt = hd, *ptype = hd;
    ITEM_NODE *pi, *p;
    char type_name;
    
    char *plabel_name1[] = {"此类别不存在。",
                           "插入基本信息失败。",
                           "确认"
                          };
    char *plabel_name2[] = {"此客房编号已被使用",
                           "插入基本信息失败",
                           "确认"
                          };
    move(30, 4);
    printf("请输入您所要添加的客房基本信息：");
    p = (ITEM_NODE*)malloc(sizeof(ITEM_NODE));
    move(30, 6);
    printf("客房类别: ");
    scanf("%c",&type_name);
    getchar();
    for(pt = hd; pt != NULL && type_name != (pt->type_id); pt = pt->next);  /*查找基本信息对应的分类结点pt*/
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
    printf("客房编号（三位数字）: ");
    scanf("%s", p->number);
    getchar();
    while(ptype != NULL)
    {
        pi = ptype->inext;
        while(pi != NULL)
        {  /*遍历链表，检查客房编号否重复*/
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
    printf("电话号码（四位数字）：");
    scanf("%s", p->phone);
    getchar(); 
    move(30, 10);
    printf("面积；");
	scanf("%f",&p->area);
	getchar();
	move(30, 11);
	printf("每月租金：");
	scanf("%f",&p->sale);
	getchar();
	move(30, 12);
	printf("是否有阳台'1'表示有阳台'0'表示没有阳台：");
	scanf("%c",&p->balcony);
	getchar();
	move(30, 13);
	printf("是否有客人'y'已有客人入住'n'未住:");
	scanf("%c",&p->human);
    getchar();
    p->snext = NULL;
    p->next = pt->inext;
    pt->inext = p;
    pt = hd;
    move(30, 14);
    printf("按任意键继续...\n");
    getch();

    char *plabel_name3[] = {"插入基本信息成功",
                       "确认"
                      };

    ClearScreen();
    ShowMenu();
    ShowModule(plabel_name3, 2);

    return;
}



/**
 * 函数名称: ModifItemNode
 * 函数功能: 从十字链表中修改指定的基本信息结点.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明: 根据客房编号查找基本信息并作出修改
 */
void ModifItemNode(TYPE_NODE *hd)
{
    TYPE_NODE *pt = hd, *pt2;
    ITEM_NODE *pi = NULL, *pitem = NULL, *pi2;
    CLIENT_NODE *pc;
    int flag = 0, find = 0;
    char name[4], type_id;
    
    char *plabel_name1[] = {"修改基本信息失败。",
                           "确认"
                           };
    char *plabel_name2[] = {"修改基本信息成功。",
                            "确认"
                            };

    move(17, 2);
    printf("请输入您所要修改的基本信息的客房编号：");
    scanf("%s", name);
    getchar();
    while(pt != NULL)
    {
        pi = pt->inext;
        while(pi != NULL)
        {  /*根据客房编号查找基本信息结点pi,以及pi的前驱pi2*/
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
        printf("此客房编号不存在！");
        ShowModule(plabel_name1, 2);
        ClearScreen();
        ShowMenu();

        return;
    }
    
	printf("\n\t\t请修改电话号码（四位数字）：");
	scanf("%s",&pi->phone) ;
	getchar();
    printf("\n\t\t请修改客房类别('D''S''T''F')：");
	scanf("%c",&type_id) ;
	getchar();
	for(pt2 = hd; pt2 != NULL && type_id != pt2->type_id; pt2 = pt2->next);/*查找类别对应的分类信息结点*/
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
    printf("\n\t\t请修改客房面积：");/*修改客房面积*/
	scanf("%f",&pi->area) ;
	getchar();
    printf("\n\t\t请修改客房租金：");/*修改客房租金*/
	scanf("%f",&pi->sale) ;
	getchar();
	pc = pi->snext;
	while(pc != NULL)
	{
		pc->should = (pi->sale)*pc->month;
		pc = pc->next;
    } 
    printf("\n\t\t请确定是否有阳台：");/*确定是否有阳台*/
	scanf("%c",&pi->balcony) ;
	getchar();
    printf("\n\t\t请确定是否有人入住：");/*确定是否有人入住*/
	scanf("%c",&pi->human) ;
	getchar();
    ClearScreen();
    ShowMenu();
    ShowModule(plabel_name2, 2);
    
    return;            
}

/**
 * 函数名称: DelItemNode
 * 函数功能: 从十字链表中删除指定的基本信息结点.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明: 根据客房编号删除信息
 */
void DelItemNode(TYPE_NODE *hd)
{
    TYPE_NODE *pt = hd;
    ITEM_NODE *pi, *pi2 = NULL;
    CLIENT_NODE *pc;
    char name[4];
    
    char *plabel_name1[] = {"删除基本信息成功",
                         "确认"
                         };
    char *plabel_name2[] = {"此客房编号不存在",
                       "删除基本信息失败",
                       "确认"
                      };
                      
    move(20, 4);
    printf("请输入您所要删除的基本信息的客房编号：");
    scanf("%s", name);
    getchar();

    while(pt != NULL)
    {   /*根据商品名称查找商品基本信息结点pi,以及pi的前驱pi2*/
        pi = pt->inext;
        while(pi != NULL)
        {
            if(strcmp(name, pi->number) == 0)
            {
                if(pt->inext == pi)      /*判断是否为头结点*/
                {
                    pt->inext = pi->next;
                }
                else
                {
                    pi2->next = pi->next;
                }

                pc = pi->snext;    /*释放此商品对应的销售信息结点的存储空间*/
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
   /*若没有符合条件的基本信息结点，提示删除失败*/

    ShowModule(plabel_name2, 3);
    ClearScreen();
    ShowMenu();
    
    return;
}


/**
 * 函数名称: InsertClientNode
 * 函数功能: 在十字链表中插入一个客人入住信息结点.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void InsertClientNode(TYPE_NODE* hd)
{
    TYPE_NODE *pt = hd;
    ITEM_NODE *pi;
    CLIENT_NODE  *p;
    char number[4];
    int find=0;
    
    char *plabel_name2[] = {"插入客人入住信息成功。",
                           "确认"
                           };
    char *plabel_name1[] = {"该客房不存在。",
                           "插入客人入住信息失败。",
                           "确认"
                           };
                           
    move(28, 4);
    printf("请输入您要添加的客人信息：");/*根据客人信息查找对应的基本信息结点*/
    p = (CLIENT_NODE*)malloc(sizeof(CLIENT_NODE));
    move(28, 6);
    printf("客人入住客房编号: ");/*客人入住客房编号查找对应的基本信息结点*/
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
        }/*根据客房编号查找对应的基本信息结点*/

    if(find == 0)
    {
        ShowModule(plabel_name1, 3);
        ClearScreen();
        ShowMenu();

        return;
    }
    strcpy(p->number, pi->number);

    printf("\n\t\t\t   客人身份证号（18位）：");/*客人身份证号（18位）*/
	scanf("%s",p->people_id);
	getchar();
	printf("\n\t\t\t   客人姓名：");/*客人姓名*/
	scanf("%s",p->people_name);
	getchar();
	printf("\n\t\t\t   入住时间（例2015/03/05-13:00）：");/*入住时间（例2015/03/05-13:00）*/
	scanf("%s",p->time_in);
	getchar(); 
	printf("\n\t\t\t   退房时间（例2015/03/06-11:00）：");/*退房时间（例2015/03/06-11:00）*/
	scanf("%s",p->time_out);
	getchar(); 
	printf("\n\t\t\t   入住月数：");/*入住月数*/
	scanf("%f",&p->month);
	getchar();
	printf("\n\t\t\t   押金：");/*押金*/
	scanf("%f",&p->money);
	getchar();
	printf("\n\t\t\t   应缴金额：");/* 应缴金额*/
	scanf("%f",&p->should);
	getchar();
	printf("\n\t\t\t   实缴金额：");/* 实缴金额*/
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
 * 函数名称: ModifClientNode
 * 函数功能: 从十字链表中修改指定的基本信息结点.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明: 根据客房编号以及客人姓名查找并修改信息
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
    
    char *plabel_name1[] = {"修改客人入住信息成功。",
                       "确认"
                      };
    char *plabel_name2[] = {"修改客人入住信息失败。",
                       "确认"
                      };
                      
    move(24, 3);
    printf("请输入您要修改的客人入住信息的客房编号：");/*根据客人入住信息的客房编号查找客人入住信息节点*/
    scanf("%s", number);
    getchar();
    
    move(24, 5);
    printf("请输入您所要修改的客人入住信息的姓名：");/*根据客人入住信息的姓名查找客人入住信息节点*/
	scanf("%s",name);
	getchar();
	
    while(pt != NULL && flag != 1)
    {   /*根据客房编号查找客人入住信息节点*/
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
        printf("此客人不存在");
        move(24,9);
        printf("按任意键继续...");
        getch();
        ShowModule(plabel_name2, 2);
        ClearScreen();
        ShowMenu();
        return;
    }

        move(24,7);
        printf("请修改客人身份证号（18位）："); /*请修改客人身份证号（18位）*/
        scanf("%s",pc->people_id);
        getchar(); 
        move(24,8);
        printf("请修改客人姓名：");  /*请修改客人姓名*/
        scanf("%s",pc->people_name);
        getchar(); 
		move(24,9);
		printf("请修改客人入住时间：");  /*请修改客人入住时间*/
		scanf("%s",pc->time_in); 
		getchar(); 
		move(24,10);
		printf("请修改客人退房时间：");  /*请修改客人退房时间*/
		scanf("%s",pc->time_out); 
		getchar(); 
		move(24,11);
		printf("请修改客人入住月数：");  /*请修改客人入住月数*/
		scanf("%f",&pc->month); 
		getchar();  
		move(24,12);
		printf("请修改押金：");  /*根据客房编号查找客人入住信息节点*/
		scanf("%f",&pc->money); 
		getchar(); 
		move(24,13);
		printf("请修改客人应缴金额：");  /*修改客人应缴金额*/
		scanf("%f",&pc->should); 
		getchar(); 
		move(24,14);
		printf("请修改客人实缴金额：");  /*请修改客人实缴金额*/
		scanf("%f",&pc->reality); 
		getchar(); 
		
		ClearScreen();
        ShowMenu();
		ShowModule(plabel_name1, 2);
		
        return;          
}

/**
 * 函数名称: DelClientNode
 * 函数功能: 从十字链表中删除指定的基本信息结点.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明: 根据客房编号以及客人姓名查找并删除信息
 */
void DelClientNode(TYPE_NODE *hd)
{
    TYPE_NODE *pt = hd;
    ITEM_NODE *pi;
    CLIENT_NODE *pc, *pc2;
    char id[4];
    char name[20];
    int flag = 0;
    
    char *plabel_name1[] = {"删除客人入住信息成功",
                       "确认"
                      };
    char *plabel_name2[] = {"此客房或客人不存在",
                       "删除客人入住信息失败",
                       "确认"
                      };

    move(20, 4);
    printf("请输入您所要删除的客人入住信息的客房编号：");/*客人入住信息的客房编号，查找客人入住信息结点*/
    scanf("%s", id);
    getchar();
    move(20, 6);
    printf("请输入您所要删除的客人入住信息的姓名：");/*客人入住信息的姓名，查找客人入住信息结点*/
	scanf("%s",name) ;
	getchar();

    while(pt != NULL && flag != 1)
    { /*遍历链表，查找客人入住信息结点*/
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
                        pi->snext = pc->next;  /*判断是否为头结点*/
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
 * 函数名称: SeekType_id
 * 函数功能: 根据输入的客房类别查找分类信息
 * 输入参数: head 主链头指针
 * 输出参数: 满足条件的客房分类信息
 * 返 回 值: 无
 *
 * 调用说明:
 */
void SeekType_id(TYPE_NODE *head)
{
    char type_Id;
    TYPE_NODE *find;
    
    move(25, 5);
    printf("请输入客房类别:");/*请输入客房类别*/
    scanf("%c", &type_Id);
    getchar();
    
    for(find = gp_head;find != NULL;find = find->next)
        if(find->type_id == type_Id)	
        {   /*遍历分类信息链表，根据分类编码查找分类信息结点*/
            break;
		}   
    
    if(find != NULL)
    {
    	move(25,6);
    	printf("最多入住人数:%d",find->people_max);/*最多入住人数*/
		move(25,7);
		printf("客房套数:%d",find->room_number);/*客房套数*/
		move(25,8);
		printf("客房未住套数:%d",find->room_no);/*客房未住套数*/
    }
    else 
	{
		move(25,6);
		printf("此分类编码不存在");/*此分类编码不存在*/
	}
	
    move(25,9);
    printf("按任意键继续...");
    getch();
    ClearScreen();
    ShowMenu();
    
    return;
}

/**
 * 函数名称: SeekType
 * 函数功能: 遍历分类信息链表，输出所有分类信息
 * 输入参数: head 主链头指针
 * 输出参数: 所有的客房分类信息
 * 返 回 值: 无
 *
 * 调用说明:
 */
void SeekType(TYPE_NODE *head)
{
    TYPE_NODE *pt = head;
    int node_num = 0;
    
    printf("\n\t\t\t\t客房类别\t最多入住\t客房套数\t未住套数\n");
    while(pt != NULL)
    {
        if(node_num == 15)/*每页输出15条*/
        {
            printf("\n\t\t\t\t按任意键切换至下一页...");
            getch();
            ClearScreen();
            ShowMenu();
            printf("\n\t\t\t\t客房类别\t最多入住\t客房套数\t未住套数\n");
            node_num = 0;
        }
        printf("\t\t\t\t%-8c\t%-8d\t%-8d\t%-8d\n", pt->type_id, pt->people_max,pt->room_number,pt->room_no);
        pt = pt->next;
        node_num ++;
    }

    printf("\n\t\t\t\t按任意键继续...");
    getch();
    ClearScreen();
    ShowMenu();
    
    return;
}
/**
 * 函数名称: SeekNumber
 * 函数功能: 按客房编号查询客房基本信息 
 * 输入参数: head 主链头指针
 * 输出参数: 该编号下的客房基本信息 
 * 返 回 值: 无
 *
 * 调用说明:
 */
void SeekNumber(TYPE_NODE *head)
{
    TYPE_NODE *pt;
    ITEM_NODE *pi;
    char number[4];
    int flag = 0;
    
    pt = head;
    move(30, 2);
    printf("请输入需要查询的客房编号："); /*输入需要查询的客房编号*/
    scanf("%s", number);
    getchar();
    while(pt != NULL)
    {
        pi = pt->inext;

        while(pi != NULL)
        {
            if(strstr(pi->number, number))   /*查找基本信息结点*/
            {
            	move(30, 4);
            	printf("该客房电话号码：%s",pi->phone); /*客房电话号码*/
            	move(30, 5);
            	printf("该客房类型：%c",pi->type_id); /*客房类型*/
            	move(30, 6);
            	printf("该客房面积：%f",pi->area); /*客房面积*/
            	move(30, 7);
            	printf("该客房每月租金：%f",pi->sale); /*客房每月租金*/
            	move(30, 8);
            	printf("该客房是否有阳台：%c",pi->balcony); /*客房是否有阳台*/
            	move(30, 9);
            	printf("该客房是否有客人：%c",pi->human); /*客房是否有客人*/
                flag = 1;  
            }pi = pi->next;
        }pt = pt->next;
    }   /*遍历链表，输出符合条件的基本信息*/


    if(flag == 0)
    {
    	move(30,4);
        printf("查不到该客房编号\n");
    }
    move(30,10);

    printf("按任意键继续...");
    getch();
    ClearScreen();
	ShowMenu();
	
	return;
}

 /**
 * 函数名称: SeekId_Price
 * 函数功能: 按类别和每月租金查找客房基本信息
 * 输入参数: hd 主链头指针
 * 输出参数: 所有满足条件的客房基本信息
 * 返 回 值: 无
 *
 * 调用说明:
 */
void SeekId_Price(TYPE_NODE *head)
{
    TYPE_NODE *pt = head;
    ITEM_NODE *pi;
    char id;
    float sale_a;
    int flag = 0;

    move(30, 2);
    printf("请输入客房类别："); /*输入客房类别*/
    scanf("%c", &id);
    getchar();
    move(30, 4);
    printf("请输入需要查询的每月租金："); /*输入需要查询的每月租金*/
    scanf("%f",&sale_a);
    getchar();
    printf("\n\t客房编号\t电话号码\t客房类别\t客房面积\t每月租金\t是否有阳台\t是否有人入住\n");
    for(; pt!=NULL && pt->type_id!=id; pt = pt->next);

    if(pt == NULL)
    {
        printf("\n\t\t\t     此客房类别不存在\n");
        printf("\n\t\t\t     按任意键继续...");
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
            {  /*查找处于价格区间内的基本信息结点（包括端点）*/
                printf("\t%-8s\t%-8s\t%-8c\t%-8.0f\t%-8.0f\t%-10c\t%-12c\t\n",pi->number,pi->phone,pi->type_id,pi->area,pi->sale,pi->balcony,pi->human);
                flag = 1;  
            }pi = pi->next;
        }
    }

    if(flag == 0)
    {
        printf("\n\t\t\t     无符合条件的客房\n");
    }

    printf("\n\t\t\t     按任意键继续...");
    getch();
    ClearScreen();
	ShowMenu();
	
	return;
}


 /**
 * 函数名称: Seekpeople_id
 * 函数功能: 按客人身份证号码销查找客房租房信息
 * 输入参数: head 主链头指针
 * 输出参数: 所有满足条件的客人租房信息
 * 返 回 值: 无
 *
 * 调用说明:
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
    printf("\t\t\t\t请输入客人的身份证号码（18位）： ");/*输入客人的身份证号码*/
    scanf("%s", people_ID);
    getchar();
    while(pt != NULL)
    {   /*遍历链表，输出符合条件的销售信息*/
        pi = pt->inext;
        while(pi != NULL)
        {
            pc = pi->snext;
            while(pc != NULL)
            {
                if(strcmp(people_ID, pc->people_id) == 0)
                {
                    if(node_num ==1)/*每页输出1条*/
                    {
                        printf("\n\t\t\t\t按任意键切换至下一页...");
                        getch();
                        ClearScreen();
                        ShowMenu();
                        node_num = 0;
                    }
                    flag = 1;
                    printf("\n\n\n\n\n") ;
                    printf("\t\t\t\t客人身份证号码：%s\n",pc->people_id);/*客人身份证号码*/
                    printf("\t\t\t\t客人姓名：%s\n",pc->people_name);/*客人姓名*/
                    printf("\t\t\t\t入住客房编号：%s\n",pc->number);/*入住客房编号*/
                    printf("\t\t\t\t客人入住时间：%s\n",pc->time_in);/*客人入住时间*/
                    printf("\t\t\t\t客人退房时间：%s\n",pc->time_out);/*客人退房时间*/
                    printf("\t\t\t\t客人入住月数：%f\n",pc->month);/*客人入住月数*/
                    printf("\t\t\t\t押金：%f\n",pc->money);/*押金*/
                    printf("\t\t\t\t客人应缴金额：%f\n",pc->should);/*客人应缴金额*/
                    printf("\t\t\t\t客人实缴金额：%f\n",pc->reality);/*客人实缴金额*/
                    node_num++;
                }pc = pc->next;
            }pi = pi->next;
        }pt = pt->next;
    }

    if(flag == 0)
    {
        printf("\n\t\t\t无符合条件的客人租房信息\n");
    }

    printf("\n\t\t\t\t按任意键继续...");
    getch();
    ClearScreen();
    ShowMenu();
    
    return;
}

 /**
 * 函数名称: SeekName_Time
 * 函数功能: 按客人的姓或名及入住时间范围查找客人租房信息
 * 输入参数: head 主链头指针
 * 输出参数: 所有满足条件的客人入住信息
 * 返 回 值: 无
 *
 * 调用说明:
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
    printf("请输入客人姓名: ");/*按客人的姓或名及入住时间范围查找客人租房信息*/ 
    scanf("%s", name);
    getchar(); 
    move(30, 4);
    printf("请输入需要查询的时间（2015/03/05-13:00）: ");/*按客人的姓或名及入住时间范围查找客人租房信息*/ 
    scanf("%s", time);
    getchar();

    while(pt != NULL)
    {   /*遍历链表，输出符合条件的销售信息*/
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
                    if(node_num == 1)/*每页输出1条*/
                    {
                        printf("\n\t\t\t\t\t按任意键切换至下一页...");
                        getch();
                        ClearScreen();
                        ShowMenu();
                        node_num = 0;
                    }
                    f = 1;
                    printf("\t\t\t\t\t客人身份证号码：%s\n",pc->people_id);/*客人身份证号码*/
                    printf("\t\t\t\t\t客人姓名：%s\n",pc->people_name);/*客人姓名*/
                    printf("\t\t\t\t\t入住客房编号：%s\n",pc->number);/*入住客房编号*/
                    printf("\t\t\t\t\t客人入住时间：%s\n",pc->time_in);/*客人入住时间*/
                    printf("\t\t\t\t\t客人退房时间：%s\n",pc->time_out);/*客人退房时间*/
                    printf("\t\t\t\t\t客人入住月数：%f\n",pc->month);/*客人入住月数*/
                    printf("\t\t\t\t\t押金：%f\n",pc->money);/*押金*/
                    printf("\t\t\t\t\t客人应缴金额：%f\n",pc->should);/*客人应缴金额*/
                    printf("\t\t\t\t\t客人实缴金额：%f\n",pc->reality);/*客人实缴金额*/
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
        printf("无符合条件的客人入住信息\n");
    }
    printf("\n\t\t\t\t\t按任意键继续...");
    getch();
    ClearScreen();
    ShowMenu();
    
    return;
}


 /**
 * 函数名称: StatTyperuzhu
 * 函数功能: 按类别统计所有类别总体的入住情况
 * 输入参数: 无
 * 输出参数: 所有满足条件的客人入住信息
 * 返 回 值: 无
 *
 * 调用说明:
 */
void StatTyperuzhu(void)
{	
	TYPE_NODE *pt;
    ITEM_NODE *pi;
    CLIENT_NODE *ps;
    TYPE_STAT *head;
    char time[18];

    printf("\n\t\t请输入需要统计的时间：");
    scanf("%s",time);
    getchar();
    head = StatTypeInfo(gp_head, time);  /*调用函数StatTypeInfo,生成分类信息统计链表*/
    printf("\n\t\t客房类别\t客房总数\t已入住数\t未入住数\n");
    while(head !=NULL)
    {

        printf("\t\t%-8c\t%-8d\t%-8d\t%8d\n", head->type_id, head->room_number, head->room_y,head->room_n);
        head = head->next;

    }

    printf("\n\t\t按任意键继续...");
    getch();
    ClearScreen();
    ShowMenu();
    
    return;
}
 /**
 * 函数名称: StatTypeInfo
 * 函数功能: 生成分类信息统计链表
 * 输入参数: head 主链头指针
 * 输出参数: 无
 * 返 回 值: 链头指针phead
 *
 * 调用说明:
 */
TYPE_STAT * StatTypeInfo(TYPE_NODE *head,char time[18])
{
    TYPE_STAT *p1, *phead;
    ITEM_NODE *pi;
    CLIENT_NODE *ps;
    char time_a[18]="0";

    phead = (TYPE_STAT *)malloc(sizeof(TYPE_STAT));/*建立头结点*/
    p1 = phead;
    /*建立带头指针的先进后出的统计链表*/
    while(head != NULL)
    {
        p1->next = (TYPE_STAT *)malloc(sizeof(TYPE_STAT));/*建立新的结点*/
        p1 = p1->next;
        p1->type_id = head->type_id;
        p1->room_number = head->room_number;
        p1->room_n = p1->room_number;
        p1->room_y = 0;
        pi = head->inext;
        while(pi != NULL) /*遍历支链，统计入住与未入住客房数*/
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
                }ps = ps->next; /*指向下一个结点*/
            }pi = pi->next; /*指向下一个结点*/
        }head = head->next;  /*统计下一个分类的信息*/
    }
    p1->next = NULL;
    phead = phead->next;

    return phead;
}

 /**
 * 函数名称: StatMonthYear
 * 函数功能: 按年份统计每个月的各类客房销售业绩 
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void StatMonthYear(void)
{
    MONTH_STAT * head;
    char year[5];
    
    printf("\n\t\t\t请输入需要查询的年份：");
    scanf("%s", year); /*获取年份，确定统计范围*/
    getchar();
    head = StatYearInfo(gp_head, year);   /*调用StatYearInfo，生成统计结果链表*/
    printf("\n\t\t\t月份\t单人间\t双人间\t三人间");
    while(head !=NULL)
    {  /*遍历链表，输出统计结果*/

        printf("\n\t\t\t%-4d\t%-6.0f\t%-6.0f\t%-6.0f", head->month, head->s, head->d, head->t);
        head = head->next;
    }

    printf("\n\t\t\t按任意键继续...");
    getch();
    ClearScreen();
    ShowMenu();
    
    return;
}



 /**
 * 函数名称: StatYearInfo
 * 函数功能: 按年统计每个月的客房出租情况并创建统计结果链表
 * 输入参数: head 主链头指针
 * 输出参数: 无
 * 返 回 值: 统计得到的客房情况链表的链头指针phead
 *
 * 调用说明:
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
    /*建立带头指针的先进先出的统计结果链表*/
    for(j=1;j<13;j++)
    {
    	pt=head;
    	p1->next = (MONTH_STAT *)malloc(sizeof(MONTH_STAT));/*新建统计结果结点*/
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
    phead = phead->next; /*处理链表头指针和尾指针*/
    return phead; /*返回结果链表的头指针*/
}

 /**
 * 函数名称: StatYearruzhu
 * 函数功能: 按年份统计各种客房的入住情况
 * 输入参数: 无
 * 输出参数: 无 
 * 返 回 值: 无
 *
 * 调用说明:
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
	printf("\n\t请输入需要查询营业额的年份：");
	scanf("%s",year);
	getchar();
	while(pt != NULL)
    {
        pi = pt->inext;
        while(pi != NULL)
        {
        	pc->next = (NUMBER_STAT *)malloc(sizeof(NUMBER_STAT));/*新建统计结果结点*/
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
    phead = phead->next; /*处理链表头指针和尾指针*/
    
    printf("\t客房编号\t营业额\t入住月数\t入住率");
    while(phead!=NULL){
    	printf("\n\t%-8s\t%-6.0f\t%-8d\t%-6.2f",phead->number,phead->sale,phead->month,phead->ruzhu);
    	phead =phead->next;
	} 
	printf("\n\t按任意键继续...");
    getch();
    ClearScreen();
    ShowMenu();
    
    return;
}

 /**
 * 函数名称: StatClient1
 * 函数功能: 按时间统计在该时间中客人的住宿情况 
 * 输入参数: 无
 * 输出参数: 所有客人的住宿缴费情况 
 * 返 回 值: 无
 *
 * 调用说明:
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
    printf("请输入统计时间:");
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
				pk->next = (CLIENT_STAT *)malloc(sizeof(CLIENT_STAT));/*新建统计结果结点*/
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
    phead = phead->next; /*处理链表头指针和尾指针*/
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
	printf("身份证号        \t姓名            \t住宿月数\t应缴费用\t实缴费用");
	while(phead!=NULL){
    	printf("\n%-20s\t%-20s\t%-8d\t%-8.0f\t%-8.0f",phead->people_id,phead->people_name,phead->month,phead->should,phead->should);
    	phead =phead->next;
	} 
	printf("\n按任意键继续...");
    getch();
    ClearScreen();
    ShowMenu();
    
    return;

}

 /**
 * 函数名称: StatClient2
 * 函数功能: 按时间统计各个客户的客房住宿情况 
 * 输入参数: 无
 * 输出参数: 十个住宿月数最多的客人的住宿信息
  
 * 返 回 值: 无
 *
 * 调用说明:
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
    printf("请输入统计时间:");/*输入统计时间*/
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
				pk->next = (CLIENT_STAT *)malloc(sizeof(CLIENT_STAT));/*新建统计结果结点*/
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
    phead = phead->next; /*处理链表头指针和尾指针*/	
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
    /*根据购买重量通过交换数据域的方式进行冒泡排序*/
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
	
	printf("身份证号        \t姓名            \t住宿月数\t应缴费用\t实缴费用");
	while(phead!=NULL && k!=10){
    	printf("\n%-20s\t%-20s\t%-8d\t%-8.0f\t%-8.0f",phead->people_id,phead->people_name,phead->month,phead->should,phead->should);
    	phead =phead->next;
    	k++;
	} 
	printf("\n按任意键继续...");
    getch();
    ClearScreen();
    ShowMenu();
    
    return;
}

/**
 * 函数名称: SaveSysData
 * 函数功能: 保存系统代码表和三类基础数据.
 * 输入参数: hd 主链头结点指针
 * 输出参数:
 * 返 回 值: BOOL类型, 总是为TRUE
 *
 * 调用说明:
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
        /*保存客房分类信息*/
        fwrite(ptype_node, sizeof(TYPE_NODE), 1, pfout);
    }
    fclose(pfout);

    pfout = fopen(gp_item_info_filename, "wb");
    for(ptype_node = hd; ptype_node != NULL; ptype_node = ptype_node->next)
    {
        /*保存客房基本信息*/
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
        /*保存客人入住信息*/
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
 * 函数名称: BackupSysData
 * 函数功能: 将系统代码表和三类基础数据备份到一个数据文件.
 * 输入参数: hd 主链头结点指针
 *           filename 数据文件名
 * 输出参数:
 * 返 回 值: BOOL类型, 总是为TRUE
 *
 * 调用说明:
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

	/* 遍历十字链表，分别统计三种基础信息的记录总数 */
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

	/* 保存三类基础数据的记录总数 */
    write(handle, (char *)&type_node_num, sizeof(type_node_num));
    write(handle, (char *)&item_node_num, sizeof(item_node_num));
    write(handle, (char *)&client_node_num, sizeof(client_node_num));

    for(ptype_node = hd; ptype_node != NULL; ptype_node = ptype_node->next)
    {
        /*保存客房类信息*/
        write(handle, (char *)ptype_node, sizeof(TYPE_NODE));
    }

    for(ptype_node = hd; ptype_node != NULL; ptype_node = ptype_node->next)
    {
        /*保存客房基本信息*/
        pitem_node = ptype_node->inext;
        while(pitem_node != NULL)
        {
            write(handle, (char *)pitem_node, sizeof(ITEM_NODE));
            pitem_node = pitem_node->next;
        }
    }

    for(ptype_node = hd; ptype_node != NULL; ptype_node = ptype_node->next)
    {
        /*保存客人入住信息*/
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
 * 函数名称: RestoreSysData
 * 函数功能: 从指定数据文件中恢复三类基础数据.
 * 输入参数: phead 主链头结点指针的地址
 *           filename 存放备份数据的数据文件名
 * 输出参数:
 * 返 回 值: BOOL类型, 总是为TRUE
 *
 * 调用说明:
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

   /*读取三种基本数据信息的记录数*/
    read(handle, (char *)&type_node_num, sizeof(type_node_num));
    read(handle, (char *)&item_node_num, sizeof(item_node_num));
    read(handle, (char *)&client_node_num, sizeof(client_node_num));

    /*读取客房分类信息，建立十字主链*/
    for(ulloop = 1; ulloop <= type_node_num; ulloop++)
    {
        ptype_node = (TYPE_NODE *)malloc(sizeof(TYPE_NODE));
        read(handle, (char *)ptype_node, sizeof(TYPE_NODE));
        ptype_node->inext = NULL;
        ptype_node->next = hd;
        hd = ptype_node;
    }
    *phead = hd;

    /*读取客房基本信息，建立客房信息支链*/
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

    /*读取客人入住信息，建立客人入住信息链表*/
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
    SaveSysData(hd);    /*将内存中数据保存到数据文件*/

    return TRUE;
}

 /**
 * 函数名称: ShowModule
 * 函数功能: 展示小窗口函数 
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
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

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
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
 * 函数名称: ShowModule3
 * 函数功能: 将含有2个字符串与1个"返回"字符串的标签束设置成热区并等待用户输入
 * 输入参数: pString 指针，n标签个数
 * 输出参数: 无
 * 返 回 值: 用鼠标单击、按回车或空格时返回当前热区编号
 *
 * 调用说明:
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

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
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
 * 函数名称: ShowModule4
 * 函数功能: 将含有3个字符串与1个"返回"字符串的标签束设置成热区并等待用户输入
 * 输入参数: pString 指针，n标签个数
 * 输出参数: 无
 * 返 回 值: 用鼠标单击、按回车或空格时返回当前热区编号
 *
 * 调用说明:
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

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
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


