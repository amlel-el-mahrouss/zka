/*
 *      ========================================================
 *
 *      HCore
 *      Copyright Mahrouss Logic, all rights reserved.
 *
 *      ========================================================
 */

#ifndef __KERNELKIT_SHARED_OBJECT_HXX__
#define __KERNELKIT_SHARED_OBJECT_HXX__

#include <KernelKit/Loader.hpp>
#include <KernelKit/PEF.hpp>
#include <KernelKit/PEFCodeManager.hxx>
#include <NewKit/Defines.hpp>

namespace HCore {
  /**
   * @brief Shared Library class
   * Load library from this class
   */
class SharedObject final {
 public:
  struct SharedObjectTraits final {
    VoidPtr fImageObject;
    VoidPtr fImageEntrypointOffset;
  };

 public:
  explicit SharedObject() = default;
  ~SharedObject() = default;

 public:
  HCORE_COPY_DEFAULT(SharedObject);

 private:
  SharedObjectTraits *fMounted{nullptr};

 public:
  SharedObjectTraits **GetAddressOf() { return &fMounted; }

  SharedObjectTraits *Get() { return fMounted; }

 public:
  void Mount(SharedObjectTraits *to_mount) {
    if (!to_mount ||
        !to_mount->fImageObject)
        return;

    fMounted = to_mount;

    if (fLoader && to_mount) {
      delete fLoader;
      fLoader = nullptr;
    }

    if (!fLoader) {
      fLoader = new PEFLoader(fMounted->fImageObject);
    }
  }

  void Unmount() {
    if (fMounted) fMounted = nullptr;
  };

  template <typename SymbolType>
  SymbolType Load(const char *symbol_name) {
    auto ret = reinterpret_cast<SymbolType>(
        fLoader->FindSymbol(symbol_name, kPefCode));

    if (!ret)
      ret = reinterpret_cast<SymbolType>(
          fLoader->FindSymbol(symbol_name, kPefData));

    if (!ret)
      ret = reinterpret_cast<SymbolType>(
          fLoader->FindSymbol(symbol_name, kPefZero));

    return ret;
  }

 private:
  PEFLoader *fLoader{nullptr};
};

inline void hcore_pure_call(void) {
  // virtual placeholder.
  return;
}
}  // namespace HCore

#endif /* ifndef __KERNELKIT_SHARED_OBJECT_HXX__ */