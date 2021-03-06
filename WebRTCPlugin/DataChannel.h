// DataChannel.h : Declaration of the DataChannel
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "WebRTCPlugin_i.h"
#include "CallbackDispatcher.h"
#include "api/datachannelinterface.h"
#include "WebRTCProxy.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

// DataChannel
class ATL_NO_VTABLE DataChannel :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IDataChannel, &IID_IDataChannel, &LIBID_WebRTCPluginLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IOleControlImpl<DataChannel>,
	public IOleObjectImpl<DataChannel>,
	public IOleInPlaceActiveObjectImpl<DataChannel>,
	public IViewObjectExImpl<DataChannel>,
	public IOleInPlaceObjectWindowlessImpl<DataChannel>,
	public CComCoClass<DataChannel, &CLSID_DataChannel>,
	public CComControl<DataChannel>,
    public webrtc::DataChannelObserver,
    public CallbackDispatcher<IUnknown>
{
public:


	DataChannel()
	{
	}

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_INVISIBLEATRUNTIME |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_DATACHANNEL)


DECLARE_NOT_AGGREGATABLE(DataChannel)

BEGIN_COM_MAP(DataChannel)
	COM_INTERFACE_ENTRY(IDataChannel)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
END_COM_MAP()

BEGIN_PROP_MAP(DataChannel)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()


BEGIN_MSG_MAP(DataChannel)
	CHAIN_MSG_MAP(CComControl<DataChannel>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// IViewObjectEx
	DECLARE_VIEW_STATUS(0)

// IDataChannel
public:
  HRESULT OnDraw(ATL_DRAWINFO& di)
  {
    return S_OK;
  }

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
    SetThread(WebRTCProxy::GetEventThread());
		return S_OK;
	}

	void FinalRelease()
	{
    datachannel->UnregisterObserver();
    datachannel = nullptr;
	}

  void Attach(rtc::scoped_refptr<webrtc::DataChannelInterface > &datachannel)
  {
    this->datachannel = datachannel;
    this->datachannel->RegisterObserver(this);
  }

  void SetPriority(const std::string &priority)
  {
    this->priority = priority;
  }
  // The data channel state have changed.
  virtual void OnStateChange() override;
  virtual void OnMessage(const webrtc::DataBuffer& buffer) override;
  virtual void OnBufferedAmountChange(uint64_t previous_amount) override;

  STDMETHOD(send)(VARIANT data);
  STDMETHOD(close)();

  STDMETHOD(get_label)(VARIANT* val);
  STDMETHOD(get_ordered)(VARIANT* val);
  STDMETHOD(get_maxPacketLifeTime)(VARIANT* val);
  STDMETHOD(get_negotiated)(VARIANT* val);
  STDMETHOD(get_id)(VARIANT* val);
  STDMETHOD(get_priority)(VARIANT* val);
  STDMETHOD(get_readyState)(VARIANT* val);
  STDMETHOD(get_bufferedAmount)(VARIANT* val);
  STDMETHOD(get_bufferedAmountLowThreshold)(VARIANT* val);
  STDMETHOD(put_bufferedAmountLowThreshold)(VARIANT val);

  STDMETHOD(put_onopen)(VARIANT handler);
  STDMETHOD(put_onbufferedamountlow)(VARIANT handler);
  STDMETHOD(put_onerror)(VARIANT handler);
  STDMETHOD(put_onclose)(VARIANT handler);
  STDMETHOD(put_onmessage)(VARIANT handler);

private:
  rtc::scoped_refptr<webrtc::DataChannelInterface> datachannel;
  std::vector<webrtc::DataBuffer> pending;
  std::string priority = "low";
  uint64_t bufferedAmountLowThreshold = 0;
  Callback onopen;
  Callback onbufferedamountlow;
  Callback onerror;
  Callback onclose;
  Callback onmessage;
};

OBJECT_ENTRY_AUTO(__uuidof(DataChannel), DataChannel)
