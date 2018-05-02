#pragma once

#include "afxcmn.h"
#include "afxwin.h"
#include "HalconCpp.h"
//#include "HDevThread.h"

static int WM_FGDISP = RegisterWindowMessage(_T("MYFGDisp"));
static int WM_IPDISP = RegisterWindowMessage(_T("MYIPDisp"));                  // 线程让主窗口显示
static int UM_UPDATESTATUS = RegisterWindowMessage(_T("MYUM_UPDATESTATUS"));   // 线程更新状栏文字
static int UM_UPDATE_RIBBON = RegisterWindowMessage(_T("UM_UPDATE_RIBBON"));

static int UM_UPDATESTATUS_APPSTATE = RegisterWindowMessage(_T("UM_UPDATESTATUS_APPSTATE"));
static int UM_LOGOUT = RegisterWindowMessage(_T("UM_LOGOUT"));                  // 显示日志信息


// 采图时用的曝光等参数 


enum FGF
{
	FGF_INVALID = 0x00000001,      // Data area might be damaged
	FGF_LAST = 0x00000002,      // Last in queue
	FGF_DMAHALTED = 0x00000004,      // Dma was halted in between
	FGF_FORCEPOST = 0x10000000      // Force post to driver in LIMPMODE
};


// Enumeration for resolutions
enum FG_RESOLUTION
{
	RES_160_120 = 0,
	RES_320_240,
	RES_640_480,
	RES_800_600,
	RES_1024_768,
	RES_1280_960,
	RES_1600_1200,
	RES_1920_1080,
	RES_SCALABLE,
	RES_LAST
};

enum FG_COLORMODE
{
	CM_Y8 = 0,
	CM_YUV411,
	CM_YUV422,
	CM_YUV444,
	CM_RGB8,
	CM_Y16,
	CM_RGB16,
	CM_SY16,
	CM_SRGB16,
	CM_RAW8,
	CM_RAW16,
	CM_LAST
};

// Enumeration for frame rates
enum FG_FRAMERATE
{
	FR_1_875 = 0,
	FR_3_75,
	FR_7_5,
	FR_15,
	FR_30,
	FR_60,
	FR_120,
	FR_240,
	FR_LAST
};

// Enumeration for DMA mode
enum FG_DMA
{
	DMA_CONTINOUS = 0,
	DMA_LIMP,
	DMA_REPLACE,
	DMA_LAST
};

// Specific Parameter data types
enum FG_PARSPECIFIC
{
	FGPS_INVALID = 0,
	FGPS_FEATUREINFO,
	FGPS_TRIGGERINFO,
	FGPS_COLORFORMAT,
	FGPS_LAST
};

// Error codes
enum FG_ERROR
{
	FCE_NOERROR = 0,     /* No Error */
	FCE_ALREADYOPENED = 1001,     /* Something already opened */
	FCE_NOTOPENED = 1002,     /* Need open before */
	FCE_NODETAILS = 1003,     /* No details */
	FCE_DRVNOTINSTALLED = 1004,     /* Driver not installed */
	FCE_MISSINGBUFFERS = 1005,     /* Don't have buffers */
	FCE_INPARMS = 1006,     /* Parameter error */
	FCE_CREATEDEVICE = 1007,     /* Error creating WinDevice */
	FCE_WINERROR = 1008,     /* Internal Windows error */
	FCE_IOCTL = 1009,     /* Error DevIoCtl */
	FCE_DRVRETURNLENGTH = 1010,     /* Wrong length return data */
	FCE_INVALIDHANDLE = 1011,     /* Wrong handle */
	FCE_NOTIMPLEMENTED = 1012,     /* Function not implemented */
	FCE_DRVRUNNING = 1013,     /* Driver runs already */
	FCE_STARTERROR = 1014,     /* Couldn't start */
	FCE_INSTALLERROR = 1015,     /* Installation error */
	FCE_DRVVERSION = 1016,     /* Driver has wrong version */
	FCE_NODEADDRESS = 1017,     /* Wrong nodeaddress */
	FCE_PARTIAL = 1018,     /* Partial info. copied */
	FCE_NOMEM = 1019,     /* No memory */
	FCE_NOTAVAILABLE = 1020,     /* Requested function not available */
	FCE_NOTCONNECTED = 1021,     /* Not connected to target */
	FCE_ADJUSTED = 1022     /* A pararmeter had to be adjusted */
};

// HALER
enum HALER
{
	/* Lowest layer errors */
	HALER_NOERROR = 0,
	HALER_NOCARD = 1,   /* Card is not present */
	HALER_NONTDEVICE = 2,   /* No logical Device */
	HALER_NOMEM = 3,   /* Not enough memory */
	HALER_MODE = 4,   /* Not allowed in this mode */
	HALER_TIMEOUT = 5,   /* Timeout */
	HALER_ALREADYSTARTED = 6,   /* Something is started */
	HALER_NOTSTARTED = 7,   /* Not started */
	HALER_BUSY = 8,   /* Busy at the moment */
	HALER_NORESOURCES = 9,   /* No resources available */
	HALER_NODATA = 10,   /* No data available */
	HALER_NOACK = 11,   /* Didn't get acknowledge */
	HALER_NOIRQ = 12,   /* Interruptinstallerror */
	HALER_NOBUSRESET = 13,   /* Error waiting for busreset */
	HALER_NOLICENSE = 14,   /* No license */
	HALER_RCODEOTHER = 15,   /* RCode not RCODE_COMPLETE */
	HALER_PENDING = 16,   /* Something still pending */
	HALER_INPARMS = 17,   /* Input parameter range */
	HALER_CHIPVERSION = 18,   /* Unrecognized chipversion */
	HALER_HARDWARE = 19,   /* Hardware error */
	HALER_NOTIMPLEMENTED = 20,   /* Not implemented */
	HALER_CANCELLED = 21,   /* Cancelled */
	HALER_NOTLOCKED = 22,   /* Memory is not locked */
	HALER_GENERATIONCNT = 23,   /* Bus reset in between */
	HALER_NOISOMANAGER = 24,   /* No IsoManager present */
	HALER_NOBUSMANAGER = 25,   /* No BusManager present */
	HALER_UNEXPECTED = 26,   /* Unexpected value */
	HALER_REMOVED = 27,   /* Target was removed */
	HALER_NOBUSRESOURCES = 28,   /* No ISO resources available */
	HALER_DMAHALTED = 29   /* DMA halted */
};

 // Parameters
enum FG_PARAMETER
{
	FGP_IMAGEFORMAT = 0,                            // Compact image format
	FGP_ENUMIMAGEFORMAT,                          // Enumeration (Reset,Get)
	FGP_BRIGHTNESS,                               // Set image brightness
	FGP_AUTOEXPOSURE,                             // Set auto exposure
	FGP_SHARPNESS,                                // Set image sharpness
	FGP_WHITEBALCB,                               // Blue
	FGP_WHITEBALCR,                               // Red
	FGP_HUE,                                      // Set image hue
	FGP_SATURATION,                               // Set color saturation
	FGP_GAMMA,                                    // Set gamma
	FGP_SHUTTER,                                  // Shutter time
	FGP_GAIN,                                     // Gain
	FGP_IRIS,                                     // Iris
	FGP_FOCUS,                                    // Focus
	FGP_TEMPERATURE,                              // Color temperature
	FGP_TRIGGER,                                  // Trigger
	FGP_TRIGGERDLY,                               // Delay of trigger
	FGP_WHITESHD,                                 // Whiteshade
	FGP_FRAMERATE,                                // Frame rate
	FGP_ZOOM,                                     // Zoom
	FGP_PAN,                                      // Pan
	FGP_TILT,                                     // Tilt
	FGP_OPTICALFILTER,                            // Filter
	FGP_CAPTURESIZE,                              // Size of capture
	FGP_CAPTUREQUALITY,                           // Quality
	FGP_PHYSPEED,                                 // Set speed for asy/iso
	FGP_XSIZE,                                    // Image XSize
	FGP_YSIZE,                                    // Image YSize
	FGP_XPOSITION,                                // Image x position
	FGP_YPOSITION,                                // Image y position
	FGP_PACKETSIZE,                               // Packet size
	FGP_DMAMODE,                                  // DMA mode (continuous or limp)
	FGP_BURSTCOUNT,                               // Number of images to produce
	FGP_FRAMEBUFFERCOUNT,                         // Number of frame buffers
	FGP_USEIRMFORBW,                              // Allocate bandwidth or not (IsoRscMgr)
	FGP_ADJUSTPARAMETERS,                         // Adjust parameters or fail
	FGP_STARTIMMEDIATELY,                         // Start bursting immediately
	FGP_FRAMEMEMORYSIZE,                          // Read only: Frame buffer size
	FGP_COLORFORMAT,                              // Read only: Colorformat
	FGP_IRMFREEBW,                                // Read only: Free iso bytes for 400MBit
	FGP_DO_FASTTRIGGER,                           // Fast trigger (no ACK)
	FGP_DO_BUSTRIGGER,                            // Broadcast trigger
	FGP_RESIZE,                                   // Start/Stop resizing
	FGP_USEIRMFORCHN,                             // Get channel over isochronous resource manager
	FGP_CAMACCEPTDELAY,                           // Delay after writing values
	FGP_ISOCHANNEL,                               // Iso channel
	FGP_CYCLETIME,                                // Read cycle time
	FGP_DORESET,                                  // Reset camera
	FGP_DMAFLAGS,                                 // Flags for ISO DMA
	FGP_R0C,                                      // Ring 0 call gate
	FGP_BUSADDRESS,                               // Exact bus address
	FGP_CMDTIMEOUT,                               // Global bus command timeout
	FGP_CARD,                                     // Card number of this camera (set before connect)
	FGP_LICENSEINFO,                              // Query license information
	FGP_PACKETCOUNT,                              // Read only: Packet count
	FGP_DO_MULTIBUSTRIGGER,                       // Do trigger on several busses
	FGP_CARDRESET,                                // Do reset on card (for hard errors)

	FGP_LAST
};
