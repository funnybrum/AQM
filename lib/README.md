# BME680

The BME680 folder contains the Bosch BSEC library version 1.4.6. All used headers and source files from it are here. There is additional required file - the library (libalgobsec.a) that is located in the parent folder.

The folder structure is flattened. According to Arduino v1.0 specification only the source files from ROOT of the library should be built.

##Notes##
 - The BSEC library require modifcation to the espressif8266 framework in order to avoid linking errors. Check [BSEC-Arduino-library|https://github.com/BoschSensortec/BSEC-Arduino-library] for details. But shortly - locate the ```eagle.app.v6.common.ld``` file and put ```*libalgobsec.a:(.literal.* .text.*)``` line below the ```*libwps.a:(.literal.* .text.*)``` line.