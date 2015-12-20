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

#include "CDXAudioWriteCallback.h"

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

#define FILENAME L"CDXAudioWriteCallback.cpp"
#define CHECK_HR(Line) if (FAILED(hr)) { m_Callback->OnObjectFailure(FILENAME, Line, hr); return; }

CDXAudioWriteCallback::CDXAudioWriteCallback() : m_RefCount(1) { }

CDXAudioWriteCallback::~CDXAudioWriteCallback() { 
	MFShutdown();
}

HRESULT CDXAudioWriteCallback::Initialize(IAudioGraphCallback* pAudioGraphCallback) {
	m_Callback = pAudioGraphCallback;

	return S_OK;
}

VOID CDXAudioWriteCallback::QueueAudioGraph(IAudioGraph* pAudioGraph) {
	m_PlaybackQueue.push((CAudioGraph*)(pAudioGraph));
}

VOID CDXAudioWriteCallback::OnObjectFailure(LPCWSTR File, UINT Line, HRESULT hr) {
	m_Callback->OnObjectFailure(File, Line, hr);
}

VOID CDXAudioWriteCallback::OnProcess(FLOAT SampleRate, FLOAT* OutputBuffer, UINT BufferFrames) {
	HRESULT hr = S_OK;
	UINT Written = 0;

	while (BufferFrames > 0 && !m_PlaybackQueue.empty()) {
		CComPtr<CAudioGraph> Graph = m_PlaybackQueue.front();

		// If graph isn't currently active, activate it
		if (!Graph->IsPlaying()) {
			Graph->Setup(m_MediaType);
			Graph->SetPlaying(true);
		}

		Written = Graph->Process(OutputBuffer, BufferFrames);

		BufferFrames -= Written;
		OutputBuffer += Written * 2;

		// If graph is done playing, flush its buffers and remove it from the queue.
		if (BufferFrames > 0) {
			Graph->Flush();
			m_PlaybackQueue.pop();
		}
	}

	if (BufferFrames > 0) {
		ZeroMemory(OutputBuffer, BufferFrames * sizeof(FLOAT) * 2);
	}
}

VOID CDXAudioWriteCallback::OnThreadInit() {
	HRESULT hr = S_OK;

	hr = MFStartup (
		MF_VERSION
	); CHECK_HR(__LINE__);

	hr = MFCreateMediaType (
		&m_MediaType
	); CHECK_HR(__LINE__);

	hr = m_MediaType->SetGUID (
		MF_MT_MAJOR_TYPE,
		MFMediaType_Audio
	); CHECK_HR(__LINE__);

	hr = m_MediaType->SetGUID (
		MF_MT_SUBTYPE,
		MFAudioFormat_PCM
	); CHECK_HR(__LINE__);
}