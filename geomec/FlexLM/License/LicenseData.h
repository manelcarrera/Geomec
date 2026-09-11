/*****************************************************************
 * FILENAME:
 *   $Workfile: LicenseData.h $
 *
 * PURPOSE:
 *   Contains the api for checking out a license.
 *   Please be sure that this library is included in the
 *   main project only. When you need to use it in other locations
 *	 you need to create hooks to work with the license.
 *
 * COPYRIGHT 1999 Shell Internationale Research Mij. B.V.
 *
 * CONFIDENTIALITY:
 *   This is a CONFIDENTIAL document.
 *   Neither the  whole  or any  part of this document may
 *   be  reproduced,  stored in  any retrieval  system  or
 *   transmitted in any form or by any means  (electronic,
 *   mechanical,  reprographic,  recording  or  otherwise)
 *   without the  prior  consent of the  copyright  owner.
 *
 * VERSION INFORMATION:
 *   $Revision: 15 $
 *   $Author: Esseniuse $
 *   $Date: 9/18/06 3:57p $
 *
 * PART OF MODULE:
 *    License
 *
 * MODULE CONTROLLER:
 *    Logica
 *****************************************************************/
#pragma once

#include <string>
#include <time.h>

class MessageReporter;

#define LICENSE_ONEDAYSECONDS 86400
#define LICENSE_MAX_MSG_LEN 255
#define LICENSE_MINIMUM_BORROW_DAYS 5
#define LICENSE_MINIMUM_BORROW_SECONDS (LICENSE_MINIMUM_BORROW_DAYS * LICENSE_ONEDAYSECONDS)
#define LICENSE_MAXIMUM_BORROW_DAYS 30

#ifdef EPTFLEXLM_MAX_MESS_LEN
#define PTA_MAX_BORROWLIST_LEN EPTFLEXLM_MAX_MESS_LEN
#else
#define PTA_MAX_BORROWLIST_LEN 1025
#endif

class COivLicenseData {
  static const char *m_licenseDaemon;
  static const char *m_FeatureNames[];

public:
  static void setupBorrow(const int day, const int month, const int year);
  static void returnBorrow();
};

/////////////////////////////////////////////////////////////////////////////
// LicenseData view

class LicenseData {
public:
  enum LicenseType { NO_LICENSE = -1, DEMO_LICENSE, FULL_LICENSE };

  LicenseData(const char *p_csAppName, const char *p_csAppVersion);

  // Attributes
public:
  bool m_bInitialized;

protected:
  static LicenseData *m_pLicenseInstance;

  // Operations
public:
  bool CheckOut(const char *p_pType, const char *p_pVersionNumber);
  bool CheckOut();
  void CheckIn();
  void BorrowLicense(__time64_t p_ctEndDate);
  // p_pBorrow should be at least EPTFLEXLM_MAX_MESS_LEN characters long.
  char *GetBorrowList(char *p_pBorrow);
  bool ReturnBorrowLicense();
  static LicenseData *CreateInstance(const char *p_csAppName, const char *p_csAppVersion);
  static LicenseData *GetInstance();
  static bool DestroyInstance();
  virtual bool Initialize();
  virtual void SetIntialized(bool p_bInitialized);
  bool IsInitialized();
  LicenseType GetLicenseType() { return m_eLicenseType; }
  void SetLicenseType(LicenseType p_eLicenseType) { m_eLicenseType = p_eLicenseType; }
  const char *GetFullLicenseName() { return m_csFullLicense; }
  void SetFullLicenseName(const char *p_csFullLicense) { m_csFullLicense = p_csFullLicense; }
  const char *GetDemoLicenseName() { return m_csDemoLicense; }
  void SetDemoLicenseName(const char *p_csDemoLicense) { m_csDemoLicense = p_csDemoLicense; }
  bool SetupLicense(const bool p_bChecking, bool p_bLogginOnly = false, bool (*p_GracePeriod)() = NULL);
  void SetReporter(MessageReporter *p_pReporter);

  // Implementation
protected:
  virtual ~LicenseData();

private:
  const char *m_csFullLicense;
  const char *m_csDemoLicense;
  const char *m_csAppName;
  const char *m_csAppVersion;
  int m_nLicenseCounter;

  char *GetFeature();
  bool ReInitAndCheckoutLicense();
  bool HasLicenseBorrowed();

  LicenseType m_eLicenseType;
  MessageReporter *m_pReporter;
};
