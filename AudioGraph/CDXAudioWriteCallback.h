/*
** Copyright (C) 2015 Austin Borger <aaborger@gmail.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
** API documentation is available here:
**		https://github.com/AustinBorger/DXAudio
*/

#pragma once

#include <comdef.h>
#include <atlbase.h>
#include <Windows.h>
#include <queue>

#include "DXAudio.h"
#include "AudioGraph.h"
#include "QueryInterface.h"

class CDXAudioWriteCallback : public IDXAudioWriteCallback {
public:
	CDXAudioWriteCallback();

	~CDXAudioWriteCallback();

	//IUnknown methods

	ULONG STDMETHODCALLTYPE AddRef() {
		return ++m_RefCount;
	}

	ULONG STDMETHODCALLTYPE Release() {
		m_RefCount--;

		if (m_RefCount <= 0) {
			this->~CDXAudioWriteCallback(); //don't use delete, since placement new is used by CAudioGraphFactory
			return 0;
		}

		return m_RefCount;
	}

	//New methods

	HRESULT Initialize(IAudioGraphCallback* pAudioGraphCallback);

	VOID QueueAudioGraph(IAudioGraph* pAudioGraph);

private:
	long m_RefCount;

	CComPtr<IAudioGraphCallback> m_Callback;
	std::queue<CComPtr<IAudioGraph>> m_PlaybackQueue;

	//IUnknown methods

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) final {
		QUERY_INTERFACE_CAST(IDXAudioWriteCallback);
		QUERY_INTERFACE_CAST(IDXAudioCallback);
		QUERY_INTERFACE_CAST(IUnknown);
		QUERY_INTERFACE_FAIL();
	}

	//IDXAudioWriteCallback methods

	VOID STDMETHODCALLTYPE OnObjectFailure(LPCWSTR File, UINT Line, HRESULT hr) final;

	VOID STDMETHODCALLTYPE OnProcess(FLOAT SampleRate, FLOAT* OutputBuffer, UINT BufferFrames) final;

	VOID STDMETHODCALLTYPE OnThreadInit() final;
};