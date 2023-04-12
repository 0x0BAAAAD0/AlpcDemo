// AlpcDoMyRPCClient.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include "..\AlpcPubInclude\UserModeDefs.h"
#include "..\AlpcPubInclude\ntlpcapi.h"
#include "..\AlpcPubInclude\ALPC.h"
#include"..\AlpcPubInclude\AlpcData.h"
#include"..\AlpcPubInclude\RpcInsideHeader.h"
#pragma comment(lib,"ntdll.lib")

void AlpcDoMyRPCClient()
{
    WCHAR* ServerName = (WCHAR*)MYTEST_ALPC_NAME;
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
    //以下都根据Windbg调试系统的调用得出的参数
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
    AppendDataToMsg((PPORT_MESSAGE)&pMessageSendRecv, g_pTestBindDataToSend, sizeof(g_pTestBindDataToSend));
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
    AppendDataToMsg((PPORT_MESSAGE)&pMessageSendRecv, g_pTestParamDataToSend, sizeof(g_pTestParamDataToSend));
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
}

int main()
{
    DWORD dwSize = sizeof(rpcconn_bind_hdr_t);
    rpcconn_bind_hdr_t* pBindHeader = 0;
    rpcconn_request_hdr_t* pReqHeader = (rpcconn_request_hdr_t *)((char*)g_pRunUACDataToSend-2);
    DWORD dwSize2 = sizeof(rpcconn_request_hdr_t);
    AlpcDoMyRPCClient();
    std::cout << "Hello World!\n";
}
