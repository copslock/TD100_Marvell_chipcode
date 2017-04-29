// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__1F2A9000_2D22_4556_8FAE_E768D226CC5B__INCLUDED_)
#define AFX_STDAFX_H__1F2A9000_2D22_4556_8FAE_E768D226CC5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define OASISLIBRARY_EXPORTS	1

#include <windows.h>

// TODO: reference additional headers your program requires here
#include "ntddndis.h"  
#include "Mrvioctl.h"
#include "odSuppTk.h"
#include "MrvlMfg.h"
#include "Mrv8000xInt.h"
#include "Registry.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__1F2A9000_2D22_4556_8FAE_E768D226CC5B__INCLUDED_)
