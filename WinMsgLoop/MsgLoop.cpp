#include "stdafx.h"
#include "MsgLoop.h"

WinMsgLoop::WinMsgLoop()
{
}

WinMsgLoop::~WinMsgLoop()
{
}

bool WinMsgLoop::AddHandle( HANDLE hHandle,std::function<void(HANDLE)> cbHandle )
{
    bool bAddResult = false;
    if ( !IsHandleExit(hHandle) ) 
    {
        m_vecHandle.push_back( std::pair<HANDLE,std::function<void(HANDLE)>>(hHandle,cbHandle) );
        bAddResult = true;
    }
    return bAddResult;
}

bool WinMsgLoop::DelHandle( HANDLE hHandle )
{
    bool bDelResult = false;

    for( auto it=m_vecHandle.begin();it!=m_vecHandle.end();++it )
    {
        if ( hHandle==it->first )
        {
            it = m_vecHandle.erase( it );
            bDelResult = true;
            break;
        }
    }

    return bDelResult;
}

int WinMsgLoop::RunMsgLoop( HACCEL hAccelTable )
{
    auto GetHandle=[this]()->std::vector<HANDLE> 
    {
        std::vector<HANDLE> vecHandle;
        for( auto it=m_vecHandle.begin();it!=m_vecHandle.end();++it )
        {
            vecHandle.push_back( it->first );
        }
        return vecHandle;
    };

    MSG msg = {0};
    while( true )
    {
        const int Count = m_vecHandle.size();
        std::vector<HANDLE> vecHandle=GetHandle();
        int iRetCode=MsgWaitForMultipleObjects( Count,(HANDLE*)vecHandle._Myfirst,FALSE,-1,QS_ALLEVENTS );
        assert ( WAIT_FAILED!=iRetCode );
        if ( WAIT_TIMEOUT==iRetCode ) continue;


        if ( PeekMessage(&msg,NULL,0,0,PM_REMOVE) &&
             !TranslateAccelerator(msg.hwnd, hAccelTable,&msg) )
        {
            if ( WM_QUIT==msg.message ) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);

        }

        // OK,以下是句柄的消息
        if ( iRetCode<(int)m_vecHandle.size() )
        {
            std::pair<HANDLE,std::function<void(HANDLE)>> pair=m_vecHandle[iRetCode];
            pair.second( pair.first );
        }
    }
    return msg.wParam;
}

bool WinMsgLoop::IsHandleExit( HANDLE hHandle )
{
    bool bExit = false;
    for( auto it=m_vecHandle.begin();it!=m_vecHandle.end();++it )
    {
        if ( hHandle==it->first )
        {
            bExit = true;
            break;
        }
    }
    return bExit;
}