#pragma once

#include <cstdint>

enum class Command : uint8_t
{
    Reset = 0x01,
    SleepOut = 0x11,
    // GammaSet = 0x26,
    // DisplayOff = 0x28,
    DisplayOn = 0x29,
    ColumnAddressSet = 0x2A,
    PageAddressSet = 0x2B,
    MemoryWrite = 0x2C,
    MemoryAccessControl = 0x36,
    PixelFormatSet = 0x3A,
    // WriteDisplayBrightness = 0x51,
    // WriteCTRLDisplay = 0x53,
    // RgbInterfaceSignalControl = 0xB0,
    // FrameControlNormalMode = 0xB1,
    // BlankingPorchControl = 0xB5,
    // DisplayFunctionControl = 0xB6,
    // PowerControl1 = 0xC0,
    // PowerControl2 = 0xC1,
    // VComControl1 = 0xC5,
    // VComControl2 = 0xC7,
    // PowerControlA = 0xCB,
    // PowerControlB = 0xCF,
    // PositiveGammaCorrection = 0xE0,
    // NegativeGammaCorrection = 0xE1,
    // DriverTimingControlA = 0xE8,
    // DriverTimingControlB = 0xEA,
    // PowerOnSequenceControl = 0xED,
    // Enable3GammaControl = 0xF2,
    // InterfaceControl = 0xF6,
    // PumpRatioControl = 0xF7
};
