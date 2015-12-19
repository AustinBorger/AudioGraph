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

CDXAudioWriteCallback::CDXAudioWriteCallback() : m_RefCount(1) { }

CDXAudioWriteCallback::~CDXAudioWriteCallback() { }

HRESULT CDXAudioWriteCallback::Initialize(IAudioGraphCallback* pAudioGraphCallback) {
	m_Callback = pAudioGraphCallback;

	return S_OK;
}

VOID CDXAudioWriteCallback::QueueAudioGraph(IAudioGraph* pAudioGraph) {
	m_PlaybackQueue.push(pAudioGraph);
}

VOID CDXAudioWriteCallback::OnObjectFailure(LPCWSTR File, UINT Line, HRESULT hr) {
	m_Callback->OnObjectFailure(File, Line, hr);
}

VOID CDXAudioWriteCallback::OnProcess(FLOAT SampleRate, FLOAT* OutputBuffer, UINT BufferFrames) {

}

VOID CDXAudioWriteCallback::OnThreadInit() {

}