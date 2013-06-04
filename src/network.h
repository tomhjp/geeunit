#ifndef network_h
#define network_h

#include "names.h"

/* Network specification */

typedef enum {falling, low, rising, high} asignal;
typedef enum {aswitch, aclock, asiggen, andgate, nandgate, orgate,
	      norgate, xorgate, dtype, baddevice} devicekind;

struct outputrec {
  name_t     id;
  asignal    sig;
  outputrec* next;
};
typedef outputrec* outplink;
struct inputrec {
  name_t    id;
  outplink  connect;
  int       lineDefd;
  inputrec* next;
};
typedef inputrec* inplink;
struct devicerec {
  name_t id;
  inplink ilist;
  outplink olist;
  devicerec* next;
  devicekind kind;
  /* the next elements are only used by some of the device kinds */
  asignal swstate;      // used when kind == aswitch
  int frequency;        // used when kind == aclock
  int counter;          // used when kind == aclock or asiggen
  int patcount;         // used when kind == asiggen.  keeps index to pattern array. 
  asignal memory;       // used when kind == dtype
  vector<int> pattern;  // used when kind == asiggen
};
typedef devicerec* devlink;

class network {
  names* nmz;  // the instatiation of the names class that we are going to use.

 public:
  devlink devicelist (void);
    /* Returns list of devices                                             */
 
  devlink finddevice (name_t id);
   /* Returns link to device with specified name. Returns NULL if not       */
   /* found.                                                               */
   
   devicekind netzdevkind(name_t did);
   /* Returns the devicekind for a particular device id of type name_t      */
 
  inplink findinput (devlink dev, name_t id);
    /* Returns link to input of device pointed to by dev with specified    */
    /* name.  Returns NULL if not found.                                    */
 
  outplink findoutput (devlink dev, name_t id);
    /* Returns link to output of device pointed to by dev with specified   */
    /* name.  Returns NULL if not found.                                    */
 
  void adddevice (devicekind dkind, name_t did, devlink& dev);
    /* Adds a device to the device list with given name and returns a link */
    /* to it via 'dev'.                                                    */
 
  void addinput (devlink dev, name_t iid);
    /* Adds an input to the device pointed to by 'dev' with the specified  */
    /* name.                                                               */
 
  void addoutput (devlink dev, name_t oid);
    /* Adds an output to the device pointed to by 'dev' with the specified */
    /* name.                                                               */

  void makeconnection (name_t idev, name_t inp, name_t odev, name_t outp, int line, bool& ok);
    /* Makes a connection between the 'inp' input of device 'idev' and the */
    /* 'outp' output of device 'odev'. 'ok' is set true if operation       */
    /* succeeds.                                                           */
 
  string checknetwork (bool& ok);
    /* Checks that all inputs are connected to an output, and returns a    */
    /* string contining a list of unconnected inputs                       */
 
  int getLineDefd(devlink dev, name_t id);
    /* Returns the line at which the input connection was defined. 	 */
    /* used for error reporting 					                 */
    
  network (names* names_mod);
  /* Called on system initialisation.                                      */

 private:
  devlink devs;          // the list of devices
  devlink lastdev;       // last device in list of devices

};

#endif /* network_h */





