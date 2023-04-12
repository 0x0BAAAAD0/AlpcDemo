// AlpcDoRunAdminProcess.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include "..\AlpcPubInclude\UserModeDefs.h"
#include "..\AlpcPubInclude\ntlpcapi.h"
#include "..\AlpcPubInclude\ALPC.h"
#include"..\AlpcPubInclude\AlpcData.h"
#pragma comment(lib,"ntdll.lib")

void AlpcDoRunAdminProcess()
{
    WCHAR* ServerName = (WCHAR*)EXECUAC_ALPC_NAME;
    HANDLE ConnectionHandle = 0;
    UNICODE_STRING usPortName;
    NTSTATUS status;
    OBJECT_ATTRIBUTES oa;

    ULONG BufferLength = sizeof(PORT_MESSAGE);
    ALPC_PORT_ATTRIBUTES apa;
    ALPC_MESSAGE_ATTRIBUTES ama;
    ALPC_MESSAGE pMessageSendRecv;
    memset(&pMessageSendRecv, 0, sizeof(ALPC_MESSAGE));
    PALPC_MESSAGE_ATTRIBUTES pMsgAttrSendRecv;
    SIZE_T dwRecvBufferLen;

    apa.Flags = 0x10000;
    apa.SecurityQos.Length = sizeof(SECURITY_QUALITY_OF_SERVICE);
    apa.SecurityQos.ImpersonationLevel = DEFAULT_IMPERSONATION_LEVEL;
    apa.SecurityQos.ContextTrackingMode = SECURITY_DYNAMIC_TRACKING;
    apa.SecurityQos.EffectiveOnly = FALSE;

    apa.MaxMessageLength = 0x1000;
    apa.MemoryBandwidth = 0;
    apa.MaxPoolUsage = -1;
    apa.MaxSectionSize = -1;
    apa.MaxViewSize = -1;
    apa.MaxTotalSectionSize = -1;
    apa.DupObjectTypes = 0;
    apa.Reserved = 0;

    ama.AllocatedAttributes = 0;
    ama.ValidAttributes = 0;
    InitializeObjectAttributes(&oa, 0, 0, 0, NULL);
    RtlInitUnicodeString(&usPortName, ServerName);

    status = NtAlpcConnectPort(&ConnectionHandle,
        &usPortName,
        &oa,
        &apa,//PortAttributes
        ALPC_MSGFLG_SYNC_REQUEST,    //Flags
        0,    //RequiredServerSid
        0,
        0,
        0,    //OutMessageAttributes
        &ama,    //InMessageAttributes
        0);    //timeout
    if (!NT_SUCCESS(status))
    {
        printf("NtAlpcConnectPort error:%X", status);
        return;
    }
    pMsgAttrSendRecv = MakeRPCTestMsgAttr(ALPC_MESSAGE_CONTEXT_ATTRIBUTE);
    dwRecvBufferLen = sizeof(ALPC_MESSAGE);
    memset(&pMessageSendRecv, 0, sizeof(ALPC_MESSAGE));
    AppendDataToMsg((PPORT_MESSAGE)&pMessageSendRecv, g_pRunUACBindDataToSend, sizeof(g_pRunUACBindDataToSend));
    status = NtAlpcSendWaitReceivePort(
        ConnectionHandle,
        ALPC_MSGFLG_SYNC_REQUEST,
        (PPORT_MESSAGE)&pMessageSendRecv,
        pMsgAttrSendRecv,
        (PPORT_MESSAGE)&pMessageSendRecv,
        &dwRecvBufferLen,
        pMsgAttrSendRecv,
        0);

    HeapFree(GetProcessHeap(), 0, pMsgAttrSendRecv);

    pMsgAttrSendRecv = MakeRPCTestMsgAttr(ALPC_MESSAGE_VIEW_ATTRIBUTE | ALPC_MESSAGE_CONTEXT_ATTRIBUTE);
    dwRecvBufferLen = sizeof(ALPC_MESSAGE);
    memset(&pMessageSendRecv, 0, sizeof(ALPC_MESSAGE));
    AppendDataToMsg((PPORT_MESSAGE)&pMessageSendRecv, g_pRunUACDataToSend, sizeof(g_pRunUACDataToSend));
    status = NtAlpcSendWaitReceivePort(
        ConnectionHandle,
        ALPC_MSGFLG_SYNC_REQUEST,
        (PPORT_MESSAGE)&pMessageSendRecv,
        pMsgAttrSendRecv,
        (PPORT_MESSAGE)&pMessageSendRecv,
        &dwRecvBufferLen,
        pMsgAttrSendRecv,
        0);

    PPROCESS_INFORMATION pProcInfo = (PPROCESS_INFORMATION)((PBYTE)&pMessageSendRecv + 0x40);
    ResumeThread(pProcInfo->hThread);
    HeapFree(GetProcessHeap(), 0, pMsgAttrSendRecv);
}

int main()
{
    AlpcDoRunAdminProcess();
    std::cout << "Hello World!\n";
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
