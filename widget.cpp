#include "widget.h"
#include "ui_widget.h"

#include <windows.h>
#include <QWindow>
#include <QDebug>

#include <QProcess>

#include <QFileDialog>
#include <QDateTime>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    connect(ui->test,&QPushButton::clicked,this,&Widget::test);
    ui->label->resize(640,480);


}

Widget::~Widget()
{
    delete ui;
}


HANDLE handle;//process handle
HWND apphwnd;//window handle

/*************Global functions for hosting******************/
//Function to enumerate all windows.
int CALLBACK EnumWindowsProc(HWND hwnd, LPARAM param)
{
    DWORD pID;
    DWORD TpID = GetWindowThreadProcessId(hwnd, &pID);//get process id
    if (TpID == (DWORD)param)
    {
        apphwnd = hwnd;//hwnd is the window handle
        return false;
    }
    return true;
}


//Functio to start a orocess and return the process handle
HANDLE StartProcess(LPCTSTR program, LPCTSTR args)
{
     HANDLE hProcess = NULL;
     PROCESS_INFORMATION processInfo;
     STARTUPINFO startupInfo;
     ::ZeroMemory(&startupInfo, sizeof(startupInfo));
     startupInfo.cb = sizeof(startupInfo);
     if(::CreateProcess(program, (LPTSTR)args,
                        NULL,  // process security
                        NULL,  // thread security
                        FALSE, // no inheritance
                        0,     // no startup flags
                        NULL,  // no special environment
                        NULL,  // default startup directory
                        &startupInfo,
                        &processInfo))
        { /* success */
            Sleep(200);
            ::EnumWindows(&EnumWindowsProc, processInfo.dwThreadId);//Iterate all windows
            hProcess = processInfo.hProcess;
        } /* success */
     return hProcess;
}

void Widget::test()
{
    //创建进程
    handle=StartProcess(L"D:/UnityProject/Draw2DLine/Draw2DLine.exe",L"");//Start ms paint
    if(apphwnd!=NULL)//check for window handle
    {
        QWindow *m_window = nullptr;
        m_window = QWindow::fromWinId((WId)apphwnd);
        m_window->setFlags(Qt::ForeignWindow);
        m_window->setFlags(m_window->flags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint); //
        QWidget *m_widget;
        m_widget = QWidget::createWindowContainer(m_window,this->ui->label);
        m_widget->setMinimumSize(640,480);
        m_widget->show();
    }else{
        qDebug()<<"Cannot find Window";
    }
}

void Widget::closeEvent(QCloseEvent *event)
{
    TerminateProcess(handle,0);	//kill the process using handle
    if (!WaitForSingleObject(handle,INFINITE))// 等待程序关闭
    {
        qDebug()<<(" closed!");
    }
}
