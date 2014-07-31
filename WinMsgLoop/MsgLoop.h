#pragma  once
 
#include <Windows.h>

#include <map>
#include <cassert>
#include <vector>
#include <functional>


class WinMsgLoop
{
public:
    WinMsgLoop();
    ~WinMsgLoop();

    bool AddHandle( HANDLE hHandle,std::function<void(HANDLE)> cbHandle );
    bool DelHandle( HANDLE hHandle );

    int RunMsgLoop( HACCEL hAccelTable );
private:
    bool IsHandleExit( HANDLE hHandle );
private:
    std::vector<std::pair<HANDLE,std::function<void(HANDLE)>>> m_vecHandle;
};