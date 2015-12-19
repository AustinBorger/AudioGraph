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

struct IAudioGraphCallback;
struct IAudioGraphEdge;
struct IAudioGraphNode;
struct IAudioGraphFile;
struct IAudioGraph;
struct IAudioGraphFactory;

/* IAudioGraphCallback is an interface that acts as a callback boundary between the application and the
** library. */
struct __declspec(uuid("b7fa0e54-41d7-4161-81d6-3036900cfc80")) IAudioGraphCallback : public IUnknown {
	/* OnObjectFailure() is a callback mechanism for error reporting.  If something unexpected happens,
	** or the library is operating out of its expected conditions, then this will be called with the
	** HRESULT that was provided by the Windows API when the error occurred. Note that this must be implemented. */
	virtual VOID STDMETHODCALLTYPE OnObjectFailure(LPCWSTR File, UINT Line, HRESULT hr) PURE;

	/* OnTransition() is called when a node is about to finish playing.  The implementation of this method should return
	** the desired trigger string, which identifies which node is to be traveled to next. If no edge
	** exists connected to the current node with the returned trigger string, OnObjectFailure() will be called. */
	virtual LPCSTR STDMETHODCALLTYPE OnTransition(IAudioGraph* pAudioGraph, IAudioGraphNode* pNode) PURE;
};

/* IAudioGraphEdge represents a directed edge in the audio graph. Associated with it is a trigger, 
** which is used to identify which particular edge is the path to take when a node is finished playing. */
struct __declspec(uuid("2a4bee1e-2d02-4f9c-bed9-eaedfb95331d")) IAudioGraphEdge : public IUnknown {
	/* Returns the ID of this particular edge. */
	virtual LPCSTR STDMETHODCALLTYPE GetID() PURE;

	/* Returns the trigger string associated with this edge. */
	virtual LPCSTR STDMETHODCALLTYPE GetTrigger() PURE;

	/* Returns this edge's formatted style string, which was used to create it. */
	virtual LPCSTR STDMETHODCALLTYPE GetStyleString() PURE;

	/* Retrieves the source node of this particular edge. */
	virtual VOID STDMETHODCALLTYPE GetFrom(IAudioGraphNode** ppNode) PURE;

	/* Retrieves the destination node of this particular edge. */
	virtual VOID STDMETHODCALLTYPE GetTo(IAudioGraphNode** ppNode) PURE;

	/* Retrieves the audio graph that this edge is attached to. */
	virtual VOID STDMETHODCALLTYPE GetAudioGraph(IAudioGraph** ppAudioGraph) PURE;

	/* Retrieves the audio graph file that this edge is associated with, if there is one. */
	virtual VOID STDMETHODCALLTYPE GetAudioGraphFile(IAudioGraphFile** ppAudioGraphFile) PURE;
};

/* IAudioGraphNode represents a node in an audio graph.  It can only be a member of a single audio graph -
** two audio graphs can't share a node object. */
struct __declspec(uuid("b8fd4cc2-4360-4701-bb1c-8715fd77d38e")) IAudioGraphNode : public IUnknown {
	/* Returns the ID of this particular node. */
	virtual LPCSTR STDMETHODCALLTYPE GetID() PURE;

	/* Returns this node's formatted style string, which was used to create it. */
	virtual LPCSTR STDMETHODCALLTYPE GetStyleString() PURE;

	/* Returns the name of the audio file that this node is streamed from. */
	virtual LPCSTR STDMETHODCALLTYPE GetAudioFilename() PURE;

	/* Returns the offset this node has from the start of the PCM audio data in the
	** associated file, in samples. */
	virtual UINT STDMETHODCALLTYPE GetSampleOffset() PURE;

	/* Returns the duration this node will play for, in samples. */
	virtual UINT STDMETHODCALLTYPE GetSampleDuration() PURE;

	/* Returns the offset this node has from the start of the PCM audio data in the
	** associated file, in seconds. */
	virtual FLOAT STDMETHODCALLTYPE GetTimeOffset() PURE;

	/* Returns the duration this node will play for, in seconds. */
	virtual FLOAT STDMETHODCALLTYPE GetTimeDuration() PURE;

	/* Returns the number of edges extending from this particular node. */
	virtual UINT STDMETHODCALLTYPE GetNumEdges() PURE;

	/* Retrieves an edge extending from this node by given array index. */
	virtual VOID STDMETHODCALLTYPE EnumEdge(UINT EdgeNum, IAudioGraphEdge** ppEdge) PURE;

	/* Retrieves an edge based on a given identifier. */
	virtual VOID STDMETHODCALLTYPE GetEdgeByID(LPCSTR ID, IAudioGraphEdge** ppEdge) PURE;

	/* Retrieves the audio graph that this node is attatched to. */
	virtual VOID STDMETHODCALLTYPE GetGraph(IAudioGraph** ppAudioGraph) PURE;

	/* Retrieves the audio graph file that this node is associated with, if there is one. */
	virtual VOID STDMETHODCALLTYPE GetAudioGraphFile(IAudioGraphFile** ppAudioGraphFile) PURE;
};

/* IAudioGraph represents a single audio graph, which is composed of nodes and directed edges. */
struct __declspec(uuid("b1f2bb1c-f1da-4f0a-ba3a-b7dbe2a7c824")) IAudioGraph : public IUnknown {
	/* Returns the ID of this particular graph. */
	virtual LPCSTR STDMETHODCALLTYPE GetID() PURE;

	/* Returns an arbitrary type string describing this graph. */
	virtual LPCSTR STDMETHODCALLTYPE GetType() PURE;

	/* Returns the style string of this graph. */
	virtual LPCSTR STDMETHODCALLTYPE GetStyleString() PURE;

	/* Creates a node that will be associated with this graph.  Style is a string listing the
	** attributes of the node, in XML syntax. */
	virtual VOID STDMETHODCALLTYPE CreateNode(LPCSTR Style, IAudioGraphNode** ppNode) PURE;

	/* Creates an edge that will be associated with this graph.  Style is a string listing the
	** attributes of the edge, in XML syntax. */
	virtual VOID STDMETHODCALLTYPE CreateEdge(LPCSTR Style, IAudioGraphEdge** ppEdge) PURE;

	/* Removes a node associated with this graph.  Note that the application is still
	** expected to release any references to the node. */
	virtual VOID STDMETHODCALLTYPE RemoveNode(IAudioGraphNode* pNode) PURE;

	/* Removes an edge associated with this graph.  Note that the application is still
	** expected to release any references to the edge. */
	virtual VOID STDMETHODCALLTYPE RemoveEdge(IAudioGraphEdge* pEdge) PURE;

	/* Returns the number of nodes associated with this particular graph. */
	virtual UINT STDMETHODCALLTYPE GetNumNodes() PURE;

	/* Retrieves an node associted with this graph by array index. */
	virtual VOID STDMETHODCALLTYPE EnumNode(UINT NodeNum, IAudioGraphNode** ppNode) PURE;

	/* Retrieves a node based on a given node identifier. */
	virtual VOID STDMETHODCALLTYPE GetNodeByID(LPCSTR ID, IAudioGraphNode** ppNode) PURE;

	/* Returns the number of edges associated with this particular graph. */
	virtual UINT STDMETHODCALLTYPE GetNumEdges() PURE;

	/* Retrieves an edge associted with this graph by array index. */
	virtual VOID STDMETHODCALLTYPE EnumEdge(UINT EdgeNum, IAudioGraphEdge** ppEdge) PURE;

	/* Retrieves an edge based on a given edge identifier. */
	virtual VOID STDMETHODCALLTYPE GetEdgeByID(LPCSTR ID, IAudioGraphEdge** ppEdge) PURE;

	/* Retrieves the audio graph file that this graph is associated with, if there is one. */
	virtual VOID STDMETHODCALLTYPE GetAudioGraphFile(IAudioGraphFile** ppAudioGraphFile) PURE;
};

/* IAudioGraphFile represents an XML file's state.  It can be loaded and parsed via IAudioGraphFactory::ParseAudioGraphFile().
** It can also be saved at runtime, allowing an application to dynamically create saved audio graph content. */
struct __declspec(uuid("91a4fdda-c694-4c6c-b33e-78a04545eeaa")) IAudioGraphFile : public IUnknown {
	/* Returns the object's filename. */
	virtual LPCWSTR STDMETHODCALLTYPE GetFilename() PURE;

	/* Returns the number of graphs contained in this file. */
	virtual UINT STDMETHODCALLTYPE GetNumGraphs() PURE;

	/* Retrieves a graph based on the given array index. */
	virtual VOID STDMETHODCALLTYPE EnumGraph(UINT GraphNum, IAudioGraph** ppAudioGraph) PURE;

	/* Retrieves a graph based on a given graph identifier. */
	virtual VOID STDMETHODCALLTYPE GetGraphByID(LPCSTR ID, IAudioGraph** ppAudioGraph) PURE;

	/* Appends an existing audio graph to the file. */
	virtual VOID STDMETHODCALLTYPE AppendGraph(IAudioGraph* pAudioGraph) PURE;

	/* Saves the file to disk in XML format. */
	virtual VOID STDMETHODCALLTYPE Save() PURE;
};

/* IAudioGraphFactory provides several APIs to create audio graphs.  It also provides the connection
** between the application and the Windows audio service.  There should be one of these per application. */
struct __declspec(uuid("b824c4eb-5a50-4706-8c14-bcc2f207d6ee")) IAudioGraphFactory : public IUnknown {
	/* Parses an XML file defining a set of audio graphs. */
	virtual VOID STDMETHODCALLTYPE ParseAudioGraphFile(LPCWSTR Filename, IAudioGraphFile** ppAudioGraphFile) PURE;

	/* Creates a blank XML file defining a set of audio graphs. */
	virtual VOID STDMETHODCALLTYPE CreateAudioGraphFile(LPCWSTR Filename, IAudioGraphFile** ppAudioGraphFile) PURE;

	/* Creates a blank audio graph. */
	virtual VOID STDMETHODCALLTYPE CreateAudioGraph(LPCSTR Style, IAudioGraph** ppAudioGraph) PURE;

	/* Places an audio graph in the playback queue. */
	virtual VOID STDMETHODCALLTYPE QueueAudioGraph(IAudioGraph* pAudioGraph) PURE;
};

#ifndef _AUDIO_GRAPH_EXPORT_TAG
	#ifdef _AUDIO_GRAPH_DLL_PROJECT
		#define _AUDIO_GRAPH_EXPORT_TAG __declspec(dllexport)
	#else
		#define _AUDIO_GRAPH_EXPORT_TAG __declspec(dllimport)
	#endif
#endif

/* AudioGraphCreateFactory() is the entry point into the API. 
** pAudioGraphCallback is required for transitions and error reporting.
** ppAudioGraphFactory is a pointer to a reference to the newly created factory. */
extern "C" HRESULT _AUDIO_GRAPH_EXPORT_TAG AudioGraphCreateFactory (
	IAudioGraphCallback* pAudioGraphCallback,
	IAudioGraphFactory** ppAudioGraphFactory
);