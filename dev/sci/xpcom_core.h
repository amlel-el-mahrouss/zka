/* -------------------------------------------

Copyright ZKA Web Services Co.

File: rt.internal.inl
Purpose: Base code of XPCOM.

------------------------------------------- */

/// @internal Reserved for internal definitions only.

#ifndef __TOOLCHAINKIT__
#define object	 class
#define protocol class
#define clsid(X)

#warning ! You may be using the clang compiler, please be cautious that some features mayn't be present. !
#endif // !__TOOLCHAINKIT__

protocol IUnknown; // Refrenced from an IDB entry.
protocol ICLSID;   // From the IDB, the constructor of the object, e.g: TextUCLSID.
object	 UUID;
object	 IStr;

/// @brief Unknown XPCOM interface
protocol clsid("d7c144b6-0792-44b8-b06b-02b227b547df") IUnknown
{
public:
	explicit IUnknown() = default;
	virtual ~IUnknown() = default;

	IUnknown& operator=(const IUnknown&) = default;
	IUnknown(const IUnknown&)			 = default;

	virtual SInt32	  Release()					= 0;
	virtual Void	  RemoveRef()				= 0;
	virtual IUnknown* AddRef()					= 0;
	virtual VoidPtr	  QueryClass(UUID * p_uuid) = 0;
};

/// @brief Allocate new XPCOM object.
/// @tparam TCLS the class type.
/// @tparam UCLSID UCLS factory class type.
/// @param uclsidOfCls UCLS factory class
/// @return TCLS interface
template <typename TCLS, typename UCLSID, typename... Args>
inline TCLS* XPCOMQueryInterface(UCLSID* uclsidOfCls, Args&&... args)
{
	if (uclsidOfCls == nullptr)
		return nullptr;

	uclsidOfCls->AddRef();
	return uclsidOfCls->QueryInterfaceWithArgs(args...);
}

/// @brief Release XPCOM object.
/// @tparam TCLS the class type.
/// @param cls the class to release.
/// @return status code.
template <typename TCLS>
inline SInt32 XPCOMReleaseClass(TCLS** cls)
{
	if (!*cls)
		return -kErrorInvalidData;

	(*cls)->RemoveRef();
	(*cls)->Release();

	*cls = nullptr;

	return kErrorSuccess;
}

/// @brief Event listener interface.
/// @tparam FnSign the event listener function type.
/// @tparam ClsID the event listener class ID.
template <typename FnSign, typename ClsID>
protocol IEventListener : public ClsID
{
	friend ClsID;

	explicit IEventListener() = default;
	virtual ~IEventListener() = default;

	IEventListener& operator=(const IEventListener&) = default;
	IEventListener(const IEventListener&)			 = default;

	virtual IEventListener& operator-=(const IStr* event_name)
	{
		this->RemoveEventListener(event_name);
		return *this;
	}

	virtual IEventListener& operator+=(FnSign arg)
	{
		this->AddEventListener(arg);
		return *this;
	}
};