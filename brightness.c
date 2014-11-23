/* brightness.c - set brightness of display and keyboard backlights in OSX
 *
 * Can be used to set the brightness of the backilghts well below the minimum
 * level permitted by the normal function keys. This is especially useful for
 * e.g. setting the keybaord backlight at night (0.01 is a suggested value).
 *
 *
 * Building:
 *  gcc -o brightness brightness.c -framework IOKit \
        -framework ApplicationServices -framework CoreFoundation
 *
 * Note: works on 64-bit machines only, due to use of IOConnectCallScalarMethod
 *
 * Inspired by:
 * https://github.com/stevedekorte/io/blob/master/addons/AppleExtras
 * http://www.osxbook.com/book/bonus/chapter10/light/
 * 
 */ 

#include <stdio.h>
#include <IOKit/graphics/IOGraphicsLib.h> 
#include <ApplicationServices/ApplicationServices.h> 
#include <IOKit/IOKitLib.h>

#define PROGNAME "brightness" 

static io_connect_t dataPort = 0;

enum {
    kGetSensorReadingID = 0,    // getSensorReading(int *, int *)
    kGetLEDBrightnessID = 1,    // getLEDBrightness(int, int *)
    kSetLEDBrightnessID = 2,    // setLEDBrightness(int, int, int *)
    kSetLEDFadeID = 3,          // setLEDFade(int, int, int, int *)
};

void usage(void) {
	fprintf(stderr, "usage: %s <brightness>\n" " where 0.0 <= brightness <= 1.0\n", PROGNAME);
	exit(1);
}

io_connect_t getDataPort(void) {
    kern_return_t kr;
    io_service_t serviceObject;
    if (dataPort) return dataPort;
    
    // Look up a registered IOService object whose class is AppleLMUController
    serviceObject = IOServiceGetMatchingService(kIOMasterPortDefault, 
        IOServiceMatching("AppleLMUController"));
    
    if (!serviceObject) {
        printf("getDataPort() error: failed to find ambient light sensor\n");
        return 0;
    }
    
    // Create a connection to the IOService object
    kr = IOServiceOpen(serviceObject, mach_task_self(), 0, &dataPort);
    IOObjectRelease(serviceObject);
    if (kr != KERN_SUCCESS) {
        printf("getDataPort() error: failed to open IoService object\n");
        return 0;
    }
    return dataPort;
}

void setKeyboardBrightness(float in) {
    // static io_connect_t dp = 0; // shared?
    kern_return_t kr;
    uint64_t inputCount = 2;
    uint64_t inputValues[2];
    uint64_t in_unknown = 0;
    uint64_t in_brightness = in * 0xfff;
    inputValues[0] = in_unknown;
    inputValues[1] = in_brightness;
    uint32_t outputCount = 1;
    uint64_t outputValues[1];
    uint32_t out_brightness;

    kr = IOConnectCallScalarMethod(getDataPort(), kSetLEDBrightnessID, 
        inputValues, inputCount, outputValues, &outputCount);
    out_brightness = outputValues[0];
    if (kr != KERN_SUCCESS) {
        printf("setKeyboardBrightness() error\n");
        return;
    } else {
        printf("keyboard brightness is %f\n", in);
    }
}

int main(int argc, char **argv) {
	CGDisplayErr dErr;
	io_service_t service;
	CGDirectDisplayID targetDisplay;
	CFStringRef key = CFSTR(kIODisplayBrightnessKey);
	float brightness = HUGE_VALF;

    switch (argc) {
	 	case 1: 
	 		break; 

	 	case 2: 
	 		brightness = strtof(argv[1], NULL);
	 		break; 

	 	default:
	 		usage();
	 		break;
    }

    targetDisplay = CGMainDisplayID();
    service = CGDisplayIOServicePort(targetDisplay);

    if (brightness != HUGE_VALF) {
    	// set the display brightness, if requested
    	dErr = IODisplaySetFloatParameter(service, kNilOptions, key, brightness);

    	//set keyboard brightness
    	setKeyboardBrightness(brightness);
    }
    
    // now get the brightness
    dErr = IODisplayGetFloatParameter(service, kNilOptions, key, &brightness);

    if (dErr != kIOReturnSuccess) {
    	fprintf(stderr, "operation failed\n");
    } else {
    	printf("brightness of main display is %f\n", brightness);
    }

    exit(dErr);
}
