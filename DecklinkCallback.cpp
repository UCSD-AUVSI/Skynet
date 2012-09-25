#include "stdafx.h"
#include "DecklinkCallback.h"
#include "VisionController.h"

using namespace Decklink;


// This creates everything associated with the Decklink card and passes in the callback
// class, which has to keep reference counts to dispose of itself properly
Callback::Callback( Vision::VisionController ^ visionCtrlRef )
{
	IDeckLinkIterator * deckLinkIterator = NULL;
	IDeckLink *	deckLink = NULL;
	HRESULT result;

	m_callback = new DecklinkCallback( visionCtrlRef );	

	// Initialize COM
	result = CoInitialize(NULL);
	if (FAILED(result))
	{
		System::Diagnostics::Trace::WriteLine("Initialization of COM failed - result = " + result);
	}

	// Create IDeckLinkIterator
	result = CoCreateInstance(CLSID_CDeckLinkIterator, NULL, CLSCTX_ALL, IID_IDeckLinkIterator, (void**)&deckLinkIterator);
	if (FAILED(result))
	{
		System::Diagnostics::Trace::WriteLine("A DeckLink iterator could not be created.  The DeckLink drivers may not be installed.");
	}

	if( deckLinkIterator->Next(&deckLink) == S_OK )
	{			
		IDeckLinkConfiguration * config;
		// we have device
		if( deckLink->QueryInterface( IID_IDeckLinkInput, (void**)&m_input) != S_OK ) 
		{
			System::Diagnostics::Trace::WriteLine("Could not obtain the IDeckLinkInput interface");
		}
		else // have input thingy
		{
			if( deckLink->QueryInterface( IID_IDeckLinkConfiguration, (void**)&config) != S_OK )
			{
				System::Diagnostics::Trace::WriteLine("Could not obtain the IDeckLinkConfiguration interface");
			}

			// bmdVideoConnectionSDI bmdVideoConnectionComposite



			config->SetInt(bmdDeckLinkConfigVideoOutputConnection, bmdVideoConnectionComposite);

			result = m_input->SetCallback( m_callback );
			if (FAILED(result))
				System::Diagnostics::Trace::WriteLine("StartStreams failed.");
 
			// bmdFormat8BitYUV bmdFormat8BitARGB bmdFormat8BitBGRA
			// bmdModeHD1080i5994  bmdModeNTSC
			result = m_input->EnableVideoInput( bmdModeNTSC, bmdFormat8BitYUV, 0 );
			if (FAILED(result))
				System::Diagnostics::Trace::WriteLine("EnableVideoInput failed.");

			result = m_input->StartStreams();
			if (FAILED(result))
				System::Diagnostics::Trace::WriteLine("StartStreams failed.");
		}
	}

}

Callback::~Callback(void)
{
	m_input->StopStreams();
	m_input->Release();
	
	delete m_callback;	
	
	CoUninitialize(); // un-init com	
}


void 
Callback::dontShow()
{
	m_callback->dontShow = true;

}

void 
Callback::doShow() 
{

	m_callback->dontShow = false;

}

DecklinkCallback::DecklinkCallback(gcroot<Vision::VisionController ^> handle)
{
	firstFrame = true;
	visionCtrlPtr = handle;
	m_RefCount = 1;
	
	dontShow = false;
}


DecklinkCallback::~DecklinkCallback(void)
{
	//CoUninitialize(); // un-init com
	//m_input->StopStreams();
}

HRESULT STDMETHODCALLTYPE
DecklinkCallback::VideoInputFrameArrived( IDeckLinkVideoInputFrame *videoFrame, IDeckLinkAudioInputPacket *audioPacket)
{
	if( videoFrame == NULL )
		return E_FAIL;

	if (dontShow)
		return E_FAIL;

	HRESULT result;
	int * frameBuffer = NULL;	
	int frameHeight, frameWidth; 
	int rowBytes;
	//char * outputBuffer = NULL;
	float * outputBuffer = NULL;

	frameHeight = videoFrame->GetHeight();
	frameWidth  = videoFrame->GetWidth();
	rowBytes    = videoFrame->GetRowBytes();

	//outputBuffer = new char[frameHeight * frameWidth * 3];
	outputBuffer = new float[frameHeight * frameWidth * 4];	   	

	// get video data
	result = videoFrame->GetBytes( (void **)&frameBuffer );

	if( FAILED(result) )
	{
		delete outputBuffer;

		return E_FAIL;
	}

	if( firstFrame )
	{
		visionCtrlPtr->gotFirstFrame( frameWidth, frameHeight );
	}

	int col2 = 0;
	for( int row=0; row < frameHeight; row += 2 ) 
	{
		float * ptr = &outputBuffer[row * frameWidth * 4 / 2];

		if( col2 >= rowBytes / 4 )
			col2 = 0;

		for( int col=0; col < frameWidth; col++ ) 
		{
			int value = frameBuffer[row * rowBytes / 4 + col2];
				
			int Y2 = (value & 0xFF000000) >> 24;
			int Y  = (value & 0x0000FF00) >> 8;
			int Cr = (value & 0x00FF0000) >> 16;
			int Cb = (value & 0x000000FF) >> 0;

			if( col % 2 == 1 )
			{
				Y = Y2;
				col2++;
			}		

			int r = (int)(1.164*(Y-16) + 1.596*(Cr-128));
			int g = (int)(1.164*(Y-16) - 0.813*(Cr-128) - 0.391*(Cb-128));
			int b = (int)(1.164*(Y-16) + 2.018*(Cb-128));

			r = (r < 0) ? 0 : (r > 255) ? 255 : r;
			g = (g < 0) ? 0 : (g > 255) ? 255 : g;
			b = (b < 0) ? 0 : (b > 255) ? 255 : b;


			for( int chan = 0; chan < 4; chan++ )
			{
				switch( chan )
				{
				case 0:
					*ptr++ = ((float)r)/255.0f;
					break;
				case 1:
					*ptr++ =  ((float)g)/255.0f;
					break;
				case 2:
					*ptr++ =  ((float)b)/255.0f;
					break;
				case 3:
					*ptr++ =  1.0f;
					break;
				}
			}
		}
	}	

	// send frame to vision pathway
	if (!dontShow)
		visionCtrlPtr->receiveFrame( outputBuffer );
	
	//System::Diagnostics::Trace::WriteLine( "update buffer from callback");
	firstFrame = false;

	//delete outputBuffer;

	return S_OK;
}


HRESULT STDMETHODCALLTYPE
DecklinkCallback::QueryInterface(REFIID iid, LPVOID *ppv)
{
	HRESULT			result = E_NOINTERFACE;
	
	// Initialise the return result
	*ppv = NULL;
	
	// Obtain the IUnknown interface and compare it the provided REFIID
	if (iid == IID_IUnknown)
	{
		*ppv = this;
		AddRef();
		result = S_OK;
	}
	else if (iid == IID_IDeckLinkInputCallback)
	{
		*ppv = (IDeckLinkInputCallback*)this;
		AddRef();
		result = S_OK;
	}
	
	return result;
}

ULONG STDMETHODCALLTYPE
DecklinkCallback::Release()
{
	int		newRefValue;
	
	newRefValue = InterlockedDecrement((LONG*)&m_RefCount);
	if (newRefValue == 0)
	{
		delete this;
		return 0;
	}
	
	return newRefValue;
}

ULONG STDMETHODCALLTYPE
DecklinkCallback::AddRef()
{
	return InterlockedIncrement((LONG*)&m_RefCount);
}
HRESULT STDMETHODCALLTYPE
DecklinkCallback::VideoInputFormatChanged(BMDVideoInputFormatChangedEvents notificationEvents, IDeckLinkDisplayMode* newDisplayMode, BMDDetectedVideoInputFormatFlags detectedSignalFlags)
{
	return 0;
}
