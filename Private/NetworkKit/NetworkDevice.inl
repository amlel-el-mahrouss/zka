/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

/***
    Dtor and ctors.
*/

namespace HCore {
NetworkDevice::NetworkDevice(void (*out)(NetworkDeviceCommand),
                             void (*in)(NetworkDeviceCommand),
                             void (*on_cleanup)(void))
    : DeviceInterface<NetworkDeviceCommand>(out, in), fCleanup(on_cleanup) {
  kcout << "NK: NetworkDevice initialize.\r\n";

  MUST_PASS(out && in && on_cleanup);
}

NetworkDevice::~NetworkDevice() {
  MUST_PASS(fCleanup);

  kcout << "NK: NetworkDevice cleanup.\r\n";
  if (fCleanup) fCleanup();
}
} // namespace HCore