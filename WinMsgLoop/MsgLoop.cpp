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
        int iRetCode=MsgWaitForMultipleObjects( Count,(HANDLE*)vecHandle._Myfirst,FALSE,-1,QS_ALLEVENTS|QS_SENDMESSAGE );
        assert ( WAIT_FAILED!=iRetCode );
        if ( WAIT_TIMEOUT==iRetCode ) continue;

        if ( PeekMessage(&msg,NULL,0,0,PM_REMOVE) &&
             !TranslateAccelerator(msg.hwnd, hAccelTable,&msg) )
        {
            if ( WM_QUIT==msg.message ) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);

        }

        const int HandleSignalIndex = WAIT_OBJECT_0 + iRetCode - 1;
        assert( HandleSignalIndex>=0&&HandleSignalIndex<m_vecHandle.size() );
        HANDLE h = m_vecHandle[HandleSignalIndex].first;
        m_vecHandle[HandleSignalIndex].second( h );
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
