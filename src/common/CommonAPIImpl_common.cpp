#ifdef OS_WINDOWS
# include <windows.h>
#else
# include <winlnxdefs.h>
#endif // OS_WINDOWS
#include <assert.h>

#include <PluginAPI.h>

#include <N64.h>
#include <GLideN64.h>
#include <RSP.h>
#include <RDP.h>
#include <VI.h>
#include <Config.h>
#include <FrameBufferInfo.h>
#include <TextureFilterHandler.h>
#include <Log.h>
#include "Graphics/Context.h"
#include <DisplayWindow.h>

#include "osal_keys.h"

PluginAPI & PluginAPI::get()
{
	static PluginAPI api;
	return api;
}

#ifdef RSPTHREAD
class GLContextGuard
{
public:
	GLContextGuard()
	{
		_hasFlushControl = dwnd().hasFlushControl();
		if (_hasFlushControl)
			dwnd().enterContext();
	}

	~GLContextGuard()
	{
		if (_hasFlushControl)
			dwnd().leaveContext();
	}

private:
	bool _hasFlushControl;
};
#endif

#ifdef RSPTHREAD
#define PAGE_SIZE 4096
#define MAX_PAGES (8*1024*1024) / PAGE_SIZE
static uint16_t sUnprotectedPages[MAX_PAGES];
static uint16_t sUnprotectedPagesCount = 0;

static int32_t memoryFilter(uint32_t dwExptCode, void* lpExceptionPointer)
{
	LPEXCEPTION_POINTERS lpEP = (LPEXCEPTION_POINTERS)lpExceptionPointer;
	uint32_t MemAddress = (char*)lpEP->ExceptionRecord->ExceptionInformation[1] - (char*)RDRAM;

	if (MemAddress >= RDRAMSize)
	{
		return EXCEPTION_EXECUTE_HANDLER;
    }

	uint32_t start = MemAddress;
    uint32_t end = MemAddress + 16; // assume SSE2 will not access more than 16 bytes at once

    uint16_t startPage = start / PAGE_SIZE;
    uint16_t endPage = end / PAGE_SIZE;

	for (int page = startPage; page <= endPage; page++)
	{
		void* addr = (void*)((uintptr_t)RDRAM + page * PAGE_SIZE);
        DWORD oldProtect;
		BOOL ok = VirtualProtect(addr, PAGE_SIZE, PAGE_READWRITE, &oldProtect);
		if (ok)
		{
			if (oldProtect == PAGE_READONLY)
			{
				sUnprotectedPages[sUnprotectedPagesCount++] = page;
			}
			else
			{
				// this should never happen... something is up with protection
                VirtualProtect(addr, PAGE_SIZE, oldProtect, &oldProtect);
            }
		}
	}

	return EXCEPTION_CONTINUE_EXECUTION;
}

static void memoryProtectionInit()
{
	sUnprotectedPagesCount = 0;
}

static void memoryProtectionRollback()
{
	for (uint16_t i = 0; i < sUnprotectedPagesCount; i++)
	{
		uint16_t page = sUnprotectedPages[i];
		void* addr = (void*)((uintptr_t)RDRAM + page * PAGE_SIZE);
		DWORD oldProtect;
		VirtualProtect(addr, PAGE_SIZE, PAGE_READONLY, &oldProtect);
	}
}

struct MemoryProtectionGuard
{
	MemoryProtectionGuard()
	{
		memoryProtectionInit();
	}
	~MemoryProtectionGuard()
	{
		memoryProtectionRollback();
	}
};

static void doProcessDList()
{
	GLContextGuard lck;
	MemoryProtectionGuard memGuard;
	__try
	{
		RSP_ProcessDList();
	}
	__except (memoryFilter(_exception_code(), _exception_info()))
	{ }
}

static void doProcessRDPList()
{
	GLContextGuard lck;
	if (!Combiner_IsInit())
	{
		Combiner_Init();
	}

	MemoryProtectionGuard memGuard;
	__try
	{
		RDP_ProcessRDPList();
	}
	__except (memoryFilter(_exception_code(), _exception_info()))
	{ }
}
#endif

void PluginAPI::ProcessDList()
{
	LOG(LOG_APIFUNC, "ProcessDList\n");
#ifdef RSPTHREAD
	if (__builtin_expect(!m_executor.sync(doProcessDList), false))
	{
		RSP_ProcessDList_Trivial();
	}
#else
	RSP_ProcessDList();
#endif
}

void PluginAPI::ProcessRDPList()
{
	LOG(LOG_APIFUNC, "ProcessRDPList\n");
#ifdef RSPTHREAD
	if (__builtin_expect(!m_executor.sync(doProcessRDPList), false))
	{
		RDP_ProcessRDPList_Trivial();
	}
#else
	RDP_ProcessRDPList();
#endif
}

void PluginAPI::RomClosed()
{
	LOG(LOG_APIFUNC, "RomClosed\n");
	m_bRomOpen = false;
#ifdef RSPTHREAD
	std::lock_guard<std::mutex> lck(m_initMutex);
#if 1
	bool main = GetCurrentThreadId() == hWndThread;
	bool running = m_executor.stopAsync([&]()
	{
		TFH.dumpcache();
		dwnd().stop();
		GBI.destroy();
		if (main)
		{
			PostThreadMessage(hWndThread, WM_APP + 1, 0, 0);
		}
	});

	if (main && running)
	{
		MSG msg;
		while (GetMessage(&msg, 0, 0, 0))
		{
			if (msg.message == WM_APP + 1)
				break;

			DispatchMessage(&msg);
		}
	}

	if (running)
	{
		m_executor.stopWait();
	}
#else
	bool running = m_executor.disableTasksAndAsync([&]()
	 {
		TFH.dumpcache();
		dwnd().stop();
		GBI.destroy();
	 });
	 if (running)
	 {
		 m_executor.stop();
	 }
#endif
#else
	TFH.dumpcache();
	dwnd().stop();
	GBI.destroy();
#endif
}

void PluginAPI::RomOpen()
{
	osal_keys_init();

	LOG(LOG_APIFUNC, "RomOpen\n");
#ifdef RSPTHREAD
	std::lock_guard<std::mutex> lck(m_initMutex);
	m_executor.start([]()
	{
		RSP_Init();
		GBI.init();
		Config_LoadConfig();
		dwnd().start();
		return dwnd().hasFlushControl();
	});
#else
	RSP_Init();
	GBI.init();
	Config_LoadConfig();
	dwnd().start();
#endif
	m_bRomOpen = true;
}

void PluginAPI::Restart()
{
#ifdef RSPTHREAD
	std::lock_guard<std::mutex> lck(m_initMutex);
	if (m_bRomOpen)
	{
		bool main = GetCurrentThreadId() == hWndThread;
		bool running = m_executor.stopAsync([&]()
		{
			TFH.dumpcache();
			dwnd().stop();
			GBI.destroy();
			if (main)
			{
				PostThreadMessage(hWndThread, WM_APP + 1, 0, 0);
			}
		});

		if (!running)
			return;

		if (main)
		{
			MSG msg;
			while (GetMessage(&msg, 0, 0, 0))
			{
				if (msg.message == WM_APP + 1)
					break;

				DispatchMessage(&msg);
			}
		}

		m_executor.stopWait();
		m_executor.start([]()
		{
			RSP_Init();
			GBI.init();
			Config_LoadConfig();
			dwnd().start();
			return dwnd().hasFlushControl();
		});
	}
#else
	Config_LoadConfig();
	dwnd().restart();
#endif
}

void PluginAPI::ShowCFB()
{
	gDP.changed |= CHANGED_CPU_FB_WRITE;
}

void PluginAPI::UpdateScreen()
{
	VIRegsSample regs = sampleVI();
	LOG(LOG_APIFUNC, "UpdateScreen\n");
#ifdef RSPTHREAD
	m_executor.async([regs]()
	{
		GLContextGuard lck;
		VI_UpdateScreen(regs);
	});
#else
	VI_UpdateScreen();
#endif
}

void PluginAPI::_initiateGFX(const GFX_INFO & _gfxInfo) const {
	HEADER = _gfxInfo.HEADER;
	DMEM = _gfxInfo.DMEM;
	IMEM = _gfxInfo.IMEM;
	RDRAM = _gfxInfo.RDRAM;

	REG.MI_INTR = _gfxInfo.MI_INTR_REG;
	REG.DPC_START = _gfxInfo.DPC_START_REG;
	REG.DPC_END = _gfxInfo.DPC_END_REG;
	REG.DPC_CURRENT = _gfxInfo.DPC_CURRENT_REG;
	REG.DPC_STATUS = _gfxInfo.DPC_STATUS_REG;
	REG.DPC_CLOCK = _gfxInfo.DPC_CLOCK_REG;
	REG.DPC_BUFBUSY = _gfxInfo.DPC_BUFBUSY_REG;
	REG.DPC_PIPEBUSY = _gfxInfo.DPC_PIPEBUSY_REG;
	REG.DPC_TMEM = _gfxInfo.DPC_TMEM_REG;

	REG._VI_STATUS = _gfxInfo.VI_STATUS_REG;
	REG._VI_ORIGIN = _gfxInfo.VI_ORIGIN_REG;
	REG._VI_WIDTH = _gfxInfo.VI_WIDTH_REG;
	REG._VI_INTR = _gfxInfo.VI_INTR_REG;
	REG._VI_V_CURRENT_LINE = _gfxInfo.VI_V_CURRENT_LINE_REG;
	REG._VI_TIMING = _gfxInfo.VI_TIMING_REG;
	REG._VI_V_SYNC = _gfxInfo.VI_V_SYNC_REG;
	REG._VI_H_SYNC = _gfxInfo.VI_H_SYNC_REG;
	REG._VI_LEAP = _gfxInfo.VI_LEAP_REG;
	REG._VI_H_START = _gfxInfo.VI_H_START_REG;
	REG._VI_V_START = _gfxInfo.VI_V_START_REG;
	REG._VI_V_BURST = _gfxInfo.VI_V_BURST_REG;
	REG._VI_X_SCALE = _gfxInfo.VI_X_SCALE_REG;
	REG._VI_Y_SCALE = _gfxInfo.VI_Y_SCALE_REG;

	CheckInterrupts = _gfxInfo.CheckInterrupts;

	REG.SP_STATUS = nullptr;
}

void PluginAPI::SetSpStatus(uint32_t* v) const
{
    REG.SP_STATUS = v;
}

void PluginAPI::ChangeWindow()
{
	LOG(LOG_APIFUNC, "ChangeWindow\n");
	dwnd().setToggleFullscreen();
	//if (!m_bRomOpen)
	//	dwnd().closeWindow();
}

void PluginAPI::FBWrite(unsigned int _addr, unsigned int _size)
{
#ifdef RSPTHREAD
	m_executor.sync([=]()
	{
		GLContextGuard lck;
		FBInfo::fbInfo.Write(_addr, _size);
	});
#else
	FBInfo::fbInfo.Write(_addr, _size);
#endif
}

void PluginAPI::FBRead(unsigned int _addr)
{
#ifdef RSPTHREAD
	m_executor.sync([=]()
	{
		GLContextGuard lck;
		FBInfo::fbInfo.Read(_addr);
	});
#else
	FBInfo::fbInfo.Read(_addr);
#endif
}

void PluginAPI::FBGetFrameBufferInfo(void * _pinfo)
{
#ifdef RSPTHREAD
	m_executor.sync([=]()
	{
		GLContextGuard lck;
		FBInfo::fbInfo.GetInfo(_pinfo);
	});
#else
	FBInfo::fbInfo.GetInfo(_pinfo);
#endif
}

#ifndef MUPENPLUSAPI
void PluginAPI::FBWList(FrameBufferModifyEntry * _plist, unsigned int _size)
{
#ifdef RSPTHREAD
	m_executor.sync([=]()
	{
		GLContextGuard lck;
		FBInfo::fbInfo.WriteList(reinterpret_cast<FBInfo::FrameBufferModifyEntry*>(_plist), _size);
	});
#else
	FBInfo::fbInfo.WriteList(reinterpret_cast<FBInfo::FrameBufferModifyEntry*>(_plist), _size);
#endif
}

void PluginAPI::ReadScreen(void **_dest, long *_width, long *_height)
{
#ifdef RSPTHREAD
	m_executor.sync([=]()
	{
		GLContextGuard lck;
		dwnd().readScreen(_dest, _width, _height);
	});
#else
	dwnd().readScreen(_dest, _width, _height);
#endif
}
#endif
