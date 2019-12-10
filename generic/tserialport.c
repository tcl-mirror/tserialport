/*
    tserialport - tcl package of the library libserialport

       libserialport is a minimal, cross-platform shared library written in C that
       is intended to take care of the OS-specific details when writing software
       that uses serial ports. It is licensed under the terms of the GNU Lesser
       General Public License, version 3 or later. Note: While libserialport is
       hosted on sigrok.org (and sigrok uses libserialport), this is a completely
       independent library that can be used by other projects as well. The
       libserialport library does not depend on any sigrok related libraries or
       projects. The tcl package is licensed under the terms of the BSD-3 License.
       http://sigrok.org/wiki/Libserialport
	   

    Copyright (C) 2017-2019 Alexander Schoepe, Bochum, DE, <alx.tcl@sowaswie.de>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the project nor the names of its contributors may be used
       to endorse or promote products derived from this software without specific
       prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT
    SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
    TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
*/


#define MY_TCL_INITSTUBS "8.5"

#ifdef _WIN32
#include <windows.h>
#ifndef DECLSPEC_EXPORT
#define DECLSPEC_EXPORT __declspec ( dllexport )
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <tcl.h>

#include <libserialport.h>

#ifndef FALSE
#define FALSE 0
#define TRUE (!FALSE)
#endif

static char rcs[] = "@(#)tserialport.c $Revision: 1.2 $ $Date: 2017/05/15 12:45:30 $ (BSD 3 License) Alexander Schoepe, Bochum, DE";


static int Tserialport_Getports (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]) {
  Tcl_Obj *keyv[2];
  enum sp_return rc;
  struct sp_port **ports, *port;
  struct sp_port_config *conf;
  int i = 0, en, mode = SP_MODE_READ;
  int usb_bus, usb_address, usb_vid, usb_pid, baudrate, bits, stopbits;
  char *cp, *name, *description, *manufacturer, *product, *serial, *bluetooth;
  Tcl_Obj *dObjPtr = Tcl_NewDictObj();

  static const char *const command[] = {
    "open", NULL
  };
  enum command {
    TSP_OPEN, TSP_IGNORE
  } cmd;
  cmd = TSP_IGNORE;

  if (objc > 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "?open?");
    return TCL_ERROR;
  }
  if (objc == 2) {
    if (Tcl_GetIndexFromObj(interp, objv[1], command, "command", 0, (int *)&cmd) != TCL_OK) return TCL_ERROR;
  }

  if ((rc = sp_new_config(&conf)) == SP_OK) {
    if ((rc = sp_list_ports(&ports)) == SP_OK) {
      port = ports[i];
      while (port != NULL) {
	keyv[0] = Tcl_NewIntObj(i);

	name = sp_get_port_name(port);
	if (name != NULL) {
	  keyv[1] = Tcl_NewStringObj("device", -1);
	  Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_NewStringObj(name, -1));
	}

	description = sp_get_port_description(port);
	if (description != NULL) {
	  keyv[1] = Tcl_NewStringObj("description", -1);
	  Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_NewStringObj(description, -1));
	}

	manufacturer = sp_get_port_usb_manufacturer(port);
	if (manufacturer != NULL) {
	  keyv[1] = Tcl_NewStringObj("manufacturer", -1);
	  Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_NewStringObj(manufacturer, -1));
	}

	product = sp_get_port_usb_product(port);
	if (product != NULL) {
	  keyv[1] = Tcl_NewStringObj("product", -1);
	  Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_NewStringObj(product, -1));
	}

	serial = sp_get_port_usb_serial(port);
	if (serial != NULL) {
	  keyv[1] = Tcl_NewStringObj("serial", -1);
	  Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_NewStringObj(serial, -1));
	}

	if (sp_get_port_usb_vid_pid(port, &usb_vid, &usb_pid) == SP_OK) {
	  keyv[1] = Tcl_NewStringObj("vendor_id", -1);
	  Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_ObjPrintf("0x%x", usb_vid));
	  keyv[1] = Tcl_NewStringObj("product_id", -1);
	  Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_ObjPrintf("0x%x", usb_pid));
	}

	bluetooth = sp_get_port_bluetooth_address(port);
	if (bluetooth != NULL) {
	  keyv[1] = Tcl_NewStringObj("bluetooth", -1);
	  Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_NewStringObj(bluetooth, -1));
	}

	if (sp_get_port_usb_bus_address(port, &usb_bus, &usb_address) == SP_OK) {
	  keyv[1] = Tcl_NewStringObj("usb_bus", -1);
	  Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_ObjPrintf("0x%x", usb_bus));
	  keyv[1] = Tcl_NewStringObj("usb_address", -1);
	  Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_ObjPrintf("0x%x", usb_address));
	}

	switch (sp_get_port_transport(port)) {
	  case SP_TRANSPORT_NATIVE:
	    cp = "native";
	    break;
	  case SP_TRANSPORT_USB:
	    cp = "usb";
	    break;
	  case SP_TRANSPORT_BLUETOOTH:
	    cp = "bluetooth";
	    break;
	  default:
	    cp = "unknown";
	}
	keyv[1] = Tcl_NewStringObj("transport", -1);
	Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_NewStringObj(cp, -1));

	if (cmd == TSP_OPEN) {
	  if ((rc = sp_open(port, mode)) == SP_OK) {
	    keyv[1] = Tcl_NewStringObj("open", -1);
	    Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_NewStringObj("true", -1));

	    if (sp_get_config(port, conf) == SP_OK) {
	      if (sp_get_config_baudrate(conf, &baudrate) == SP_OK) {
		keyv[1] = Tcl_NewStringObj("baudrate", -1);
		Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_NewIntObj(baudrate));
	      }

	      if (sp_get_config_bits(conf, &bits) == SP_OK) {
		keyv[1] = Tcl_NewStringObj("bits", -1);
		Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_NewIntObj(bits));
	      }

	      if (sp_get_config_parity(conf, &en) == SP_OK) {
		switch (en) {
		  case SP_PARITY_INVALID:
		    cp = "invalid";
		    break;
		  case SP_PARITY_NONE:
		    cp = "none";
		    break;
		  case SP_PARITY_ODD:
		    cp = "odd";
		    break;
		  case SP_PARITY_EVEN:
		    cp = "even";
		    break;
		  case SP_PARITY_MARK:
		    cp = "mark";
		    break;
		  case SP_PARITY_SPACE:
		    cp = "space";
		    break;
		  default:
		    cp = "unknown";
		}
		keyv[1] = Tcl_NewStringObj("parity", -1);
		Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_NewStringObj(cp, -1));
	      }

	      if (sp_get_config_stopbits(conf, &stopbits) == SP_OK) {
		keyv[1] = Tcl_NewStringObj("stopbits", -1);
		Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_NewIntObj(stopbits));
	      }

	      if (sp_get_config_cts(conf, &en) == SP_OK) {
		switch (en) {
		  case SP_CTS_INVALID:
		    cp = "invalid";
		    break;
		  case SP_CTS_IGNORE:
		    cp = "ignore";
		    break;
		  case SP_CTS_FLOW_CONTROL:
		    cp = "flow control";
		    break;
		  default:
		    cp = "unknown";
		}
		keyv[1] = Tcl_NewStringObj("cts", -1);
		Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_NewStringObj(cp, -1));
	      }

	      if (sp_get_config_dsr(conf, &en) == SP_OK) {
		switch (en) {
		  case SP_DSR_INVALID:
		    cp = "invalid";
		    break;
		  case SP_DSR_IGNORE:
		    cp = "ignore";
		    break;
		  case SP_DSR_FLOW_CONTROL:
		    cp = "flow control";
		    break;
		  default:
		    cp = "unknown";
		}
		keyv[1] = Tcl_NewStringObj("dsr", -1);
		Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_NewStringObj(cp, -1));
	      }

	      if (sp_get_config_dtr(conf, &en) == SP_OK) {
		switch (en) {
		  case SP_DTR_INVALID:
		    cp = "invalid";
		    break;
		  case SP_DTR_OFF:
		    cp = "off";
		    break;
		  case SP_DTR_ON:
		    cp = "on";
		    break;
		  case SP_DTR_FLOW_CONTROL:
		    cp = "flow control";
		    break;
		  default:
		    cp = "unknown";
		}
		keyv[1] = Tcl_NewStringObj("dtr", -1);
		Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_NewStringObj(cp, -1));
	      }

	      if (sp_get_config_rts(conf, &en) == SP_OK) {
		switch (en) {
		  case SP_RTS_INVALID:
		    cp = "invalid";
		    break;
		  case SP_RTS_OFF:
		    cp = "off";
		    break;
		  case SP_RTS_ON:
		    cp = "on";
		    break;
		  case SP_RTS_FLOW_CONTROL:
		    cp = "flow control";
		    break;
		  default:
		    cp = "unknown";
		}
		keyv[1] = Tcl_NewStringObj("rts", -1);
		Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_NewStringObj(cp, -1));
	      }

	      if (sp_get_config_xon_xoff(conf, &en) == SP_OK) {
		switch (en) {
		  case SP_XONXOFF_INVALID:
		    cp = "invalid";
		    break;
		  case SP_XONXOFF_DISABLED:
		    cp = "disabled";
		    break;
		  case SP_XONXOFF_IN:
		    cp = "in";
		    break;
		  case SP_XONXOFF_OUT:
		    cp = "out";
		    break;
		  case SP_XONXOFF_INOUT:
		    cp = "in out";
		    break;
		  default:
		    cp = "unknown";
		}
		keyv[1] = Tcl_NewStringObj("xon_xoff", -1);
		Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_NewStringObj(cp, -1));
	      }
	    }

	    sp_close(port);
	  } else {
	    switch (rc) {
	      case SP_OK:
		cp = "operation completed successfully";
		break;
	      case SP_ERR_ARG:
		cp = "invalid arguments were passed to the function";
		break;
	      case SP_ERR_FAIL:
		cp = "a system error occurred while executing the operation";
		break;
	      case SP_ERR_MEM:
		cp = "a memory allocation failed while executing the operation";
		break;
	      case SP_ERR_SUPP:
		cp = "the requested operation is not supported by this system or device";
		break;
	      default:
		cp = "unknown";
	    }
	    keyv[1] = Tcl_NewStringObj("open", -1);
	    Tcl_DictObjPutKeyList(interp, dObjPtr, 2, keyv, Tcl_NewStringObj(cp, -1));
	  }
	}
	port = ports[++i];
      }
    }
    if (ports != NULL) sp_free_port_list(ports);
  }
  if (conf != NULL) sp_free_config(conf);

  Tcl_SetObjResult(interp, dObjPtr);
  return TCL_OK;
}


static int Tserialport_RcsId (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]) {
  Tcl_SetObjResult(interp, Tcl_NewStringObj(rcs, -1));
  return TCL_OK;
}


#ifdef _WIN32
DECLSPEC_EXPORT
#endif
int Tserialport_Init(Tcl_Interp *interp) {
#ifdef USE_TCL_STUBS
  if (Tcl_InitStubs(interp, MY_TCL_INITSTUBS, 0) == NULL) {
    return TCL_ERROR;
  }
#endif

  Tcl_CreateObjCommand(interp, "::tserialport::getports", Tserialport_Getports, (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);
  Tcl_CreateObjCommand(interp, "::tserialport::rcsid", Tserialport_RcsId, (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

  Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION);
  return TCL_OK;
}

#ifdef _WIN32
DECLSPEC_EXPORT
#endif
int Tserialport_SafeInit(Tcl_Interp *interp) {
  return Tserialport_Init(interp);
}

