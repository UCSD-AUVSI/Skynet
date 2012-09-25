#pragma once

#include "DeckLinkAPI_h.h"
#include <vcclr.h>

namespace Vision
{
    ref class VisionController;
}

namespace Decklink
{
	// Wrapper for handling all of the callback stuff
	// This does not actually act as the callback itself, it just manages
	// the references to the callback

	class DecklinkCallback;

	class Callback
	{
	public:
		Callback( Vision::VisionController ^ );
		~Callback(void);
		void dontShow();
		void doShow();

	protected:
		IDeckLinkInput * m_input;
		DecklinkCallback * m_callback;
	};


	// This is the actual callback function class
	class DecklinkCallback :
	public IDeckLinkInputCallback
	{
	public:
		DecklinkCallback( gcroot<Vision::VisionController ^> handle);
		virtual ~DecklinkCallback(void);
		bool dontShow;

		// following are abstract but we don't use:
		virtual HRESULT STDMETHODCALLTYPE	QueryInterface(REFIID iid, LPVOID *ppv);
		virtual ULONG STDMETHODCALLTYPE		AddRef(void);
		virtual ULONG STDMETHODCALLTYPE		Release(void);	
		virtual HRESULT STDMETHODCALLTYPE	VideoInputFormatChanged(BMDVideoInputFormatChangedEvents notificationEvents, IDeckLinkDisplayMode* newDisplayMode, BMDDetectedVideoInputFormatFlags detectedSignalFlags);

		// we use this one:
		virtual HRESULT STDMETHODCALLTYPE	VideoInputFrameArrived(IDeckLinkVideoInputFrame* pArrivedFrame, IDeckLinkAudioInputPacket*);
		
	protected:
		bool firstFrame;
		
		//IDeckLinkInput * m_input;
		gcroot<Vision::VisionController ^> visionCtrlPtr;
		int	m_RefCount;


	};
}

