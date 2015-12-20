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

	if (!m_PlaybackQueue.empty()) {
		CComPtr<CAudioGraph> Graph = m_PlaybackQueue.back();

		// If graph isn't currently active, activate it
		if (!Graph->IsPlaying()) {
			m_AudioFileMap.clear();

			UINT NumNodes = Graph->GetNumNodes();

			// Create an IMFSourceReader for every file in the graph
			for (UINT i = 0; i < NumNodes; i++) {
				CComPtr<IAudioGraphNode> Node = nullptr;
				Graph->EnumNode(i, &Node);

				LPCSTR Filename = Node->GetAudioFilename();
				int Length = strlen(Filename);

				if (m_AudioFileMap[Filename] == nullptr) {
					// Source: http://stackoverflow.com/questions/10737644/convert-const-char-to-wstring

					int size_needed = MultiByteToWideChar(CP_UTF8, 0, Filename, Length, NULL, 0);
					std::wstring wFilename(size_needed, 0);
					MultiByteToWideChar(CP_UTF8, 0, Filename, Length, &wFilename[0], size_needed);

					hr = MFCreateSourceReaderFromURL (
						wFilename.c_str(),
						nullptr,
						&m_AudioFileMap[Filename]
					); CHECK_HR(__LINE__);
				}
			}

			Graph->SetPlaying(true);
		}
	} else {
		ZeroMemory(OutputBuffer, BufferFrames * sizeof(FLOAT) * 2);
	}
}

VOID CDXAudioWriteCallback::OnThreadInit() {
	HRESULT hr = S_OK;

	hr = MFStartup (
		MF_VERSION
	); CHECK_HR(__LINE__);
}