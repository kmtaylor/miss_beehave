/***************************************************************************//**
* \file USBFS_descr.c
* \version 3.20
*
* \brief
*  This file contains the USB descriptors and storage.
*
********************************************************************************
* \copyright
* Copyright 2008-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "USBFS_pvt.h"


/*****************************************************************************
*  User supplied descriptors.  If you want to specify your own descriptors,
*  define USER_SUPPLIED_DESCRIPTORS below and add your descriptors.
*****************************************************************************/
/* `#START USER_DESCRIPTORS_DECLARATIONS` Place your declaration here */

/* `#END` */


/***************************************
*  USB Customizer Generated Descriptors
***************************************/

#if !defined(USER_SUPPLIED_DESCRIPTORS)
/*********************************************************************
* Device Descriptors
*********************************************************************/
const uint8 CYCODE USBFS_DEVICE0_DESCR[18u] = {
/* Descriptor Length                       */ 0x12u,
/* DescriptorType: DEVICE                  */ 0x01u,
/* bcdUSB (ver 2.0)                        */ 0x00u, 0x02u,
/* bDeviceClass                            */ 0x00u,
/* bDeviceSubClass                         */ 0x00u,
/* bDeviceProtocol                         */ 0x00u,
/* bMaxPacketSize0                         */ 0x08u,
/* idVendor                                */ 0xB4u, 0x04u,
/* idProduct                               */ 0x32u, 0xF2u,
/* bcdDevice                               */ 0x01u, 0x00u,
/* iManufacturer                           */ 0x01u,
/* iProduct                                */ 0x02u,
/* iSerialNumber                           */ 0x80u,
/* bNumConfigurations                      */ 0x01u
};
/*********************************************************************
* Config Descriptor  
*********************************************************************/
const uint8 CYCODE USBFS_DEVICE0_CONFIGURATION0_DESCR[] = {
/*  Config Descriptor Length               */ 0x09u,
/*  DescriptorType: CONFIG                 */ 0x02u,
/*  wTotalLength                           */ 0x90u, 0x00u,
/*  bNumInterfaces                         */ 0x02u,
/*  bConfigurationValue                    */ 0x01u,
/*  iConfiguration                         */ 0x01u,
/*  bmAttributes                           */ 0x80u,
/*  bMaxPower                              */ 0x32u,
/*********************************************************************
* Interface Descriptor
*********************************************************************/
/*  Interface Descriptor Length            */ 0x09u,
/*  DescriptorType: INTERFACE              */ 0x04u,
/*  bInterfaceNumber                       */ 0x00u,
/*  bAlternateSetting                      */ 0x00u,
/*  bNumEndpoints                          */ 0x00u,
/*  bInterfaceClass                        */ 0x0Eu,
/*  bInterfaceSubClass                     */ 0x01u,
/*  bInterfaceProtocol                     */ 0x01u,
/*  iInterface                             */ 0x02u,
/*********************************************************************
* Video Control Header
*********************************************************************/
/*  Video control descriptor length        */ 0x0Du,
/*  DescriptorType: Class interface        */ 0x24u,
/*  Subtype                                */ 0x01u,
/*  Version                                */ 0x50u, 0x01u,
/*  Length                                 */ 0x00u, 0x00u,
/*  Clock frequency                        */ 0x00u, 0x00u, 0x00u, 0x00u,
/*  Num streaming interfaces               */ 0x01u,
/*  First streaming interface              */ 0x01u,
/*********************************************************************
* Video Input Terminal Descriptor
*********************************************************************/
/*  Length                                 */ 0x08u,
/*  DescriptorType: Class interface        */ 0x24u,
/*  Subtype                                */ 0x02u,
/*  Terminal ID                            */ 0x01u,
/*  Terminal type                          */ 0x01u, 0x01u,
/*  Associated terminal                    */ 0x02u,
/*  String descriptor                      */ 0x00u,
/*********************************************************************
* Processing Unit Descriptor
*********************************************************************/
/*  Length                                 */ 0x0Du,
/*  DescriptorType: Class interface        */ 0x24u,
/*  Subtype                                */ 0x05u,
/*  Unit ID                                */ 0x02u,
/*  Source ID                              */ 0x01u,
/*  Max Multiplier                         */ 0x00u, 0x00u,
/*  Control Size                           */ 0x03u,
/*  Controls                               */ 0x01u, 0x00u, 0x00,
/*  iProcessing                            */ 0x00u,
/*  Video Standards                        */ 0x00u,
/*********************************************************************
* Video Output Terminal Descriptor
*********************************************************************/
/*  Length                                 */ 0x09u,
/*  DescriptorType: Class interface        */ 0x24u,
/*  Subtype                                */ 0x03u,
/*  Terminal ID                            */ 0x03u,
/*  Terminal type                          */ 0x01u, 0x03u,
/*  Associated terminal                    */ 0x01u,
/*  Source ID                              */ 0x02u,
/*  String descriptor                      */ 0x00u,
/*********************************************************************
* Interface Descriptor
*********************************************************************/
/*  Interface Descriptor Length            */ 0x09u,
/*  DescriptorType: INTERFACE              */ 0x04u,
/*  bInterfaceNumber                       */ 0x01u,
/*  bAlternateSetting                      */ 0x00u,
/*  bNumEndpoints                          */ 0x01u,
/*  bInterfaceClass                        */ 0x0Eu,
/*  bInterfaceSubClass                     */ 0x02u,
/*  bInterfaceProtocol                     */ 0x01u,
/*  iInterface                             */ 0x02u,
/*********************************************************************
* Video Streaming Header
*********************************************************************/
/*  Video streaming descriptor length      */ 0x09u,
/*  DescriptorType: Class interface        */ 0x24u,
/*  Subtype                                */ 0x02u,
/*  Number of formats                      */ 0x01u,
/*  Length                                 */ 0x00u, 0x00u,
/*  Endpoint address                       */ 0x01u,
/*  Terminal ID                            */ 0x01u,
/*  Control size                           */ 0x00u,
/*********************************************************************
* Format descriptor
*********************************************************************/
/*  Length                                 */ 0x1Cu,
/*  DescriptorType: Class interface        */ 0x24u,
/*  Subtype                                */ 0x10u,
/*  Index                                  */ 0x00u,
/*  Number of frame descriptors            */ 0x01u,
/*  GUID format BGR888                     */ 0x7du, 0xebu, 0x36u, 0xe4u,
                                              0x4fu, 0x52u, 0xceu, 0x11u,
                                              0x9fu, 0x53u, 0x00u, 0x20u,
                                              0xafu, 0x0bu, 0xa7u, 0x70u,
/*  Bits per pixel                         */ 0x18u,
/*  Default frame index                    */ 0x00u,
/*  Aspect ratio X                         */ 0x00u,
/*  Aspect ratio Y                         */ 0x00u,
/*  Interface flags                        */ 0x02u,
/*  Copy protect                           */ 0x00u,
/*  Variable size                          */ 0x00u,
/*********************************************************************
* Frame descriptor
*********************************************************************/
/*  Length                                 */ 0x1Eu,
/*  DescriptorType: Class interface        */ 0x24u,
/*  Subtype                                */ 0x11u,
/*  Index                                  */ 0x00u,
/*  Capabilities                           */ 0x00u,
/*  Width                                  */ 0x40u, 0x00u,
/*  Height                                 */ 0x30u, 0x00u,
/*  MinBitRate                             */ 0x00u, 0x00u, 0x00u, 0x00u,
/*  MaxBitRate                             */ 0x00u, 0x00u, 0x00u, 0x00u,
/*  Default frame interval                 */ 0x40u, 0x0du, 0x03u, 0x00u,
/*  Frame interval type                    */ 0x01u,
/*  Bytes per line                         */ 0x00u, 0x00u, 0x00u, 0x00u,
/*  Frame rate                             */ 0x40u, 0x0du, 0x03u, 0x00u,
/*********************************************************************
* Endpoint Descriptor
*********************************************************************/
/*  Endpoint Descriptor Length             */ 0x07u,
/*  DescriptorType: ENDPOINT               */ 0x05u,
/*  bEndpointAddress                       */ 0x01u,
/*  bmAttributes                           */ 0x02u,
/*  wMaxPacketSize                         */ 0x40u, 0x00u,
/*  bInterval                              */ 0x00u
};

/*********************************************************************
* String Descriptor Table
*********************************************************************/
const uint8 CYCODE USBFS_STRING_DESCRIPTORS[71u] = {
/*********************************************************************
* Language ID Descriptor
*********************************************************************/
/* Descriptor Length                       */ 0x04u,
/* DescriptorType: STRING                  */ 0x03u,
/* Language Id                             */ 0x09u, 0x0Cu,
/*********************************************************************
* String Descriptor: "Cypress Semiconductor"
*********************************************************************/
/* Descriptor Length                       */ 0x2Cu,
/* DescriptorType: STRING                  */ 0x03u,
 (uint8)'C', 0u,(uint8)'y', 0u,(uint8)'p', 0u,(uint8)'r', 0u,(uint8)'e', 0u,
 (uint8)'s', 0u,(uint8)'s', 0u,(uint8)' ', 0u,(uint8)'S', 0u,(uint8)'e', 0u,
 (uint8)'m', 0u,(uint8)'i', 0u,(uint8)'c', 0u,(uint8)'o', 0u,(uint8)'n', 0u,
 (uint8)'d', 0u,(uint8)'u', 0u,(uint8)'c', 0u,(uint8)'t', 0u,(uint8)'o', 0u,
 (uint8)'r', 0u,
/*********************************************************************
* String Descriptor: "LED Raster"
*********************************************************************/
/* Descriptor Length                       */ 0x16u,
/* DescriptorType: STRING                  */ 0x03u,
 (uint8)'L', 0u,(uint8)'E', 0u,(uint8)'D', 0u,(uint8)' ', 0u,(uint8)'R', 0u,
 (uint8)'a', 0u,(uint8)'s', 0u,(uint8)'t', 0u,(uint8)'e', 0u,(uint8)'r', 0u,
/*********************************************************************/
/* Marks the end of the list.              */ 0x00u};
/*********************************************************************/

/*********************************************************************
* Serial Number String Descriptor
*********************************************************************/
const uint8 CYCODE USBFS_SN_STRING_DESCRIPTOR[2] = {
/* Descriptor Length                       */ 0x02u,
/* DescriptorType: STRING                  */ 0x03u,

};



/*********************************************************************
* Endpoint Setting Table -- This table contain the endpoint setting
*                           for each endpoint in the configuration. It
*                           contains the necessary information to
*                           configure the endpoint hardware for each
*                           interface and alternate setting.
*********************************************************************/
const T_USBFS_EP_SETTINGS_BLOCK CYCODE USBFS_DEVICE0_CONFIGURATION0_EP_SETTINGS_TABLE[1u] = {
/* IFC  ALT    EPAddr bmAttr MaxPktSize Class ********************/
{0x01u, 0x00u, 0x01u, 0x02u, 0x0040u,   0x0Eu}
};
const uint8 CYCODE USBFS_DEVICE0_CONFIGURATION0_INTERFACE_CLASS[2u] = {
0x0Eu, 0x0Eu
};
/*********************************************************************
* Config Dispatch Table -- Points to the Config Descriptor and each of
*                          and endpoint setup table and to each
*                          interface table if it specifies a USB Class
*********************************************************************/
const T_USBFS_LUT CYCODE USBFS_DEVICE0_CONFIGURATION0_TABLE[5u] = {
    {0x01u,     &USBFS_DEVICE0_CONFIGURATION0_DESCR},
    {0x01u,     &USBFS_DEVICE0_CONFIGURATION0_EP_SETTINGS_TABLE},
    {0x00u,    NULL},
    {0x00u,    NULL},
    {0x00u,     &USBFS_DEVICE0_CONFIGURATION0_INTERFACE_CLASS}
};
/*********************************************************************
* Device Dispatch Table -- Points to the Device Descriptor and each of
*                          and Configuration Tables for this Device 
*********************************************************************/
const T_USBFS_LUT CYCODE USBFS_DEVICE0_TABLE[3u] = {
    {0x01u,     &USBFS_DEVICE0_DESCR},
    {0x00u,    NULL},
    {0x01u,     &USBFS_DEVICE0_CONFIGURATION0_TABLE}
};
/*********************************************************************
* Device Table -- Indexed by the device number.
*********************************************************************/
const T_USBFS_LUT CYCODE USBFS_TABLE[1u] = {
    {0x01u,     &USBFS_DEVICE0_TABLE}
};

#endif /* USER_SUPPLIED_DESCRIPTORS */

#if defined(USBFS_ENABLE_MSOS_STRING)

    /******************************************************************************
    *  USB Microsoft OS String Descriptor
    *  "MSFT" identifies a Microsoft host
    *  "100" specifies version 1.00
    *  USBFS_GET_EXTENDED_CONFIG_DESCRIPTOR becomes the bRequest value
    *  in a host vendor device/class request
    ******************************************************************************/

    const uint8 CYCODE USBFS_MSOS_DESCRIPTOR[USBFS_MSOS_DESCRIPTOR_LENGTH] = {
    /* Descriptor Length                       */   0x12u,
    /* DescriptorType: STRING                  */   0x03u,
    /* qwSignature - "MSFT100"                 */   (uint8)'M', 0u, (uint8)'S', 0u, (uint8)'F', 0u, (uint8)'T', 0u,
                                                    (uint8)'1', 0u, (uint8)'0', 0u, (uint8)'0', 0u,
    /* bMS_VendorCode:                         */   USBFS_GET_EXTENDED_CONFIG_DESCRIPTOR,
    /* bPad                                    */   0x00u
    };

    /* Extended Configuration Descriptor */

    const uint8 CYCODE USBFS_MSOS_CONFIGURATION_DESCR[USBFS_MSOS_CONF_DESCR_LENGTH] = {
    /*  Length of the descriptor 4 bytes       */   0x28u, 0x00u, 0x00u, 0x00u,
    /*  Version of the descriptor 2 bytes      */   0x00u, 0x01u,
    /*  wIndex - Fixed:INDEX_CONFIG_DESCRIPTOR */   0x04u, 0x00u,
    /*  bCount - Count of device functions.    */   0x01u,
    /*  Reserved : 7 bytes                     */   0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    /*  bFirstInterfaceNumber                  */   0x00u,
    /*  Reserved                               */   0x01u,
    /*  compatibleID    - "CYUSB\0\0"          */   (uint8)'C', (uint8)'Y', (uint8)'U', (uint8)'S', (uint8)'B',
                                                    0x00u, 0x00u, 0x00u,
    /*  subcompatibleID - "00001\0\0"          */   (uint8)'0', (uint8)'0', (uint8)'0', (uint8)'0', (uint8)'1',
                                                    0x00u, 0x00u, 0x00u,
    /*  Reserved : 6 bytes                     */   0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
    };

#endif /* USBFS_ENABLE_MSOS_STRING */

/* DIE ID string descriptor for 8 bytes ID */
#if defined(USBFS_ENABLE_IDSN_STRING)
    uint8 USBFS_idSerialNumberStringDescriptor[USBFS_IDSN_DESCR_LENGTH];
#endif /* USBFS_ENABLE_IDSN_STRING */


/* [] END OF FILE */
