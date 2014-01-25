/*
 * CAN bus driver interface.
 * Copyright (C) 2013 Pavel Kirienko <pavel.kirienko@gmail.com>
 */

#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>

namespace uavcan
{
/**
 * Raw CAN frame, as passed to/from the CAN driver.
 */
struct CanFrame
{
    static const uint32_t MASK_STDID = 0x000007FF;
    static const uint32_t MASK_EXTID = 0x1FFFFFFF;
    static const uint32_t FLAG_EFF = 1 << 31;                  ///< Extended frame format
    static const uint32_t FLAG_RTR = 1 << 30;                  ///< Remote transmission request

    uint32_t id;        ///< CAN ID with flags (above)
    uint8_t data[8];
    uint8_t dlc;        ///< Data Length Code

    Frame()
    : id(0)
    , dlc(0)
    { }

    Frame(uint32_t id, const uint8_t* data, uint8_t dlc)
    : id(id)
    , dlc(dlc)
    {
        assert(data && dlc <= 8);
        std::memmove(this->data, data, dlc);
    }

    bool isExtended() const { return id & FLAG_EFF; }
    bool isRemoteTransmissionRequest() const { return id & FLAG_RTR; }
};

/**
 * CAN hardware filter config struct. @ref ICanDriver::filter().
 */
struct CanFilterConfig
{
    uint32_t id;
    uint32_t mask;
};

/**
 * Single non-blocking CAN interface.
 */
class ICanIface
{
public:
    virtual ~ICanIface() { }

    /**
     * Non-blocking transmission.
     * If the frame wasn't transmitted upon TX timeout expiration, the driver should discard it.
     * @return 1 = one frame transmitted, 0 = TX buffer full, negative for error.
     */
    virtual int send(const Frame& frame, uint64_t tx_timeout_usec) = 0;

    /**
     * Non-blocking reception.
     * out_utc_timestamp_usec must be provided by the driver, ideally by the hardware CAN controller; 0 if unknown.
     * @return 1 = one frame received, 0 = RX buffer empty, negative for error.
     */
    virtual int receive(Frame& out_frame, uint64_t& out_utc_timestamp_usec) = 0;

    /**
     * Configure the hardware CAN filters. @ref CanFilterConfig.
     * @return 0 = success, negative for error.
     */
    virtual int filter(const CanFilterConfig* filter_configs, int num_configs) = 0;

    /**
     * Number of available hardware filters.
     */
    virtual int getNumFilters() const = 0;

    /**
     * Continuously incrementing counter of detected hardware errors.
     */
    virtual uint64_t getNumErrors() const = 0;
};

/**
 * Generic CAN driver.
 */
class ICanDriver
{
public:
    virtual ~ICanDriver() { }

    /**
     * Returns the interface by index, or null pointer if the index is out of range.
     */
    virtual ICanIface* getIface(int iface_index) = 0;

    /**
     * Total number of available CAN interfaces.
     */
    virtual int getNumIfaces() const = 0;

    /**
     * Block until the blocking timeout expires, or one of the specified interfaces becomes available for read or write.
     * Iface masks will be modified by the driver to indicate which exactly interfaces are available for IO.
     * Bit position in the masks defines interface index.
     * @param [in,out] inout_write_iface_mask Mask indicating which interfaces are needed/available to write.
     * @param [in,out] inout_read_iface_mask  Same as above for reading.
     * @param [in]     timeout_usec           Zero means non-blocking operation.
     * @return Positive number of ready interfaces or negative error code.
     */
    virtual int select(int& inout_write_iface_mask, int& inout_read_iface_mask, uint64_t timeout_usec) = 0;
};

}
