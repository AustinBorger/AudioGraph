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
**		https://github.com/AustinBorger/AudioGraph
*/

#pragma once

#include <comdef.h>

struct __declspec(uuid("b7fa0e54-41d7-4161-81d6-3036900cfc80")) IAudioGraphCallback : public IUnknown {
	/* OnObjectFailure() is a callback mechanism for error reporting.  If something unexpected happens,
	** or the library is operating out of its expected conditions, then this will be called with the
	** HRESULT that was provided by the Windows API when the error occurred. Note that this must be implemented. */
	virtual VOID STDMETHODCALLTYPE OnObjectFailure(LPCWSTR File, UINT Line, HRESULT hr) PURE;

	/* OnTransition() is called when a node is about to finish playing.  The implementation of this method should return
	** the desired trigger string, which identifies which node is to be traveled to next. If no edge
	** exists connected to the current node with the returned trigger string, OnObjectFailure() will be called. */
	virtual LPCWSTR STDMETHODCALLTYPE OnTransition(IAudioGraph* pAudioGraph, IAudioGraphNode* pNode) PURE;
};

/* IAudioGraphEdge represents a directed edge in the audio graph. Associated with it is a trigger, 
** which is used to identify which particular edge is the path to take when a node is finished playing. */
struct __declspec(uuid("2a4bee1e-2d02-4f9c-bed9-eaedfb95331d")) IAudioGraphEdge : public IUnknown {
	/* Returns the ID of this particular edge. */
	virtual LPCWSTR STDMETHODCALLTYPE GetID() PURE;

	/* Retrieves the source node of this particular edge. */
	virtual VOID STDMETHODCALLTYPE GetFrom(IAudioGraphNode** ppNode) PURE;

	/* Retrieves the destination node of this particular edge. */
	virtual VOID STDMETHODCALLTYPE GetTo(IAudioGraphNode** ppNode) PURE;

	/* Returns the trigger string associated with this edge. */
	virtual LPCWSTR STDMETHODCALLTYPE GetTrigger() PURE;

	/* Returns this edge's formatted style string, which was used to create it. */
	virtual LPCWSTR STDMETHODCALLTYPE GetStyleString() PURE;
};

struct __declspec(uuid("b8fd4cc2-4360-4701-bb1c-8715fd77d38e")) IAudioGraphNode : public IUnknown {
	/* Returns the ID of this particular node. */
	virtual LPCWSTR STDMETHODCALLTYPE GetID() PURE;

	/* Returns the number of edges extending from this particular node. */
	virtual LONG STDMETHODCALLTYPE GetNumEdges() PURE;

	/* Retrieves an edge extending from this node by given array index. */
	virtual VOID STDMETHODCALLTYPE EnumEdge(LONG EdgeNum, IAudioGraphEdge** ppEdge) PURE;

	/* Retrieves an edge based on a given identifier. */
	virtual VOID STDMETHODCALLTYPE GetEdgeByID(LPCWSTR ID, IAudioGraphEdge** ppEdge) PURE;

	/* Returns the stretch coefficient associated with this particular node. */
	virtual FLOAT STDMETHODCALLTYPE GetStretch() PURE;

	/* Returns the gain coefficient associated with this particular node. */
	virtual FLOAT STDMETHODCALLTYPE GetGain() PURE;

	/* Returns the name of the file that this node streams from. */
	virtual LPCWSTR STDMETHODCALLTYPE GetFilename() PURE;

	/* Returns the offset this node has from the start of the PCM audio data in the
	** associated file, in samples. */
	virtual LONG STDMETHODCALLTYPE GetSampleOffset() PURE;

	/* Returns the duration this node will play for, in samples. */
	virtual LONG STDMETHODCALLTYPE GetSampleDuration() PURE;

	/* Returns the offset this node has from the start of the PCM audio data in the
	** associated file, in seconds. */
	virtual FLOAT STDMETHODCALLTYPE GetTimeOffset() PURE;

	/* Returns the duration this node will play for, in seconds. */
	virtual FLOAT STDMETHODCALLTYPE GetTimeDuration() PURE;

	/* Returns this node's formatted style string, which was used to create it. */
	virtual LPCWSTR STDMETHODCALLTYPE GetStyleString() PURE;
};

struct __declspec(uuid("b1f2bb1c-f1da-4f0a-ba3a-b7dbe2a7c824")) IAudioGraph : public IUnknown {
	/* Returns the ID of this particular graph. */
	virtual LPCWSTR STDMETHODCALLTYPE GetID() PURE;

	/* Returns an arbitrary type string describing this graph. */
	virtual LPCWSTR STDMETHODCALLTYPE GetType() PURE;

	/* Returns the style string of this graph. */
	virtual LPCWSTR STDMETHODCALLTYPE GetStyleString() PURE;

	/* Returns the gain of this graph. */
	virtual FLOAT STDMETHODCALLTYPE GetGain() PURE;

	/* Creates a node that will be associated with this graph.  Style is a string listing the
	** attributes of the node, in XML syntax. */
	virtual VOID STDMETHODCALLTYPE CreateNode(LPCWSTR Style, IAudioGraphNode** ppNode) PURE;

	/* Creates an edge that will be associated with this graph.  Style is a string listing the
	** attributes of the edge, in XML syntax. */
	virtual VOID STDMETHODCALLTYPE CreateEdge(LPCWSTR Style, IAudioGraphEdge** ppEdge) PURE;

	/* Removes a node associated with this graph.  Note that the application is still
	** expected to release any references to the node. */
	virtual VOID STDMETHODCALLTYPE RemoveNode(IAudioGraphNode* pNode) PURE;

	/* Removes an edge associated with this graph.  Note that the application is still
	** expected to release any references to the edge. */
	virtual VOID STDMETHODCALLTYPE RemoveEdge(IAudioGraphEdge* pEdge) PURE;

	/* Retrieves a node based on a given node identifier. */
	virtual VOID STDMETHODCALLTYPE GetNodeByID(LPCWSTR ID, IAudioGraphNode** ppNode) PURE;

	/* Retrieves an edge based on a given edge identifier. */
	virtual VOID STDMETHODCALLTYPE GetEdgeByID(LPCWSTR ID, IAudioGraphEdge** ppEdge) PURE;

	/* Gets the mix gain of this graph.  The mix gain is used for mixing/fading between different
	** audio graphs, where multiple graphs are playing concurrently. */
	virtual FLOAT STDMETHODCALLTYPE GetMixVolume() PURE;

	/* Sets the mix gain of this graph.  The mix gain is used for mixing/fading between different
	** audio graphs, where multiple graphs are playing concurrently. */
	virtual VOID STDMETHODCALLTYPE SetMixVolume(FLOAT Volume) PURE;
};

struct __declspec(uuid("91a4fdda-c694-4c6c-b33e-78a04545eeaa")) IAudioGraphFile : public IUnknown {
	virtual LONG STDMETHODCALLTYPE GetNumGraphs() PURE;

	virtual VOID STDMETHODCALLTYPE EnumGraph(LONG GraphNum, IAudioGraph** ppAudioGraph) PURE;

	virtual VOID STDMETHODCALLTYPE GetGraphByID(LPCWSTR ID, IAudioGraph** ppAudioGraph) PURE;

	virtual LPCWSTR STDMETHODCALLTYPE GetFilename() PURE;

	virtual VOID STDMETHODCALLTYPE AppendGraph(IAudioGraph* pAudioGraph) PURE;

	virtual VOID STDMETHODCALLTYPE Save() PURE;
};

/* IAudioGraphFactory provides several APIs to create audio graphs.  It also provides the connection
** between the application and the Windows audio service.  There should be one of these per application. */
struct __declspec(uuid("b824c4eb-5a50-4706-8c14-bcc2f207d6ee")) IAudioGraphFactory : public IUnknown {
	virtual VOID STDMETHODCALLTYPE ParseAudioGraphFile(LPCWSTR Filename, IAudioGraphFile** ppAudioGraphFile) PURE;

	virtual VOID STDMETHODCALLTYPE CreateAudioGraphFile(LPCWSTR Filename, IAudioGraphFile** ppAudioGraphFile) PURE;

	virtual VOID STDMETHODCALLTYPE CreateAudioGraph(LPCWSTR Style, IAudioGraph** ppAudioGraph) PURE;
};

#ifndef _AUDIO_GRAPH_EXPORT_TAG
	#ifdef _AUDIO_GRAPH_DLL_PROJECT
		#define _AUDIO_GRAPH_EXPORT_TAG __declspec(dllexport)
	#else
		#define _AUDIO_GRAPH_EXPORT_TAG __declspec(dllimport)
	#endif
#endif

extern "C" HRESULT _AUDIO_GRAPH_EXPORT_TAG AudioGraphCreateFactory (
	IAudioGraphCallback* pAudioGraphCallback,
	IAudioGraphFactory** ppAudioGraphFactory
);