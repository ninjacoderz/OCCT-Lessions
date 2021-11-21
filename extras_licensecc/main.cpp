#include <iostream>

// License protection
#include <licensecc.h>

//! Some useful functions.
namespace LicenseccUtils
{
  inline const char* GetStatusString(const LCC_EVENT_TYPE evt)
  {
    switch ( evt )
    {
      case LICENSE_OK:                       return "OK";
      case LICENSE_FILE_NOT_FOUND:           return "license file not found";
      case LICENSE_SERVER_NOT_FOUND:         return "license server can't be contacted";
      case ENVIRONMENT_VARIABLE_NOT_DEFINED: return "environment variable not defined";
      case FILE_FORMAT_NOT_RECOGNIZED:       return "license file has invalid format (not .ini file)";
      case LICENSE_MALFORMED:                return "some mandatory fields are missing, or data can't be fully read";
      case PRODUCT_NOT_LICENSED:             return "this product was not licensed";
      case PRODUCT_EXPIRED:                  return "product expired";
      case LICENSE_CORRUPTED:                return "license signature didn't match with current license";
      case IDENTIFIERS_MISMATCH:             return "calculated identifier and the one provided in license didn't match";
      case LICENSE_SPECIFIED:                return "license location was specified";
      case LICENSE_FOUND:                    return "license file has been found or license data has been located";
      case PRODUCT_FOUND:                    return "license has been loaded and the declared product has been found";
      case SIGNATURE_VERIFIED:               return "signature verified";

      default: break;
    }

    return "Unknown status (NOK)";
  }

  inline bool IsLicenseOk()
  {
    // Here we'll check that our license is Ok or not.

    // Check license.
    LCC_EVENT_TYPE licCode = acquire_license(nullptr, nullptr, nullptr);
    std::cout << "License status: " << GetStatusString(licCode) << std::endl;
    //
    if ( licCode == LICENSE_OK )
      return true; // License is Ok.

    // If not, we can give the user a hint, how to get a license for his PC.

    // Get PC identifier.
    char* pLccIdentifier = nullptr;
    size_t buffSize = 0;
    identify_pc(STRATEGY_DEFAULT, pLccIdentifier, &buffSize, nullptr); // Get buffer size.
    //
    if ( buffSize )
    {
      pLccIdentifier = new char[buffSize];
      identify_pc(STRATEGY_DEFAULT, pLccIdentifier, &buffSize, nullptr); // Initialize buffer.

      std::cout << "License is missing. Your PC identifier is '" << pLccIdentifier << "'." << std::endl;
    }

    return false;
  }
}

int main(int argc, char** argv)
{
  if ( !LicenseccUtils::IsLicenseOk() )
  {
    std::cout << "Cannot run this awesome program without a license!" << std::endl;
    return 1;
  }

  std::cout << "Hello Protected World!" << std::endl;

  return 0;
}
