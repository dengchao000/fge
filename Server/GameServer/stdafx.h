// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <winsock2.h>
#include <windows.h>

#include <strstream>
#include <algorithm>
#include <deque>
#include <functional>
#include <vector>
#include <crtdbg.h>

#include "fgeUtil.h"
#include "fgeLog.h"
#include "fgeLzCompress.h"
#include "fgeSharedObject.h"
#include "fgeMath.h"
#include "fgeList.h"
#include "fgeMessageManager.h"
#include "fgeClock.h"

#include "Typedef\PlayerDef.h"
#include "../ServerFrame/ServerObject.h"
#include "fgeNet/oclSocket.h"
#include "fgeNet/oclFrameProtocol.h"
#include "fgeNet/oclProtocol.h"
#include "Protocol/BaseProtoco.h"
#include "Protocol/GameProtocolName.h"
#include "Protocol/GameProtocol.h"

#include "Net/NetMsg.h"
#include "GameObject.h"
using namespace fge;
